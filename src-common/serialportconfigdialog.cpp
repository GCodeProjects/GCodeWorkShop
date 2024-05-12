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

#include <QButtonGroup>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QPalette>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QValidator>

#include <utils/medium.h> // Medium

#include "serialportconfigdialog.h"  // QDialog Qt QWidget


// TODO: move into class SerialPortConfigDialog
static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SerialPortConfigDialog::SerialPortConfigDialog(QWidget* parent, QString confName,
        Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Serial port configuration"));
	setModal(true);

	configName = confName;

	baudGroup = new QButtonGroup(this);
	baudGroup->setExclusive(true);
	baudGroup->addButton(b1CheckBox, 300);
	baudGroup->addButton(b2CheckBox, 600);
	baudGroup->addButton(b3CheckBox, QSerialPort::Baud1200);
	baudGroup->addButton(b4CheckBox, QSerialPort::Baud2400);
	baudGroup->addButton(b5CheckBox, QSerialPort::Baud4800);
	baudGroup->addButton(b6CheckBox, QSerialPort::Baud9600);
	baudGroup->addButton(b7CheckBox, QSerialPort::Baud19200);
	baudGroup->addButton(b8CheckBox, QSerialPort::Baud38400);
	baudGroup->addButton(b9CheckBox, QSerialPort::Baud57600);
	baudGroup->addButton(b11CheckBox, QSerialPort::Baud115200);

	dataBitsGroup = new QButtonGroup(this);
	dataBitsGroup->setExclusive(true);
	dataBitsGroup->addButton(d1CheckBox, QSerialPort::Data5);
	dataBitsGroup->addButton(d2CheckBox, QSerialPort::Data6);
	dataBitsGroup->addButton(d3CheckBox, QSerialPort::Data7);
	dataBitsGroup->addButton(d4CheckBox, QSerialPort::Data8);

	stopBitsGroup = new QButtonGroup(this);
	stopBitsGroup->setExclusive(true);
	stopBitsGroup->addButton(s1CheckBox, QSerialPort::OneStop);
	stopBitsGroup->addButton(s2CheckBox, QSerialPort::TwoStop);

	parityGroup = new QButtonGroup(this);
	parityGroup->setExclusive(true);
	parityGroup->addButton(p1CheckBox, QSerialPort::NoParity);
	parityGroup->addButton(p2CheckBox, QSerialPort::OddParity);
	parityGroup->addButton(p3CheckBox, QSerialPort::EvenParity);

	flowCtlGroup = new QButtonGroup(this);
	flowCtlGroup->setExclusive(true);
	flowCtlGroup->addButton(f1CheckBox, QSerialPort::NoFlowControl);
	flowCtlGroup->addButton(f2CheckBox, QSerialPort::HardwareControl);
	flowCtlGroup->addButton(f3CheckBox, QSerialPort::SoftwareControl);

#ifdef Q_OS_WIN32
	browseButton->setEnabled(false);
#else
	connect(browseButton, SIGNAL(clicked()), SLOT(browseButtonClicked()));
#endif

	connect(browse1PushButton, SIGNAL(clicked()), SLOT(browse1ButtonClicked()));
	connect(browse2PushButton, SIGNAL(clicked()), SLOT(browse2ButtonClicked()));
	connect(browse3PushButton, SIGNAL(clicked()), SLOT(browse3ButtonClicked()));
	connect(browse4PushButton, SIGNAL(clicked()), SLOT(browse4ButtonClicked()));

	connect(saveButton, SIGNAL(clicked()), SLOT(saveButtonClicked()));
	connect(saveCloseButton, SIGNAL(clicked()), SLOT(saveCloseButtonClicked()));
	connect(deleteButton, SIGNAL(clicked()), SLOT(deleteButtonClicked()));
	connect(closeButton, SIGNAL(clicked()), SLOT(closeButtonClicked()));
	connect(helpButton, SIGNAL(clicked()), SLOT(helpButtonClicked()));

	connect(addButton, SIGNAL(clicked()), SLOT(addButtonClicked()));
	connect(removeButton, SIGNAL(clicked()), SLOT(removeButtonClicked()));

	connect(addEobButton, SIGNAL(clicked()), SLOT(addEobButtonClicked()));
	connect(deleteEobButton, SIGNAL(clicked()), SLOT(deleteEobButtonClicked()));

	connect(addfileNameButton, SIGNAL(clicked()), SLOT(addFileNameButtonClicked()));
	connect(removeFileNameButton, SIGNAL(clicked()), SLOT(removeFileNameButtonClicked()));

	connect(addEobCharButton, SIGNAL(clicked()), SLOT(addEobCharButtonClicked()));
	connect(deleteEobCharButton, SIGNAL(clicked()), SLOT(deleteEobCharButtonClicked()));
	connect(eobComboBox, SIGNAL(editTextChanged(const QString)),
	        SLOT(eobComboBoxEditTextChanged(const QString)));

	connect(autoSaveCheckBox, SIGNAL(stateChanged(int)), SLOT(autoSaveCheckBoxChanged(int)));
	connect(fileServerCheckBox, SIGNAL(stateChanged(int)), SLOT(fileServerCheckBoxChanged(int)));
	connect(appendExtCheckBox, SIGNAL(stateChanged(int)), SLOT(appendExtCheckBoxChanged(int)));
	connect(useAsExtCheckBox, SIGNAL(stateChanged(int)), SLOT(useAsExtCheckBoxChanged(int)));

	connect(searchPath1LineEdit, SIGNAL(textChanged(const QString)),
	        SLOT(readPath1Changed(const QString)));
	connect(searchPath2LineEdit, SIGNAL(textChanged(const QString)),
	        SLOT(readPath2Changed(const QString)));
	connect(searchPath3LineEdit, SIGNAL(textChanged(const QString)),
	        SLOT(readPath3Changed(const QString)));

	//connect(flowCtlGroup, SIGNAL(buttonReleased(int)), SLOT(flowCtlGroupReleased()));

	loadSettings();
	flowCtlGroupReleased();

	connect(configNameBox, SIGNAL(currentIndexChanged(int)), SLOT(changeSettings()));
	connect(portNameComboBox, SIGNAL(currentIndexChanged(QString)),
	        SLOT(portNameComboBoxIndexChanged(QString)));

	tabWidget->setCurrentIndex(0);

	QRegularExpression rx("(LF|CR){1,6}");
	rx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	eobComboBox->setValidator(new QRegularExpressionValidator(rx, this));

	xonInput->setValidator(new QIntValidator(0, 127, this));

	xoffInput->setValidator(new QIntValidator(0, 127, this));

	setResult(QDialog::Rejected);
}

