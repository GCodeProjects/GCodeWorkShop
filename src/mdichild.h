/***************************************************************************
 *   Copyright (C) 2006-2013 by Artur Kozioł                               *
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

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QtGui>

#include "highlighter.h"
#include "commoninc.h"
#include "basic_interpreter.h"

#include "ui_mdichildform.h"


#define ERR_NO_BRAC       -1
#define ERR_NO_PARAM      -2
#define ERR_CONVERT       -3
#define ERR_UNKNOWN_FUNC  -4
#define ERR_DOUBLE_DOT    -5


#define FILENAME_SINU840     "%_N_[a-zA-Z0-9_]{1,31}_(MPF|SPF|TEA|COM|PLC|DEF|INI)\\n"
#define FILENAME_OSP         "\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"
#define FILENAME_FANUC1      "(:|O)[0-9]{1,}"
#define FILENAME_FANUC2      "O[0-9]{1,}"
#define FILENAME_SINU        "%\\b(MPF|SPF|TEA)[\\s]{0,3}[0-9]{1,4}\\b"
#define FILENAME_HEID1       "%[a-zA-Z0-9_]{1,30}(\\s)"
#define FILENAME_HEID2       "(BEGIN)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)"
#define FILENAME_PHIL        "%PM[\\s]{1,}N[0-9]{4,4}"



class MdiChild : public QWidget, public Ui::MdiChild
{
    Q_OBJECT

public:
    MdiChild(QWidget *parent = 0, Qt::WindowFlags f = Qt::SubWindow);
    ~MdiChild();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString currentFile() { return QDir::toNativeSeparators(curFile); }
    _editor_properites getMdiWindowProperites();
    void setMdiWindowProperites(_editor_properites opt);
    int doRenumber(int &mode, int &startAt, int &from, int &prec, int &inc, int &to, bool &renumEmpty, bool &renumComm, bool &renumMarked);
    void doRemoveSpace();
    void doRemoveEmptyLines();
    void doInsertEmptyLines();
    void doInsertSpace();
    void doInsertDot();
    void doI2M();
    int compileMacro();
    void highlightFindText(QString searchString, QTextDocument::FindFlags options = 0, bool ignoreComments = true);
    void doUndo();
    void doRedo();
    QString filePath();
    QString fileName();
    void setHighligthMode(int mod);
    int getHighligthMode();
    void doDiff();
    QString getCurrentFileInfo();
    bool findText(const QString &text, QTextDocument::FindFlags options = 0, bool ignoreComments = true);
    QString guessFileName();
    QStringList splitFile();
    bool foundTextMatched(QString pattern, QString text);
    void setCurrentFile(const QString &fileName, const QString &text);



protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
    bool event(QEvent *event);

private slots:
    void documentWasModified();
    void cleanUp(QString *str);


private:
    bool maybeSave();
    //QString strippedName(const QString &fullFileName);
    int processBrc(QString *str);
    int compute(QString *str);
    void macroShowError(int error, QString tx);
    void macroShowBasicError(int error);
    void detectHighligthMode();




    QString curFileInfo;
    QString curFile;
    QString saveFileFilter;
    QByteArray saveDialogState;
    void updateWindowTitle();
    bool isUntitled;
    Highlighter *highlighter;
    _editor_properites mdiWindowProperites;
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> findTextExtraSelections;
    QTextEdit::ExtraSelection selection;




private slots :
    void highlightCurrentLine();

signals:
    void message(const QString&, int );

};


#endif
