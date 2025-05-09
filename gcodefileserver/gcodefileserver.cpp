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

#include <QAction>          // for QAction
#include <QCloseEvent>      // for QCloseEvent
#include <QComboBox>        // for QComboBox
#include <QDesktopServices> // for QDesktopServices
#include <QDialog>          // for QDialog
#include <QIcon>            // for QIcon
#include <QList>            // for QList
#include <QMdiArea>         // for QMdiArea
#include <QMdiSubWindow>    // for QMdiSubWindow
#include <QMenu>            // for QMenu
#include <QMessageBox>      // for QMessageBox
#include <QSettings>        // for QSettings
#include <QSignalMapper>    // for QSignalMapper
#include <QString>          // for QString, operator==
#include <QStringList>      // for QStringList
#include <QTabBar>          // for QTabBar
#include <QTimer>           // for QTimer
#include <QToolBar>         // for QToolBar
#include <QUrl>             // for QUrl
#include <QVariant>         // for QVariant
#include <QWidget>          // for QWidget
#include <Qt>               // for WidgetAttribute, ToolBarArea, WindowModality, WindowType
#include <QtGlobal>         // for QForeachContainer, qMakeForeachContainer, QTypeInfo<>::isLarge, QTypeI...

#include <serialportconfigdialog.h>     // for SerialPortConfigDialog
#include <serialtransmissiondialog.h>   // for SerialTransmissionDialog
#include <utils/medium.h>               // for Medium
#include <version.h>

#include "gcodefileserver.h"
#include "filechecker.h"        // for FileChecker
#include "ui_gcodefileserver.h" // for Ui::GCodeFileServer


GCodeFileServer::GCodeFileServer(QWidget* parent) : QMainWindow(parent), ui(new Ui::GCodeFileServer)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	setWindowModality(Qt::NonModal);

	setWindowTitle(tr("GCodeWorkShop - Serial port file server"));

	windowMapper = new QSignalMapper(this);
	connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this,
	        SLOT(activeWindowChanged(QMdiSubWindow*)));

	closable = false;
	ui->mdiArea->setViewMode(QMdiArea::TabbedView);
	QTabBar* tab = ui->mdiArea->findChild<QTabBar*>();

	if (tab) {
		//connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
		//tab->setTabsClosable(true);
		// The tabs might be very wide
		tab->setExpanding(false);
	}

	createActions();

	createTrayIcon();

	loadSettings();

	if (startMinimizedAction->isChecked()) {
		QTimer::singleShot(500, this, SLOT(hide()));
	} else {
		showNormal();
	}
}

GCodeFileServer::~GCodeFileServer()
{
	delete ui;
}

void GCodeFileServer::closeEvent(QCloseEvent* event)
{
	//    QMessageBox::StandardButton result = QMessageBox::warning(this, tr("GCodeWorkShop - Serial port file server"),
	//                                                              tr("You are trying to close a file server.\nAre you sure?"),
	//                                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	//    if(result == QMessageBox::No)
	//    {
	//        event->ignore();
	//        return;
	//    }

	if (closable) {
		event->accept();
	} else {
		if (trayIcon->isVisible()) {
			QMessageBox::information(this, tr("GCodeWorkShop - Serial port file server"),
			                         tr("The program will keep running in the "
			                            "system tray. To terminate the program, "
			                            "choose <b>Quit</b> in the context menu "
			                            "of the system tray entry."));
			hide();
			event->ignore();
		}
	}

	//    saveSettings();

	//    foreach(const QMdiSubWindow *window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder))
	//    {
	//        SerialTransmissionDialog *mdiChild = qobject_cast<SerialTransmissionDialog *>(window->widget());
	//        mdiChild->close();
	//        mdiChild->parentWidget()->close();
	//    }

	//    event->accept();
}

void GCodeFileServer::saveSettings()
{
	QStringList activeConfigs;

	foreach (QMdiSubWindow* window, ui->mdiArea->subWindowList()) {
		SerialTransmissionDialog* mdiChild = qobject_cast<SerialTransmissionDialog*>(window->widget());

		if (mdiChild != nullptr) {
			activeConfigs.append(mdiChild->configName());
		}
	}

	QSettings& settings = *Medium::instance().settings();
	settings.beginGroup("CommApp");
	settings.setValue("StartMinimized", startMinimizedAction->isChecked());
	settings.remove("ActiveConfigs");
	activeConfigs.removeDuplicates();
	activeConfigs.removeAll("");
	settings.setValue("ActiveConfigs", activeConfigs);
	settings.endGroup();
}