SerialPortConfigDialog::~SerialPortConfigDialog()
{
}

void SerialPortConfigDialog::flowCtlGroupReleased()
{
}

void SerialPortConfigDialog::browseButtonClicked()
{
	QString fileName = QFileDialog::getOpenFileName(
	                       this,
	                       tr("Select serial port device"),
	                       "/dev/",
	                       tr("All files (*)"));

	QFileInfo file(fileName);

	if ((file.exists()) && (file.isReadable())) {
		portNameComboBox->addItem(fileName);
		portNameComboBox->setCurrentIndex(portNameComboBox->count() - 1);
	}
}

void SerialPortConfigDialog::saveButtonClicked()
{
	QStringList list;
	QString item, curItem;

	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("SerialPortConfigs");

	curItem = configNameBox->currentText();

	//#ifndef Q_OS_WIN32
	//    list.clear();
	//    list.prepend(portNameComboBox->currentText());
	//    for(int i = 0; i <= portNameComboBox->count(); i++)
	//    {
	//       item = portNameComboBox->itemText(i);
	//       if(!item.isEmpty())
	//         if(!list.contains(item))
	//           list.prepend(item);
	//    }

	//    while(list.size() > 64)
	//    {
	//       list.removeLast();
	//    }
	//    settings.setValue("PortNameList", list);
	//#endif

	list.clear();
	list.prepend(configNameBox->currentText());

	for (int i = 0; i <= configNameBox->count(); i++) {
		item = configNameBox->itemText(i);

		if (!item.isEmpty())
			if (!list.contains(item)) {
				list.prepend(item);
			}
	}

	while (list.size() > 64) {
		list.removeLast();
	}

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
	settings.setValue("RemoveBefore", removeBefore->isChecked());
	settings.setValue("SendingStartDelay", startDelaySpinBox->value());
	settings.setValue("ReceiveTimeoutTime", receiveTimeoutSpinBox->value());
	settings.setValue("SendTimeoutTime", sendTimeoutSpinBox->value());
	settings.setValue("AutoCloseTime", autoCloseSpinBox->value());
	//settings.setValue("EndOfBlockLF", endOfBlockLF->isChecked());
	settings.setValue("RemoveSpaceEOB", removeSpaceEOB->isChecked());
	settings.setValue("EobChar", eobComboBox->currentText());
	settings.setValue("DataToLogFile", logDataCheckBox->isChecked());
	settings.setValue("WaitForCTS", waitForCtsCheckBox->isChecked());
	settings.setValue("WaitForXON", waitForXonCheckBox->isChecked());
	settings.setValue("SendXONAtStart", sendXonCheckBox->isChecked());
	settings.setValue("RemoveFromReceived", removeLineEdit->text());

	settings.setValue("AutoSave", autoSaveCheckBox->isChecked());
	settings.setValue("CreateLogFile", logFileCheckBox->isChecked());
	settings.setValue("CreateBackup", renameCheckBox->isChecked());
	settings.setValue("RemoveLetters", removeLettersCheckBox->isChecked());
	settings.setValue("DetectFormFileName", detectFormFileNameCheckBox->isChecked());
	settings.setValue("AppendExtension", appendExtCheckBox->isChecked());
	settings.setValue("UseAsExtension", useAsExtCheckBox->isChecked());
	settings.setValue("SplitPrograms", splitProgramsCheckBox->isChecked());
	settings.setValue("SavePath", savePathLineEdit->text());
	settings.setValue("SaveExt", saveExtComboBox->currentText());

	settings.setValue("FileServer", fileServerCheckBox->isChecked());
	settings.setValue("FileNameExpFSSelected", fileNameExpFSComboBox->currentText());
	settings.setValue("FileNameExpASSelected", fileNameExpASComboBox->currentText());
	settings.setValue("EndOfProgExpSelected", endOfProgComboBox->currentText());
	settings.setValue("FileNameLowerCase", fileNameLowerCaseCheckBox->isChecked());
	settings.setValue("CallerProg", callerProgNameLineEdit->text());
	settings.setValue("SearchPath1", searchPath1LineEdit->text());
	settings.setValue("SearchExt1", searchExt1ComboBox->currentText());
	settings.setValue("SearchPath2", searchPath2LineEdit->text());
	settings.setValue("SearchExt2", searchExt2ComboBox->currentText());
	settings.setValue("SearchPath3", searchPath3LineEdit->text());
	settings.setValue("SearchExt3", searchExt3ComboBox->currentText());

	settings.endGroup();

	QStringList eList;
	eList.prepend(fileNameExpFSComboBox->currentText());

	for (int i = 0; i <= fileNameExpFSComboBox->count(); i++) {
		item = fileNameExpFSComboBox->itemText(i);

		if (!item.isEmpty())
			if (!eList.contains(item)) {
				eList.prepend(item);
			}
	}

	eList.removeDuplicates();
	eList.sort();
	settings.setValue("FileNameExpFS", eList);

	eList.clear();
	eList.prepend(fileNameExpASComboBox->currentText());

	for (int i = 0; i <= fileNameExpASComboBox->count(); i++) {
		item = fileNameExpASComboBox->itemText(i);

		if (!item.isEmpty())
			if (!eList.contains(item)) {
				eList.prepend(item);
			}
	}

	eList.removeDuplicates();
	eList.sort();
	settings.setValue("FileNameExpAS", eList);

	eList.clear();
	eList.prepend(endOfProgComboBox->currentText());

	for (int i = 0; i <= endOfProgComboBox->count(); i++) {
		item = endOfProgComboBox->itemText(i);

		if (!item.isEmpty())
			if (!eList.contains(item)) {
				eList.prepend(item);
			}
	}

	eList.removeDuplicates();
	eList.sort();
	settings.setValue("EndOfProgExp", eList);

	eList.clear();
	eList.prepend(eobComboBox->currentText());

	for (int i = 0; i <= eobComboBox->count(); i++) {
		item = eobComboBox->itemText(i);

		if (!item.isEmpty())
			if (!eList.contains(item)) {
				eList.prepend(item);
			}
	}

	eList.removeDuplicates();
	eList.sort();
	settings.setValue("EndOfBlockCodes", eList);

	settings.endGroup();

	configNameBox->clear();
	configNameBox->addItems(list);
	int id = configNameBox->findText(curItem);

	if (id >= 0) {
		configNameBox->setCurrentIndex(id);
	}
}

