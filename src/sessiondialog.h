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


#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QtWidgets>
#include <QString>
#include <QStringList>


#include "ui_sessiondialog.h"
#include "ui_newsessiondialog.h"

class sessionDialog: public QDialog, private Ui::sessionDialog
{
   Q_OBJECT

   public:
     sessionDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
     ~sessionDialog();

     void setSelectedSession(QString name);
     QString selectedSession();
     QStringList sessionList();
     void setSessionList(QStringList list);


public slots:


   protected:

   signals :



   private slots:
     void newButtonClicked();
     void renameButtonClicked();
     void deleteButtonClicked();
     void cloneButtonClicked();
     void switchButtonClicked();
     void sessionListItemitemActivated(QListWidgetItem *item);



   private:
     void clearChecked();
     void copySession(QString oldName, QString newName, bool deleteOld = false);
     void deleteSession(QString name);


};


//**************************************************************************************************
//
//**************************************************************************************************


class newSessionDialog: public QDialog, private Ui::newSessionDialog
{
   Q_OBJECT

   public:
     newSessionDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
     ~newSessionDialog();

   public slots:
     QString getName();
     void setName(QString name);

   protected:

   signals :



   private slots:



   private:



};


#endif // SESSIONDIALOG_H
