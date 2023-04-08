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
#include <QSettings>        // for QSettings
#include <QSpinBox>         // for QSpinBox
#include <QString>          // for QString
#include <QStringList>      // for QStringList
#include <QVariant>         // for QVariant
#include <QWidget>          // for QWidget
#include <QtGlobal>         // for Q_UNUSED

#include <utils/medium.h>   // for Medium

#include "swapaxesdialog.h"

SwapAxesDialog::SwapAxesDialog(QWidget *parent) :
    QDialog(parent),
    Ui::SwapAxesDialog()
{
    setupUi(this);

    QSettings &settings = *Medium::instance().settings();

    settings.beginGroup("SwapAxisDialog");

    saveAtCloseCheckBox->setChecked(settings.value("SaveSettings", false).toBool());

    if (saveAtCloseCheckBox->isChecked()) {
        betweenCheckBox->setChecked(settings.value("BeetwenEnabled", false).toBool());
        modifyCheckBox->setChecked(settings.value("ModifyEnabled", false).toBool());

        bool ok;
        QStringList list;

        list = settings.value("FromComboValues", (QStringList() << "X" << "Y" << "Z")).toStringList();
        fromComboBox->clear();
        fromComboBox->addItems(list);
        fromComboBox->setCurrentIndex(fromComboBox->findText(settings.value("FromComboIndex",
                                      "X").toString()));

        list = settings.value("ToComboValues", (QStringList() << "X" << "Y" << "Z")).toStringList();
        toComboBox->clear();
        toComboBox->addItems(list);
        toComboBox->setCurrentIndex(toComboBox->findText(settings.value("ToComboIndex",
                                    "Y").toString()));

        minDoubleSpinBox->setValue(settings.value("Min", 0).toDouble(&ok));
        maxDoubleSpinBox->setValue(settings.value("Max", 0).toDouble(&ok));
        modiferDoubleSpinBox->setValue(settings.value("Modifer", 0).toDouble(&ok));

        operatorComboBox->setCurrentIndex(settings.value("OperatorComboIndex", 0).toInt(&ok));
        precisionSpinBox->setValue(settings.value("Precision", 3).toInt(&ok));
    }

    settings.endGroup();

    betweenCheckBoxClicked(betweenCheckBox->isChecked());
    modifyCheckBoxClicked(modifyCheckBox->isChecked());

    //fromComboBox->setValidator();

    connect(betweenCheckBox, SIGNAL(clicked(bool)), this, SLOT(betweenCheckBoxClicked(bool)));
    connect(modifyCheckBox, SIGNAL(clicked(bool)), this, SLOT(modifyCheckBoxClicked(bool)));

    connect(precisionSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(precisionSpinBoxChanded(int)));

    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

SwapAxesDialog::~SwapAxesDialog()
{
}

void SwapAxesDialog::saveSettings()
{

    QSettings &settings = *Medium::instance().settings();

    settings.beginGroup("SwapAxisDialog");

    settings.setValue("SaveSettings", saveAtCloseCheckBox->isChecked());

    if (saveAtCloseCheckBox->isChecked()) {
        settings.setValue("BeetwenEnabled", betweenCheckBox->isChecked());
        settings.setValue("ModifyEnabled", modifyCheckBox->isChecked());

        QStringList list;

        list.clear();
        list.append(fromComboBox->currentText());

        for (int i = 0; i < fromComboBox->count(); i++) {
            list.append(fromComboBox->itemText(i));
        }

        list.removeDuplicates();
        list.sort();
        settings.setValue("FromComboValues", list);
        settings.setValue("FromComboIndex", fromComboBox->currentText());

        list.clear();
        list.append(toComboBox->currentText());

        for (int i = 0; i < toComboBox->count(); i++) {
            list.append(toComboBox->itemText(i));
        }

        list.removeDuplicates();
        list.sort();
        settings.setValue("ToComboValues", list);
        settings.setValue("ToComboIndex", toComboBox->currentText());

        settings.setValue("Min", minDoubleSpinBox->value());
        settings.setValue("Max", maxDoubleSpinBox->value());
        settings.setValue("Modifer", modiferDoubleSpinBox->value());

        settings.setValue("Precision", precisionSpinBox->value());

        settings.setValue("OperatorComboIndex", operatorComboBox->currentIndex());
    }

    settings.endGroup();
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

double SwapAxesDialog::getMinValue()
{
    double val = -999999;

    if (minDoubleSpinBox->isEnabled()) {
        val = minDoubleSpinBox->value();
    }

    return val;
}

double SwapAxesDialog::getMaxValue()
{
    double val = -999999;

    if (maxDoubleSpinBox->isEnabled()) {
        val = maxDoubleSpinBox->value();
    }

    return val;
}

double SwapAxesDialog::getModiferValue()
{
    double val = 0;

    if (modiferDoubleSpinBox->isEnabled()) {
        val = modiferDoubleSpinBox->value();
    }

    return val;
}

int SwapAxesDialog::getPrecision()
{
    int val = 0;

    val = precisionSpinBox->value();

    return val;
}

int SwapAxesDialog::getOperator()
{
    int val = -1;

    if (operatorComboBox->isEnabled()) {
        val = operatorComboBox->currentIndex();
    }

    return val;
}

QString SwapAxesDialog::getFirstAxis()
{
    return fromComboBox->currentText();
}

QString SwapAxesDialog::getSecondAxis()
{
    return toComboBox->currentText();
}

int SwapAxesDialog::exec()
{
    return QDialog::exec();
}