void SerialPortConfigDialog::changeSettings()
{
	int id;
	QString item, port;
	QStringList list;

	QSettings& settings = *Medium::instance().settings();

	QStringList extensions = settings.value("Extensions",
	                                        (QStringList() << "*.nc" << "*.cnc")).toStringList();
	extensions.removeDuplicates();
	extensions.replaceInStrings("*", "");
	saveExtComboBox->insertItems(0, extensions);
	searchExt1ComboBox->insertItems(0, extensions);
	searchExt2ComboBox->insertItems(0, extensions);
	searchExt3ComboBox->insertItems(0, extensions);

	settings.beginGroup("SerialPortConfigs");

	list.clear();
	list.append(settings.value("EndOfBlockCodes",
	                           (QStringList() << "LF" << "CR" << "CRLF" << "CRCRLF" << "LFCRCR" << "")).toStringList());
	list.removeDuplicates();
	eobComboBox->insertItems(0, list);

	list.clear();
	list.append(settings.value("EndOfProgExp",
	                           (QStringList() << "(M30|M02|M2|M99)[\\n\\r\\s]{1,}%" << "")).toStringList());
	list.removeDuplicates();
	endOfProgComboBox->insertItems(0, list);

	list.clear();
	list.append(settings.value("FileNameExpFS",
	                           (QStringList() << "\\([0-9]{4,4}\\)" << "\\([0-9]{4,4}\\.NC\\)" <<
	                            "\\((O|:){0,1}[0-9]{4,4}\\.(NC|CNC|AGC){1,1}\\)")).toStringList());
	list.removeDuplicates();
	fileNameExpFSComboBox->insertItems(0, list);

	list.clear();
	list.append(settings.value("FileNameExpAS",
	                           (QStringList() << "\\([0-9]{4,4}\\)" << "\\([0-9]{4,4}\\.NC\\)" <<
	                            "\\((O|:){0,1}[0-9]{4,4}\\.(NC|CNC|AGC){1,1}\\)")).toStringList());
	list.removeDuplicates();
	fileNameExpASComboBox->insertItems(0, list);

	settings.beginGroup(configNameBox->currentText());

#ifdef Q_OS_WIN32
	port = "COM1";
#else
	port = "/dev/ttyUSB0";
#endif

	item = settings.value("PortName", port).toString();
	id = portNameComboBox->findText(item);

	if (id == -1) {
		portNameComboBox->addItems(QStringList(item));
		id = portNameComboBox->findText(item);
	}

	portNameComboBox->setCurrentIndex(id);
	portNameComboBoxIndexChanged(item);

	id = settings.value("BaudRate", QSerialPort::Baud9600).toInt();

	switch (id) {
	case 300:
		b1CheckBox->setChecked(true);
		break;

	case 600:
		b2CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud1200:
		b3CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud2400:
		b4CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud4800:
		b5CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud9600:
		b6CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud19200:
		b7CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud38400:
		b8CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud57600:
		b9CheckBox->setChecked(true);
		break;

	case QSerialPort::Baud115200:
		b11CheckBox->setChecked(true);
		break;
	}

	id = settings.value("DataBits", QSerialPort::Data8).toInt();

	switch (id) {
	case QSerialPort::Data5:
		d1CheckBox->setChecked(true);
		break;

	case QSerialPort::Data6:
		d2CheckBox->setChecked(true);
		break;

	case QSerialPort::Data7:
		d3CheckBox->setChecked(true);
		break;

	case QSerialPort::Data8:
		d4CheckBox->setChecked(true);
		break;
	}

	id = settings.value("StopBits", QSerialPort::TwoStop).toInt();

	switch (id) {
	case QSerialPort::OneStop:
		s1CheckBox->setChecked(true);
		break;

	case QSerialPort::TwoStop:
		s2CheckBox->setChecked(true);
		break;
	}

	id = settings.value("Parity", QSerialPort::NoParity).toInt();

	switch (id) {
	case QSerialPort::NoParity:
		p1CheckBox->setChecked(true);
		break;

	case QSerialPort::OddParity:
		p2CheckBox->setChecked(true);
		break;

	case QSerialPort::EvenParity:
		p3CheckBox->setChecked(true);
		break;
	}

	id = settings.value("FlowControl", QSerialPort::HardwareControl).toInt();

	switch (id) {
	case QSerialPort::NoFlowControl:
		f1CheckBox->setChecked(true);
		break;

	case QSerialPort::HardwareControl:
		f2CheckBox->setChecked(true);
		break;

	case QSerialPort::SoftwareControl:
		f3CheckBox->setChecked(true);
		break;
	}

	eotInput->setText(settings.value("SendAtEnd", "").toString());
	stInput->setText(settings.value("SendAtBegining", "").toString());
	xonInput->setText(settings.value("Xon", "0").toString());
	xoffInput->setText(settings.value("Xoff", "0").toString());
	delayDoubleSpinBox->setValue(settings.value("LineDelay", 0).toDouble());
	deleteControlChars->setChecked(settings.value("DeleteControlChars", true).toBool());
	removeEmptyLines->setChecked(settings.value("RemoveEmptyLines", true).toBool());
	removeBefore->setChecked(settings.value("RemoveBefore", false).toBool());
	startDelaySpinBox->setValue(settings.value("SendingStartDelay", 0).toInt());
	autoCloseSpinBox->setValue(settings.value("AutoCloseTime", 15).toInt());
	sendTimeoutSpinBox->setValue(settings.value("SendTimeoutTime", 30).toInt());
	receiveTimeoutSpinBox->setValue(settings.value("ReceiveTimeoutTime", 3).toInt());
	removeSpaceEOB->setChecked(settings.value("RemoveSpaceEOB", false).toBool());
	logFileCheckBox->setChecked(settings.value("CreateLogFile", true).toBool());
	logDataCheckBox->setChecked(settings.value("DataToLogFile", false).toBool());
	waitForCtsCheckBox->setChecked(settings.value("WaitForCTS", false).toBool());
	waitForXonCheckBox->setChecked(settings.value("WaitForXON", false).toBool());
	sendXonCheckBox->setChecked(settings.value("SendXONAtStart", false).toBool());
	removeLineEdit->setText(settings.value("RemoveFromReceived", "").toString());

	id = eobComboBox->findText(settings.value("EobChar", "CRLF").toString());

	if (id >= 0) {
		eobComboBox->setCurrentIndex(id);
	}

	autoSaveCheckBox->setChecked(settings.value("AutoSave", false).toBool());
	renameCheckBox->setChecked(settings.value("CreateBackup", true).toBool());
	removeLettersCheckBox->setChecked(settings.value("RemoveLetters", true).toBool());
	detectFormFileNameCheckBox->setChecked(settings.value("DetectFormFileName", true).toBool());
	appendExtCheckBox->setChecked(settings.value("AppendExtension", false).toBool());
	useAsExtCheckBox->setChecked(settings.value("UseAsExtension", false).toBool());
	splitProgramsCheckBox->setChecked(settings.value("SplitPrograms", true).toBool());
	savePathLineEdit->setText(settings.value("SavePath", "").toString());
	id = saveExtComboBox->findText(settings.value("SaveExt", ".nc").toString());

	if (id >= 0) {
		saveExtComboBox->setCurrentIndex(id);
	}

	id = endOfProgComboBox->findText(settings.value("EndOfProgExpSelected", "").toString());

	if (id >= 0) {
		endOfProgComboBox->setCurrentIndex(id);
	}

	fileServerCheckBox->setChecked(settings.value("FileServer", false).toBool());
	callerProgNameLineEdit->setText(settings.value("CallerProg", "O5555").toString());
	fileNameLowerCaseCheckBox->setChecked(settings.value("FileNameLowerCase", true).toBool());
	reconnectSpinBox->setValue(settings.value("ReconnectTimeoutTime", 60).toInt());

	id = fileNameExpFSComboBox->findText(settings.value("FileNameExpFSSelected", "").toString());

	if (id >= 0) {
		fileNameExpFSComboBox->setCurrentIndex(id);
	}

	id = fileNameExpASComboBox->findText(settings.value("FileNameExpASSelected", "").toString());

	if (id >= 0) {
		fileNameExpASComboBox->setCurrentIndex(id);
	}

	searchPath1LineEdit->setText(settings.value("SearchPath1", "").toString());
	id = searchExt1ComboBox->findText(settings.value("SearchExt1", ".nc").toString());

	if (id >= 0) {
		searchExt1ComboBox->setCurrentIndex(id);
	}

	searchPath2LineEdit->setText(settings.value("SearchPath2", "").toString());
	id = searchExt2ComboBox->findText(settings.value("SearchExt2", ".nc").toString());

	if (id >= 0) {
		searchExt2ComboBox->setCurrentIndex(id);
	}

	searchPath3LineEdit->setText(settings.value("SearchPath3", "").toString());
	id = searchExt3ComboBox->findText(settings.value("SearchExt3", ".nc").toString());

	if (id >= 0) {
		searchExt3ComboBox->setCurrentIndex(id);
	}

	settings.endGroup();
	settings.endGroup();

	autoSaveCheckBoxChanged((autoSaveCheckBox->isChecked() ? Qt::Checked : Qt::Unchecked));
	fileServerCheckBoxChanged((fileServerCheckBox->isChecked() ? Qt::Checked : Qt::Unchecked));
}

