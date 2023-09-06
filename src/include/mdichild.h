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

#ifndef MDICHILD_H
#define MDICHILD_H

#include <QByteArray>
#include <QChar>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <Qt>        // Qt::WindowFlags
#include <QTextEdit> // QTextEdit::ExtraSelection
#include <QWidget>

#include "commoninc.h" // _editor_properites

class QLineEdit;
class QPlainTextEdit;
class QPrinter;

class Highlighter;
class LongJobHelper;

namespace Ui {
class MdiChild;
}


class MdiChild : public QWidget
{
    Q_OBJECT

protected:
    Ui::MdiChild *ui;

public:
    MdiChild(QWidget *parent = 0, Qt::WindowFlags f = Qt::SubWindow);
    ~MdiChild();

    QPlainTextEdit *textEdit();
    void newFile();
    void newFile(const QString &fileName);
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString currentFile();  //filename with full path
    _editor_properites getMdiWindowProperites();
    void setMdiWindowProperites(_editor_properites opt);
    int doRenumber(int &mode, int &startAt, int &from, int &prec, int &inc, int &to,
                   bool &renumEmpty, bool &renumComm, bool &renumMarked);
    void doRemoveSpace();
    void doRemoveEmptyLines();
    void doInsertEmptyLines();
    void doInsertSpace();
    void doInsertDot();
    void doI2M();
    int compileMacro();
    void highlightFindText(const QString& searchString,
                           QTextDocument::FindFlags options = QTextDocument::FindFlags(),
                           bool ignoreComments = true);
    void doUndo();
    void doRedo();
    QString filePath();
    QString fileName();
    void setHighligthMode(int mod);
    int highligthMode();
    void doDiff();
    QString currentFileInfo(); // Text from first comment in CNC program
    QString guessFileName();
    bool foundTextMatched(const QString& pattern, QString text);
    void setCurrentFile(const QString &fileName, const QString &text);
    void semiComment();
    void paraComment();
    bool findNext(QString textToFind, QTextDocument::FindFlags options, bool ignoreComments);
    bool replaceNext(QString textToFind, QString replacedText, QTextDocument::FindFlags options,
                     bool ignoreComments);
    bool replaceAll(QString textToFind, QString replacedText, QTextDocument::FindFlags options,
                    bool ignoreComments);
    bool swapAxes(QString textToFind, QString replacedText, double min, double max, int oper,
                  double modifier, QTextDocument::FindFlags options, bool ignoreComments, int prec);
    void doSwapAxes(QString textToFind, QString replacedText, double min, double max, int oper,
                    double modifier, QTextDocument::FindFlags options, bool ignoreComments, int precision);
    void highlightCodeBlock(QString searchString, int rMin, int rMax);
    void filePrintPreview();
    bool isModified();
    void setModified(bool mod = false);
    bool isReadOnly();
    bool hasSelection();
    bool isUndoAvailable();
    bool isRedoAvailable();
    bool overwriteMode();
    QTextCursor textCursor();
    //    void setFileChangeMonitor(QFileSystemWatcher *monitor);

public slots :
    void showInLineCalc();

protected:
    void changeDateInComment();
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
    bool event(QEvent *event);

private:
    bool maybeSave();
    //QString strippedName(const QString &fullFileName);
    void macroShowError(int error, QString tx);
    void macroShowBasicError(int error);
    void detectHighligthMode();
    bool findText(const QString &text,
                  QTextDocument::FindFlags options = QTextDocument::FindFlags(),
                  bool ignoreComments = true);
    void fileChangeMonitorAddPath(QString fileName);
    void fileChangeMonitorRemovePath(QString fileName);

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
    QList<QTextEdit::ExtraSelection> blockExtraSelections;
    //QPointer<QFileSystemWatcher> fileChangeMonitor; //QFileSystemWatcher *fileChangeMonitor;
    QPointer<QLineEdit> calcLineEdit;
    QStringList calcLineEditWordList;
    QChar inLineCalcChar;
    LongJobHelper *m_Helper;

private slots :
    void highlightCurrentLine();
    void printPreview(QPrinter *printer);
    //void createContextMenuActions();
    void showContextMenu(const QPoint &pt);
    void documentWasModified();
    void cleanUp(QString *str);
    void semiCommentSlot();
    void paraCommentSlot();
    void inLineCalcEditingFinished();
    void inLineCalcReturnPressed();

signals:
    void message(const QString &, int);
    void addRemoveFileWatch(const QString &, bool);
};

#endif // MDICHILD_H
