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

#include <cmath>    // for sin, M_PI, asin, acos, atan, cos

// IWYU pragma: no_forward_declare QValidator
// IWYU pragma: no_forward_declare QWidget
#include <QApplication>     // for QApplication
#include <QChar>            // for operator==, QChar
#include <QCheckBox>        // for QCheckBox
#include <QDoubleValidator> // for QDoubleValidator
#include <QEvent>           // for QEvent, QEvent::KeyPress
#include <QKeyEvent>        // for QKeyEvent
#include <QLabel>           // for QLabel
#include <QLineEdit>        // for QLineEdit
#include <QLocale>          // for QLocale
#include <QPalette>         // for QPalette
#include <QPixmap>          // for QPixmap
#include <QPushButton>      // for QPushButton
#include <QSettings>        // for QSettings
#include <QValidator>       // for QValidator
#include <QWidget>          // for QWidget
#include <Qt>               // for Key_Comma, Key_Period, NoModifier, blue, WA_DeleteOnClose, black, red

#include "triangledialog.h"


TriangleDialog::TriangleDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent)
{
    setupUi(this);

    mSettings = settings;

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Solution of triangles"));

    pic1 = new QPixmap(":/images/trig.png");
    pic2 = new QPixmap(":/images/trig1.png");

    picLabel->setPixmap(*pic1);
    picLabel->adjustSize();

    connect(rightTriangleCheckBox, SIGNAL(toggled(bool)), SLOT(rightTriangleCheckBoxToggled()));
    connect(aCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(bCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(cCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(aACheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(aBCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(aCCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));

    QValidator *aInputValid = new QDoubleValidator(0.001, 9999, 3, this);
    aInput->setValidator(aInputValid);
    aInput->installEventFilter(this);

    QValidator *bInputValid = new QDoubleValidator(0.001, 9999, 3, this);
    bInput->setValidator(bInputValid);
    bInput->installEventFilter(this);

    QValidator *cInputValid = new QDoubleValidator(0.001, 9999, 3, this);
    cInput->setValidator(cInputValid);
    cInput->installEventFilter(this);

    aAInputValid = new QDoubleValidator(0.001, 179.999, 3, this);
    aAInput->setValidator(aAInputValid);
    aAInput->installEventFilter(this);

    aBInputValid = new QDoubleValidator(0.001, 179.999, 3, this);
    aBInput->setValidator(aBInputValid);
    aBInput->installEventFilter(this);

    aCInputValid = new QDoubleValidator(0.001, 179.999, 3, this);
    aCInput->setValidator(aCInputValid);
    aCInput->installEventFilter(this);

    setMaximumSize(width(), height());

    connect(aInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(bInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(cInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(aAInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(aBInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(aCInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));

    rightTriangleCheckBoxToggled();
    checkBoxToggled();
}

TriangleDialog::~TriangleDialog()
{
}

bool TriangleDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if (!qobject_cast<QLineEdit *>(obj)) {
        // pass the event on to the parent class
        return QDialog::eventFilter(obj, ev);
    }

    if (ev->type() == QEvent::KeyPress) {
        return false;
    }

    QKeyEvent *k = (QKeyEvent *) ev;

    if (QLocale().decimalPoint() == '.' && k->key() == Qt::Key_Comma) {
        QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier,
                                ".", false, 1));
        return true;
    }

    if (QLocale().decimalPoint() == ',' && k->key() == Qt::Key_Period) {
        QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier, ",",
                                false, 1));
        return true;
    }

    return false;
}

void TriangleDialog::inputChanged()
{
    QPalette paletteRed, paletteBlue;

    paletteRed.setColor(aInput->foregroundRole(), Qt::red);
    paletteBlue.setColor(aInput->foregroundRole(), Qt::blue);

    if (aInput->hasAcceptableInput()) {
        if (aInput->isReadOnly()) {
            aInput->setPalette(paletteBlue);
        } else {
            aInput->setPalette(QPalette());
        }
    } else {
        aInput->setPalette(paletteRed);
    }

    if (bInput->hasAcceptableInput()) {
        if (bInput->isReadOnly()) {
            bInput->setPalette(paletteBlue);
        } else {
            bInput->setPalette(QPalette());
        }
    } else {
        bInput->setPalette(paletteRed);
    }

    if (cInput->hasAcceptableInput()) {
        if (cInput->isReadOnly()) {
            cInput->setPalette(paletteBlue);
        } else {
            cInput->setPalette(QPalette());
        }
    } else {
        cInput->setPalette(paletteRed);
    }

    if (aAInput->hasAcceptableInput()) {
        if (aAInput->isReadOnly()) {
            aAInput->setPalette(paletteBlue);
        } else {
            aAInput->setPalette(QPalette());
        }
    } else {
        aAInput->setPalette(paletteRed);
    }

    if (aBInput->hasAcceptableInput()) {
        if (aBInput->isReadOnly()) {
            aBInput->setPalette(paletteBlue);
        } else {
            aBInput->setPalette(QPalette());
        }
    } else {
        aBInput->setPalette(paletteRed);
    }

    if (aCInput->hasAcceptableInput()) {
        if (aCInput->isReadOnly()) {
            aCInput->setPalette(paletteBlue);
        } else {
            aCInput->setPalette(QPalette());
        }
    } else {
        aCInput->setPalette(paletteRed);
    }
}

void TriangleDialog::computeButtonClicked()
{
    double angle1, angle2, angle3;
    bool ok;

    angle1 = QLocale().toDouble(aAInput->text(), &ok);
    angle2 = QLocale().toDouble(aBInput->text(), &ok);
    angle3 = QLocale().toDouble(aCInput->text(), &ok);

    if (!aCheckBox->isChecked()) {
        aInput->setText("--");
    }

    if (!bCheckBox->isChecked()) {
        bInput->setText("--");
    }

    if (!cCheckBox->isChecked()) {
        cInput->setText("--");
    }

    if (!aACheckBox->isChecked()) {
        aAInput->setText("--");
    }

    if (!aBCheckBox->isChecked()) {
        aBInput->setText("--");
    }

    if (!aCCheckBox->isChecked()) {
        aCInput->setText("--");
    }

    if ((angle1 + angle2 + angle3) > 180) {
        return;
    }

    if (option1() == 0) {
        return;
    }

    if (option2() == 0) {
        return;
    }

    if (option3() == 0) {
        return;
    }

    option4();
}

int TriangleDialog::option1()  //any one side and two angles known
{
    double side1, side2, side3, angle1, angle2, angle3;
    bool ok;

    switch (mode) {
    case 0x31:
        side1 = QLocale().toDouble(aInput->text(), &ok);
        angle1 = QLocale().toDouble(aAInput->text(), &ok);
        angle2 = QLocale().toDouble(aBInput->text(), &ok);
        angle3 = 180 - (angle1 + angle2);
        break;

    case 0x51:
        side1 = QLocale().toDouble(aInput->text(), &ok);
        angle1 = QLocale().toDouble(aAInput->text(), &ok);
        angle2 = QLocale().toDouble(aCInput->text(), &ok);
        angle3 = 180 - (angle1 + angle2);
        break;

    case 0x61:
        side1 = QLocale().toDouble(aInput->text(), &ok);
        angle3 = QLocale().toDouble(aBInput->text(), &ok);
        angle2 = QLocale().toDouble(aCInput->text(), &ok);
        angle1 = 180 - (angle2 + angle3);
        break;

    case 0x32:
        side1 = QLocale().toDouble(bInput->text(), &ok);
        angle1 = QLocale().toDouble(aBInput->text(), &ok);
        angle2 = QLocale().toDouble(aAInput->text(), &ok);
        angle3 = 180 - (angle1 + angle2);
        break;

    case 0x52:
        side1 = QLocale().toDouble(bInput->text(), &ok);
        angle3 = QLocale().toDouble(aAInput->text(), &ok);
        angle2 = QLocale().toDouble(aCInput->text(), &ok);
        angle1 = 180 - (angle2 + angle3);
        break;

    case 0x62:
        side1 = QLocale().toDouble(bInput->text(), &ok);
        angle1 = QLocale().toDouble(aBInput->text(), &ok);
        angle2 = QLocale().toDouble(aCInput->text(), &ok);
        angle3 = 180 - (angle1 + angle2);
        break;


    case 0x34:
        side1 = QLocale().toDouble(cInput->text(), &ok);
        angle3 = QLocale().toDouble(aAInput->text(), &ok);
        angle2 = QLocale().toDouble(aBInput->text(), &ok);
        angle1 = 180 - (angle2 + angle3);
        break;

    case 0x54:
        side1 = QLocale().toDouble(cInput->text(), &ok);
        angle1 = QLocale().toDouble(aCInput->text(), &ok);
        angle2 = QLocale().toDouble(aAInput->text(), &ok);
        angle3 = 180 - (angle1 + angle2);
        break;

    case 0x64:
        side1 = QLocale().toDouble(cInput->text(), &ok);
        angle1 = QLocale().toDouble(aCInput->text(), &ok);
        angle2 = QLocale().toDouble(aBInput->text(), &ok);
        angle3 = 180 - (angle1 + angle2);
        break;

    default:
        return (-1);
    }

    side2 = (side1 * sin((M_PI / 180) * angle2)) / sin((M_PI / 180) * angle1);
    side3 = (side1 * sin((M_PI / 180) * angle3)) / sin((M_PI / 180) * angle1);

    switch (mode) {
    case 0x31:
        bInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x51:
        bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x61:
        bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
        break;

    case 0x32:
        aInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x52:
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
        break;

    case 0x62:
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x34:
        bInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
        break;

    case 0x54 :
        bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x64:
        bInput->setText(QString("%1").arg(side2, 0, 'f', 3));
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;
    }

    return (0);
}

int TriangleDialog::option2()  // two sides and the included angle
{
    double side1, side2, side3, angle1, angle2, angle3;
    bool ok;

    switch (mode) {
    case 0x43:
        side1 = QLocale().toDouble(aInput->text(), &ok);
        side2 = QLocale().toDouble(bInput->text(), &ok);
        angle1 = QLocale().toDouble(aCInput->text(), &ok);
        break;

    case 0x25:
        side2 = QLocale().toDouble(aInput->text(), &ok);
        side1 = QLocale().toDouble(cInput->text(), &ok);
        angle1 = QLocale().toDouble(aBInput->text(), &ok);
        break;

    case 0x16:
        side1 = QLocale().toDouble(bInput->text(), &ok);
        side2 = QLocale().toDouble(cInput->text(), &ok);
        angle1 = QLocale().toDouble(aAInput->text(), &ok);
        break;

    default:
        return (-1);
    }

    angle2 = (side1 * sin((M_PI / 180) * angle1)) / (side2 - side1 * cos((M_PI / 180) * angle1));
    angle2 = atan(angle2) / (M_PI / 180);
    side3 = (side1 * sin((M_PI / 180) * angle1)) / sin((M_PI / 180) * angle2);
    angle3 = 180 - (angle1 + angle2);

    switch (mode) {
    case 0x43:
        cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x25:
        bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        break;

    case 0x16:
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;
    }

    return (0);
}

int TriangleDialog::option3()  // two sides and the opposite angle
{
    double side1, side2, side3, angle1, angle2, angle3;
    bool ok;

    switch (mode) {
    case 0x13:
        side1 = QLocale().toDouble(aInput->text(), &ok);
        side2 = QLocale().toDouble(bInput->text(), &ok);
        angle1 = QLocale().toDouble(aAInput->text(), &ok);
        break;

    case 0x15:
        side1 = QLocale().toDouble(aInput->text(), &ok);
        side2 = QLocale().toDouble(cInput->text(), &ok);
        angle1 = QLocale().toDouble(aAInput->text(), &ok);
        break;

    case 0x46:
        side1 = QLocale().toDouble(cInput->text(), &ok);
        side2 = QLocale().toDouble(bInput->text(), &ok);
        angle1 = QLocale().toDouble(aCInput->text(), &ok);
        break;

    case 0x45:
        side1 = QLocale().toDouble(cInput->text(), &ok);
        side2 = QLocale().toDouble(aInput->text(), &ok);
        angle1 = QLocale().toDouble(aCInput->text(), &ok);
        break;

    case 0x23:
        side1 = QLocale().toDouble(bInput->text(), &ok);
        side2 = QLocale().toDouble(aInput->text(), &ok);
        angle1 = QLocale().toDouble(aBInput->text(), &ok);
        break;

    case 0x26:
        side1 = QLocale().toDouble(bInput->text(), &ok);
        side2 = QLocale().toDouble(cInput->text(), &ok);
        angle1 = QLocale().toDouble(aBInput->text(), &ok);
        break;

    default:
        return (-1);
    }

    angle2 = (side2 * sin((M_PI / 180) * angle1)) / side1;
    angle2 = asin(angle2) / (M_PI / 180);
    angle3 = 180 - (angle1 + angle2);
    side3 = (side1 * sin((M_PI / 180) * angle3)) / sin((M_PI / 180) * angle1);

    switch (mode) {
    case 0x13:
        cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x15:
        bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        break;

    case 0x46:
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;

    case 0x45:
        bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        break;

    case 0x23:
        cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        break;

    case 0x26:
        aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
        aCInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
        aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
        break;
    }

    return (0);
}

void TriangleDialog::option4()  // all sides given
{
    double side1, side2, side3, angle1, angle2, angle3;
    bool ok;

    if (mode != 0x07) {
        return;
    }

    side1 = QLocale().toDouble(aInput->text(), &ok);
    side2 = QLocale().toDouble(bInput->text(), &ok);
    side3 = QLocale().toDouble(cInput->text(), &ok);

    angle1 = ((side2 * side2) + (side3 * side3) - (side1 * side1)) / (2 * side2 * side3);
    angle1 = acos(angle1) / (M_PI / 180);

    angle2 = (side2 * sin((M_PI / 180) * angle1)) / side1;

    angle2 = asin(angle2) / (M_PI / 180);
    angle3 = 180 - (angle1 + angle2);

    aAInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
    aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
    aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
}

void TriangleDialog::rightTriangleCheckBoxToggled()
{
    if (rightTriangleCheckBox->isChecked()) {
        aACheckBox->setChecked(true);
        aAInput->setText("90");
        aACheckBox->setEnabled(false);
        picLabel->setPixmap(*pic2);
    } else {
        aACheckBox->setChecked(false);
        aACheckBox->setEnabled(true);
        picLabel->setPixmap(*pic1);
    }
}

void TriangleDialog::checkBoxToggled()
{
    QPalette palette;

    mode = 0;

    if (aCheckBox->isChecked()) {
        mode |= 0x01;
    }

    if (bCheckBox->isChecked()) {
        mode |= 0x02;
    }

    if (cCheckBox->isChecked()) {
        mode |= 0x04;
    }

    if (aACheckBox->isChecked()) {
        mode |= 0x10;
    }

    if (aBCheckBox->isChecked()) {
        mode |= 0x20;
    }

    if (aCCheckBox->isChecked()) {
        mode |= 0x40;
    }

    palette.setColor(aInput->foregroundRole(), Qt::black);

    if ((mode == 0x31) || (mode == 0x32) || (mode == 0x34) || (mode == 0x51) || (mode == 0x52) ||
            (mode == 0x54) || (mode == 0x61) || (mode == 0x62) || (mode == 0x64) || (mode == 0x07) ||
            (mode == 0x43) || (mode == 0x25) || (mode == 0x16) || (mode == 0x13) || (mode == 0x15) ||
            (mode == 0x23) || (mode == 0x26) || (mode == 0x45) || (mode == 0x46)) {

        computeButton->setEnabled(true);

        if (!(mode & 0x01)) {
            aCheckBox->setEnabled(false);
        } else {
            aInput->setReadOnly(false);
            aInput->setPalette(palette);
        }

        if (!(mode & 0x02)) {
            bCheckBox->setEnabled(false);
        } else {
            bInput->setReadOnly(false);
            bInput->setPalette(palette);
        }

        if (!(mode & 0x04)) {
            cCheckBox->setEnabled(false);
        } else {
            cInput->setReadOnly(false);
            cInput->setPalette(palette);
        }

        if (!(mode & 0x10)) {
            aACheckBox->setEnabled(false);
        } else {
            if (!rightTriangleCheckBox->isChecked()) {
                aAInput->setReadOnly(false);
                aAInput->setPalette(palette);
            }
        }

        if (!(mode & 0x20)) {
            aBCheckBox->setEnabled(false);
        } else {
            aBInput->setReadOnly(false);
            aBInput->setPalette(palette);
        }

        if (!(mode & 0x40)) {
            aCCheckBox->setEnabled(false);
        } else {
            aCInput->setReadOnly(false);
            aCInput->setPalette(palette);
        }
    } else {
        aCheckBox->setEnabled(true);
        aInput->setReadOnly(true);
        bCheckBox->setEnabled(true);
        bInput->setReadOnly(true);
        cCheckBox->setEnabled(true);
        cInput->setReadOnly(true);

        if (!rightTriangleCheckBox->isChecked()) {
            aACheckBox->setEnabled(true);
            aAInput->setReadOnly(true);
        }

        aBCheckBox->setEnabled(true);
        aBInput->setReadOnly(true);
        aCCheckBox->setEnabled(true);
        aCInput->setReadOnly(true);

        palette.setColor(aInput->foregroundRole(), Qt::blue);
        aInput->setPalette(palette);
        bInput->setPalette(palette);
        cInput->setPalette(palette);
        aAInput->setPalette(palette);
        aBInput->setPalette(palette);
        aCInput->setPalette(palette);

        computeButton->setEnabled(false);
    }
}
