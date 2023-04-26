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
#include <QSettings>    // for QSettings
#include <QSpinBox>     // for QSpinBox
#include <QWidget>      // for QWidget

#include "i2mprogdialog.h"
#include "i2mprogoptions.h" // I2MProgOptions


I2MProgDialog::I2MProgDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent)
{
    setupUi(this);

    mSettings = settings;

    setWindowTitle(tr("Convert program inch to metric"));

    connect(mInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(closeButton, SIGNAL(clicked()), SLOT(reject()));

    setFocusProxy(mInput);
    inputChanged();
}

I2MProgDialog::~I2MProgDialog()
{
}

void I2MProgDialog::inputChanged()
{
    okButton->setEnabled(mInput->hasAcceptableInput());
}

void I2MProgDialog::setOptions(const I2MProgOptions &options)
{
    inchCheckBox->setChecked(options.toInch);
    mmCheckBox->setChecked(!options.toInch);
    mInput->setText(options.axes);
    precInput->setValue(options.prec);

    inputChanged();
}

I2MProgOptions I2MProgDialog::options()
{
    I2MProgOptions options;

    options.toInch = inchCheckBox->isChecked();
    options.axes = mInput->text();
    options.prec = precInput->value();

    return options;
}
