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
#include <QWidget>      // for QWidget

#include "renumberdialog.h"
#include "renumberoptions.h"    // for RenumberOptions


RenumberDialog::RenumberDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent)
{
    setupUi(this);

    mSettings = settings;

    setWindowTitle(tr("Renumber lines"));

    connect(mRenumLines, SIGNAL(clicked()), this, SLOT(renumClicked()));
    connect(mAllLines, SIGNAL(clicked()), this, SLOT(allLinesClicked()));
    connect(mRemoveAll, SIGNAL(clicked()), this, SLOT(removeAllClicked()));
    connect(mCheckDivide, SIGNAL(clicked()), this, SLOT(divideClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    connect(mRenumHe, SIGNAL(clicked()), this, SLOT(mRenumHeClicked()));

    setMaximumSize(width(), height());
}

RenumberDialog::~RenumberDialog()
{
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

    mRenumWithComm->setEnabled(false);
    mRenumEmpty->setEnabled(false);

    mCheckDivide->setEnabled(false);
}

void RenumberDialog::setOptions(const RenumberOptions &options)
{
    startAtInput->setValue(options.startAt);
    formInput->setValue(options.from);
    incInput->setValue(options.inc);
    mRenumEmpty->setChecked(options.renumEmpty);
    mRenumWithComm->setChecked(options.renumComm);
    mRenumMarked->setChecked(options.renumMarked);
    toInput->setValue(options.to);
    mSpinBox->setValue(options.width);
    mCheckDivide->setChecked(options.applyWidth);

    if (mCheckDivide->isChecked()) {
        divideClicked();
    }

    switch (options.mode) {
    case RenumberOptions::RenumberWithN:
        mRenumLines->setChecked(true);
        renumClicked();
        break;

    case RenumberOptions::RenumberAll:
        mAllLines->setChecked(true);
        allLinesClicked();
        break;

    case RenumberOptions::RemoveAll:
        mRemoveAll->setChecked(true);
        removeAllClicked();
        break;

    case RenumberOptions::RenumberWithoutN:
        mRenumHe->setChecked(true);
        mRenumHeClicked();
        break;

    default:
        ;
    }
}

RenumberOptions RenumberDialog::options()
{
    RenumberOptions options;

    options.startAt = startAtInput->value();
    options.from = formInput->value();
    options.inc = incInput->value();
    options.renumEmpty = mRenumEmpty->isChecked();
    options.renumComm = !mRenumWithComm->isChecked();
    options.renumMarked = mRenumMarked->isChecked();
    options.to = toInput->value();
    options.width = mSpinBox->value();
    options.applyWidth = mCheckDivide->isChecked();

    options.mode = RenumberOptions::RenumberWithN;

    if (mAllLines->isChecked()) {
        options.mode = RenumberOptions::RenumberAll;
    } else if (mRemoveAll->isChecked()) {
        options.mode = RenumberOptions::RemoveAll;
    } else if (mRenumHe->isChecked()) {
        options.mode = RenumberOptions::RenumberWithoutN;
    }

    return options;
}
