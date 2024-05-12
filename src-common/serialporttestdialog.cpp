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

#include <QChar>
#include <QIcon>
#include <QIODevice>
#include <QScrollBar>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <Qt>              // Qt::WindowFlags
#include <QTextCharFormat>
#include <QTextCursor>
#include <QtGlobal>        // Q_UNUSED
#include <QTimer>
#include <QWidget>

#include <utils/medium.h> // Medium

#include "serialporttestdialog.h"   // SerialPortTestDialog QDialog QObject
#include "serialportconfigdialog.h" // SerialPortConfigDialog


SerialPortTestDialog::SerialPortTestDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Serial transmission test"));

	comPort = nullptr;

	connect(closeButton, SIGNAL(clicked()), SLOT(closeButtonClicked()));

	connect(clearButton, SIGNAL(clicked()), SLOT(clearButtonClicked()));
	connect(configButton, SIGNAL(clicked()), SLOT(configButtonClicked()));

	connect(connectButton, SIGNAL(toggled(bool)), SLOT(connectButtonToggled(bool)));

	connect(setRtsButton, SIGNAL(clicked()), SLOT(setRtsButtonClicked()));
	connect(setDtrButton, SIGNAL(clicked()), SLOT(setDtrButtonClicked()));

	loadSerialConfignames();
	changeSettings();
	connect(configBox, SIGNAL(currentIndexChanged(int)), SLOT(changeSettings()));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateLeds()));

	connect(sendLineEdit, SIGNAL(returnPressed()), SLOT(sendTextEditChanged()));

	connect(setXonButton, SIGNAL(clicked()), SLOT(setXonButtonClicked()));
	connect(setXoffButton, SIGNAL(clicked()), SLOT(setXoffButtonClicked()));


	connect(textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(textEditScroll(int)));
	connect(hexTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)),
	        SLOT(hexTextEditScroll(int)));
}

SerialPortTestDialog::~SerialPortTestDialog()
{
}

void SerialPortTestDialog::textEditScroll(int pos)
{
	Q_UNUSED(pos);
	hexTextEdit->verticalScrollBar()->setSliderPosition(
	    textEdit->verticalScrollBar()->sliderPosition());
}

void SerialPortTestDialog::hexTextEditScroll(int pos)
{
	Q_UNUSED(pos);
	textEdit->verticalScrollBar()->setSliderPosition(
	    hexTextEdit->verticalScrollBar()->sliderPosition());
}

void SerialPortTestDialog::sendTextEditChanged()
{
	QString tx, ty;
	int i;

	tx = sendLineEdit->text();
	sendLineEdit->clear();

	tx.append("\r\n");

	sendText(tx);

	QTextCursor cr = textEdit->textCursor();
	QTextCharFormat format = cr.charFormat();
	format.setForeground(Qt::black);
	cr.setCharFormat(format);
	cr.insertText(tx);
	textEdit->setTextCursor(cr);

	cr = hexTextEdit->textCursor();
	format = cr.charFormat();
	format.setForeground(Qt::black);
	cr.setCharFormat(format);

	for (i = 0; i < tx.size(); i++) {
		ty = QString("%1 ").arg((int)tx.at(i).toLatin1(), 2, 16, (QChar)'0');

		cr.insertText(ty);
	}

	hexTextEdit->setTextCursor(cr);
	hexTextEdit->insertPlainText("\n");

	textEdit->ensureCursorVisible();
	hexTextEdit->ensureCursorVisible();
}

void SerialPortTestDialog::closeButtonClicked()
{
	stop = true;
	qApp->processEvents();

	if (comPort != nullptr) {
		//comPort->close();
		//delete(comPort);
	}

	close();
}

void SerialPortTestDialog::clearButtonClicked()
{
	textEdit->clear();
	hexTextEdit->clear();
	sendLineEdit->clear();
}

