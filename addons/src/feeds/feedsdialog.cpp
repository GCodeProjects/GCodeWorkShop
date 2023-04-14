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

#include <cmath> // for round, M_PI

// IWYU pragma: no_forward_declare QValidator
// IWYU pragma: no_forward_declare QWidget
#include <QApplication>     // for QApplication
#include <QChar>            // for operator==, QChar
#include <QCheckBox>        // for QCheckBox
#include <QDoubleValidator> // for QDoubleValidator
#include <QEvent>           // for QEvent, QEvent::KeyPress
#include <QIntValidator>    // for QIntValidator
#include <QKeyEvent>        // for QKeyEvent
#include <QLineEdit>        // for QLineEdit
#include <QLocale>          // for QLocale
#include <QPalette>         // for QPalette
#include <QPoint>           // for QPoint
#include <QPushButton>      // for QPushButton
#include <QRect>            // for QRect
#include <QSettings>        // for QSettings
#include <QSize>            // for QSize
#include <QString>          // for QString
#include <QValidator>       // for QValidator
#include <QVariant>         // for QVariant
#include <QWidget>          // for QWidget
#include <Qt>               // for Key_Comma, Key_Period, NoModifier, WA_DeleteOnClose, red

#include "feedsdialog.h"
#include "feedsoptions.h"   // for FeedsOptions


#define CFG_SECTION  "FeedsDialog"
#define CFG_KEY_POS  "Position"
#define CFG_KEY_SIZE "Size"


