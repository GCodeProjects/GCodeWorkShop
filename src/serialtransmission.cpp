/***************************************************************************
 *   Copyright (C) 2006-2010 by Artur Kozioł                               *
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
   baudGroup->addButton(b10CheckBox, BAUD56000);
   baudGroup->addButton(b11CheckBox, BAUD115200);



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


#ifdef Q_OS_WIN32
   browseButton->setEnabled(false);
#else
   connect(browseButton, SIGNAL(clicked()), SLOT(browseButtonClicked()));
   b10CheckBox->setEnabled(false);
#endif


   connect(browse1PushButton, SIGNAL(clicked()), SLOT(browse1ButtonClicked()));
   connect(browse2PushButton, SIGNAL(clicked()), SLOT(browse2ButtonClicked()));
   connect(browse3PushButton, SIGNAL(clicked()), SLOT(browse3ButtonClicked()));

   connect(saveButton, SIGNAL(clicked()), SLOT(saveButtonClicked()));
   connect(saveCloseButton, SIGNAL(clicked()), SLOT(saveCloseButtonClicked()));
   connect(deleteButton, SIGNAL(clicked()), SLOT(deleteButtonClicked()));
   connect(closeButton, SIGNAL(clicked()), SLOT(closeButtonClicked()));

   connect(flowCtlGroup, SIGNAL(buttonReleased(int)), SLOT(flowCtlGroupReleased()));

   loadSettings();
   flowCtlGroupReleased();

   connect(configNameBox, SIGNAL(currentIndexChanged(int)), SLOT(changeSettings()));

   setResult(QDialog::Rejected);
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

void SPConfigDialog::flowCtlGroupReleased()
{
   if(f2CheckBox->isChecked())
   {
      startDelaySpinBox->setEnabled(false);
      startDelaySpinBox->setValue(0);
   }
   else
   {
      startDelaySpinBox->setEnabled(true);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::browseButtonClicked()
{
   QString fileName = QFileDialog::getOpenFileName(
                         this,
                         tr("Select serial port device"),
                         portNameComboBox->currentText(),
                         tr("All files (*)"));

   QFileInfo file(fileName);

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

#ifndef Q_OS_WIN32
    list.clear();
    list.prepend(portNameComboBox->currentText());
    for(int i = 0; i <= portNameComboBox->count(); i++)
    {
       item = portNameComboBox->itemText(i);
       if(!item.isEmpty())
         if(!list.contains(item))
           list.prepend(item);
    };

    while(list.size() > 64)
    {
       list.removeLast();
    };
    settings.setValue("PortNameList", list);
#endif

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
    settings.setValue("DeleteControlChars", deleteControlChars->isChecked());
    settings.setValue("RemoveEmptyLines", removeEmptyLines->isChecked());
    settings.setValue("RemoveBefore",removeBefore->isChecked());
    //settings.setValue("StartAfterXONCTS", startAfterXONCTS->isChecked());
    settings.setValue("SendingStartDelay", startDelaySpinBox->value());
    settings.setValue("DoNotShowProgressInEditor", doNotShowProgressInEditor->isChecked());    
    settings.setValue("RecieveTimeout", recieveTimeoutSpinBox->value());


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
           case BAUD56000  : b10CheckBox->setChecked(TRUE);
                             break;
           case BAUD115200 : b11CheckBox->setChecked(TRUE);
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
    delayDoubleSpinBox->setValue(settings.value("LineDelay", 0).toDouble());
    deleteControlChars->setChecked(settings.value("DeleteControlChars", true).toBool());
    removeEmptyLines->setChecked(settings.value("RemoveEmptyLines", true).toBool());
    removeBefore->setChecked(settings.value("RemoveBefore", false).toBool());
    //startAfterXONCTS->setChecked(settings.value("StartAfterXONCTS", true).toBool());
    startDelaySpinBox->setValue(settings.value("SendingStartDelay", 0).toInt());
    doNotShowProgressInEditor->setChecked(settings.value("DoNotShowProgressInEditor", false).toBool());
    recieveTimeoutSpinBox->setValue(settings.value("RecieveTimeout", 0).toInt());

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
    for(int i = 1; i <= 64; i++)
      list.append(QString("COM%1").arg(i));
    portNameComboBox->setEditable(false);

#else
    list << "/dev/ttyS0" << "/dev/ttyS1" << "/dev/ttyS2" << "/dev/ttyS3" <<
            "/dev/ttyUSB0" << "/dev/ttyUSB1" << "/dev/ttyUSB2" << "/dev/ttyUSB3";
    list = settings.value("PortNameList", list).toStringList();
    list.sort();

#endif

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

//**************************************************************************************************
//
//**************************************************************************************************

QString SPConfigDialog::browseForDir(const QString dir)
{
   QString dirName = QFileDialog::getExistingDirectory(
                         this,
                         tr("Select serial port device"),
                         dir,
                         QFileDialog::ShowDirsOnly);

   QFileInfo file(dirName);

   if((file.exists()) && (file.isReadable()))
   {
      return file.canonicalPath();
   };

   return "";
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::browse1ButtonClicked()
{
   path1LineEdit->setText(browseForDir(path1LineEdit->text()));
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::browse2ButtonClicked()
{
   path2LineEdit->setText(browseForDir(path2LineEdit->text()));
}

//**************************************************************************************************
//
//**************************************************************************************************

void SPConfigDialog::browse3ButtonClicked()
{
   path3LineEdit->setText(browseForDir(path3LineEdit->text()));
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

   comPort = NULL;

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
   connect(hexTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(hexTextEditScroll(int)));


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

void TransmissionDialog::textEditScroll(int pos)
{
   Q_UNUSED(pos);
   hexTextEdit->verticalScrollBar()->setSliderPosition(textEdit->verticalScrollBar()->sliderPosition());
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::hexTextEditScroll(int pos)
{
   Q_UNUSED(pos);
   textEdit->verticalScrollBar()->setSliderPosition(hexTextEdit->verticalScrollBar()->sliderPosition());
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::sendTextEditChanged()
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
   for(i = 0; i < tx.size(); i++)
   {
      ty = QString("%1 ").arg((int)tx.at(i).toAscii(), 2, 16, (QChar)'0');

      cr.insertText(ty);
   };
   hexTextEdit->setTextCursor(cr);
   hexTextEdit->insertPlainText("\n");

   textEdit->ensureCursorVisible();
   hexTextEdit->ensureCursorVisible();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::closeButtonClicked()
{
   stop = true;
   qApp->processEvents();
   if(comPort != NULL)
   {
      //comPort->close();
      //delete(comPort);
   };

   close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::clearButtonClicked()
{
   textEdit->clear();
   hexTextEdit->clear();
   sendLineEdit->clear();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::connectButtonToggled(bool tg)
{ 
   if(tg)
   {

      connectButton->setIcon(QIcon(":/images/connect_established.png"));
      connectButton->setText(tr("&Disconnect"));

      textEdit->clear();
      hexTextEdit->clear();
      sendLineEdit->clear();

      count = 0;

      if(comPort != NULL)
      {
         comPort->reset();
         comPort->close();
         delete(comPort);
      };

      comPort = new QextSerialPort(portName, portSettings);
      if(!comPort->open(QIODevice::ReadWrite | QIODevice::Unbuffered | QIODevice::Truncate))
      {
         showError(E_INVALID_FD);
         //showError(comPort->lastError());
         delete(comPort);
         comPort = NULL;
         connectButton->setChecked(false);
         return;
      };

      comPort->reset();
      comPort->flush();
      comPort->reset();

//      if(portSettings.FlowControl == FLOW_XONXOFF)
//      {
//         comPort->putChar(portSettings.Xon);
//      };

      setDtrButton->setEnabled(true);
      setDtrButton->setChecked(false);
      setRtsButton->setEnabled(true);
      setRtsButton->setChecked(false);

      configBox->setEnabled(false);
      configButton->setEnabled(false);


      bool en = portSettings.FlowControl == FLOW_XONXOFF;
      setXonButton->setEnabled(en);
      setXoffButton->setEnabled(en);

      sendLineEdit->setReadOnly(false);
      sendLineEdit->setFocus(Qt::MouseFocusReason);

      showError(comPort->lastError());

      stop = false;
      timer->start(20);

   }
   else
   {
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
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::setXonButtonClicked()
{
   if(comPort->isOpen())
     comPort->putChar(portSettings.Xon);
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::setXoffButtonClicked()
{
   if(comPort->isOpen())
     comPort->putChar(portSettings.Xoff);
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

void TransmissionDialog::updateLeds()
{
   ulong status;
   bool ok;
   QString tx;
   char ch;
   int i;
   QTextCursor cr;
   QTextCharFormat format;


   timer->stop();
   if(comPort == NULL)
      return;

   if(!comPort->isOpen())
      return;

   while(!stop)
   {

      status = comPort->lineStatus();

      ctsLabel->setEnabled(status & LS_CTS);
      dsrLabel->setEnabled(status & LS_DSR);
      dcdLabel->setEnabled(status & LS_DCD);
      rtsLabel->setEnabled(status & LS_RTS);
      setRtsButton->setChecked(status & LS_RTS);
      dtrLabel->setEnabled(status & LS_DTR);
      setDtrButton->setChecked(status & LS_DTR);

      i = comPort->bytesAvailable();
      qApp->processEvents();
      if(i > 0)
      {
         ok = comPort->getChar(&ch);
         if(!ok)
         {
            showError(comPort->lastError());
            return;
         };
         count++;
         errorLabel->setText(tr("Recived: %1 bytes.").arg(count));
         tx = ch;

         qApp->processEvents();

         if(ch != '\r')
         {
            cr = textEdit->textCursor();
            format = cr.charFormat();
            format.setForeground(Qt::blue);
            cr.setCharFormat(format);
            cr.insertText(tx);
            textEdit->setTextCursor(cr);
         };

         tx = QString("%1 ").arg((int)ch, 2, 16, (QChar)'0');
         cr = hexTextEdit->textCursor();
         format = cr.charFormat();
         format.setForeground(Qt::blue);
         cr.setCharFormat(format);
         cr.insertText(tx);
         hexTextEdit->setTextCursor(cr);

         if(ch == '\n')
            hexTextEdit->insertPlainText("\n");

         textEdit->ensureCursorVisible();
         hexTextEdit->ensureCursorVisible();
      }
      else
         if(i < 0)
            showError(comPort->lastError());

   };
   comPort->close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::configButtonClicked()
{
   SPConfigDialog *serialConfigDialog = new SPConfigDialog(this, configBox->currentText());

   if(serialConfigDialog->exec() == QDialog::Accepted)
      changeSettings();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::changeSettings()
{
    QString port;
    bool ok;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

#ifdef Q_OS_WIN32
       port = "COM1";
#else
       port = "/dev/ttyS0";

#endif

    settings.beginGroup(configBox->currentText());

    portName = settings.value("PortName", port).toString();

    portSettings.BaudRate = (BaudRateType) settings.value("BaudRate", BAUD9600).toInt();
    portSettings.DataBits = (DataBitsType) settings.value("DataBits", DATA_8).toInt();
    portSettings.StopBits = (StopBitsType) settings.value("StopBits", STOP_2).toInt();
    portSettings.Parity = (ParityType) settings.value("Parity", PAR_NONE).toInt();
    portSettings.FlowControl = (FlowType) settings.value("FlowControl", FLOW_HARDWARE).toInt();
    portSettings.Xon = settings.value("Xon", "17").toString().toInt(&ok, 10);
    portSettings.Xoff = settings.value("Xoff", "19").toString().toInt(&ok, 10);
    sendAtEnd = settings.value("SendAtEnd", "").toString();
    sendAtBegining = settings.value("SendAtBegining", "").toString();
    lineDelay = settings.value("LineDelay", 0).toDouble(); 


    settings.endGroup();
    settings.endGroup();
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
    list.sort();
    configBox->addItems(list);
    item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
    id = configBox->findText(item);
    configBox->setCurrentIndex(id);
    settings.endGroup();
}


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

void TransmissionDialog::sendText(QString tx)
{
   int i;
   bool xoffReceived;
   char controlChar;
   int bytesToWrite;

   if(comPort == NULL)
   return;

   if(comPort->isOpen())
   {
      tx.prepend(sendAtBegining);
      tx.append(sendAtEnd);
      if(!tx.contains("\r\n"))
         tx.replace("\n", "\r\n");

      errorLabel->setText(tr("Waiting..."));
      qApp->processEvents();

      i = 0;
      xoffReceived = true;
      while(i < tx.size())
      {
         if(xoffReceived)
            errorLabel->setText(tr("Waiting for a signal readiness..."));
         qApp->processEvents();

         if(stop)
            break;

         if(portSettings.FlowControl == FLOW_XONXOFF)
         {
            controlChar = 0;
            if(comPort->bytesAvailable() > 0)
            {
               comPort->getChar(&controlChar);
            };

            if(controlChar == portSettings.Xoff)
               xoffReceived = true;
            if(controlChar == portSettings.Xon)
               xoffReceived = false;
            //setXoffButton->setChecked(xoffReceived);
            //setXonButton->setChecked(!xoffReceived);
         }
         else
            xoffReceived = false;

         bytesToWrite = comPort->bytesToWrite();

         //qDebug() << "Bytes to write: " << bytesToWrite;

#ifdef Q_OS_UNIX
         usleep(2000);
#endif

         if((bytesToWrite == 0) && (!xoffReceived))
         {
            if(!comPort->putChar(tx[i].toAscii()))
               showError(comPort->lastError());

            errorLabel->setText(tr("Sending byte %1 of %2").arg(i + 1).arg(tx.size()));
            qApp->processEvents();

            if(lineDelay > 0)
            {
               if(tx[i].toAscii() == '\n')
               {
                  readyCont = false;
                  QTimer::singleShot(int(lineDelay * 1000), this, SLOT(lineDelaySlot()));
                  while(!readyCont)
                  {
                     qApp->processEvents();
                  };
               };
            };

            i++;
         };
      };
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void TransmissionDialog::lineDelaySlot()
{
   readyCont = true;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

TransProgressDialog::TransProgressDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
   setupUi(this);
   //setWindowTitle(tr("Serial transmission"));

   comPort = NULL;

   canceled = true;
   xon = 0;
   xoff = 0;

   connect(cancelButton, SIGNAL(clicked()), SLOT(cancelButtonClicked()));

   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(updateLeds()));
}

//**************************************************************************************************
//
//**************************************************************************************************

TransProgressDialog::~TransProgressDialog()
{
   timer->stop();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransProgressDialog::closeEvent(QCloseEvent *event)
{
   timer->stop();
   canceled = true;
   event->accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransProgressDialog::cancelButtonClicked()
{
   timer->stop();
   canceled = true;
   close();
}

//**************************************************************************************************
//
//**************************************************************************************************

bool TransProgressDialog::wasCanceled()
{
   return canceled;
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransProgressDialog::setLabelText(const QString text)
{
   label->setText(text);
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransProgressDialog::setValue(int val)
{
   progressBar->setValue(val);
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransProgressDialog::setRange(int min, int max)
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

void TransProgressDialog::open(QextSerialPort *port, char cxon, char cxoff)
{
   if(port != NULL)
   {
      canceled = false;
      show();
      comPort = port;

      xon = cxon;
      xoff = cxoff;

      timer->start(20);


   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void TransProgressDialog::updateLeds()
{
   ulong status;

   timer->stop();
   if(comPort == NULL)
      return;

   if(!comPort->isOpen())
      return;

   status = comPort->lineStatus();

   ctsLabel->setEnabled(status & LS_CTS);
   dsrLabel->setEnabled(status & LS_DSR);
   dcdLabel->setEnabled(status & LS_DCD);
   rtsLabel->setEnabled(status & LS_RTS);
   dtrLabel->setEnabled(status & LS_DTR);


   timer->start();
}

//**************************************************************************************************
//
//**************************************************************************************************

//void TransmissionDialog::reciveButtonClicked()
//{
//   QString tx;
//   int count, i;
//   bool ok;
//   char ch;
//
//
//   loadConfig();
//   comPort = new QextSerialPort(portName, portSettings);
//   if(comPort->open(QIODevice::ReadWrite | QIODevice::Unbuffered | QIODevice::Truncate))
//     stop = false;
//   else
//   {
//      stop = true;
//      showError(E_INVALID_FD);
//      delete(comPort);
//      return;
//   };
//   comPort->flush();
//   comPort->reset();
//
//   i = configBox->currentIndex();
//   newFile();
//   activeWindow = activeMdiChild();
//   if(!(activeWindow != 0))
//     return;
//   configBox->setCurrentIndex(i);
//
//   receiveAct->setEnabled(FALSE);
//   sendAct->setEnabled(FALSE);
//   QApplication::setOverrideCursor(Qt::BusyCursor);
//
//   QProgressDialog progressDialog(this);
//   progressDialog.setRange(0, 32768);
//   progressDialog.setModal(TRUE);
//   progressDialog.setLabelText(tr("Waiting for data..."));
//   progressDialog.open();
//   qApp->processEvents();
//
//   if(portSettings.FlowControl == FLOW_XONXOFF)
//   {
//      comPort->putChar(portSettings.Xon);
//   };
//
//   tx.clear();
//   while(1)
//   {
//      //progressDialog.setValue(count);
//
//#ifdef Q_OS_UNIX
//      usleep(2000);
//#endif
//
//      i = comPort->bytesAvailable();
//      if(i > 0)
//      {
//         ok = comPort->getChar(&ch);
//         if(!ok)
//         {
//            stop = true;
//            if(portSettings.FlowControl == FLOW_XONXOFF)
//            {
//               comPort->putChar(portSettings.Xoff);
//            };
//            showError(comPort->lastError());
//            break;
//         };
//         count++;
//         errorLabel->setText(tr("Recived: %1 bytes.").arg(count));
//         textEdit->insertPlainText(ch);
//         tx = QString("%1 ").arg(ch, 0, 16);
//         hexTextEdit->insertPlainText(tx.toUpper());
//
//         textEdit->ensureCursorVisible();
//         hexTextEdit->ensureCursorVisible();
//         qApp->processEvents();
//
//      };
//   };
//
//   comPort->close();
//   delete(comPort);
//   stopButton->setEnabled(FALSE);
//   reciveButton->setEnabled(TRUE);
//   sendButton->setEnabled(TRUE);
//   QApplication::restoreOverrideCursor();
//
//
//   //////////////////////////////////////////////////////////////////////
//
//
////   showError(E_NO_ERROR);
////   count = 0;
////   if(comPort->open(QIODevice::ReadOnly))
////     stop = false;
////   else
////   {
////      stop = true;
////      showError(E_INVALID_FD);
////      return;
////   };
////   reciveButton->setEnabled(FALSE);
////   sendButton->setEnabled(FALSE);
////   stopButton->setEnabled(TRUE);
////   QApplication::setOverrideCursor(Qt::BusyCursor);
////
////   while(!stop)
////   {
////      qApp->processEvents();
////
////      if(comPort->bytesAvailable() > 0)
////      {
////         ok = comPort->getChar(&ch);
////         if(!ok)
////         {
////            stop = true;
////            showError(comPort->lastError());
////            break;
////         };
////         count++;
////         errorLabel->setText(tr("Recived: %1 bytes.").arg(count));
////         textEdit->insertPlainText(ch);
////         tx = QString("%1 ").arg(ch, 0, 16);
////         textEdit->insertPlainText(tx.toUpper());
////      };
////
////   };
////
////   comPort->close();
//
////   QApplication::restoreOverrideCursor();
//
//}

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

