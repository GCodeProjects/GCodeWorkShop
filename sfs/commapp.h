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

#ifndef COMMAPP_H
#define COMMAPP_H

#include <QObject>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QWidget>

class QAction;
class QComboBox;
class QCloseEvent;
class QMenu;
class QMdiSubWindow;
class QSignalMapper;
class QToolBar;

class SerialTransmissionDialog;

namespace Ui {
class CommApp;
}


class CommApp : public QMainWindow
{
	Q_OBJECT

public:
	explicit CommApp(QWidget* parent = 0);
	~CommApp();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void serialConfig();
	void tileSubWindowsVertycally();
	void setActiveSubWindow(QWidget* window);
	void startSerialPortServer();
	void stopSerialPortServer();
	void activeWindowChanged(QMdiSubWindow* window);
	void changeActiveWindow();
	void closeTab(int i);
	void doPortReset();
	void showNewFiles();
	void browseSaveFolder();
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void quitApp();
	void about();

private:
	Ui::CommApp* ui;
	QSignalMapper* windowMapper;

	QAction* configPortAct;
	QAction* startServerAct;
	QAction* stopServerAct;
	QAction* resetServerAct;
	QAction* closeServerAct;
	QAction* browseSaveFolderAct;
	QAction* showNewFilesAct;
	QComboBox* configBox;
	QToolBar* fileToolBar;
	QAction* minimizeAction;
	QAction* maximizeAction;
	QAction* restoreAction;
	QAction* quitAction;
	QAction* aboutAction;
	QAction* startMinimizedAction;

	QSystemTrayIcon* trayIcon;
	QMenu* trayIconMenu;
	bool closable;

	SerialTransmissionDialog* findMdiChild(const QString __config);
	SerialTransmissionDialog* createSerialPortServer(QString __config);
	SerialTransmissionDialog* activeMdiChild();

	void saveSettings();
	void loadSettings();
	void loadSerialConfignames();
	void createActions();
	void createTrayIcon();
};

#endif // COMMAPP_H
