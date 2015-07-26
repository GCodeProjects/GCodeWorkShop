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



#include "serialtransmissiondialog.h"



SerialTransmissionDialog::SerialTransmissionDialog(QWidget *parent) : QDialog(parent)
{
   setupUi(this);
   setWindowTitle(tr("Serial transmission"));
   setModal(true);

   canceled = false;
   xoffReceived = false;
   prevXoffReceived = false;
   autoCloseCountnerReloadValue = 15;
   autoCloseCountner = 15;


   connect(cancelButton, SIGNAL(clicked()), SLOT(cancelButtonClicked()));
   connect(this, SIGNAL(rejected()), SLOT(cancelButtonClicked()));

   updateLedsTimer = new QTimer(this);
   connect(updateLedsTimer, SIGNAL(timeout()), this, SLOT(updateLeds()));

   autoCloseTimer = new QTimer(this);
   autoCloseTimer->setInterval(1000);
   autoCloseTimer->stop();
   connect(autoCloseTimer, SIGNAL(timeout()), this, SLOT(autoClose()));

   updateLedsTimer->start(40);

}

//**************************************************************************************************
//
//**************************************************************************************************

SerialTransmissionDialog::~SerialTransmissionDialog()
{
    updateLedsTimer->stop();
    autoCloseTimer->stop();

    if(serialPort.isOpen())
    {
        serialPort.flush();
        serialPort.close();
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::closeEvent(QCloseEvent *event)
{
   updateLedsTimer->stop();
   autoCloseTimer->stop();
   canceled = true;
   event->accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::autoClose()
{
    if(autoCloseCountnerReloadValue == 0)
    {
        autoCloseTimer->stop();
        return;
    };

    if(autoCloseCountner <= 1)
    {
        cancelButtonClicked();
    }
    else
    {
        if(!xoffReceived)
            autoCloseCountner--;
    };

    if(autoCloseCountner <= (autoCloseCountnerReloadValue - 2))
        cancelButton->setText(tr("Auto &closing in %1s").arg(autoCloseCountner));
    else
       cancelButton->setText(tr("&Close"));
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::cancelButtonClicked()
{
   updateLedsTimer->stop();
   autoCloseTimer->stop();
   canceled = true;
   close();
}

//**************************************************************************************************
//
//**************************************************************************************************

bool SerialTransmissionDialog::wasCanceled()
{
   return canceled;
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::setLabelText(const QString text)
{
   label->setText(text);
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::setValue(int val)
{
   progressBar->setValue(val);
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::setRange(int min, int max)
{
   if(max == 0)
   {
      progressBar->hide();
      cancelButton->setText(tr("&Close"));
      cancelButton->setIcon(QIcon(":/images/window-close.png"));
   }
   else
      progressBar->setRange(min, max);

   progressBar->setValue(0);
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::updateLeds()
{
   updateLedsTimer->stop();

   if(!serialPort.isOpen())
      return;

   QSerialPort::PinoutSignals status = serialPort.pinoutSignals();


   ctsLabel->setEnabled(status & QSerialPort::ClearToSendSignal);
   dsrLabel->setEnabled(status & QSerialPort::DataSetReadySignal);
   dcdLabel->setEnabled(status & QSerialPort::DataCarrierDetectSignal);
   rtsLabel->setEnabled(status & QSerialPort::RequestToSendSignal);
   dtrLabel->setEnabled(status & QSerialPort::DataTerminalReadySignal);
   //rngLabel->setEnabled(status & QSerialPort::RingIndicatorSignal);


   if(sendStartDelay > 0)
       setLabelText(tr("Start in %1s").arg(sendStartDelay));
   else
       if(xoffReceived || !(status & QSerialPort::ClearToSendSignal))
       {
           setLabelText(tr("Waiting for a signal readiness..."));
       };


   if((xoffReceived || prevXoffReceived))  // catch change of xoffReceived
   {
       prevXoffReceived = xoffReceived;
       if(!xoffReceived) // try to restart trasmission
           if(serialPort.bytesToWrite() == 0)
               serialPortBytesWritten(0);

       qDebug() << "CHANGED xoffReceived" << xoffReceived;
   };


   updateLedsTimer->start();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::sendStartDelayTimeout()
{
    if(sendStartDelay > 0)
        sendStartDelay--;
    else
        xoffReceived = false;
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::stopButtonClicked()
{
    stop = true;
    qApp->processEvents();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::showSerialPortError(QSerialPort::SerialPortError error)
{
    QString text;
    QMessageBox msgBox;

    qDebug() << "Serial port error: " << error << " " << serialPort.errorString();

    switch(error)
    {
        case QSerialPort::NoError                   : text = tr("No Error has occured");
            //statusBar()->showMessage(text);
            return;
        case QSerialPort::DeviceNotFoundError       : text = tr("An error occurred while attempting to open an non-existing serial port.");
            stop = true;
            break;
        case QSerialPort::PermissionError           : text = tr("An error occurred while attempting to open an already opened serial port by another process or a user not having enough permission and credentials to open.");
            stop = true;
            break;
        case QSerialPort::OpenError                 : text = tr("An error occurred while attempting to open an already opened serial port.");
            stop = true;
            break;
        case QSerialPort::NotOpenError              : text = tr("Serial port is not opened.");
            stop = true;
            break;
        case QSerialPort::ParityError               : text = tr("Parity error detected by the hardware while reading data");
            stop = true;
            break;
        case QSerialPort::FramingError              : text = tr("Framing error detected by the hardware while reading data");
            stop = true;
            break;
        case QSerialPort::BreakConditionError       : text = tr("Break condition detected by the hardware on the input line");
            stop = true;
            break;
        case QSerialPort::WriteError                : text = tr("An I/O error occurred while writing the data");
            break;
        case QSerialPort::ReadError                 : text = tr("An I/O error occurred while reading the data");
            stop = true;
            break;
        case QSerialPort::ResourceError             : text = tr("An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system");
            stop = true;
            break;
        case QSerialPort::UnsupportedOperationError : text = tr("The requested serial port operation is not supported or prohibited by the running operating system.");
            stop = true;
            break;
        case QSerialPort::TimeoutError              : text = tr("A timeout error occurred.");
           stop = true;
           break;
        case QSerialPort::UnknownError              : text = tr("An unidentified error occurred");
           stop = true;
           break;
        default                                     : text = tr("An unidentified error occurred");
    };


    //statusBar()->showMessage(text);
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::serialPortBytesWritten(qint64 bytes)
{
    bytesWritten += bytes;

    setValue(bytesWritten - 1);
    setLabelText(tr("Sending byte %1 of %2").arg(bytesWritten - 1).arg(noOfBytes));

    qDebug() << "Bytes written" << bytesWritten << " of " << noOfBytes;

    if(stop)
        return;

    if(it == serialPortWriteBuffer.end())
    {
        stop = true;
        autoCloseCountner = autoCloseCountnerReloadValue;
        if(!autoCloseTimer->isActive())
            autoCloseTimer->start();
        return;
    };

    if((lineDelay > 0) && (bytes > 0))
    {
        QTimer::singleShot(int(lineDelay * 1000), this, SLOT(lineDelaySlot()));
        return;
    };

    if(!xoffReceived)
    {
        QByteArray buff;
        buff.clear();
        buff.append(*it);

        if(endOfBlockLF) // insert line endings
            buff.append("\n");
        else
            buff.append("\r\n");

        //qDebug() << "W data:" << buff;

        serialPort.write(buff, buff.size());
        it++;

    };

    qDebug() << "xoffReceived" << xoffReceived << " Stop" << stop;

    autoCloseCountner = autoCloseCountnerReloadValue;
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::serialPortReadyRead()
{
    QByteArray buff(serialPort.readAll());

    int xoffPos = buff.lastIndexOf(portSettings.Xoff);
    int xonPos = buff.lastIndexOf(portSettings.Xon);

    if(xoffPos >= 0) //only XOFF received
    {
        xoffReceived = true;
        setLabelText(tr("XOFF received..."));
    };

    if(xonPos >= 0)  //only XON received
    {
        xoffReceived = false;
        sendStartDelay = 0;
        setLabelText(tr("XON received..."));
    };

    if(xoffPos > xonPos) //both XOFF/XON received but XOFF last
    {
        xoffReceived = true;
        setLabelText(tr("XOFF received..."));
    };


    //buff.remove(portSettings.Xoff);
    //buff.remove(portSettings.Xon);
    serialPortReadBuffer.append(buff);

    setLabelText(tr("Receiving byte %1").arg(serialPortReadBuffer.size() - 1));

    qDebug() << "Data read" << buff << "xoffReceived" << xoffReceived;

    autoCloseCountner = autoCloseCountnerReloadValue;
    if(!autoCloseTimer->isActive())
        autoCloseTimer->start();

}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::serialPortRequestToSend(bool set)
{
    qDebug() << "Request To Send RTS" << set;
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::sendData(QString dataToSend, QString configName)
{
    QTimer *sendStartDelayTimer = NULL;

    if(dataToSend.isEmpty())
        return;

    if(configName.isEmpty())
        return;

    loadConfig(configName);

    bytesWritten = 0;
    xoffReceived = false;
    prevXoffReceived = false;
    stop = false;
    serialPortReadBuffer.clear();
    serialPortWriteBuffer.clear();
    autoCloseTimer->stop();


    // prepare data to send
    if(removeBefore && (dataToSend.count('%') > 1))
    {
        dataToSend.remove(0, dataToSend.indexOf("%"));
    };

    if(!sendAtBegining.isEmpty())
        dataToSend.prepend(sendAtBegining);

    if(!sendAtEnd.isEmpty())
        dataToSend.append(sendAtEnd);

    if(dataToSend.contains("\r\n"))
        dataToSend.replace("\r\n", "\n");

    serialPortWriteBuffer = dataToSend.split("\n", QString::KeepEmptyParts);
    noOfBytes = dataToSend.length();

    if(!endOfBlockLF) // add line endings bytes
        noOfBytes += (serialPortWriteBuffer.size());
    setRange(0, noOfBytes);


    // prepare serial port
    serialPort.setPortName(portName);

    connect(&serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(showSerialPortError(QSerialPort::SerialPortError)));
    connect(&serialPort, SIGNAL(bytesWritten(qint64)), SLOT(serialPortBytesWritten(qint64)));
    connect(&serialPort, SIGNAL(readyRead()), SLOT(serialPortReadyRead()));

    serialPort.setBaudRate(portSettings.BaudRate, QSerialPort::Output);
    serialPort.setBaudRate(portSettings.BaudRate, QSerialPort::Input);
    serialPort.setDataBits(portSettings.DataBits);
    serialPort.setParity(portSettings.Parity);
    serialPort.setStopBits(portSettings.StopBits);
    serialPort.setFlowControl(portSettings.FlowControl);

    if(portSettings.FlowControl == QSerialPort::HardwareControl)
        connect(&serialPort, SIGNAL(requestToSendChanged(bool)), SLOT(serialPortRequestToSend(bool)));

    if(serialPort.open(QIODevice::ReadWrite))
    {
        stop = false;
        serialPort.clear(QSerialPort::Output);
        serialPort.clear(QSerialPort::Input);
    }
    else
    {
        stop = true;
        return;
    };

    if(sendStartDelay > 0)
    {
        sendStartDelayTimer = new QTimer(this);
        connect(sendStartDelayTimer, SIGNAL(timeout()), this, SLOT(sendStartDelayTimeout()));
        sendStartDelayTimer->setInterval(1000);
        xoffReceived = true;
        prevXoffReceived = true;
        sendStartDelayTimer->start();
    };

    if((sendStartDelay == 0) && (portSettings.FlowControl == QSerialPort::SoftwareControl))
    {
        xoffReceived = true;
        prevXoffReceived = true;
    };

    it = serialPortWriteBuffer.begin();
    serialPortBytesWritten(0);  // start
    exec();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::loadConfig(QString configName)
{
    QString port, fTx;
    int pos;
    QRegExp exp;
    char chr;
    bool ok;


    stop = true;
    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

#ifdef Q_OS_WIN32
    port = "COM1";
#else
    port = "/dev/ttyUSB0";
#endif

    settings.beginGroup(configName);


    portName = settings.value("PortName", port).toString();
    portSettings.BaudRate = (QSerialPort::BaudRate) settings.value("BaudRate", QSerialPort::Baud9600).toInt();
    portSettings.DataBits = (QSerialPort::DataBits) settings.value("DataBits", QSerialPort::Data8).toInt();
    portSettings.StopBits = (QSerialPort::StopBits) settings.value("StopBits", QSerialPort::TwoStop).toInt();
    portSettings.Parity = (QSerialPort::Parity) settings.value("Parity", QSerialPort::NoParity).toInt();
    portSettings.FlowControl = (QSerialPort::FlowControl) settings.value("FlowControl", QSerialPort::HardwareControl).toInt();
    lineDelay = settings.value("LineDelay", 0).toDouble();
    portSettings.Xon = settings.value("Xon", "17").toString().toInt(&ok, 10);
    portSettings.Xoff = settings.value("Xoff", "19").toString().toInt(&ok, 10);
    sendAtEnd = settings.value("SendAtEnd", "").toString();
    sendAtBegining = settings.value("SendAtBegining", "").toString();
    deleteControlChars = settings.value("DeleteControlChars", true).toBool();
    removeEmptyLines = settings.value("RemoveEmptyLines", true).toBool();
    removeBefore = settings.value("RemoveBefore", false).toBool();
    endOfBlockLF = settings.value("EndOfBlockLF", false).toBool();
    removeSpaceEOB = settings.value("RemoveSpaceEOB", false).toBool();
    autoCloseCountnerReloadValue = settings.value("AutoCloseTime", 15).toInt();
    sendStartDelay = settings.value("SendingStartDelay", 0).toInt();
    doNotShowProgressInEditor = settings.value("DoNotShowProgressInEditor", false).toBool();
    recieveTimeout = settings.value("RecieveTimeout", 0).toInt();


    settings.endGroup();
    settings.endGroup();

    portSettings.Timeout_Millisec = 50;

    exp.setPattern("0x[0-9a-fA-F]{1,2}");
    pos = 0;
    while((pos = sendAtBegining.indexOf(exp, pos)) >= 0)
    {
        fTx = sendAtBegining.mid(pos, exp.matchedLength());
        chr = fTx.toInt(&ok, 16);
        sendAtBegining.replace(pos, exp.matchedLength(), QString(chr));
    };
    sendAtBegining.remove(" ");

    pos = 0;
    while((pos = sendAtEnd.indexOf(exp, pos)) >= 0)
    {
        fTx = sendAtEnd.mid(pos, exp.matchedLength());
        chr = fTx.toInt(&ok, 16);
        sendAtEnd.replace(pos, exp.matchedLength(), QString(chr));
    };
    sendAtEnd.remove(" ");
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::lineDelaySlot()
{
    serialPortBytesWritten(0);
}

//**************************************************************************************************
//
//**************************************************************************************************

void SerialTransmissionDialog::receiveData(QString *readData, QString configName)
{
    if(configName.isEmpty())
        return;

    loadConfig(configName);

    bytesWritten = 0;
    xoffReceived = false;
    prevXoffReceived = false;
    stop = false;
    serialPortReadBuffer.clear();
    serialPortWriteBuffer.clear();
    it = serialPortWriteBuffer.end(); // we don't send anything
    autoCloseTimer->stop();

    setRange(0, 0); // do not display progress bar


    // prepare serial port
    serialPort.setPortName(portName);


    connect(&serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(showSerialPortError(QSerialPort::SerialPortError)));
    //connect(serialPort, SIGNAL(bytesWritten(qint64)), SLOT(serialPortBytesWritten(qint64)));
    connect(&serialPort, SIGNAL(readyRead()), SLOT(serialPortReadyRead()));

    if(portSettings.FlowControl == QSerialPort::HardwareControl)
        connect(&serialPort, SIGNAL(requestToSendChanged(bool)), SLOT(serialPortRequestToSend(bool)));

    serialPort.setBaudRate(portSettings.BaudRate, QSerialPort::Output);
    serialPort.setBaudRate(portSettings.BaudRate, QSerialPort::Input);
    serialPort.setDataBits(portSettings.DataBits);
    serialPort.setParity(portSettings.Parity);
    serialPort.setStopBits(portSettings.StopBits);
    serialPort.setFlowControl(portSettings.FlowControl);


    if(serialPort.open(QIODevice::ReadWrite))
    {
        stop = false;
        serialPort.clear(QSerialPort::Output);
        serialPort.clear(QSerialPort::Input);
    }
    else
    {
        stop = true;
        return;
    };



    setLabelText(tr("Receiving byte %1").arg(0));

    exec();

    readData->clear();

    if(serialPortReadBuffer.isEmpty())
        return;

    QString eobString;
    qint64 j;
    qint64 i = serialPortReadBuffer.size();

    for(j = 0; j < i; j++)
    {
        if(deleteControlChars)
            if(((serialPortReadBuffer.at(j) <= 0x1F) || (serialPortReadBuffer.at(j) >= 0x7F))) // is control character (below 0x1F and above 0x7F)
                if((serialPortReadBuffer.at(j) != 0x0A) && (serialPortReadBuffer.at(j) != 0x0D)) // but not LF or CR
                    continue; //skip this character

        if((serialPortReadBuffer.at(j) == 0x0A) || (serialPortReadBuffer.at(j) == 0x0D))
        {
            eobString.append(serialPortReadBuffer.at(j));
            if(eobString.contains("\n\r\r")) //known EOB codes
            {
                readData->append("\r\n");
                eobString.clear();
            }
            else
                if(eobString.contains("\r\n"))
                {
                    readData->append("\r\n");
                    eobString.clear();
                };
        }
        else
        {
            if(!eobString.isEmpty()) //unknown EOB codes or only LF
            {
                if(eobString.contains("\n"))
                    eobString.replace("\n", "\r\n");

                readData->append(eobString);
                eobString.clear();
            };

            readData->append(serialPortReadBuffer.at(j));
        };
    };

    if(removeSpaceEOB) //removes white space at end of line added by Fanuc
    {
        if(readData->contains("\r\n"))
            readData->replace(" \r\n", "\r\n");
        else
            readData->replace(" \n", "\r\n");
    };

    if(removeEmptyLines)
    {
        QRegExp exp("[\\n]{2,}");
        //exp.setPattern("[\\n]{2,}");
        int i = 0;
        while(i >= 0)
        {
            i = readData->indexOf(exp, 0);
            if(i >= 0)
            {
                readData->replace(exp, "\r\n");
            };
        };
    };
}

//**************************************************************************************************
//
//**************************************************************************************************