void SerialPortConfigDialog::loadSettings()
{
	int id;
	QStringList list;

	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("SerialPortConfigs");

	foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
		list.append(info.portName());
	}

	list.removeDuplicates();
	list.sort();
	portNameComboBox->clear();
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

void SerialPortConfigDialog::portNameComboBoxIndexChanged(QString name)
{
	QString description;
	QString manufacturer;
	QString serialNumber;

	QSerialPortInfo* info = new QSerialPortInfo(name);

	description = info->description();
	manufacturer = info->manufacturer();
	serialNumber = info->serialNumber();

	portInfoListWidget->clear();
	//portInfoListWidget->addItem(tr("Port name:          %1").arg(info->portName()));
	portInfoListWidget->addItem(tr("Description       : %1").arg((!description.isEmpty() ?
	                            description : blankString)));
	portInfoListWidget->addItem(tr("Manufacturer      : %1").arg((!manufacturer.isEmpty() ?
	                            manufacturer : blankString)));
	portInfoListWidget->addItem(tr("SerialNumber      : %1").arg((!serialNumber.isEmpty() ?
	                            serialNumber : blankString)));
	portInfoListWidget->addItem(tr("System location   : %1").arg(info->systemLocation()));
	portInfoListWidget->addItem(tr("Vendor identifier : %1").arg((info->vendorIdentifier() ?
	                            QString::number(info->vendorIdentifier(), 16) : blankString)));
	portInfoListWidget->addItem(tr("Product identifier: %1").arg((info->productIdentifier() ?
	                            QString::number(info->productIdentifier(), 16) : blankString)));
}