void GCodeFileServer::loadSettings()
{
	QSettings& settings = *Medium::instance().settings();
	settings.beginGroup("CommApp");

	startMinimizedAction->setChecked(settings.value("StartMinimized", false).toBool());

	QStringList activeConfigs = settings.value("ActiveConfigs",
	                            (QStringList() << "")).toStringList();
	activeConfigs.removeDuplicates();
	activeConfigs.removeAll("");
	settings.endGroup();

	QStringList list = activeConfigs;
	QStringList::Iterator it = list.begin();

	while (it != list.end()) {
		SerialTransmissionDialog* spServer = createSerialPortServer(*it);
		spServer->show();
		++it;
	}
}

SerialTransmissionDialog* GCodeFileServer::createSerialPortServer(QString __config)
{
	SerialTransmissionDialog* spServer = new SerialTransmissionDialog(this, Qt::SubWindow,
	        true);  // Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint
	spServer->setAttribute(Qt::WA_DeleteOnClose);
	ui->mdiArea->addSubWindow(spServer);

	spServer->startFileServer(__config);

	return spServer;
}

void GCodeFileServer::startSerialPortServer()
{
	QString _config = configBox->currentText();

	SerialTransmissionDialog* _existing = findMdiChild(_config);

	if (_existing) {
		_existing->setFocus();
	} else {
		SerialTransmissionDialog* spServer = createSerialPortServer(_config);
		//spServer->startFileServer(_config, true);
		spServer->show();
	}
}

void GCodeFileServer::stopSerialPortServer()
{
	QString _config = configBox->currentText();

	SerialTransmissionDialog* _existing = findMdiChild(_config);

	if (_existing != nullptr) {
		_existing->close();
		_existing->parentWidget()->close();
	}

	changeActiveWindow();
}

void GCodeFileServer::createActions()
{
	configPortAct = new QAction(QIcon(":/images/serialconfig.png"), tr("Serial port configuration"),
	                            this);
	//configPortAct->setShortcut(tr("F3"));
	configPortAct->setToolTip(tr("Serial port configuration"));
	connect(configPortAct, SIGNAL(triggered()), this, SLOT(serialConfig()));

	startServerAct = new QAction(QIcon(":/images/connect_creating.png"), tr("&Start"), this);
	//receiveAct->setShortcut(tr("Shift+F3"));
	startServerAct->setToolTip(tr("Start"));
	connect(startServerAct, SIGNAL(triggered()), this, SLOT(startSerialPortServer()));

	stopServerAct = new QAction(QIcon(":/images/stop.png"), tr("S&top"), this);
	//sendAct->setShortcut(tr("F3"));
	stopServerAct->setToolTip(tr("Stop"));
	connect(stopServerAct, SIGNAL(triggered()), this, SLOT(stopSerialPortServer()));


	resetServerAct = new QAction(QIcon(":/images/reset.png"), tr("&Reset"), this);
	resetServerAct->setToolTip(tr("Reset serial port"));
	connect(resetServerAct, SIGNAL(triggered()), this, SLOT(doPortReset()));


	closeServerAct = new QAction(QIcon(":/images/window-close.png"), tr("&Minimize to tray"), this);
	closeServerAct->setToolTip(tr("Minimize to system tray"));
	connect(closeServerAct, SIGNAL(triggered()), this, SLOT(hide()));

	browseSaveFolderAct = new QAction(QIcon(":/images/browse.png"), tr("&Browse save folder"),
	                                  this);
	browseSaveFolderAct->setToolTip(tr("Browse save folder"));
	connect(browseSaveFolderAct, SIGNAL(triggered()), this, SLOT(browseSaveFolder()));

	showNewFilesAct = new QAction(QIcon(":/images/project_new.png"), tr("&Show saved files"), this);
	showNewFilesAct->setToolTip(tr("Show saved files"));
	connect(showNewFilesAct, SIGNAL(triggered()), this, SLOT(showNewFiles()));

	aboutAction = new QAction(tr("&About"), this);
	aboutAction->setToolTip(tr("Show the application's About box"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));


	minimizeAction = new QAction(tr("Mi&nimize"), this);
	connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

	maximizeAction = new QAction(tr("Ma&ximize"), this);
	connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

	quitAction = new QAction(QIcon(":/images/exit.png"), tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(quitApp()));

	startMinimizedAction = new QAction(tr("Start minimized"), this);
	startMinimizedAction->setCheckable(true);
	startMinimizedAction->setChecked(false);

	configBox = new QComboBox();
	configBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	configBox->setDuplicatesEnabled(false);

	connect(configBox, SIGNAL(currentIndexChanged(int)), SLOT(changeActiveWindow()));

	ui->toolBar->addWidget(configBox);
	ui->toolBar->addAction(configPortAct);
	ui->toolBar->addSeparator();
	ui->toolBar->addAction(startServerAct);
	ui->toolBar->addAction(stopServerAct);

	ui->toolBar->addSeparator();
	ui->toolBar->addAction(resetServerAct);

	fileToolBar = new QToolBar(tr("FileToolBar"));
	addToolBar(Qt::TopToolBarArea, fileToolBar);
	fileToolBar->setObjectName("FileToolBar");


	fileToolBar->addAction(browseSaveFolderAct);
	fileToolBar->addAction(showNewFilesAct);
	fileToolBar->addSeparator();
	fileToolBar->addAction(closeServerAct);

	ui->menu_File->addAction(startMinimizedAction);
	ui->menu_File->addSeparator();
	//    ui->menu_File->addAction(browseSaveFolderAct);
	//    ui->menu_File->addAction(showNewFilesAct);
	//    ui->menu_File->addSeparator();
	ui->menu_File->addAction(closeServerAct);
	ui->menu_File->addSeparator();
	ui->menu_File->addAction(quitAction);

	ui->menu_Help->addAction(aboutAction);


	loadSerialConfignames();
	configBox->adjustSize();
	//updateCurrentSerialConfig();
}