FeedsDialog::FeedsDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent)
{
    setupUi(this);

    mSettings = settings;

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Cutting parameters"));

    QValidator *vcInputValid = new QIntValidator(1, 9999, this);
    vcInput->setValidator(vcInputValid);

    QValidator *fzInputValid = new QDoubleValidator(0.0001, 999, 4, this);
    fzInput->setValidator(fzInputValid);
    fzInput->installEventFilter(this);

    QValidator *dInputValid = new QDoubleValidator(0.01, 9000, 4, this);
    dInput->setValidator(dInputValid);
    dInput->installEventFilter(this);

    QValidator *zInputValid = new QIntValidator(1, 500, this);
    zInput->setValidator(zInputValid);

    QValidator *sInputValid = new QIntValidator(1, 99999, this);
    sInput->setValidator(sInputValid);

    QValidator *fInputValid = new QDoubleValidator(0.01, 99999, 4, this);
    fInput->setValidator(fInputValid);
    fInput->installEventFilter(this);

    connect(vcInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
    connect(fzInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
    connect(sInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
    connect(fInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
    connect(dInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
    connect(zInput, SIGNAL(editingFinished()), SLOT(setDefButton()));

    connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));
    connect(computeVcButton, SIGNAL(clicked()), SLOT(computeVcButtonClicked()));

    inputChanged();
    checkBoxChanged();

    setMaximumSize(width(), height());

    connect(vcInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(fzInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(dInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(zInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(sInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(fInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));

    connect(mmCheckBox, SIGNAL(stateChanged(int)), SLOT(checkBoxChanged()));
    connect(inchCheckBox, SIGNAL(stateChanged(int)), SLOT(checkBoxChanged()));
}

FeedsDialog::~FeedsDialog()
{
}

bool FeedsDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if (!qobject_cast<QLineEdit *>(obj)) {
        // pass the event on to the parent class
        return QDialog::eventFilter(obj, ev);
    }

    if (ev->type() != QEvent::KeyPress) {
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

void FeedsDialog::setDefButton()
{
    if (fInput->hasFocus() || sInput->hasFocus()) {
        computeButton->setDefault(false);
        computeVcButton->setDefault(true);
    } else if (vcInput->hasFocus() || fzInput->hasFocus()) {
        computeVcButton->setDefault(false);
        computeButton->setDefault(true);
    }
}

void FeedsDialog::checkBoxChanged()
{
    if (mmCheckBox->isChecked()) {
        vcInput->setToolTip(tr("Cutting speed (m/min)"));
        fzInput->setToolTip(tr("Feed (chip load) per tooth (mm)"));
        dInput->setToolTip(tr("Tool diameter (mm)"));
        zInput->setToolTip(tr("Number of teeth on the cutter"));
        fInput->setToolTip(tr("Table feed (mm/min)"));
        sInput->setToolTip(tr("Spindle speed (revs/min)"));
        fInput->setMaxLength(9);
        fzInput->setMaxLength(8);
    } else if (inchCheckBox->isChecked()) {
        vcInput->setToolTip(tr("Cutting speed (ft/min)"));
        fzInput->setToolTip(tr("Feed (chip load) per tooth (inches)"));
        dInput->setToolTip(tr("Tool diameter (inches)"));
        zInput->setToolTip(tr("Number of teeth on the cutter"));
        fInput->setToolTip(tr("Table feed (inches/min)"));
        sInput->setToolTip(tr("Spindle speed (revs/min)"));
        fInput->setMaxLength(10);
        fzInput->setMaxLength(9);
    }
}

void FeedsDialog::computeButtonClicked()
{
    double Vc, Fz, D, z, S, F;
    bool ok;

    Vc = QLocale().toDouble(vcInput->text(), &ok);
    Fz = QLocale().toDouble(fzInput->text(), &ok);
    D = QLocale().toDouble(dInput->text(), &ok);
    z = QLocale().toDouble(zInput->text(), &ok);

    if (mmCheckBox->isChecked()) {
        S = (Vc * 1000) / (M_PI * D);
        F = S * (Fz * z);

        sInput->setText(QString("%1").arg(round(S)));
        fInput->setText(QString("%1").arg(F, 0, 'f', 3));
    }

    if (inchCheckBox->isChecked()) {
        S = (Vc * 12) / (M_PI * D);
        F = S * (Fz * z);

        sInput->setText(QString("%1").arg(round(S)));
        fInput->setText(QString("%1").arg(F, 0, 'f', 4));
    }
}

void FeedsDialog::computeVcButtonClicked()
{
    double Vc, Fz, D, z, S, F;
    bool ok;

    S = sInput->text().toInt(&ok);
    F = QLocale().toDouble(fInput->text(), &ok);
    D = QLocale().toDouble(dInput->text(), &ok);
    z = QLocale().toDouble(zInput->text(), &ok);

    if (mmCheckBox->isChecked()) {
        Vc = (M_PI * D * S) / 1000;
        Fz = (F / S) / z;

        vcInput->setText(QString("%1").arg(round(round(Vc))));
        fzInput->setText(QString("%1").arg(Fz, 0, 'f', 3));
    }

    if (inchCheckBox->isChecked()) {
        Vc = (M_PI * D * S) / 12;
        Fz = (F / S) / z;

        vcInput->setText(QString("%1").arg(round(round(Vc))));
        fzInput->setText(QString("%1").arg(Fz, 0, 'f', 4));
    }
}

void FeedsDialog::inputChanged()
{
    bool ena, ena1;
    QPalette palette;

    palette.setColor(vcInput->foregroundRole(), Qt::red);

    if (vcInput->hasAcceptableInput()) {
        vcInput->setPalette(QPalette());
    } else {
        vcInput->setPalette(palette);
    }

    if (fzInput->hasAcceptableInput()) {
        fzInput->setPalette(QPalette());
    } else {
        fzInput->setPalette(palette);
    }

    if (zInput->hasAcceptableInput()) {
        zInput->setPalette(QPalette());
    } else {
        zInput->setPalette(palette);
    }

    if (dInput->hasAcceptableInput()) {
        dInput->setPalette(QPalette());
    } else {
        dInput->setPalette(palette);
    }

    if (sInput->hasAcceptableInput()) {
        sInput->setPalette(QPalette());
    } else {
        sInput->setPalette(palette);
    }

    if (fInput->hasAcceptableInput()) {
        fInput->setPalette(QPalette());
    } else {
        fInput->setPalette(palette);
    }

    ena = vcInput->hasAcceptableInput() && fzInput->hasAcceptableInput()
          && zInput->hasAcceptableInput() && dInput->hasAcceptableInput();
    ena1 = sInput->hasAcceptableInput() && fInput->hasAcceptableInput()
           && zInput->hasAcceptableInput() && dInput->hasAcceptableInput();

    computeButton->setEnabled(ena);
    computeVcButton->setEnabled(ena1);
}

void FeedsDialog::setOptions(const FeedsOptions &options)
{
    inchCheckBox->setChecked(options.useInch);
    mmCheckBox->setChecked(!options.useInch);

    if (options.Vc.in) {
        vcInput->setText(QString::number(options.Vc.value));
    } else {
        vcInput->clear();
    }

    if (options.z.in) {
        zInput->setText(QString::number(options.z.value));
    } else {
        zInput->clear();
    }

    if (options.Fz.in) {
        fzInput->setText(QString::number(options.Fz.value));
    } else {
        fzInput->clear();
    }

    if (options.diam.in) {
        dInput->setText(QString::number(options.diam.value));
    } else {
        dInput->clear();
    }

    if (options.speed.in) {
        sInput->setText(QString::number(options.speed.value));
    } else {
        sInput->clear();
    }

    if (options.feed.in) {
        fInput->setText(QString::number(options.feed.value));
    } else {
        fInput->clear();
    }

    inputChanged();
    checkBoxChanged();
}

FeedsOptions FeedsDialog::options()
{
    FeedsOptions options;

    options.useInch = inchCheckBox->isChecked();
    options.Vc.value = vcInput->text().toDouble(&options.Vc.in);
    options.z.value = zInput->text().toDouble(&options.z.in);
    options.Fz.value = fzInput->text().toDouble(&options.Fz.in);
    options.diam.value = dInput->text().toDouble(&options.diam.in);
    options.speed.value = sInput->text().toInt(&options.speed.in);
    options.feed.value = fInput->text().toDouble(&options.feed.in);

    return options;
}

void FeedsDialog::loadSettings(const FeedsOptions &defaultOptions)
{
    if (mSettings.isNull()) {
        return;
    }

    mSettings->beginGroup(CFG_SECTION);

    QPoint pos = mSettings->value(CFG_KEY_POS, geometry().topLeft()).toPoint();
    QSize size = mSettings->value(CFG_KEY_SIZE, geometry().size()).toSize();
    setGeometry(QRect(pos, size));

    FeedsOptions opt;
    opt.load(mSettings, defaultOptions);

    mSettings->endGroup();

    setOptions(opt);
}

void FeedsDialog::saveSettings(bool saveOptions)
{
    if (mSettings.isNull()) {
        return;
    }

    mSettings->beginGroup(CFG_SECTION);

    mSettings->setValue(CFG_KEY_POS, geometry().topLeft());
    mSettings->setValue(CFG_KEY_SIZE, geometry().size());

    if (saveOptions) {
        options().save(mSettings);
    }

    mSettings->endGroup();
}