void SerialPortConfigDialog::deleteButtonClicked()
{
	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("SerialPortConfigs");
	settings.remove(configNameBox->currentText());
	settings.endGroup();

	int id = configNameBox->findText(configNameBox->currentText());
	configNameBox->removeItem(id);
}

void SerialPortConfigDialog::closeButtonClicked()
{
	QSettings& settings = *Medium::instance().settings();
	settings.beginGroup("SerialPortConfigs");
	settings.setValue("CurrentSerialPortSettings", configNameBox->currentText());
	settings.endGroup();

	close();
}

void SerialPortConfigDialog::saveCloseButtonClicked()
{
	saveButtonClicked();
	closeButtonClicked();
	accept();
}

QString SerialPortConfigDialog::browseForDir(QString dir, QString windowTitle)
{
	QDir directory(dir);

	if ((!directory.exists()) || (!directory.isReadable()) || (dir.isEmpty())) {
		directory.setPath(QDir::homePath());
	}

	dir = QFileDialog::getExistingDirectory(
	          this,
	          windowTitle,
	          directory.canonicalPath(),
	          QFileDialog::ShowDirsOnly);

	if (!dir.isEmpty()) {
		directory.setPath(dir);
	}

	return directory.canonicalPath();
}

void SerialPortConfigDialog::browse1ButtonClicked()
{
	savePathLineEdit->setText(browseForDir(savePathLineEdit->text(), tr("Select search path")));
}