void GCodeFileServer::serialConfig()
{
	SerialPortConfigDialog* serialConfigDialog = new SerialPortConfigDialog(this,
	        configBox->currentText());

	if (serialConfigDialog->exec() == QDialog::Accepted) {
		loadSerialConfignames();
	}
}

void GCodeFileServer::loadSerialConfignames()
{
	int id;
	QStringList list;
	QString item;

	QSettings& settings = *Medium::instance().settings();
	settings.beginGroup("SerialPortConfigs");

	configBox->clear();
	list = settings.value("SettingsList", QStringList(tr("Default"))).toStringList();
	list.sort();
	configBox->addItems(list);
	item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
	id = configBox->findText(item);

	if (id >= 0) {
		configBox->setCurrentIndex(id);
	}

	settings.endGroup();
}

//void CommApp::updateCurrentSerialConfig()
//{
//    int id;
//    QDir dir;

//    bool hasMdiChild = (activeMdiChild() != nullptr);
//    if(hasMdiChild && (serialToolBar != nullptr))
//    {
//        dir.setPath(activeMdiChild()->filePath());
//        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
//        dir.setSorting(QDir::Name);
//        dir.setNameFilters(QStringList("*.ini"));

//        QFileInfoList list = dir.entryInfoList();

//        if(!list.isEmpty())
//        {
//            QFileInfo name = list.at(0);
//            id = configBox->findText(name.baseName());
//            if(id >= 0)
//                configBox->setCurrentIndex(id);
//        }
//    }
//}

void GCodeFileServer::tileSubWindowsVertycally()
{
	//    if(ui->mdiArea->subWindowList().isEmpty())
	//        return;

	//    QPoint position(0, 0);
	//    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
	//    {
	//        QRect rect(0, 0, ui->mdiArea->width(),
	//                   ui->mdiArea->height() / ui->mdiArea->subWindowList().count());
	//        window->setGeometry(rect);
	//        window->move(position);
	//        position.setY(position.y() + window->height());
	//    }
}

SerialTransmissionDialog* GCodeFileServer::findMdiChild(const QString __config)
{
	foreach (QMdiSubWindow* window, ui->mdiArea->subWindowList()) {
		SerialTransmissionDialog* mdiChild = qobject_cast<SerialTransmissionDialog*>(window->widget());

		if (mdiChild != nullptr)
			if (mdiChild->configName() == __config) {
				return mdiChild;
			}
	}

	return nullptr;
}

void GCodeFileServer::setActiveSubWindow(QWidget* window)
{
	if (!window) {
		return;
	}

	ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}

SerialTransmissionDialog* GCodeFileServer::activeMdiChild()
{
	if (QMdiSubWindow* activeSubWindow = ui->mdiArea->activeSubWindow()) {
		return qobject_cast<SerialTransmissionDialog*>(activeSubWindow->widget());
	}

	return nullptr;
}

