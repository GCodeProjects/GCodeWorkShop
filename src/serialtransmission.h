/***************************************************************************
 *   Copyright (C) 2006-2009 by Artur Kozioł                               *
 *   artkoz@poczta.onet.pl                                                 *
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

#include <QtGui>


#include <qextserialport.h>

#include "ui_spconfigdialog.h"
#include "ui_transmissiondialog.h"


class SPConfigDialog : public QDialog, private Ui::SPConfigDialog
{
   Q_OBJECT

   public:
     SPConfigDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
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


   private:
     QButtonGroup *baudGroup;
     QButtonGroup *dataBitsGroup;
     QButtonGroup *stopBitsGroup;
     QButtonGroup *parityGroup;
     QButtonGroup *flowCtlGroup;

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
     void sendButtonClicked();
     void reciveButtonClicked();
     void configButtonClicked();
     void stopButtonClicked();
     void changeSettings();
     void updateLeds();
     void setRtsButtonClicked();
     void setDtrButtonClicked();




   private:
     void loadSerialConfignames();
     void showError(int error);


     bool stop;
     QString portName, sendAtEnd, sendAtBegining;
     int baudRate;
     int dataBits;
     int stopBits;
     int parity;
     int flowControl;
     QextSerialPort *comPort;
     QTimer *timer;


};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



#endif // SERIALTRANSMISSION_H
