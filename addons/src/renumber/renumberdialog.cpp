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

// IWYU pragma: no_forward_declare QWidget
#include <QCheckBox>    // for QCheckBox
#include <QPushButton>  // for QPushButton
#include <QRadioButton> // for QRadioButton
#include <QSettings>    // for QSettings
#include <QSpinBox>     // for QSpinBox
#include <QVariant>     // for QVariant
#include <QWidget>      // for QWidget


#include <utils/medium.h>   // for Medium

#include "renumberdialog.h"


RenumberDialog::RenumberDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);

    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Renumber lines"));

    connect(mRenumLines, SIGNAL(clicked()), this, SLOT(renumClicked()));
    connect(mAllLines, SIGNAL(clicked()), this, SLOT(allLinesClicked()));
    connect(mRemoveAll, SIGNAL(clicked()), this, SLOT(removeAllClicked()));
    connect(mCheckDivide, SIGNAL(clicked()), this, SLOT(divideClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(okButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
    connect(mRenumHe, SIGNAL(clicked()), this, SLOT(mRenumHeClicked()));

    setMaximumSize(width(), height());
    //okButton->setEnabled(formInput->hasAcceptableInput());

    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("RenumberDialog");

    startAtInput->setValue(settings.value("StartAt", 10).toInt());
    formInput->setValue(settings.value("From", 10).toInt());
    incInput->setValue(settings.value("Inc", 10).toInt());
    toInput->setValue(settings.value("To", 8000).toInt());
    mSpinBox->setValue(settings.value("Prec", 4).toInt());

    mRenumEmpty->setChecked(settings.value("RenumEmpty", false).toBool());
    mRenumWithComm->setChecked(settings.value("RenumWithComm", false).toBool());
    mRenumMarked->setChecked(settings.value("RenumMarked", true).toBool());
    mCheckDivide->setChecked(settings.value("CheckDivide", false).toBool());

    if (mCheckDivide->isChecked()) {
        divideClicked();
    }

    int mode = settings.value("Mode", 1).toInt();

    switch (mode) {
    case 1:
        mRenumLines->setChecked(true);
        renumClicked();
        break;

    case 2:
        mAllLines->setChecked(true);
        allLinesClicked();
        break;

    case 3:
        mRemoveAll->setChecked(true);
        removeAllClicked();
        break;

    case 4:
        mRenumHe->setChecked(true);
        mRenumHeClicked();
        break;
    }

    settings.endGroup();
}

RenumberDialog::~RenumberDialog()
{
}

void RenumberDialog::okButtonClicked()
{
    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("RenumberDialog");

    settings.setValue("StartAt", startAtInput->value());
    settings.setValue("From", formInput->value());
    settings.setValue("Inc", incInput->value());
    settings.setValue("To", toInput->value());
    settings.setValue("Prec", mSpinBox->value());

    settings.setValue("RenumEmpty", mRenumEmpty->isChecked());
    settings.setValue("RenumWithComm", mRenumWithComm->isChecked());
    settings.setValue("RenumMarked", mRenumMarked->isChecked());
    settings.setValue("CheckDivide", mCheckDivide->isChecked());

    int mode = 1;

    if (mAllLines->isChecked()) {
        mode = 2;
    } else if (mRemoveAll->isChecked()) {
        mode = 3;
    } else if (mRenumHe->isChecked()) {
        mode = 4;
    }

    settings.setValue("Mode", mode);

    settings.endGroup();

    accept();
}

void RenumberDialog::getState(int &mode, int &startAt, int &from, int &prec, int &inc, int &to,
                              bool &renumEmpty, bool &renumComm, bool &renumMarked)
{
    startAt = startAtInput->value();
    from = formInput->value();
    inc = incInput->value();
    renumEmpty = mRenumEmpty->isChecked();
    renumComm = !mRenumWithComm->isChecked();
    renumMarked = mRenumMarked->isChecked();

    to = toInput->value();

    if (mCheckDivide->isChecked()) {
        prec = mSpinBox->value();
    } else {
        prec = 0;
    }

    mode = 1;

    if (mAllLines->isChecked()) {
        mode = 2;
    } else if (mRemoveAll->isChecked()) {
        mode = 3;
    } else if (mRenumHe->isChecked()) {
        mode = 4;
    }
}

void RenumberDialog::atEndClicked()
{
    mCheckDivide->setChecked(!mRemoveAll->isChecked());
    mSpinBox->setEnabled(mCheckDivide->isChecked());
}

void RenumberDialog::divideClicked()
{
    mSpinBox->setEnabled(mCheckDivide->isChecked());
}

void RenumberDialog::renumClicked()
{
    formInput->setEnabled(true);
    startAtInput->setEnabled(true);
    incInput->setEnabled(true);
    toInput->setEnabled(true);

    //startAtInput->setValue(10);
    //incInput->setValue(10);
    //toInput->setValue(8000);

    mRenumLines->setChecked(true);
    mAllLines->setChecked(false);
    mRemoveAll->setChecked(false);

    mRenumWithComm->setEnabled(false);
    mRenumEmpty->setEnabled(false);

    mCheckDivide->setEnabled(true);
}

void RenumberDialog::mRenumHeClicked()
{
    formInput->setEnabled(false);
    startAtInput->setEnabled(true);
    incInput->setEnabled(true);
    toInput->setEnabled(false);

    //startAtInput->setValue(0);
    //incInput->setValue(1);

    //mRenumLines->setChecked(true);
    //mAllLines->setChecked(false);
    //mRemoveAll->setChecked(false);

    mRenumWithComm->setEnabled(false);
    mRenumEmpty->setEnabled(true);

    mCheckDivide->setEnabled(false);
}

void RenumberDialog::allLinesClicked()
{
    formInput->setEnabled(false);
    startAtInput->setEnabled(true);
    incInput->setEnabled(true);
    toInput->setEnabled(false);

    //startAtInput->setValue(10);
    //incInput->setValue(10);

    //mRenumLines->setChecked(false);
    //mAllLines->setChecked(true);
    //mRemoveAll->setChecked(false);

    mRenumWithComm->setEnabled(true);
    mRenumEmpty->setEnabled(true);

    mCheckDivide->setEnabled(true);
}

void RenumberDialog::removeAllClicked()
{
    formInput->setEnabled(false);
    startAtInput->setEnabled(false);
    incInput->setEnabled(false);
    toInput->setEnabled(false);

    //mRenumLines->setChecked(false);
    // mAllLines->setChecked(false);
    //mRemoveAll->setChecked(true);

    mRenumWithComm->setEnabled(false);
    mRenumEmpty->setEnabled(false);

    mCheckDivide->setEnabled(false);
}
