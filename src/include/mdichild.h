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
#include <QDir>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <Qt>        // Qt::WindowFlags
#include <QTextEdit> // QTextEdit::ExtraSelection
#include <QWidget>

#include <documentinfo.h>

#include "commoninc.h" // _editor_properites

class QLineEdit;
class QPlainTextEdit;
class QPrinter;

class Highlighter;

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
    QString path() const;
    void setPath(const QString &path);
    QString fileName() const;
    void setFileName(const QString &fileName);
    QString filePath() const;  //filename with full path
    void setFilePath(const QString &filePath);
    DocumentInfo::Ptr documentInfo() const;
    void setDocumentInfo(const DocumentInfo::Ptr &info);
    _editor_properites getMdiWindowProperites();
    void setMdiWindowProperites(_editor_properites opt);
    void highlightFindText(const QString& searchString,
                           QTextDocument::FindFlags options = QTextDocument::FindFlags(),
                           bool ignoreComments = true);
    void doUndo();
    void doRedo();
    void setHighligthMode(int mod);
    int highligthMode() const;
    void doDiff();
    QString brief(); // Text from first comment in CNC program
    QString guessFileName();
    bool foundTextMatched(const QString& pattern, QString text);
    bool findNext(QString textToFind, QTextDocument::FindFlags options, bool ignoreComments);
    bool replaceNext(QString textToFind, QString replacedText, QTextDocument::FindFlags options,
                     bool ignoreComments);
    bool replaceAll(QString textToFind, QString replacedText, QTextDocument::FindFlags options,
                    bool ignoreComments);
    void highlightCodeBlock(QString searchString, int rMin, int rMax);
    void filePrintPreview();
    bool isModified();
    void setModified(bool mod = false);
    bool isReadOnly() const;
    void setReadOnly(bool ro);
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
    void updateBrief();

private:
    bool maybeSave();
    //QString strippedName(const QString &fullFileName);
    void detectHighligthMode();
    bool findText(const QString &text,
                  QTextDocument::FindFlags options = QTextDocument::FindFlags(),
                  bool ignoreComments = true);
    void fileChangeMonitorAddPath(QString fileName);
    void fileChangeMonitorRemovePath(QString fileName);

    QString m_brief;
    QDir m_dir;
    QString m_fileName;
    QString saveFileFilter;
    QByteArray saveDialogState;
    void updateWindowTitle();
    bool isUntitled;
    Highlighter *highlighter;
    int m_highlightMode;
    _editor_properites mdiWindowProperites;
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> findTextExtraSelections;
    QTextEdit::ExtraSelection selection;
    QList<QTextEdit::ExtraSelection> blockExtraSelections;
    //QPointer<QFileSystemWatcher> fileChangeMonitor; //QFileSystemWatcher *fileChangeMonitor;
    QPointer<QLineEdit> calcLineEdit;
    QStringList calcLineEditWordList;
    QChar inLineCalcChar;

private slots :
    void highlightCurrentLine();
    void printPreview(QPrinter *printer);
    //void createContextMenuActions();
    void showContextMenu(const QPoint &pt);
    void documentWasModified();
    void inLineCalcEditingFinished();
    void inLineCalcReturnPressed();

signals:
    void message(const QString &, int);
    void addRemoveFileWatch(const QString &, bool);
};

#endif // MDICHILD_H
