/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of GCodeWorkShop.                                   *
 *                                                                         *
 *   GCodeWorkShop is free software; you can redistribute it and/or modify *
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

#include <QByteArray>   // for QByteArray
#include <QDialog>      // for QDialog
#include <QObject>      // for slots, Q_OBJECT
#include <QSerialPort>  // for QSerialPort
#include <QString>      // for QString
#include <QStringList>  // for QStringList
#include <Qt>           // for WindowType, WindowFlags
#include <QtGlobal>     // for qint64

class QCloseEvent;
class QTimer;
class QWidget;

#include "serialportsettings.h"             // for SerialPortSettings
#include "ui_serialtransmissiondialog.h"    // for Ui::SerialTransmissionDialog


class SerialTransmissionDialog : public QDialog, private Ui::SerialTransmissionDialog
{
	Q_OBJECT

public:
	SerialTransmissionDialog(QWidget* parent = 0, Qt::WindowFlags f = Qt::Dialog,
	                         bool mode = false);
	~SerialTransmissionDialog();

	QStringList receiveData(QString configName);
	void sendData(QString dataToSend, QString configName);
	void startFileServer(QString configName);
	QString configName();
	QString savePath();
	QStringList readPaths();

public slots:
	bool wasCanceled();
	void portReset();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void cancelButtonClicked();
	void updateStatus();
	void sendStartDelayTimeout();
	void stopButtonClicked();
	void autoCloseTimerTimeout();
	void showSerialPortError(QSerialPort::SerialPortError error);
	void serialPortBytesWritten(qint64 bytes);
	void serialPortReadyRead();
	void serialPortRequestToSend(bool set);
	void lineDelaySlot();
	void fileServerProcessData();
	void fileServerBytesWritten(qint64 bytes);
	void sendTimeoutTimerTimeout();
	void receiveTimeoutTimerTimeout();
	void reset(bool re);
	void reconnectTimerTimeout();

private:
	void setRange(int min, int max);
	void setValue(int val);
	void setLabelText(const QString text, bool multiline = false, bool writeToLog = false);
	void loadConfig(QString configName);
	QStringList guessFileName(QString* text);
	QString saveDataToFile(QString* text);
	void writeLog(QString msg, QString timeStamp = "");
	QStringList processReceivedData();
	void prepareDataBeforeSending(QString* data);
	void procesSpecialCharacters(QString* text, QString* fileData);
	void resetTransmission(bool portRestart = false);

	QSerialPort serialPort;
	bool canceled;
	SerialPortSettings portSettings;
	QByteArray serialPortReadBuffer;
	// FIXME: serialPortWriteBuffer must be QList<QBiteArray>
	QStringList serialPortWriteBuffer;
	qint64 bytesWritten;
	qint64 noOfBytes;
	QStringList::iterator writeBufferIterator;
	bool xoffReceived;
	bool prevXoffReceived;
	unsigned int autoCloseCountner;
	unsigned int sendStartDelayCountner;
	//int autoCloseCountnerReloadValue;
	//    int fileServerDataTimeoutCountner;
	//    int fileServerDataTimeoutCountnerReloadValue;
	unsigned int sendTimeoutCountner;
	//int sendTimeoutCountnerReloadValue;
	unsigned int receiveTimeoutCountner;
	//int receiveTimeoutCountnerReloadValue;
	bool stop;
	bool serverMode;
	bool sending;
	QTimer* sendStartDelayTimer;
	QTimer* updateStatusTimer;
	QTimer* autoCloseTimer;
	//QTimer *fileServerDataTimeoutTimer;
	QTimer* sendTimeoutTimer;
	QTimer* receiveTimeoutTimer;
	QTimer* reconnectTimer;
};

#endif // SERIALTRANSMISSIONDIALOG_H