void SerialPortConfigDialog::browse2ButtonClicked()
{
	searchPath1LineEdit->setText(browseForDir(searchPath1LineEdit->text(),
	                             tr("Select search path")));
}

void SerialPortConfigDialog::browse3ButtonClicked()
{
	searchPath2LineEdit->setText(browseForDir(searchPath2LineEdit->text(),
	                             tr("Select search path")));
}

void SerialPortConfigDialog::autoSaveCheckBoxChanged(int state)
{
	savePathLineEdit->setEnabled(state == Qt::Checked);
	saveExtComboBox->setEnabled(state == Qt::Checked);
	browse1PushButton->setEnabled(state == Qt::Checked);
	renameCheckBox->setEnabled(state == Qt::Checked);
	removeLettersCheckBox->setEnabled(state == Qt::Checked);
	detectFormFileNameCheckBox->setEnabled(state == Qt::Checked);
	appendExtCheckBox->setEnabled(state == Qt::Checked);
	useAsExtCheckBox->setEnabled(state == Qt::Checked);
	splitProgramsCheckBox->setEnabled(state == Qt::Checked);
	endOfProgComboBox->setEnabled(state == Qt::Checked);
	addEobButton->setEnabled(state == Qt::Checked);
	deleteEobButton->setEnabled(state == Qt::Checked);
	tabWidget->setTabEnabled(4, state == Qt::Checked);

	if (fileServerCheckBox->isChecked() && !(state == Qt::Checked)) {
		fileServerCheckBox->setChecked(false);
	}

	flowCtlGroupReleased();
}

