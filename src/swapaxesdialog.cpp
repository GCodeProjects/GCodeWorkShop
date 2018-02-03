/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of EdytorNC.                                        *
 *                                                                         *
 *   EdytorNC is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/





#include "swapaxesdialog.h"
#include "ui_swapaxesdialog.h"


swapAxesDialog::swapAxesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::swapAxesDialog)
{
    ui->setupUi(this);




    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SwapAxisDialog");

    ui->saveAtCloseCheckBox->setChecked(settings.value("SaveSettings", false).toBool());

    if(ui->saveAtCloseCheckBox->isChecked())
    {
        ui->betweenCheckBox->setChecked(settings.value("BeetwenEnabled", false).toBool());
        ui->modifyCheckBox->setChecked(settings.value("ModifyEnabled", false).toBool());

        bool ok;
        QStringList list;

        list = settings.value("FromComboValues", (QStringList() << "X" << "Y" << "Z")).toStringList();
        ui->fromComboBox->clear();
        ui->fromComboBox->addItems(list);
        ui->fromComboBox->setCurrentIndex(ui->fromComboBox->findText(settings.value("FromComboIndex", "X").toString()));

        list = settings.value("ToComboValues", (QStringList() << "X" << "Y" << "Z")).toStringList();
        ui->toComboBox->clear();
        ui->toComboBox->addItems(list);
        ui->toComboBox->setCurrentIndex(ui->toComboBox->findText(settings.value("ToComboIndex", "Y").toString()));

        ui->minDoubleSpinBox->setValue(settings.value("Min", 0).toDouble(&ok));
        ui->maxDoubleSpinBox->setValue(settings.value("Max", 0).toDouble(&ok));
        ui->modiferDoubleSpinBox->setValue(settings.value("Modifer", 0).toDouble(&ok));


        ui->operatorComboBox->setCurrentIndex(settings.value("OperatorComboIndex", 0).toInt(&ok));
        ui->precisionSpinBox->setValue(settings.value("Precision", 3).toInt(&ok));
    };

    settings.endGroup();


    betweenCheckBoxClicked( ui->betweenCheckBox->isChecked());
    modifyCheckBoxClicked( ui->modifyCheckBox->isChecked());


    //ui->fromComboBox->setValidator();


    connect(ui->betweenCheckBox, SIGNAL(clicked(bool)), this, SLOT(betweenCheckBoxClicked(bool)));
    connect(ui->modifyCheckBox, SIGNAL(clicked(bool)), this, SLOT(modifyCheckBoxClicked(bool)));

    connect(ui->precisionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(precisionSpinBoxChanded(int)));


    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));


}

//**************************************************************************************************
//
//**************************************************************************************************

swapAxesDialog::~swapAxesDialog()
{
    delete ui;
}

//**************************************************************************************************
//
//**************************************************************************************************

void swapAxesDialog::saveSettings()
{

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SwapAxisDialog");

    settings.setValue("SaveSettings", ui->saveAtCloseCheckBox->isChecked());

    if(ui->saveAtCloseCheckBox->isChecked())
    {
        settings.setValue("BeetwenEnabled", ui->betweenCheckBox->isChecked());
        settings.setValue("ModifyEnabled", ui->modifyCheckBox->isChecked());

        QStringList list;

        list.clear();
        list.append(ui->fromComboBox->currentText());
        for(int i = 0; i < ui->fromComboBox->count(); i++)
        {
            list.append(ui->fromComboBox->itemText(i));
        };
        list.removeDuplicates();
        list.sort();
        settings.setValue("FromComboValues", list);
        settings.setValue("FromComboIndex", ui->fromComboBox->currentText());


        list.clear();
        list.append(ui->toComboBox->currentText());
        for(int i = 0; i < ui->toComboBox->count(); i++)
        {
            list.append(ui->toComboBox->itemText(i));
        };
        list.removeDuplicates();
        list.sort();
        settings.setValue("ToComboValues", list);
        settings.setValue("ToComboIndex", ui->toComboBox->currentText());


        settings.setValue("Min", ui->minDoubleSpinBox->value());
        settings.setValue("Max", ui->maxDoubleSpinBox->value());
        settings.setValue("Modifer", ui->modiferDoubleSpinBox->value());

        settings.setValue("Precision", ui->precisionSpinBox->value());

        settings.setValue("OperatorComboIndex", ui->operatorComboBox->currentIndex());

    };

    settings.endGroup();

}

//**************************************************************************************************
//
//**************************************************************************************************

void swapAxesDialog::betweenCheckBoxClicked(bool checked)
{
    ui->minDoubleSpinBox->setEnabled(checked);
    ui->maxDoubleSpinBox->setEnabled(checked);
}

//**************************************************************************************************
//
//**************************************************************************************************

void swapAxesDialog::precisionSpinBoxChanded(int val)
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void swapAxesDialog::modifyCheckBoxClicked(bool checked)
{
    ui->operatorComboBox->setEnabled(checked);
    ui->modiferDoubleSpinBox->setEnabled(checked);
}

//**************************************************************************************************
//
//**************************************************************************************************

double swapAxesDialog::getMinValue()
{
    double val = -999999;

    if(ui->minDoubleSpinBox->isEnabled())
        val = ui->minDoubleSpinBox->value();

    return val;
}

//**************************************************************************************************
//
//**************************************************************************************************

double swapAxesDialog::getMaxValue()
{
    double val = -999999;

    if(ui->maxDoubleSpinBox->isEnabled())
        val = ui->maxDoubleSpinBox->value();

    return val;
}

//**************************************************************************************************
//
//**************************************************************************************************

double swapAxesDialog::getModiferValue()
{
    double val = 0;

    if(ui->modiferDoubleSpinBox->isEnabled())
        val = ui->modiferDoubleSpinBox->value();

    return val;
}

//**************************************************************************************************
//
//**************************************************************************************************

int swapAxesDialog::getPrecision()
{
    int val = 0;

    val = ui->precisionSpinBox->value();

    return val;
}

//**************************************************************************************************
//
//**************************************************************************************************

int swapAxesDialog::getOperator()
{
    int val = -1;

    if(ui->operatorComboBox->isEnabled())
        val = ui->operatorComboBox->currentIndex();

    return val;
}

//**************************************************************************************************
//
//**************************************************************************************************

QString swapAxesDialog::getFirstAxis()
{
    return ui->fromComboBox->currentText();
}

//**************************************************************************************************
//
//**************************************************************************************************

QString swapAxesDialog::getSecondAxis()
{
    return ui->toComboBox->currentText();
}

//**************************************************************************************************
//
//**************************************************************************************************

int swapAxesDialog::exec()
{
    return QDialog::exec();
}

//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************

