/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of EdytorNC.
 *
 *  EdytorNC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Enable the M_PI constant in MSVC
// see https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/math-constant
#define _USE_MATH_DEFINES

#include <cmath>    // for cos, sin, M_PI
#include <cstdlib>  // for abs

#include <QCheckBox>        // for QCheckBox
#include <QColor>           // for QColor
#include <QLineEdit>        // for QLineEdit
#include <QLocale>          // for QLocale
#include <QPushButton>      // for QPushButton
#include <QSettings>        // for QSettings
#include <QSpinBox>         // for QSpinBox
#include <QString>          // for QString
#include <QTabWidget>       // for QTabWidget
#include <QTableWidget>     // for QTableWidget
#include <QTableWidgetItem> // for QTableWidgetItem
#include <QVBoxLayout>      // for QVBoxLayout
#include <QWidget>          // for QWidget
#include <Qt>               // for operator|, AlignRight, AlignVCenter
#include <QtGlobal>         // for qreal, qMax, QFlags

#include <utils/removezeros.h>  // for Utils::removeZeros

#include "bhcdialog.h"
#include "bhcdraw.h"        // for BHCDraw
#include "bhcoptions.h"     // for BHCOptions
#include "bhctab.h"         // for BHCTab
#include "bhctaboptions.h"  // for BHCTabOptions


BHCDialog::BHCDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent)
{
    setupUi(this);

    mSettings = settings;

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Bolt circle"));

    drawing = nullptr;

    parentHeight = parent->height() - 10;
    parentWidth = parent->width() - 15;

    tabBar = new QTabWidget(this);
    pageLayout->addWidget(tabBar);

    BHCTab *page1 = new BHCTab(this);
    BHCTab *page2 = new BHCTab(this);
    BHCTab *page3 = new BHCTab(this);
    BHCTab *page4 = new BHCTab(this);

    tabBar->addTab(page1, tr("Circle 1 - green"));
    tabBar->addTab(page2, tr("Circle 2 - blue"));
    tabBar->addTab(page3, tr("Circle 3 - red"));
    tabBar->addTab(page4, tr("Circle 4 - yellow"));

    connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));
    connect(clearAllButton, SIGNAL(clicked()), SLOT(clearAll()));
    connect(page1, SIGNAL(commonChk()), SLOT(comChk()));
    connect(page2, SIGNAL(commonChk()), SLOT(comChk()));
    connect(page3, SIGNAL(commonChk()), SLOT(comChk()));
    connect(page4, SIGNAL(commonChk()), SLOT(comChk()));

    adjustSize();
}

BHCDialog::~BHCDialog()
{
}

void BHCDialog::comChk()
{
    BHCTab *tab;
    int tabId, roat, activTab;
    bool mirX, mirY;

    tab = (BHCTab *)tabBar->currentWidget();
    activTab = tabBar->currentIndex();

    if (tab->all->isChecked()) {
        roat = tab->roateInput->value();
        mirX = tab->mirrorX->isChecked();
        mirY = tab->mirrorY->isChecked();

        for (tabId = 0; tabId < tabBar->count(); tabId++) {
            tab = (BHCTab *)tabBar->widget(tabId);

            if (tab == nullptr) {
                continue;
            }

            if (tabId == activTab) {
                continue;
            }

            if (tab->all->isChecked()) {
                tab->roateInput->setValue(roat);
                tab->mirrorX->setChecked(mirX);
                tab->mirrorY->setChecked(mirY);
            }
        }
    }

    computeButtonClicked();
}

void BHCDialog::clearAll()
{
    for (int tabId = 0; tabId < tabBar->count(); tabId++) {
        BHCTab *tab = (BHCTab *)tabBar->widget(tabId);

        if (tab == nullptr) {
            continue;
        }

        tab->xCenterInput->setText("0");
        tab->yCenterInput->setText("0");
        tab->diaInput->setText("0");
        tab->angleStartInput->setText("0");
        tab->angleBeetwenInput->setText("0");
        tab->holesInput->setText("0");
        tab->roateInput->setValue(0);

        tab->resultTable->setRowCount(0);
        tab->resultTable->clearContents();

    }

    if (drawing != nullptr) {
        drawing->close();
    }
}

