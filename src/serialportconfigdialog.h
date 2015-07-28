/***************************************************************************
 *   Copyright (C) 2006-2015 by Artur Kozioł                               *
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


#ifndef SERIALPORTCONFIGDIALOG_H
#define SERIALPORTCONFIGDIALOG_H


#include <QtWidgets>

#include "ui_serialportconfigdialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>


struct PortSettings
{
    QSerialPort::BaudRate BaudRate;
    QSerialPort::DataBits DataBits;
    QSerialPort::Parity Parity;
    QSerialPort::StopBits StopBits;
    QSerialPort::FlowControl FlowControl;
    long Timeout_Millisec;
    char Xon;
    char Xoff;
};

class SerialPortConfigDialog : public QDialog, private Ui::SerialPortConfigDialog
{
   Q_OBJECT

   public:
     SerialPortConfigDialog(QWidget *parent = 0, QString confName = "", Qt::WindowFlags f = Qt::Dialog);
     ~SerialPortConfigDialog();

   public slots:


   protected:

   signals :



   private slots:
     void browseButtonClicked();
     void saveButtonClicked();
     void loadSettings();
     void changeSettings();
     void closeButtonClicked();
     void deleteButtonClicked();
     void saveCloseButtonClicked();
     void flowCtlGroupReleased();
     void browse1ButtonClicked();
     void browse2ButtonClicked();
     void browse3ButtonClicked();
     void browse4ButtonClicked();
     void portNameComboBoxIndexChanged(QString name);
     void autoSaveCheckBoxChanged(int state);
     void appendExtCheckBoxChanged(int state);
     void useAsExtCheckBoxChanged(int state);
     void fileServerCheckBoxChanged(int state);


   private:
     QString browseForDir(const QString dir, QString windowTitle);

     QButtonGroup *baudGroup;
     QButtonGroup *dataBitsGroup;
     QButtonGroup *stopBitsGroup;
     QButtonGroup *parityGroup;
     QButtonGroup *flowCtlGroup;
     QString configName;

};


#endif // SERIALPORTCONFIGDIALOG_H
