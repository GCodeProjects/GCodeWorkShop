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

    if(logFile.isOpen())
    {
        logFile.flush();
        logFile.close();
    };

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


    msgBox.setWindowTitle(tr("EdytorNC - serial transmission"));
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::Critical);
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

    serialPortReadBuffer.append(buff);

    setLabelText(tr("Receiving byte %1").arg(serialPortReadBuffer.size() - 1));

    //qDebug() << "Data read" << buff << "xoffReceived" << xoffReceived;

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
    createLogFile = settings.value("CreateLogFile", true).toBool();

    autoSave = settings.value("AutoSave", false).toBool();
    endOfProgChar = settings.value("EndOfProgChar", "M30").toString();
    renameIfExists = settings.value("CreateBackup", true).toBool();
    removeLetters = settings.value("RemoveLetters", true).toBool();
    guessFileNameByProgName = settings.value("DetectFormFileName", true).toBool();
    appendExt = settings.value("AppendExtension", false).toBool();
    useAsExt = settings.value("UseAsExtension", false).toBool();
    splitPrograms = settings.value("SplitPrograms", true).toBool();
    savePath = settings.value("SavePath", "").toString();
    saveExt = settings.value("SaveExt", ".nc").toString();



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


    // log file
    if((!savePath.isEmpty()) && createLogFile)
    {
        logFile.setFileName(savePath+ "/" + configName + "_serial_log" + ".txt");
        if(!logFile.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            qDebug() << "Cannot open log file";
        };

    };

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

QStringList SerialTransmissionDialog::receiveData(QString configName)
{
    QStringList outputList;

    outputList.clear();
    if(configName.isEmpty())
        return outputList;

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
        return outputList;
    };

    setLabelText(tr("Receiving byte %1").arg(0));

    exec();

    return processReceivedData();
}

//**************************************************************************************************
//
//**************************************************************************************************

QStringList SerialTransmissionDialog::processReceivedData()
{
    QString readData;
    QStringList outputList;
    QString eobString;
    qint64 j;
    qint64 i;


    outputList.clear();
    if(serialPortReadBuffer.isEmpty())
        return outputList;


    i = serialPortReadBuffer.size();
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
                readData.append("\r\n");
                eobString.clear();
            }
            else
                if(eobString.contains("\r\n"))
                {
                    readData.append("\r\n");
                    eobString.clear();
                };
        }
        else
        {
            if(!eobString.isEmpty()) //unknown EOB codes or only LF
            {
                if(eobString.contains("\n"))
                    eobString.replace("\n", "\r\n");

                readData.append(eobString);
                eobString.clear();
            };

            readData.append(serialPortReadBuffer.at(j));
        };
    };

    if(removeSpaceEOB) //removes white space at end of line added by Fanuc
    {
        if(readData.contains("\r\n"))
            readData.replace(" \r\n", "\r\n");
        else
            readData.replace(" \n", "\r\n");
    };

    if(removeEmptyLines)
    {
        QRegExp exp("[\\n]{2,}");
        //exp.setPattern("[\\n]{2,}");
        int i = 0;
        while(i >= 0)
        {
            i = readData.indexOf(exp, 0);
            if(i >= 0)
            {
                readData.replace(exp, "\r\n");
            };
        };
    };

    if(!autoSave)
    {
        outputList.append(readData);
        return outputList;
    }
    else
        outputList.append("#FILE_LIST#");

    if(splitPrograms) // check for more than one program in received data
    {
        QStringList progList = splitFile(&readData);

        QStringList::const_iterator it = progList.constBegin();
        while(it != progList.constEnd())
        {
            readData.clear();
            readData.append(*it);
            outputList.append(saveDataToFile(&readData));
            it++;
        };
    }
    else
        outputList.append(saveDataToFile(&readData));

    return outputList;
}

//**************************************************************************************************
// Tries to guess the filename
//**************************************************************************************************