void SerialPortTestDialog::connectButtonToggled(bool tg)
{
	if (tg) {

		connectButton->setIcon(QIcon(":/images/connect_established.png"));
		connectButton->setText(tr("&Disconnect"));

		textEdit->clear();
		hexTextEdit->clear();
		sendLineEdit->clear();

		count = 0;

		if (comPort != nullptr) {
			comPort->reset();
			comPort->close();
			delete comPort;
		}

		comPort = new QSerialPort(portName);
		comPort->clearError();
		comPort->setBaudRate(portSettings.BaudRate);
		comPort->setDataBits(portSettings.DataBits);
		comPort->setParity(portSettings.Parity);
		comPort->setStopBits(portSettings.StopBits);
		comPort->setFlowControl(portSettings.FlowControl);

		if (!comPort->open(QIODevice::ReadWrite)) {
			showError(comPort->error());
			delete comPort;
			comPort = nullptr;
			connectButton->setChecked(false);
			return;
		}

		comPort->clear(QSerialPort::AllDirections);

		//      if(portSettings.FlowControl == FLOW_XONXOFF)
		//      {
		//         comPort->putChar(portSettings.Xon);
		//      }

		setDtrButton->setEnabled(true);
		setDtrButton->setChecked(false);
		setRtsButton->setEnabled(true);
		setRtsButton->setChecked(false);

		configBox->setEnabled(false);
		configButton->setEnabled(false);

		bool en = portSettings.FlowControl == QSerialPort::SoftwareControl;
		setXonButton->setEnabled(en);
		setXoffButton->setEnabled(en);

		sendLineEdit->setReadOnly(false);
		sendLineEdit->setFocus(Qt::MouseFocusReason);

		showError(comPort->error());

		stop = false;
		timer->start(20);

	} else {
		timer->stop();
		//qApp->processEvents();
		connectButton->setIcon(QIcon(":/images/connect_no.png"));
		connectButton->setText(tr("&Connect"));

		stop = true;
		qApp->processEvents();
		setDtrButton->setEnabled(false);
		setRtsButton->setEnabled(false);
		setXonButton->setEnabled(false);
		setXoffButton->setEnabled(false);

		ctsLabel->setEnabled(false);
		dsrLabel->setEnabled(false);
		dcdLabel->setEnabled(false);
		rtsLabel->setEnabled(false);
		dtrLabel->setEnabled(false);

		connectButton->setChecked(false);

		configBox->setEnabled(true);
		configButton->setEnabled(true);

		sendLineEdit->setReadOnly(true);
	}
}

void SerialPortTestDialog::setXonButtonClicked()
{
	if (comPort != nullptr && comPort->isOpen()) {
		comPort->putChar(portSettings.Xon);
	}
}

void SerialPortTestDialog::setXoffButtonClicked()
{
	if (comPort != nullptr && comPort->isOpen()) {
		comPort->putChar(portSettings.Xoff);
	}
}

void SerialPortTestDialog::setDtrButtonClicked()
{
	if (comPort != nullptr && comPort->isOpen()) {
		comPort->setDataTerminalReady(setDtrButton->isChecked());
	}
}

void SerialPortTestDialog::setRtsButtonClicked()
{
	if (comPort != nullptr && comPort->isOpen()) {
		comPort->setRequestToSend(setRtsButton->isChecked());
	}
}

void SerialPortTestDialog::updateLeds()
{
	QSerialPort::PinoutSignals status;
	bool ok;
	QString tx;
	char ch;
	int i;
	QTextCursor cr;
	QTextCharFormat format;

	timer->stop();

	if (comPort == nullptr || !comPort->isOpen()) {
		return;
	}

	while (!stop) {
		status = comPort->pinoutSignals();

		ctsLabel->setEnabled(status & QSerialPort::ClearToSendSignal);
		dsrLabel->setEnabled(status & QSerialPort::DataSetReadySignal);
		dcdLabel->setEnabled(status & QSerialPort::DataCarrierDetectSignal);
		rtsLabel->setEnabled(status & QSerialPort::RequestToSendSignal);
		setRtsButton->setChecked(status & QSerialPort::RequestToSendSignal);
		dtrLabel->setEnabled(status & QSerialPort::DataTerminalReadySignal);
		setDtrButton->setChecked(status & QSerialPort::DataTerminalReadySignal);


		i = comPort->bytesAvailable();
		qApp->processEvents();

		if (i > 0) {
			ok = comPort->getChar(&ch);

			if (!ok) {
				showError(comPort->error());
				return;
			}

			count++;
			errorLabel->setText(tr("Recived: %1 bytes.").arg(count));
			tx = ch;

			qApp->processEvents();

			if (ch != '\r') {
				cr = textEdit->textCursor();
				format = cr.charFormat();
				format.setForeground(Qt::blue);
				cr.setCharFormat(format);
				cr.insertText(tx);
				textEdit->setTextCursor(cr);
			}

			tx = QString("%1 ").arg((int)ch, 2, 16, (QChar)'0');
			cr = hexTextEdit->textCursor();
			format = cr.charFormat();
			format.setForeground(Qt::blue);
			cr.setCharFormat(format);
			cr.insertText(tx);
			hexTextEdit->setTextCursor(cr);

			if (ch == '\n') {
				hexTextEdit->insertPlainText("\n");
			}

			textEdit->ensureCursorVisible();
			hexTextEdit->ensureCursorVisible();
		} else if (i < 0) {
			showError(comPort->error());
		}
	}

	comPort->close();
}

