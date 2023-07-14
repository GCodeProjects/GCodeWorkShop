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
#include <QCheckBox>        // for QCheckBox
#include <QComboBox>        // for QComboBox
#include <QDoubleSpinBox>   // for QDoubleSpinBox
#include <QSpinBox>         // for QSpinBox
#include <QWidget>          // for QWidget
#include <QtGlobal>         // for Q_UNUSED

#include "swapaxesdialog.h"


SwapAxesDialog::SwapAxesDialog(QWidget *parent) :
    QDialog(parent),
    Ui::SwapAxesDialog()
{
    setupUi(this);

    connect(betweenCheckBox, SIGNAL(clicked(bool)), this, SLOT(betweenCheckBoxClicked(bool)));
    connect(modifyCheckBox, SIGNAL(clicked(bool)), this, SLOT(modifyCheckBoxClicked(bool)));

    connect(precisionSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(precisionSpinBoxChanded(int)));
}

SwapAxesDialog::~SwapAxesDialog()
{
}

void SwapAxesDialog::betweenCheckBoxClicked(bool checked)
{
    minDoubleSpinBox->setEnabled(checked);
    maxDoubleSpinBox->setEnabled(checked);
}

void SwapAxesDialog::precisionSpinBoxChanded(int val)
{
    Q_UNUSED(val);
}

void SwapAxesDialog::modifyCheckBoxClicked(bool checked)
{
    operatorComboBox->setEnabled(checked);
    modiferDoubleSpinBox->setEnabled(checked);
}
