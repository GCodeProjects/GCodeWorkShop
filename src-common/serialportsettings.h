/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru                                                   *
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

#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QSerialPort>
#include <QString>


struct SerialPortSettings {
	QSerialPort::BaudRate BaudRate;
	QSerialPort::DataBits DataBits;
	QSerialPort::Parity Parity;
	QSerialPort::StopBits StopBits;
	QSerialPort::FlowControl FlowControl;
	QString configName;
	QString portName;
	QString sendAtEnd;
	QString sendAtBegining;
	QString savePath;
	QString saveExt;
	QString endOfProgChar;
	QString callerProgName;
	double lineDelay;
	unsigned int sendStartDelayReloadValue;
	unsigned int autoCloseTimeout;
	unsigned int sendTimeout;
	unsigned int receiveTimeout;
	unsigned int reconnectTime;
	char Xon;
	char Xoff;
	bool createLogFile;
	bool autoSave;
	bool deleteControlChars;
	bool removeEmptyLines;
	bool removeBefore;
	bool removeSpaceEOB;
	bool renameIfExists;
	bool removeLetters;
	bool guessFileNameByProgName;
	bool appendExt;
	bool useAsExt;
	bool splitPrograms;
	bool fileNameLowerCase;
	bool fileServer;
	bool logData;
	bool waitForCts;
	bool waitForXon;
	bool sendXon;
	QString searchPath1;
	QString searchExt1;
	QString searchPath2;
	QString searchExt2;
	QString searchPath3;
	QString searchExt3;
	QString fileNameExpFs;
	QString fileNameExpAs;
	QString eobChar;
	QString fileNameExpSaveFile;
	QString removeFromRecieved;
};

#endif // SERIALPORTSETTINGS_H