QString SerialTransmissionDialog::guessFileName(QString *text)
{
    QString fileName, extension;
    int pos;
    QRegExp expression;

    if(text->isEmpty())
        return "";

    if(guessFileNameByProgName)
    {
        forever
        {
            expression.setPattern(FILENAME_SINU840);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove("%_N_");
                //fileName.remove(QRegExp("_(MPF|SPF|TEA|COM|PLC|DEF|INI)"));

                expression.setPattern("_(MPF|SPF|TEA|COM|PLC|DEF|INI)");
                pos = fileName.indexOf(expression);
                if(pos >= 0)
                {
                    extension = fileName.mid(pos, expression.matchedLength());
                    fileName.remove(extension);
                    extension.remove(" ");
                    extension.replace('_', '.');;
                }
                else
                   extension.clear();

                qDebug() << "3" << fileName << extension;
                break;
            };

            expression.setPattern(FILENAME_OSP);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove("$");

                expression.setPattern("\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}");
                pos = fileName.indexOf(expression);
                if(pos >= 0)
                {
                    extension = fileName.mid(pos, expression.matchedLength());
                    qDebug() << "10.1" << fileName << extension << pos;
                    fileName.remove(extension);
                    extension.remove(" ");
                    extension.remove("%");
                }
                else
                   extension.clear();

                fileName.remove(QRegExp("[%]{0,1}"));
                //fileName.remove(QRegExp(".(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"));
                qDebug() << "10" << fileName << extension;
                break;
            };

            expression.setPattern(FILENAME_SINU);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());
                //fileName.remove(QRegExp("%(MPF|SPF|TEA)[\\s]{0,3}"));

                expression.setPattern("%(MPF|SPF|TEA)[\\s]{0,3}");
                pos = fileName.indexOf(expression);
                if(pos >= 0)
                {
                    extension = fileName.mid(pos, expression.matchedLength());
                    fileName.remove(extension);
                    extension.remove(" ");
                    extension.remove("%");
                    extension.prepend('.');
                }
                else
                   extension.clear();


                //fileName.append(ext);
                qDebug() << "11" << fileName << extension;
                break;
            };

            expression.setPattern(FILENAME_PHIL);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove(QRegExp("%PM[\\s]{1,}[N]{1,1}"));
                extension.clear();
                qDebug() << "12" << fileName << extension;
                break;
            };

            expression.setPattern(FILENAME_FANUC);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());

                if(fileName.at(0)!='O')
                    fileName[0]='O';
                if(fileName.at(0)=='O' && fileName.at(1)=='O')
                    fileName.remove(0,1);

                extension.clear();
                qDebug() << "13" << fileName << extension;
                break;
            }

            expression.setPattern(FILENAME_HEID1);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove("%");
                fileName.remove(QRegExp("\\s"));
                extension.clear();
                qDebug() << "14" << fileName << extension;
                break;
            };

            expression.setPattern(FILENAME_HEID2);
            pos = text->indexOf(expression);
            if(pos >= 0)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove(QRegExp("(BEGIN)(\\sPGM\\s)"));
                fileName.remove(QRegExp("(\\sMM|\\sINCH)"));
                extension.clear();
                qDebug() << "15" << fileName << extension;
                break;
            };

            fileName = "";
            break;
        };
    }
    else
    {
        forever
        {
            expression.setPattern("(;)[\\w:*=+ -/]{4,64}");
            pos = text->indexOf(expression);
            if(pos >= 2)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove(";");

                pos = fileName.lastIndexOf('.');
                if(pos >= 0)
                {
                    extension = fileName.mid(pos, expression.matchedLength());
                    fileName.remove(expression);
                    extension.remove(" ");
                }
                else
                    extension.clear();

                qDebug() << "16" << fileName << extension;
                break;
            };

            expression.setPattern("(\\()[\\w:*=+ -/]{4,64}(\\))");
            pos = text->indexOf(expression);
            if(pos >= 2)
            {
                fileName = text->mid(pos, expression.matchedLength());
                fileName.remove("(");
                fileName.remove(")");

                pos = fileName.lastIndexOf('.');
                if(pos >= 0)
                {
                    extension = fileName.mid(pos, expression.matchedLength());
                    fileName.remove(expression);
                    extension.remove(" ");
                }
                else
                    extension.clear();

                qDebug() << "17" << fileName << extension;
                break;
            };

            fileName = "";
            break;

        };

    };

    fileName.remove(".");
    fileName.remove(",");

    qDebug() << "19" << fileName << extension << appendExt << useAsExt;

    if(fileName.isEmpty())
        return fileName;

    if(removeLetters)
    {
        QString tmpName = fileName;
        tmpName.remove(QRegExp("[a-zA-Z-.]{1,}"));
        tmpName = tmpName.simplified();
        tmpName = tmpName.trimmed();
        if(!tmpName.isEmpty()) // do not allow empty file name at this point
            fileName = tmpName;
    };

    if(useAsExt && (!extension.isEmpty()))
        fileName = savePath + "/" + fileName + extension;
    else
    {
        if(appendExt)
            fileName = savePath + "/" + fileName + extension.replace('.', '_') + saveExt;
        else
            fileName = savePath + "/" + fileName + saveExt;
    };

    fileName = fileName.simplified();
    fileName = fileName.trimmed();

    qDebug() << "19.1" << fileName << extension << appendExt << useAsExt;

    return fileName;
}

