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
#include <QLineEdit>    // for QLineEdit
#include <QPushButton>  // for QPushButton
#include <QSpinBox>     // for QSpinBox
#include <QString>      // for QString
#include <QWidget>      // for QWidget
#include <QtGlobal>     // for Q_UNUSED

#include "dotdialog.h"


DotDialog::DotDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);

    setWindowTitle(tr("Insert dots"));

    connect(mInput, SIGNAL(textChanged(const QString &)), this,
            SLOT(inputChanged(const QString &)));
    connect(mCheckAtEnd, SIGNAL(clicked()), this, SLOT(atEndClicked()));
    connect(mCheckDivide, SIGNAL(clicked()), this, SLOT(divideClicked()));
    connect(mSpinBox, SIGNAL(valueChanged(int)), this, SLOT(spinBoxVal(int)));
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));

    setMaximumSize(width(), height());
    okButton->setEnabled(mInput->hasAcceptableInput());
}

DotDialog::~DotDialog()
{
}

void DotDialog::inputChanged(const QString &text)
{
    Q_UNUSED(text);
    okButton->setEnabled(mInput->hasAcceptableInput());
}

void DotDialog::atEndClicked()
{
    mCheckDivide->setChecked(!mCheckAtEnd->isChecked());
    mSpinBox->setEnabled(mCheckDivide->isChecked());
}

void DotDialog::divideClicked()
{
    mCheckAtEnd->setChecked(!mCheckDivide->isChecked());
    mSpinBox->setEnabled(mCheckDivide->isChecked());
}

void DotDialog::spinBoxVal(int val)
{
    if (val == 99) {
        mSpinBox->setValue(10);
    } else if (val == 999) {
        mSpinBox->setValue(10);
    } else if (val == 9999) {
        mSpinBox->setValue(100);
    }

    if (val == 11) {
        mSpinBox->setValue(100);
    } else if (val == 101) {
        mSpinBox->setValue(1000);
    } else if (val == 1001) {
        mSpinBox->setValue(10000);
    }
}
