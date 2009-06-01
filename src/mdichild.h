/***************************************************************************
 *   Copyright (C) 2009 by Artur Kozioł                                    *
 *   artkoz@poczta.onet.pl                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QMainWindow>

#include "highlighter.h"
#include "commoninc.h"
#include "customfiledialog.h"

#include "ui_mdichildform.h"

class MdiChild : public QWidget, public Ui::MdiChild
{
    Q_OBJECT

public:
    MdiChild(QWidget *parent = 0, Qt::WindowFlags f = Qt::SubWindow);

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveAsWithPreview();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    _editor_properites getMdiWindowProperites();
    void setMdiWindowProperites(_editor_properites opt);
    int doRenumber(int &mode, int &startAt, int &from, int &prec, int &inc, bool &renumEmpty, bool &renumComm);
    void doRemoveSpace();
    void doInsertSpace();
    void doInsertDot();


protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    void documentWasModified();
    void cleanUp(QString *str);

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName, const QString &text);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    QString saveFileFilter;
    QByteArray saveDialogState;
    bool isUntitled;
    Highlighter *highlighter;
    _editor_properites mdiWindowProperites;


};


#endif