//**************************************************************************************************
//  Save received program to file. Return filename and empty program text if succes or leave program text unchanged
//**************************************************************************************************

QString SerialTransmissionDialog::saveDataToFile(QString *text)
{
    QFile file;

    if(!autoSave)
        return "";

    if(text->isNull() || text->isEmpty())
        return "";

    QString dateTime = QDate::currentDate().toString(Qt::DefaultLocaleShortDate) + " " + QTime::currentTime().toString(Qt::DefaultLocaleLongDate).remove(QRegularExpression(" \\w+"));

    QString fileName = guessFileName(text);


    if(fileName.isEmpty())
    {
        fileName = savePath + "/" + dateTime + saveExt;
        writeLog(tr("WARNING:\t Coulnd not find program name.\r\n"), dateTime);
        qDebug() << "18" << fileName;
    };


    qDebug() << "20" << fileName;
    file.setFileName(fileName);

    if(file.exists() && renameIfExists)
    {
        QString oldName = fileName;
        oldName.replace(saveExt, ".bak");
        QFile::remove(oldName);

        if(file.rename(fileName, oldName))
        {
            // write to log file
            writeLog(tr("SUCCES:\t Renaming file: \"%1\" to \"%2\".\r\n").arg(fileName).arg(oldName), dateTime);
        }
        else
        {
            // write error to log file
            writeLog(tr("ERROR:\t Renaming file: \"%1\". %2\r\n").arg(fileName).arg(file.errorString()), dateTime);
        };
    };

    if(file.open(QIODevice::WriteOnly))
    {
        // save received data to file
        QTextStream out(&file);
        out << *text;
        file.close();

        text->clear(); // return empty text


        // write to log file
        writeLog(tr("SUCCES:\t Saving file: \"%1\".\r\n").arg(fileName), dateTime);
    }
    else
    {
        // write error to log file
        writeLog(tr("ERROR:\t Saving file: \"%1\". %2\r\n").arg(fileName).arg(file.errorString()), dateTime);
    };

    return fileName;

}

//**************************************************************************************************
//  Write to log file
//**************************************************************************************************

void SerialTransmissionDialog::writeLog(QString msg, QString timeStamp)
{
    QByteArray text;

    if(logFile.isOpen())
    {
        if(timeStamp.isEmpty())
        {
            text.append(QDate::currentDate().toString(Qt::DefaultLocaleShortDate) + " " + QTime::currentTime().toString(Qt::DefaultLocaleLongDate).remove(QRegularExpression(" \\w+")));
        }
        else
            text.append(timeStamp);

        text.append("\t");
        logFile.write(text, text.length());

        text.clear();
        text.append(msg);
        logFile.write(text, text.length());
    };
}

//**************************************************************************************************
// Split file
//**************************************************************************************************

QStringList SerialTransmissionDialog::splitFile(QString *text)
{
   int progBegin, progEnd;
   QStringList progs, exp;
   QList<int> progBegins;
   int index;
   QString tx;

   progs.clear();
   if(text->isNull() || text->isEmpty())
       return progs;


   exp << FILENAME_SINU840
       << FILENAME_OSP
       << FILENAME_FANUC
       << FILENAME_SINU
       << FILENAME_HEID1
       << FILENAME_HEID2
       << FILENAME_PHIL;


   // detect CNC control type
   foreach(const QString expTx, exp)
   {
       QRegExp expression(expTx);
       if(text->contains(expression))
       {
           exp.clear();
           exp.append(expTx);
           break;
       };
   };


   //  prepare program list
   index = 0;
   foreach(const QString expTx, exp)
   {
      QRegExp expression(expTx);
      do
      {
         index = text->indexOf(expression, index);
         if(index >= 0)
         {
            progBegins.append(index);
            index += expression.matchedLength();
         }
         else
            index = 0;

      }while(index > 0);
   };
   qSort(progBegins.begin(), progBegins.end());


   // split file  TODO: data can be lost if filename detection fails
   QList<int>::const_iterator it = progBegins.constBegin();
   while(it != progBegins.constEnd())
   {
      progBegin = *it;
      it++;
      if(it != progBegins.constEnd())
         progEnd = *it;
      else
         progEnd = text->size();

      tx = text->mid(progBegin, progEnd - progBegin);
      if(!tx.isEmpty())
      {
         progs.append(tx);
      };
   };

   return progs;
}

//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************



