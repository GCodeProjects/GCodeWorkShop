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



#ifndef COMMAPP_H
#define COMMAPP_H

#include <QMainWindow>
#include <QComboBox>

#include "serialtransmissiondialog.h"
#include "serialportconfigdialog.h"


namespace Ui
{
class CommApp;
}

class CommApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommApp(QWidget *parent = 0);
    ~CommApp();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void serialConfig();
    void tileSubWindowsVertycally();
    void setActiveSubWindow(QWidget *window);
    void startSerialPortServer();
    void stopSerialPortServer();
    void activeWindowChanged(QMdiSubWindow *window);
    void changeActiveWindow();
    void closeTab(int i);
    void doPortReset();

private:
    Ui::CommApp *ui;
    QSignalMapper *windowMapper;

    QAction *configPortAct;
    QAction *startServerAct;
    QAction *stopServerAct;
    QAction *resetServerAct;
    QAction *closeServerAct;
    QAction *browseSaveFolderAct;
    QAction *showNewFilesAct;
    QComboBox *configBox;
    QToolBar *fileToolBar;


    void saveSettings();
    void loadSettings();
    void loadSerialConfignames();
    void createSerialToolBar();
    SerialTransmissionDialog *findMdiChild(const QString __config);
    SerialTransmissionDialog *createSerialPortServer(QString __config);
    SerialTransmissionDialog *activeMdiChild();
};

#endif // COMMAPP_H
