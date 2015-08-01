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


#ifndef SERIALTRANSMISSIONDIALOG_H
#define SERIALTRANSMISSIONDIALOG_H


#include <QtWidgets>
#include <QtSerialPort/QSerialPort>

#include "serialtransmission.h"
#include "commoninc.h"

#include "ui_serialtransmissiondialog.h"

class SerialTransmissionDialog : public QDialog, private Ui::SerialTransmissionDialog
{
    Q_OBJECT

public:
    SerialTransmissionDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog, bool mode = false);
    ~SerialTransmissionDialog();
    QStringList receiveData(QString configName);
    void sendData(QString dataToSend, QString configName);
    bool startFileServer(QString configName, bool state = false);

public slots:
    bool wasCanceled();



protected:
    void closeEvent(QCloseEvent *event);

signals :



private slots:
    void cancelButtonClicked();
    void updateLeds();
    void sendStartDelayTimeout();
    void stopButtonClicked();
    void autoClose();
    void showSerialPortError(QSerialPort::SerialPortError error);
    void serialPortBytesWritten(qint64 bytes);
    void serialPortReadyRead();
    void serialPortRequestToSend(bool set);
    void lineDelaySlot();
    void fileServerReadyRead();
    void fileServerReceiveTimeout();
    void fileServerBytesWritten(qint64 bytes);


private:
    void setRange(int min, int max);
    void setValue(int val);
    void setLabelText(const QString text, bool multiline = false);
    void loadConfig(QString configName);
    QString guessFileName(QString *text);
    QString saveDataToFile(QString *text);
    void writeLog(QString msg, QString timeStamp = "");
    QStringList splitFile(QString *text);
    QStringList processReceivedData();
    void prepareDataBeforeSending(QString *data);
    void procesSpecialCharacters(QString *text, QString *fileData);


    QSerialPort serialPort;
    bool canceled;
    PortSettings portSettings;
    QByteArray serialPortReadBuffer;
    QStringList serialPortWriteBuffer;
    QTimer *updateLedsTimer;
    QTimer *autoCloseTimer;
    QTimer *fileServerDataTimeoutTimer;
    qint64 bytesWritten;
    qint64 noOfBytes;
    QStringList::iterator writeBufferIterator;
    bool prevXoffReceived;
    int autoCloseCountner;
    int autoCloseCountnerReloadValue;
    int fileServerDataTimeoutCountner;
    int fileServerDataTimeoutCountnerReloadValue;
//    bool guessFileNameByProgName;
//    bool createLogFile;
//    bool renameIfExists;
//    bool removeLetters;
//    bool appendExt;
//    bool useAsExt;
//    bool splitPrograms;



    bool stop;
    bool xoffReceived;
//    QString portName, sendAtEnd, sendAtBegining;
//    int baudRate;
//    int dataBits;
//    int stopBits;
//    int parity;
//    int flowControl;
//    double lineDelay;
//    bool readyCont;

//    bool deleteControlChars;
//    bool removeEmptyLines;
//    bool removeBefore;
//    bool removeSpaceEOB;
//    int sendStartDelay;
//    int recieveTimeout;

//    bool autoSave;
//    QString savePath;
//    QString saveExt;
    bool serverMode;

    //QPlainTextEdit plainTextEdit;



};

#endif // SERIALTRANSMISSIONDIALOG_H
