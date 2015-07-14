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

#ifndef SERIALTRANSMISSION_H
#define SERIALTRANSMISSION_H

#include <QtWidgets>


#include "ui_spconfigdialog.h"
#include "ui_transmissiondialog.h"
#include "ui_transprogressdialog.h"
#include <qserialport.h>

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

class SPConfigDialog : public QDialog, private Ui::SPConfigDialog
{
   Q_OBJECT

   public:
     SPConfigDialog(QWidget *parent = 0, QString confName = "", Qt::WindowFlags f = Qt::Dialog);
     ~SPConfigDialog();

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
     void portNameComboBoxIndexChanged(QString name);


   private:
     QString browseForDir(const QString dir);

     QButtonGroup *baudGroup;
     QButtonGroup *dataBitsGroup;
     QButtonGroup *stopBitsGroup;
     QButtonGroup *parityGroup;
     QButtonGroup *flowCtlGroup;
     QString configName;

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class TransmissionDialog : public QDialog, private Ui::TransmissionDialog
{
   Q_OBJECT

   public:
     TransmissionDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
     ~TransmissionDialog();

   public slots:


   protected:

   signals :



   private slots:
     void closeButtonClicked();
     void clearButtonClicked();
     void connectButtonToggled(bool tg);
     void configButtonClicked();
     void changeSettings();
     void updateLeds();
     void setRtsButtonClicked();
     void setDtrButtonClicked();
     void sendTextEditChanged();
     void lineDelaySlot();
     void setXonButtonClicked();
     void setXoffButtonClicked();
     void textEditScroll(int pos);
     void hexTextEditScroll(int pos);



   private:
     void loadSerialConfignames();
     void showError(int error);
     void sendText(QString tx);


     bool stop;
     QString portName;
     QString sendAtEnd;
     QString sendAtBegining;

     QSerialPort *comPort;
     QTimer *timer;
     long int count;
     double lineDelay;
     bool readyCont;
     PortSettings portSettings;




};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class TransProgressDialog : public QDialog, private Ui::TransProgressDialog
{
   Q_OBJECT

   public:
     TransProgressDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
     ~TransProgressDialog();

   public slots:
     void setLabelText(const QString text);
     void open(QSerialPort *port, char cxon = 0, char cxoff = 0);
     bool wasCanceled();
     void setRange(int min, int max);
     void setValue(int val);


   protected:
     void closeEvent(QCloseEvent *event);

   signals :



   private slots:
     void cancelButtonClicked();
     void updateLeds();


   private:
     QSerialPort *comPort;
     bool canceled;
     char xon, xoff;

     QTimer *timer;




};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



#endif // SERIALTRANSMISSION_H