void SerialPortConfigDialog::browse4ButtonClicked()
{
	searchPath3LineEdit->setText(browseForDir(searchPath3LineEdit->text(),
	                             tr("Select search path")));
}

void SerialPortConfigDialog::appendExtCheckBoxChanged(int state)
{
	if (state == Qt::Checked) {
		useAsExtCheckBox->setChecked(false);
	}
}

void SerialPortConfigDialog::useAsExtCheckBoxChanged(int state)
{
	if (state == Qt::Checked) {
		appendExtCheckBox->setChecked(false);
	}

}

void SerialPortConfigDialog::fileServerCheckBoxChanged(int state)
{
	searchPath1LineEdit->setEnabled(state == Qt::Checked);
	searchPath2LineEdit->setEnabled(state == Qt::Checked);
	searchPath3LineEdit->setEnabled(state == Qt::Checked);
	searchExt1ComboBox->setEnabled(state == Qt::Checked);
	searchExt2ComboBox->setEnabled(state == Qt::Checked);
	searchExt3ComboBox->setEnabled(state == Qt::Checked);
	browse2PushButton->setEnabled(state == Qt::Checked);
	browse3PushButton->setEnabled(state == Qt::Checked);
	browse4PushButton->setEnabled(state == Qt::Checked);
	fileNameExpFSComboBox->setEnabled(state == Qt::Checked);
	removeButton->setEnabled(state == Qt::Checked);
	addButton->setEnabled(state == Qt::Checked);
	callerProgNameLineEdit->setEnabled(state == Qt::Checked);
	fileNameLowerCaseCheckBox->setEnabled(state == Qt::Checked);
	flowCtlGroupReleased();
}

