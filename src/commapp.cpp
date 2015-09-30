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



#include "commapp.h"
#include "ui_commapp.h"


CommApp::CommApp(QWidget *parent) : QMainWindow(parent), ui(new Ui::CommApp)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    setWindowModality(Qt::NonModal);

    setWindowTitle(tr("EdytorNC - Serial port file server"));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(activeWindowChanged(QMdiSubWindow *)));


    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    QTabBar* tab = ui->mdiArea->findChild<QTabBar*>();
    if(tab)
    {
        //connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        //tab->setTabsClosable(true);
        // The tabs might be very wide
        tab->setExpanding(false);
    };

    createSerialToolBar();

    showNormal();
    loadSettings();
}

//**************************************************************************************************
//
//**************************************************************************************************

CommApp::~CommApp()
{
    delete ui;
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::closeEvent(QCloseEvent *event)
{
    saveSettings();

    foreach(const QMdiSubWindow *window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder))
    {
        SerialTransmissionDialog *mdiChild = qobject_cast<SerialTransmissionDialog *>(window->widget());
        mdiChild->close();
        mdiChild->parentWidget()->close();
    };

    event->accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::saveSettings()
{
    QStringList activeConfigs;

    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        SerialTransmissionDialog *mdiChild = qobject_cast<SerialTransmissionDialog *>(window->widget());
        if(mdiChild > NULL)
            activeConfigs.append(mdiChild->configName());
    };

    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("CommApp");
    settings.remove("ActiveConfigs");
    activeConfigs.removeDuplicates();
    activeConfigs.removeAll("");
    settings.setValue("ActiveConfigs", activeConfigs);
    settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::loadSettings()
{
    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("CommApp");

    QStringList activeConfigs = settings.value("ActiveConfigs", (QStringList() << "")).toStringList();
    activeConfigs.removeDuplicates();
    activeConfigs.removeAll("");
    settings.endGroup();

    QStringList list = activeConfigs;
    QStringList::Iterator it = list.begin();
    while(it != list.end())
    {
        SerialTransmissionDialog *spServer = createSerialPortServer(*it);
        spServer->show();
        ++it;
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

SerialTransmissionDialog *CommApp::createSerialPortServer(QString __config)
{
    SerialTransmissionDialog *spServer = new SerialTransmissionDialog(this, Qt::SubWindow, true);  // Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint
    spServer->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(spServer);

    spServer->startFileServer(__config);

    return spServer;
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::startSerialPortServer()
{
    QString _config = configBox->currentText();

    SerialTransmissionDialog *_existing = findMdiChild(_config);
    if(_existing)
        _existing->setFocus();
    else
    {
        SerialTransmissionDialog *spServer = createSerialPortServer(_config);
        //spServer->startFileServer(_config, true);
        spServer->show();
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::stopSerialPortServer()
{
    QString _config = configBox->currentText();

    SerialTransmissionDialog *_existing = findMdiChild(_config);
    if(_existing > NULL)
    {
        _existing->close();
        _existing->parentWidget()->close();
    };

    changeActiveWindow();
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::createSerialToolBar()
{
    configPortAct = new QAction(QIcon(":/images/serialconfig.png"), tr("Serial port configuration"), this);
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


    closeServerAct = new QAction(QIcon(":/images/exit.png"), tr("&Close"), this);
    closeServerAct->setToolTip(tr("Close"));
    connect(closeServerAct, SIGNAL(triggered()), this, SLOT(close()));

    browseSaveFolderAct = new QAction(QIcon(":/images/browse.png"), tr("&Browse save folder"), this);
    browseSaveFolderAct->setToolTip(tr("Browse save folder"));
    connect(browseSaveFolderAct, SIGNAL(triggered()), this, SLOT(browseSaveFolder()));

    showNewFilesAct = new QAction(QIcon(":/images/project_new.png"), tr("&Show saved files"), this);
    showNewFilesAct->setToolTip(tr("Show saved files"));
    connect(showNewFilesAct, SIGNAL(triggered()), this, SLOT(showNewFiles()));

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

    ui->menu_File->addAction(closeServerAct);


    loadSerialConfignames();
    configBox->adjustSize();
    //updateCurrentSerialConfig();
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::serialConfig()
{
    SerialPortConfigDialog *serialConfigDialog = new SerialPortConfigDialog(this, configBox->currentText());

    if(serialConfigDialog->exec() == QDialog::Accepted)
        loadSerialConfignames();
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::loadSerialConfignames()
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
    if(id >= 0)
        configBox->setCurrentIndex(id);

    settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

//void CommApp::updateCurrentSerialConfig()
//{
//    int id;
//    QDir dir;

//    bool hasMdiChild = (activeMdiChild() != NULL);
//    if(hasMdiChild && (serialToolBar > NULL))
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
//        };
//    };
//}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::tileSubWindowsVertycally()
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

//**************************************************************************************************
//
//**************************************************************************************************

SerialTransmissionDialog *CommApp::findMdiChild(const QString __config)
{
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        SerialTransmissionDialog *mdiChild = qobject_cast<SerialTransmissionDialog *>(window->widget());
        if(mdiChild > NULL)
            if(mdiChild->configName() == __config)
                return mdiChild;
    };
    return NULL;
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::setActiveSubWindow(QWidget *window)
{
    if(!window)
        return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//**************************************************************************************************
//
//**************************************************************************************************

SerialTransmissionDialog *CommApp::activeMdiChild()
{
    if(QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<SerialTransmissionDialog *>(activeSubWindow->widget());
    return NULL;
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::activeWindowChanged(QMdiSubWindow *window)
{
    if(!window)
        return;

    SerialTransmissionDialog *mdiChild = qobject_cast<SerialTransmissionDialog *>(window->widget()); //activeMdiChild();

    if(mdiChild > NULL)
    {
        mdiChild->setFocus();

        if(configBox)
        {
            int id = configBox->findText(mdiChild->configName());
            if(id >= 0)
                configBox->setCurrentIndex(id);
        };

        configPortAct->setEnabled(false);
        startServerAct->setEnabled(false);
        resetServerAct->setEnabled(true);
        stopServerAct->setEnabled(true);
    }
    else
    {
        configPortAct->setEnabled(true);
        startServerAct->setEnabled(true);
        resetServerAct->setEnabled(false);
        stopServerAct->setEnabled(false);
    };

}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::changeActiveWindow()
{
    SerialTransmissionDialog *mdiChild = findMdiChild(configBox->currentText());

    if(mdiChild > NULL)
    {
        mdiChild->setFocus();
        configPortAct->setEnabled(false);
        startServerAct->setEnabled(false);
        resetServerAct->setEnabled(true);
        stopServerAct->setEnabled(true);
    }
    else
    {
        configPortAct->setEnabled(true);
        startServerAct->setEnabled(true);
        resetServerAct->setEnabled(false);
        stopServerAct->setEnabled(false);
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::closeTab(int i)
{
    QTabBar* tab = ui->mdiArea->findChild<QTabBar*>();
    if(tab > NULL)
    {
        tab->removeTab(i);
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::doPortReset()
{
    SerialTransmissionDialog *mdiChild = activeMdiChild();
    if(mdiChild > NULL)
        mdiChild->portReset();
}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::showNewFiles()
{
    SerialTransmissionDialog *mdiChild = activeMdiChild();
    if(mdiChild > NULL)
    {
        QSettings settings("EdytorNC", "EdytorNC");
        QStringList extensions = settings.value("Extensions", "").toStringList();
        extensions.removeDuplicates();
        extensions.sort();

        FileChecker *fileCheck = new FileChecker(this);
        fileCheck->setData(mdiChild->savePath(), mdiChild->readPaths(), extensions);
        fileCheck->findFiles();
        fileCheck->exec();
    };

}

//**************************************************************************************************
//
//**************************************************************************************************

void CommApp::browseSaveFolder()
{
    SerialTransmissionDialog *mdiChild = activeMdiChild();
    if(mdiChild > NULL)
        QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(mdiChild->savePath()), QUrl::TolerantMode));
}




