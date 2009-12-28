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



#include "serialtransmission.h"





SPConfigDialog::SPConfigDialog(QWidget *parent, QString confName, Qt::WindowFlags f) : QDialog(parent, f)
{
   setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Serial port configuration"));
   setModal(TRUE);


   //portNameComboBox->setAutoCompletion(TRUE);
   //configNameBox->setAutoCompletion(TRUE);

   configName = confName;

   baudGroup = new QButtonGroup(this);
   baudGroup->setExclusive(TRUE);
   baudGroup->addButton(b1CheckBox, BAUD300);
   baudGroup->addButton(b2CheckBox, BAUD600);
   baudGroup->addButton(b3CheckBox, BAUD1200);
   baudGroup->addButton(b4CheckBox, BAUD2400);
   baudGroup->addButton(b5CheckBox, BAUD4800);
   baudGroup->addButton(b6CheckBox, BAUD9600);
   baudGroup->addButton(b7CheckBox, BAUD19200);
   baudGroup->addButton(b8CheckBox, BAUD38400);
   baudGroup->addButton(b9CheckBox, BAUD57600);


   dataBitsGroup = new QButtonGroup(this);
   dataBitsGroup->setExclusive(TRUE);
   dataBitsGroup->addButton(d1CheckBox, DATA_5);
   dataBitsGroup->addButton(d2CheckBox, DATA_6);
   dataBitsGroup->addButton(d3CheckBox, DATA_7);
   dataBitsGroup->addButton(d4CheckBox, DATA_8);


   stopBitsGroup = new QButtonGroup(this);
   stopBitsGroup->setExclusive(TRUE);
   stopBitsGroup->addButton(s1CheckBox, STOP_1);
   stopBitsGroup->addButton(s2CheckBox, STOP_2);


   parityGroup = new QButtonGroup(this);
   parityGroup->setExclusive(TRUE);
   parityGroup->addButton(p1CheckBox, PAR_NONE);
   parityGroup->addButton(p2CheckBox, PAR_ODD);
   parityGroup->addButton(p3CheckBox, PAR_EVEN);



   flowCtlGroup = new QButtonGroup(this);
   flowCtlGroup->setExclusive(TRUE);
   flowCtlGroup->addButton(f1CheckBox, FLOW_OFF);
   flowCtlGroup->addButton(f2CheckBox, FLOW_HARDWARE);
   flowCtlGroup->addButton(f3CheckBox, FLOW_XONXOFF);



   /*xOnInput->setMaxLength(7);
   QValidator *xOnInputValid = new QIntValidator(0, 128, this);
   xOnInput->setValidator(xOnInputValid);

   xOffInput->setMaxLength(7);
   QValidator *xOffInputValid = new QIntValidator(0, 128, this);
   xOffInput->setValidator(xOffInputValid);


   eobInput->setMaxLength(7);

   delayInput->setMaxLength(7);
   QValidator *delayInputValid = new QIntValidator(0, 30, this);
   delayInput->setValidator(delayInputValid);

   eotInput->setMaxLength(7);*/


#ifdef Q_OS_WIN32
   browseButton->setEnabled(FALSE);
#else
   connect(browseButton, SIGNAL(clicked()), SLOT(browseButtonClicked()));
#endif

   connect(saveButton, SIGNAL(clicked()), SLOT(saveButtonClicked()));
   connect(saveCloseButton, SIGNAL(clicked()), SLOT(saveCloseButtonClicked()));
   //connect(loadButton, SIGNAL(clicked()), SLOT(loadButtonClicked()));
   connect(deleteButton, SIGNAL(clicked()), SLOT(deleteButtonClicked()));
   connect(closeButton, SIGNAL(clicked()), SLOT(closeButtonClicked()));


   loadSettings();

   connect(configNameBox, SIGNAL(currentIndexChanged(int)), SLOT(changeSettings()));

   setResult(QDialog::Rejected);
   //setMaximumSize(width(), height());

}

