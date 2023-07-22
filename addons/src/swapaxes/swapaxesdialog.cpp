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
#include <QPoint>           // for QPoint
#include <QRect>            // for QRect
#include <QSettings>        // for QSettings
#include <QSize>            // for QSize
#include <QSpinBox>         // for QSpinBox
#include <QStringList>      // for QStringList
#include <QVariant>         // for QVariant
#include <QWidget>          // for QWidget
#include <QtGlobal>         // for Q_UNUSED

#include "swapaxesdialog.h"
#include "swapaxesoptions.h"    // for SwapAxesOptions


#define CFG_SECTION  "SwapAxesDialog"
#define CFG_KEY_POS  "Position"
#define CFG_KEY_SIZE "Size"


SwapAxesDialog::SwapAxesDialog(QWidget *parent, QSettings *settings) :
    QDialog(parent),
    Ui::SwapAxesDialog()
{
    setupUi(this);

    mSettings = settings;

    connect(betweenCheckBox, SIGNAL(clicked(bool)), this, SLOT(betweenCheckBoxClicked(bool)));
    connect(modifyCheckBox, SIGNAL(clicked(bool)), this, SLOT(modifyCheckBoxClicked(bool)));

    connect(precisionSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(precisionSpinBoxChanded(int)));

    connect(this, SIGNAL(finished(int)), SLOT(onFinished(int)));
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

void SwapAxesDialog::setOptions(const SwapAxesOptions &options)
{
    fromComboBox->clear();
    fromComboBox->addItems(options.fromList);
    fromComboBox->setCurrentText(options.from);
    toComboBox->clear();
    toComboBox->addItems(options.toList);
    toComboBox->setCurrentText(options.to);

    precisionSpinBox->setValue(options.precision);

    betweenCheckBox->setChecked(options.limit.enable);
    betweenCheckBoxClicked(options.limit.enable);
    minDoubleSpinBox->setValue(options.limit.min);
    maxDoubleSpinBox->setValue(options.limit.max);

    modifyCheckBox->setChecked(options.convert.enable);
    modifyCheckBoxClicked(options.convert.enable);
    operatorComboBox->setCurrentIndex(options.convert.operation);
    modiferDoubleSpinBox->setValue(options.convert.value);
}

SwapAxesOptions SwapAxesDialog::options()
{
    SwapAxesOptions options;

    for (int i = 0; i < fromComboBox->count(); i++) {
        options.fromList.append(fromComboBox->itemText(i));
    }

    options.fromList.append(fromComboBox->currentText());
    options.fromList.removeDuplicates();
    options.fromList.sort();

    for (int i = 0; i < toComboBox->count(); i++) {
        options.toList.append(toComboBox->itemText(i));
    }

    options.toList.append(toComboBox->currentText());
    options.toList.removeDuplicates();
    options.toList.sort();

    options.from = fromComboBox->currentText();
    options.to = toComboBox->currentText();

    options.precision = precisionSpinBox->value();

    options.limit.enable = betweenCheckBox->isChecked();
    options.limit.min = minDoubleSpinBox->value();
    options.limit.max = maxDoubleSpinBox->value();

    options.convert.enable = modifyCheckBox->isChecked();
    options.convert.operation = operatorComboBox->currentIndex();
    options.convert.value = modiferDoubleSpinBox->value();

    return options;
}

void SwapAxesDialog::loadSettings(const SwapAxesOptions &defaultOptions)
{
    if (mSettings.isNull()) {
        return;
    }

    mSettings->beginGroup(CFG_SECTION);

    QPoint pos = mSettings->value(CFG_KEY_POS, geometry().topLeft()).toPoint();
    QSize size = mSettings->value(CFG_KEY_SIZE, geometry().size()).toSize();
    setGeometry(QRect(pos, size));

    SwapAxesOptions opt;
    opt.load(mSettings, defaultOptions);

    mSettings->endGroup();

    setOptions(opt);
}

void SwapAxesDialog::saveSettings(bool saveOptions)
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

void SwapAxesDialog::onFinished(int result)
{
    saveSettings(result == QDialog::Accepted);
}