void SerialPortTestDialog::configButtonClicked()
{
	SerialPortConfigDialog* serialConfigDialog = new SerialPortConfigDialog(this,
	    configBox->currentText());

	if (serialConfigDialog->exec() == QDialog::Accepted) {
		changeSettings();
	}
}

void SerialPortTestDialog::changeSettings()
{
	QString port;
	bool ok;

	QSettings &settings = *Medium::instance().settings();

	settings.beginGroup("SerialPortConfigs");

#ifdef Q_OS_WIN32
	port = "COM1";
#else
	port = "/dev/ttyS0";

#endif

	settings.beginGroup(configBox->currentText());

	portName = settings.value("PortName", port).toString();

	portSettings.BaudRate = (QSerialPort::BaudRate) settings.value("BaudRate",
	                        QSerialPort::Baud9600).toInt();
	portSettings.DataBits = (QSerialPort::DataBits) settings.value("DataBits",
	                        QSerialPort::Data8).toInt();
	portSettings.StopBits = (QSerialPort::StopBits) settings.value("StopBits",
	                        QSerialPort::TwoStop).toInt();
	portSettings.Parity = (QSerialPort::Parity) settings.value("Parity",
	                      QSerialPort::NoParity).toInt();
	portSettings.FlowControl = (QSerialPort::FlowControl) settings.value("FlowControl",
	                           QSerialPort::HardwareControl).toInt();
	portSettings.Xon = settings.value("Xon", "17").toString().toInt(&ok, 10);
	portSettings.Xoff = settings.value("Xoff", "19").toString().toInt(&ok, 10);
	sendAtEnd = settings.value("SendAtEnd", "").toString();
	sendAtBegining = settings.value("SendAtBegining", "").toString();
	lineDelay = settings.value("LineDelay", 0).toDouble();

	settings.endGroup();
	settings.endGroup();
}

void SerialPortTestDialog::loadSerialConfignames()
{
	int id;
	QStringList list;
	QString item;

	QSettings &settings =  *Medium::instance().settings();

	settings.beginGroup("SerialPortConfigs");

	configBox->clear();
	list = settings.value("SettingsList", QStringList(tr("Default"))).toStringList();
	list.sort();
	configBox->addItems(list);
	item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
	id = configBox->findText(item);
	configBox->setCurrentIndex(id);
	settings.endGroup();
}

void SerialPortTestDialog::showError(int error)
{
	switch (error) {
	case QSerialPort::NoError:
		errorLabel->setText("No Error has occured");
		break;

	case QSerialPort::DeviceNotFoundError:
		errorLabel->setText("Attempting to open an non-existing device");
		break;

	case QSerialPort::PermissionError:
		errorLabel->setText("Attempting to open an already opened device by another process or user not having enough permission and credentials to open");
		break;

	case QSerialPort::OpenError:
		errorLabel->setText("Attempting to open an already opened device in this object");
		break;

	case QSerialPort::WriteError:
		errorLabel->setText("An I/O error occurred while writing the data");
		break;

	case QSerialPort::ReadError:
		errorLabel->setText("An I/O error occurred while reading the data");
		break;

	case QSerialPort::ResourceError:
		errorLabel->setText("An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system");
		break;

	case QSerialPort::UnsupportedOperationError:
		errorLabel->setText("Receive buffer overflow");
		break;

	case QSerialPort::UnknownError:
		errorLabel->setText("An unidentified error occurred");
		break;

	case QSerialPort::TimeoutError:
		errorLabel->setText("Transmit buffer overflow");
		break;

	case QSerialPort::NotOpenError:
		errorLabel->setText("An operation is executed that can only be successfully performed if the device is open");
		break;

	default:
		errorLabel->setText("Unknown error");
	}
}