void GCodeFileServer::activeWindowChanged(QMdiSubWindow* window)
{
	if (!window) {
		return;
	}

	SerialTransmissionDialog* mdiChild = qobject_cast<SerialTransmissionDialog*>
	                                     (window->widget()); //activeMdiChild();

	if (mdiChild != nullptr) {
		mdiChild->setFocus();

		if (configBox) {
			int id = configBox->findText(mdiChild->configName());

			if (id >= 0) {
				configBox->setCurrentIndex(id);
			}
		}

		configPortAct->setEnabled(false);
		startServerAct->setEnabled(false);
		resetServerAct->setEnabled(true);
		stopServerAct->setEnabled(true);
	} else {
		configPortAct->setEnabled(true);
		startServerAct->setEnabled(true);
		resetServerAct->setEnabled(false);
		stopServerAct->setEnabled(false);
	}
}

void GCodeFileServer::changeActiveWindow()
{
	SerialTransmissionDialog* mdiChild = findMdiChild(configBox->currentText());

	if (mdiChild != nullptr) {
		mdiChild->setFocus();
		configPortAct->setEnabled(false);
		startServerAct->setEnabled(false);
		resetServerAct->setEnabled(true);
		stopServerAct->setEnabled(true);
	} else {
		configPortAct->setEnabled(true);
		startServerAct->setEnabled(true);
		resetServerAct->setEnabled(false);
		stopServerAct->setEnabled(false);
	}
}

void GCodeFileServer::closeTab(int i)
{
	QTabBar* tab = ui->mdiArea->findChild<QTabBar*>();

	if (tab != nullptr) {
		tab->removeTab(i);
	}
}

void GCodeFileServer::doPortReset()
{
	SerialTransmissionDialog* mdiChild = activeMdiChild();

	if (mdiChild != nullptr) {
		mdiChild->portReset();
	}
}

void GCodeFileServer::showNewFiles()
{
	SerialTransmissionDialog* mdiChild = activeMdiChild();

	if (mdiChild != nullptr) {
		QSettings& settings = *Medium::instance().settings();
		QStringList extensions = settings.value("Extensions", "").toStringList();
		extensions.removeDuplicates();
		extensions.sort();

		FileChecker* fileCheck = new FileChecker(this);
		fileCheck->setData(mdiChild->savePath(), mdiChild->readPaths(), extensions);
		fileCheck->findFiles();
		fileCheck->exec();
	}
}

void GCodeFileServer::browseSaveFolder()
{
	SerialTransmissionDialog* mdiChild = activeMdiChild();

	if (mdiChild != nullptr) {
		QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(mdiChild->savePath()),
		                               QUrl::TolerantMode));
	}
}

void GCodeFileServer::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(minimizeAction);
	trayIconMenu->addAction(maximizeAction);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(aboutAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
	        SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->setIcon(QIcon(":/images/serial.png"));
	trayIcon->show();
}

void GCodeFileServer::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		if (isVisible()) {
			hide();
		} else {
			showNormal();
		}

		break;

	case QSystemTrayIcon::DoubleClick:
		break;

	case QSystemTrayIcon::MiddleClick:
		break;

	default:
		;
	}
}

void GCodeFileServer::quitApp()
{
	saveSettings();

	foreach (const QMdiSubWindow* window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder)) {
		SerialTransmissionDialog* mdiChild = qobject_cast<SerialTransmissionDialog*>(window->widget());
		mdiChild->close();
		mdiChild->parentWidget()->close();
	}

	closable = true;
	close();
}

void GCodeFileServer::about()
{
	QMessageBox::about(this, tr("About GCodeFileServer"),
	                   tr("The <b>GCodeFileServer</b> is part of GCodeWorkShop."
	                      "<P>Version: %1"
	                      "<P>Copyright (C) 1998 - 2015 by <a href=\"mailto:artkoz78@gmail.com\">Artur Kozioł</a>"
	                      "<br>Copyright (C) 2020 - 2025 by <a href='mailto:nicegorov@yandex.ru'>Nick Egorrov</a>"
	                      "<P>GCodeFileServer contains pieces of code from other Open Source projects."
	                      "<P><i>GCodeFileServer is free software; you can redistribute it and/or modify"
	                      "it under the terms of the GNU General Public License  as published by"
	                      "the Free Software Foundation; either version 2 of the License, or"
	                      "(at your option) any later version.</i>"
	                      "<P><i>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
	                      "INCLUDING THE WARRANTY OF DESIGN,"
	                      "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</i>").arg(GCODEWORKSHOP_VERSION));
}