void BHCDialog::computeButtonClicked()
{
    BHCTab *tab;
    QColor col;
    int tabId, i, textPosY, textPosX, dir;
    bool ok;
    int holeCount;
    qreal dia, firstAngle, angleBeetwen, roate, x, y, ang, xCenter, yCenter;
    double maxDia;

    maxDia = 0;

    for (tabId = 0; tabId <= tabBar->count(); tabId++) {
        tab = (BHCTab *)tabBar->widget(tabId);

        if (tab == nullptr) {
            continue;
        }

        maxDia = qMax(maxDia, (QLocale().toDouble(tab->diaInput->text(),
                               &ok) + 2 * abs(QLocale().toDouble(tab->xCenterInput->text(), &ok))));
        maxDia = qMax(maxDia, (QLocale().toDouble(tab->diaInput->text(),
                               &ok) + 2 * abs(QLocale().toDouble(tab->yCenterInput->text(), &ok))));
    }

    for (tabId = 0; tabId < tabBar->count(); tabId++) {
        tab = (BHCTab *)tabBar->widget(tabId);

        if (tab == nullptr) {
            continue;
        }

        if (!tab->xCenterInput->hasAcceptableInput()) {
            tab->xCenterInput->setText("0");
            xCenter = 0;
        } else {
            xCenter = QLocale().toDouble(tab->xCenterInput->text(), &ok);
        }

        if (!tab->yCenterInput->hasAcceptableInput()) {
            tab->yCenterInput->setText("0");
            yCenter = 0;
        } else {
            yCenter = QLocale().toDouble(tab->yCenterInput->text(), &ok);
        }

        if (!tab->diaInput->hasAcceptableInput()) {
            tab->diaInput->setText("0");
            tab->resultTable->setRowCount(0);
            continue;
        } else {
            dia = QLocale().toDouble(tab->diaInput->text(), &ok);
        }

        if (tab->angleStartInput->hasAcceptableInput()) {
            firstAngle = QLocale().toDouble(tab->angleStartInput->text(), &ok);
        } else {
            tab->angleStartInput->setText("0");
            firstAngle = 0;
        }

        if (tab->angleBeetwenInput->hasAcceptableInput()) {
            angleBeetwen = QLocale().toDouble(tab->angleBeetwenInput->text(), &ok);
        } else {
            tab->angleBeetwenInput->setText("0");
            angleBeetwen = 0;
        }

        if (tab->holesInput->hasAcceptableInput()) {
            holeCount = tab->holesInput->text().toInt(&ok);
        } else {
            tab->holesInput->setText("0");
            holeCount = 0;
        }

        roate = tab->roateInput->text().toInt(&ok);

        if ((holeCount == 0) && (angleBeetwen == 0)) {
            tab->resultTable->setRowCount(0);
            continue;
        }

        if (dia == 0) {
            continue;
        }

        roate = roate / 10;

        if (holeCount <= 0) {
            holeCount = 360 / angleBeetwen;
        }

        if (angleBeetwen <= 0) {
            angleBeetwen = double(360 / double(holeCount));
        }

        firstAngle += roate;

        tab->resultTable->setRowCount(holeCount);

        dia = dia / 2;

        if (tab->mirrorY->isChecked()) {
            xCenter = -xCenter;
        }

        if (tab->mirrorX->isChecked()) {
            yCenter = -yCenter;
        }

        for (i = 0; i < holeCount; i++) {
            ang = firstAngle + (angleBeetwen * i);

            if (tab->mirrorY->isChecked()) {
                ang = 180 - ang;
            }

            if (tab->mirrorX->isChecked()) {
                ang = 360 - ang;
            }

            dir = ang / 360;

            if (ang >= 360) {
                ang = ang - (dir * 360);
            }

            if (ang < 0) {
                ang = ang + 360;
            }

            x = xCenter + (dia * cos((M_PI / 180) * ang));
            y = yCenter + (dia * sin((M_PI / 180) * ang));

            QTableWidgetItem *xItem = new QTableWidgetItem(Utils::removeZeros(QString("%1").arg(x, 0, 'f', 3)));
            xItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QTableWidgetItem *yItem = new QTableWidgetItem(Utils::removeZeros(QString("%1").arg(y, 0, 'f', 3)));
            yItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QTableWidgetItem *hdr = new QTableWidgetItem(Utils::removeZeros(QString("%1 - %2 ").arg(i + 1).arg(ang,
                    0, 'f', 3)));
            tab->resultTable->setVerticalHeaderItem(i, hdr);
            tab->resultTable->setItem(i, 0, xItem);
            tab->resultTable->setItem(i, 1, yItem);
        }
    }

    drawing = (BHCDraw *) findChild<BHCDraw *>();

    if (!drawing) {
        drawing = new BHCDraw(this);
        drawing->init(parentWidth - this->width(), parentHeight - this->y());
        drawing->move((this->x()) - drawing->width(), this->y());
    }

    drawing->setScale(maxDia);

    drawing->show();
    drawing->setUpdatesEnabled(false);
    drawing->clear();

    for (tabId = 0; tabId <= tabBar->count(); tabId++) {
        tab = (BHCTab *)tabBar->widget(tabId);

        if (tab == 0) {
            continue;
        }

        if (!tab->xCenterInput->hasAcceptableInput()) {
            tab->xCenterInput->setText("0");
            xCenter = 0;
        } else {
            xCenter = QLocale().toDouble(tab->xCenterInput->text(), &ok);
        }

        if (!tab->yCenterInput->hasAcceptableInput()) {
            tab->yCenterInput->setText("0");
            yCenter = 0;
        } else {
            yCenter = QLocale().toDouble(tab->yCenterInput->text(), &ok);
        }

        if (!tab->diaInput->hasAcceptableInput()) {
            tab->diaInput->setText("0");
            tab->resultTable->setRowCount(0);
            continue;
        } else {
            dia = QLocale().toDouble(tab->diaInput->text(), &ok);
        }

        if (tab->angleStartInput->hasAcceptableInput()) {
            firstAngle = QLocale().toDouble(tab->angleStartInput->text(), &ok);
        } else {
            tab->angleStartInput->setText("0");
            firstAngle = 0;
        }

        if (tab->angleBeetwenInput->hasAcceptableInput()) {
            angleBeetwen = QLocale().toDouble(tab->angleBeetwenInput->text(), &ok);
        } else {
            tab->angleBeetwenInput->setText("0");
            angleBeetwen = 0;
        }

        if (tab->holesInput->hasAcceptableInput()) {
            holeCount = tab->holesInput->text().toInt(&ok);
        } else {
            tab->holesInput->setText("0");
            holeCount = 0;
        }

        roate = tab->roateInput->text().toInt(&ok);

        if ((holeCount == 0) && (angleBeetwen == 0)) {
            tab->resultTable->setRowCount(0);
            continue;
        }

        roate = roate / 10;

        if (holeCount <= 0) {
            holeCount = 360 / angleBeetwen;
        }

        if (angleBeetwen <= 0) {
            angleBeetwen = double(360 / double(holeCount));
        }

        switch (tabId) {
        case 1:
            col = Qt::blue;
            textPosX = 8;
            textPosY = drawing->height();
            dir = 5;
            break;

        case 2:
            col = Qt::red;
            textPosX = (drawing->width() / 2) + drawing->width() / 4;
            textPosY = 0;
            dir = -1;
            break;

        case 3:
            col = Qt::yellow;
            textPosX = (drawing->width() / 2) + drawing->width() / 4;
            textPosY = drawing->height();
            dir = 5;
            break;

        default:
            col = Qt::green;
            textPosX = 8;
            textPosY = 0;
            dir = -1;
        }

        drawing->printText(textPosX, textPosY, 0 - dir, QString(tr("Diameter : %1")).arg(dia), col);
        drawing->printText(textPosX, textPosY, 1 - dir,
                           QString(tr("Number of holes : %1")).arg(holeCount), col);
        drawing->printText(textPosX, textPosY, 2 - dir,
                           QString(tr("Angle of first hole : %1")).arg(firstAngle), col);
        drawing->printText(textPosX, textPosY, 3 - dir,
                           QString(tr("Angle beetwen holes : %1")).arg(angleBeetwen), col);
        drawing->printText(textPosX, textPosY, 4 - dir,
                           QString(tr("Center position : X%1 Y%2")).arg(xCenter).arg(yCenter), col);
        firstAngle += roate;

        tab->resultTable->setRowCount(holeCount);

        dia = dia / 2;

        if (tab->mirrorY->isChecked()) {
            xCenter = -xCenter;
        }

        if (tab->mirrorX->isChecked()) {
            yCenter = -yCenter;
        }

        for (i = 0; i < holeCount; i++) {
            ang = firstAngle + (angleBeetwen * i);

            if (tab->mirrorY->isChecked()) {
                ang = 180 - ang;
            }

            if (tab->mirrorX->isChecked()) {
                ang = 360 - ang;
            }

            dir = ang / 360;

            if (ang >= 360) {
                ang = ang - (dir * 360);
            }

            if (ang < 0) {
                ang = ang + 360;
            }

            drawing->drawHole(ang, dia, xCenter, yCenter, (i == 0), (i == (holeCount - 1)), col);
        }
    }

    drawing->setUpdatesEnabled(true);
    drawing->update();
}

void BHCDialog::setOptions(const BHCOptions &options)
{
    BHCTab *tab;
    tab = (BHCTab *)tabBar->widget(0); // green
    tab->setOptions(options.green);
    tab = (BHCTab *)tabBar->widget(1); // blue
    tab->setOptions(options.blue);
    tab = (BHCTab *)tabBar->widget(2); // red
    tab->setOptions(options.red);
    tab = (BHCTab *)tabBar->widget(3); // yellow
    tab->setOptions(options.yellow);
}

BHCOptions BHCDialog::options()
{
    BHCOptions options;
    BHCTab *tab;

    tab = (BHCTab *)tabBar->widget(0); // green
    options.green = tab->options();;
    tab = (BHCTab *)tabBar->widget(1); // blue
    options.blue = tab->options();;
    tab = (BHCTab *)tabBar->widget(2); // red
    options.red = tab->options();;
    tab = (BHCTab *)tabBar->widget(3); // yellow
    options.yellow = tab->options();

    return options;
}