void SerialPortTestDialog::sendText(QString tx)
{
	int i;
	bool xoffReceived;
	char controlChar;
	int bytesToWrite;

	if (comPort == nullptr) {
		return;
	}

	if (comPort->isOpen()) {
		tx.prepend(sendAtBegining);
		tx.append(sendAtEnd);

		if (!tx.contains("\r\n")) {
			tx.replace("\n", "\r\n");
		}

		errorLabel->setText(tr("Waiting..."));
		qApp->processEvents();

		i = 0;
		xoffReceived = true;

		while (i < tx.size()) {
			if (xoffReceived) {
				errorLabel->setText(tr("Waiting for a signal readiness..."));
			}

			qApp->processEvents();

			if (stop) {
				break;
			}

			if (portSettings.FlowControl == QSerialPort::SoftwareControl) {
				controlChar = 0;

				if (comPort->bytesAvailable() > 0) {
					comPort->getChar(&controlChar);
				}

				if (controlChar == portSettings.Xoff) {
					xoffReceived = true;
				}

				if (controlChar == portSettings.Xon) {
					xoffReceived = false;
				}

				//setXoffButton->setChecked(xoffReceived);
				//setXonButton->setChecked(!xoffReceived);
			} else {
				xoffReceived = false;
			}

			bytesToWrite = comPort->bytesToWrite();

			if ((bytesToWrite == 0) && (!xoffReceived)) {
				if (!comPort->putChar(tx[i].toLatin1())) {
					showError(comPort->error());
				}

				errorLabel->setText(tr("Sending byte %1 of %2").arg(i + 1).arg(tx.size()));
				qApp->processEvents();

				if (lineDelay > 0) {
					if (tx[i].toLatin1() == '\n') {
						readyCont = false;
						QTimer::singleShot(int(lineDelay * 1000), this, SLOT(lineDelaySlot()));

						while (!readyCont) {
							qApp->processEvents();
						}
					}
				}

				i++;
			}
		}
	}
}

void SerialPortTestDialog::lineDelaySlot()
{
	readyCont = true;
}

//TransProgressDialog::TransProgressDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
//{
//   setupUi(this);
//   //setWindowTitle(tr("Serial transmission"));

//   comPort = nullptr;

//   canceled = true;

//   connect(cancelButton, SIGNAL(clicked()), SLOT(cancelButtonClicked()));
//   connect(this, SIGNAL(rejected()), SLOT(cancelButtonClicked()));

//   timer = new QTimer(this);
//   connect(timer, SIGNAL(timeout()), this, SLOT(updateLeds()));
//}

//TransProgressDialog::~TransProgressDialog()
//{
//   timer->stop();
//}

//void TransProgressDialog::closeEvent(QCloseEvent *event)
//{
//   timer->stop();
//   canceled = true;
//   event->accept();
//}

//void TransProgressDialog::cancelButtonClicked()
//{
//   timer->stop();
//   canceled = true;
//   close();
//}

//bool TransProgressDialog::wasCanceled()
//{
//   return canceled;
//}

//void TransProgressDialog::setLabelText(const QString text)
//{
//   label->setText(text);
//}

//void TransProgressDialog::setValue(int val)
//{
//   progressBar->setValue(val);
//}

//void TransProgressDialog::setRange(int min, int max)
//{
//   if(max == 0)
//   {
//      progressBar->hide();
//      cancelButton->setText(tr("&Close"));
//      cancelButton->setIcon(QIcon(":/images/window-close.png"));
//   }
//   else
//      progressBar->setRange(min, max);

//   progressBar->setValue(0);
//}

//void TransProgressDialog::open(QSerialPort *port)
//{
//   if(port != nullptr)
//   {
//      canceled = false;
//      show();
//      comPort = port;
//      timer->start(20);
//   }
//}

//void TransProgressDialog::updateLeds()
//{
//   timer->stop();
//   if(comPort == nullptr)
//      return;

//   if(!comPort->isOpen())
//      return;

//   QSerialPort::PinoutSignals status = comPort->pinoutSignals();
//   ctsLabel->setEnabled(status & QSerialPort::ClearToSendSignal);
//   dsrLabel->setEnabled(status & QSerialPort::DataSetReadySignal);
//   dcdLabel->setEnabled(status & QSerialPort::DataCarrierDetectSignal);
//   rtsLabel->setEnabled(status & QSerialPort::RequestToSendSignal);
//   dtrLabel->setEnabled(status & QSerialPort::DataTerminalReadySignal);

//   timer->start();
//}