void SerialPortConfigDialog::addButtonClicked()
{
	QString text = fileNameExpFSComboBox->currentText();

	if (!text.isEmpty()) {
		fileNameExpFSComboBox->insertItem(0, text);
	}
}

void SerialPortConfigDialog::removeButtonClicked()
{
	fileNameExpFSComboBox->removeItem(fileNameExpFSComboBox->currentIndex());
}

void SerialPortConfigDialog::addEobButtonClicked()
{
	QString text = endOfProgComboBox->currentText();

	if (!text.isEmpty()) {
		endOfProgComboBox->insertItem(0, text);
	}
}

void SerialPortConfigDialog::deleteEobButtonClicked()
{
	endOfProgComboBox->removeItem(endOfProgComboBox->currentIndex());
}

void SerialPortConfigDialog::addEobCharButtonClicked()
{
	QString text = eobComboBox->currentText();

	if (!text.isEmpty()) {
		eobComboBox->insertItem(0, text);
	}
}

void SerialPortConfigDialog::deleteEobCharButtonClicked()
{
	eobComboBox->removeItem(eobComboBox->currentIndex());
}

void SerialPortConfigDialog::eobComboBoxEditTextChanged(const QString text)
{
	eobComboBox->setCurrentText(text.toUpper());
}

void SerialPortConfigDialog::addFileNameButtonClicked()
{
	QString text = fileNameExpASComboBox->currentText();

	if (!text.isEmpty()) {
		fileNameExpASComboBox->insertItem(0, text);
	}
}

void SerialPortConfigDialog::removeFileNameButtonClicked()
{
	fileNameExpASComboBox->removeItem(fileNameExpASComboBox->currentIndex());
}

void SerialPortConfigDialog::helpButtonClicked()
{
	// TODO: change a path to the file
	QDesktopServices::openUrl(QUrl(QString("file:///%1/EdytorNC_SerialTransmission_Help.html").arg(
	                                   QCoreApplication::applicationDirPath()), QUrl::TolerantMode));
	//    SerialPortCfgHelpDialog helpDialog;
	//    helpDialog.exec();
}

void SerialPortConfigDialog::readPath1Changed(const QString text)
{
	QPalette palette;

	if (text.isEmpty()) {
		return;
	}

	palette.setColor(searchPath1LineEdit->foregroundRole(), Qt::red);

	QDir dir;
	dir.setPath(text);

	if (dir.exists()) {
		searchPath1LineEdit->setPalette(QPalette());
	} else {
		searchPath1LineEdit->setPalette(palette);
	}
}

void SerialPortConfigDialog::readPath2Changed(const QString text)
{
	QPalette palette;

	if (text.isEmpty()) {
		return;
	}

	palette.setColor(searchPath2LineEdit->foregroundRole(), Qt::red);

	QDir dir;
	dir.setPath(text);

	if (dir.exists()) {
		searchPath2LineEdit->setPalette(QPalette());
	} else {
		searchPath2LineEdit->setPalette(palette);
	}
}

void SerialPortConfigDialog::readPath3Changed(const QString text)
{
	QPalette palette;

	if (text.isEmpty()) {
		return;
	}

	palette.setColor(searchPath3LineEdit->foregroundRole(), Qt::red);

	QDir dir;
	dir.setPath(text);

	if (dir.exists()) {
		searchPath3LineEdit->setPalette(QPalette());
	} else {
		searchPath3LineEdit->setPalette(palette);
	}
}