//**************************************************************************************************
//
//**************************************************************************************************

SPConfigDialog::~SPConfigDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::browseButtonClicked()
{

   QFileInfo file;

   QString fileName = QFileDialog::getOpenFileName(
                         this,
                         tr("Select serial port device"),
                         portNameComboBox->currentText(),
                         tr("All files (*)"));

   file.setFile(fileName);

   if((file.exists()) && (file.isReadable()))
   {
      portNameComboBox->addItem(fileName);
      portNameComboBox->setCurrentIndex(portNameComboBox->count() - 1);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::saveButtonClicked()
{
    QStringList list;
    QString item, curItem;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

    curItem = configNameBox->currentText();

    list.clear();
    list.prepend(portNameComboBox->currentText());
    for(int i = 0; i <= portNameComboBox->count(); i++)
    {
       item = portNameComboBox->itemText(i);
       if(!item.isEmpty())
         if(!list.contains(item))
           list.prepend(item);
    };

    while(list.size() > 32)
    {
       list.removeLast();
    };
    settings.setValue("PortNameList", list);

    list.clear();
    list.prepend(configNameBox->currentText());
    for(int i = 0; i <= configNameBox->count(); i++)
    {
       item = configNameBox->itemText(i);
       if(!item.isEmpty())
         if(!list.contains(item))
           list.prepend(item);
    };

    while(list.size() > 64)
    {
       list.removeLast();
    };
    list.sort();

    settings.setValue("SettingsList", list);
    settings.setValue("CurrentSerialPortSettings", configNameBox->currentText());


    settings.beginGroup(configNameBox->currentText());

    settings.setValue("PortName", portNameComboBox->currentText());
    settings.setValue("BaudRate", baudGroup->checkedId());
    settings.setValue("DataBits", dataBitsGroup->checkedId());
    settings.setValue("StopBits", stopBitsGroup->checkedId());
    settings.setValue("Parity", parityGroup->checkedId());
    settings.setValue("FlowControl", flowCtlGroup->checkedId());
    settings.setValue("SendAtEnd", eotInput->text());
    settings.setValue("SendAtBegining", stInput->text());
    settings.setValue("LineDelay", delayDoubleSpinBox->value());
    settings.setValue("Xon", xonInput->text());
    settings.setValue("Xoff", xoffInput->text());


    settings.endGroup();
    settings.endGroup();


    configNameBox->clear();
    configNameBox->addItems(list);
    int id = configNameBox->findText(curItem);
    if(id >= 0)
       configNameBox->setCurrentIndex(id);

}


//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::changeSettings()
{
    int id;
    QString item, port;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");



    settings.beginGroup(configNameBox->currentText());

#ifdef Q_OS_WIN32
    port = "COM1";
#else
    port = "/dev/ttyS0";
#endif

    item = settings.value("PortName", port).toString();
    id = portNameComboBox->findText(item);
    portNameComboBox->setCurrentIndex(id);

    id = settings.value("BaudRate", BAUD9600).toInt();
    switch(id)
    {
           case BAUD300    : b1CheckBox->setChecked(TRUE);
                             break;
           case BAUD600    : b2CheckBox->setChecked(TRUE);
                             break;
           case BAUD1200   : b3CheckBox->setChecked(TRUE);
                             break;
           case BAUD2400   : b4CheckBox->setChecked(TRUE);
                             break;
           case BAUD4800   : b5CheckBox->setChecked(TRUE);
                             break;
           case BAUD9600   : b6CheckBox->setChecked(TRUE);
                             break;
           case BAUD19200  : b7CheckBox->setChecked(TRUE);
                             break;
           case BAUD38400  : b8CheckBox->setChecked(TRUE);
                             break;
           case BAUD57600  : b9CheckBox->setChecked(TRUE);
                             break;
    };

    id = settings.value("DataBits", DATA_8).toInt();
    switch(id)
    {
           case DATA_5    : d1CheckBox->setChecked(TRUE);
                            break;
           case DATA_6    : d2CheckBox->setChecked(TRUE);
                            break;
           case DATA_7    : d3CheckBox->setChecked(TRUE);
                            break;
           case DATA_8    : d4CheckBox->setChecked(TRUE);
                            break;
    };

    id = settings.value("StopBits", STOP_2).toInt();
    switch(id)
    {
           case STOP_1    : s1CheckBox->setChecked(TRUE);
                            break;
           case STOP_2    : s2CheckBox->setChecked(TRUE);
                            break;
    };

    id = settings.value("Parity", PAR_NONE).toInt();
    switch(id)
    {
           case PAR_NONE  : p1CheckBox->setChecked(TRUE);
                            break;
           case PAR_ODD   : p2CheckBox->setChecked(TRUE);
                            break;
           case PAR_EVEN  : p3CheckBox->setChecked(TRUE);
                            break;
    };

    id = settings.value("FlowControl", FLOW_HARDWARE).toInt();
    switch(id)
    {
           case FLOW_OFF       : f1CheckBox->setChecked(TRUE);
                                 break;
           case FLOW_HARDWARE  : f2CheckBox->setChecked(TRUE);
                                 break;
           case FLOW_XONXOFF   : f3CheckBox->setChecked(TRUE);
                                 break;
    };

    eotInput->setText(settings.value("SendAtEnd", "").toString());
    stInput->setText(settings.value("SendAtBegining", "").toString());
    xonInput->setText(settings.value("Xon", "17").toString());
    xoffInput->setText(settings.value("Xoff", "19").toString());

    settings.endGroup();
    settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::loadSettings()
{
    int id;
    QStringList list;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

    portNameComboBox->clear();

#ifdef Q_OS_WIN32
       list << "COM1" << "COM2" << "COM3" << "COM4" <<
              "COM5" << "COM6" << "COM7" << "COM8";
#else
       list << "/dev/ttyS0" << "/dev/ttyS1" << "/dev/ttyS2" << "/dev/ttyS3" <<
              "/dev/ttyUSB0" << "/dev/ttyUSB1" << "/dev/ttyUSB2" << "/dev/ttyUSB3";

#endif
    list = settings.value("PortNameList", list).toStringList();
    list.sort();
    portNameComboBox->addItems(list);


    configNameBox->clear();
    list = settings.value("SettingsList", tr("Default")).toStringList();
    list.sort();
    configNameBox->addItems(list);
    //item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
    id = configNameBox->findText(configName);
    configNameBox->setCurrentIndex(id);
    settings.endGroup();

    changeSettings();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::deleteButtonClicked()
{

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

    settings.beginGroup(configNameBox->currentText());

    settings.remove("PortName");
    settings.remove("BaudRate");
    settings.remove("DataBits");
    settings.remove("StopBits");
    settings.remove("Parity");
    settings.remove("FlowControl");
    settings.remove("Xon");
    settings.remove("Xoff");

    settings.endGroup();
    settings.remove(configNameBox->currentText());

    settings.endGroup();

    int id = configNameBox->findText(configNameBox->currentText());
    configNameBox->removeItem(id);

}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::closeButtonClicked()
{
   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("SerialPortConfigs");
   settings.setValue("CurrentSerialPortSettings", configNameBox->currentText());
   settings.endGroup();

   close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::saveCloseButtonClicked()
{
   saveButtonClicked();
   closeButtonClicked();
   accept();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

TransmissionDialog::TransmissionDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
   setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Serial transmission test"));

   comPort = new QextSerialPort();
   stop = true;

   connect(closeButton, SIGNAL(clicked()), SLOT(closeButtonClicked()));

   connect(clearButton, SIGNAL(clicked()), SLOT(clearButtonClicked()));
   connect(sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));
   connect(reciveButton, SIGNAL(clicked()), SLOT(reciveButtonClicked()));
   connect(configButton, SIGNAL(clicked()), SLOT(configButtonClicked()));
   connect(stopButton, SIGNAL(clicked()), SLOT(stopButtonClicked()));

   connect(setRtsButton, SIGNAL(clicked()), SLOT(setRtsButtonClicked()));
   connect(setDtrButton, SIGNAL(clicked()), SLOT(setDtrButtonClicked()));

   loadSerialConfignames();
   changeSettings();
   connect(configBox, SIGNAL(currentIndexChanged(int)), SLOT(changeSettings()));

   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(updateLeds()));
   timer->start(200);
   progressBar->reset();

}

//**************************************************************************************************
//
//**************************************************************************************************

TransmissionDialog::~TransmissionDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::closeButtonClicked()
{
   stop = true;
   comPort->close();
   close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::clearButtonClicked()
{
   textEdit->clear();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::setDtrButtonClicked()
{
   if(comPort->isOpen())
     comPort->setDtr(setDtrButton->isChecked());
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::setRtsButtonClicked()
{
   if(comPort->isOpen())
     comPort->setRts(setRtsButton->isChecked());
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::sendButtonClicked()
{
   int i;
   QString tx;


   if(comPort->open(QIODevice::WriteOnly))
     stop = false;
   else
   {
      stop = true;
      showError(E_INVALID_FD);
      return;
   };

   showError(E_NO_ERROR);
   reciveButton->setEnabled(FALSE);
   sendButton->setEnabled(FALSE);
   stopButton->setEnabled(TRUE);
   QApplication::setOverrideCursor(Qt::BusyCursor);

   tx = textEdit->toPlainText();
   if(!tx.contains("\r\n"))
      tx.replace("\n", "\r\n");

   progressBar->setRange(0, tx.size());


   for(i = 0; i <= tx.size(); i++)
   {
      qApp->processEvents();
      if(stop)
        break;

      progressBar->setValue(i);

      if(!comPort->putChar(tx[i].toAscii()))
      {
         stop = true;
         showError(E_WRITE_FAILED);
         break;
      };
      errorLabel->setText(tr("Send: %1 bytes.").arg(i));

   };

   comPort->close();
   stopButton->setEnabled(FALSE);
   reciveButton->setEnabled(TRUE);
   sendButton->setEnabled(TRUE);
   QApplication::restoreOverrideCursor();
   progressBar->reset();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::reciveButtonClicked()
{
   QString tx;
   int count, i;
   char buf[1024];

   showError(E_NO_ERROR);
   count = 0;
   if(comPort->open(QIODevice::ReadOnly))
     stop = false;
   else
   {
      stop = true;
      showError(E_INVALID_FD);
      return;
   };
   reciveButton->setEnabled(FALSE);
   sendButton->setEnabled(FALSE);
   stopButton->setEnabled(TRUE);
   QApplication::setOverrideCursor(Qt::BusyCursor);

   while(!stop)
   {
      qApp->processEvents();

      if(comPort->bytesAvailable() > 0)
      {
         i = comPort->read(buf, sizeof(buf));
         if(i < 0)
         {
            stop = true;
            showError(E_READ_FAILED);
            break;
         };
         count += i;
         errorLabel->setText(tr("Recived: %1 bytes.").arg(count));
         tx = buf;
         textEdit->insertPlainText(tx);
      };

   };

   comPort->close();
   stopButton->setEnabled(FALSE);
   reciveButton->setEnabled(TRUE);
   sendButton->setEnabled(TRUE);
   QApplication::restoreOverrideCursor();

}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::updateLeds()
{
   ulong status;

   status = comPort->lineStatus();

   ctsLabel->setEnabled(status & LS_CTS);
   dsrLabel->setEnabled(status & LS_DSR);
   dcdLabel->setEnabled(status & LS_DCD);
   rtsLabel->setEnabled(status & LS_RTS);
   dtrLabel->setEnabled(status & LS_DTR);

   //showError(comPort->lastError());


}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::configButtonClicked()
{
   SPConfigDialog *serialConfigDialog = new SPConfigDialog(this);

   serialConfigDialog->show();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::stopButtonClicked()
{
   stop = true;
   progressBar->reset();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::changeSettings()
{
    QString port;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

#ifdef Q_OS_WIN32
       port = "COM1";
#else
       port = "/dev/ttyS0";

#endif

    settings.beginGroup(configBox->currentText());

    portName = settings.value("PortName", port).toString();
    baudRate = settings.value("BaudRate", BAUD9600).toInt();
    dataBits = settings.value("DataBits", DATA_8).toInt();
    stopBits = settings.value("StopBits", STOP_2).toInt();
    parity = settings.value("Parity", PAR_NONE).toInt();
    flowControl = settings.value("FlowControl", FLOW_HARDWARE).toInt();

    settings.endGroup();
    settings.endGroup();

    comPort->setPortName(portName);
    comPort->setBaudRate(BaudRateType(baudRate));
    comPort->setDataBits(DataBitsType(dataBits));
    comPort->setFlowControl(FlowType(flowControl));
    comPort->setParity(ParityType(parity));
    comPort->setStopBits(StopBitsType(stopBits));

    //comPort->setTimeout(0,100);
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::loadSerialConfignames()
{
   int id;
    QStringList list;
    QString item;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

    configBox->clear();
    list = settings.value("SettingsList", QStringList(tr("Default"))).toStringList();
    configBox->addItems(list);
    item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
    id = configBox->findText(item);
    configBox->setCurrentIndex(id);
    settings.endGroup();
}


//**************************************************************************************************
//
//**************************************************************************************************

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::showError(int error)
{
   switch(error)
   {
      case E_INVALID_FD                   : errorLabel->setText(tr("Invalid file descriptor (port was not opened correctly)"));
                                            break;
      case E_NO_MEMORY                    : errorLabel->setText(tr("Unable to allocate memory tables"));
                                            break;
      case E_CAUGHT_NON_BLOCKED_SIGNAL    : errorLabel->setText(tr("Caught a non-blocked signal"));
                                            break;
      case E_PORT_TIMEOUT                 : errorLabel->setText(tr("Operation timed out"));
                                            break;
      case E_INVALID_DEVICE               : errorLabel->setText(tr("The file opened by the port is not a character device"));
                                            break;
      case E_BREAK_CONDITION              : errorLabel->setText(tr("The port detected a break condition"));
                                            break;
      case E_FRAMING_ERROR                : errorLabel->setText(tr("The port detected a framing error (incorrect baud rate settings ?)"));
                                            break;
      case E_IO_ERROR                     : errorLabel->setText(tr("There was an I/O error while communicating with the port"));
                                            break;
      case E_BUFFER_OVERRUN               : errorLabel->setText(tr("Character buffer overrun"));
                                            break;
      case E_RECEIVE_OVERFLOW             : errorLabel->setText(tr("Receive buffer overflow"));
                                            break;
      case E_RECEIVE_PARITY_ERROR         : errorLabel->setText(tr("The port detected a parity error in the received data"));
                                            break;
      case E_TRANSMIT_OVERFLOW            : errorLabel->setText(tr("Transmit buffer overflow"));
                                            break;
      case E_READ_FAILED                  : errorLabel->setText(tr("General read operation failure"));
                                            break;
      case E_WRITE_FAILED                 : errorLabel->setText(tr("General write operation failure"));
                                            break;
      case E_NO_ERROR                     : errorLabel->setText(tr("No Error has occured"));
                                            break;
      default                             : errorLabel->setText(tr("Unknown error"));
   };


}

//**************************************************************************************************
//
//**************************************************************************************************



//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************



//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************

