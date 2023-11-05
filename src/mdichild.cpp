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

#include <algorithm> // std::sort()

#include <QAction>
#include <QApplication>
#include <QChar>
#include <QCloseEvent>
#include <QColor>
#include <QCompleter>
#include <QDate>
#include <QDir>
#include <QEvent>              // QEvent::KeyPress
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QIODevice>
#include <QKeyEvent>
#include <QLatin1Char>
#include <QLatin1String>
#include <QLineEdit>
#include <QLocale>
#include <QMenu>
#include <QMessageBox>
#include <QMarginsF>
#include <QPageLayout>
#include <QPalette>
#include <QPoint>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <Qt>                  // Qt::WindowFlags
#include <QtDebug>             // qDebug()
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextOption>
#include <QTextStream>
#include <QtGlobal>            // QT_VERSION QT_VERSION_CHECK
#include <QToolTip>

#include <addons-actions.h>
#include <commoninc.h>         // _editor_properites
#include <edytornc.h>
#include <ui/longjobhelper.h>  // LongJobHelper
#include <utils/expressionparser.h>
#include <utils/removezeros.h>      // Utils::removeZeros()

#include "basic_interpreter.h" // BasicInterpreter
#include "highlighter.h"       // Highlighter
#include "mdichild.h"          // MdiChild QObject QWidget
#include "ui_mdichildform.h"


MdiChild::MdiChild(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    ui = new Ui::MdiChild();
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
    ui->textEdit->document()->setDocumentMargin(8);
    highlighter = nullptr;
    setFocusProxy(ui->textEdit);

    ui->marginWidget->setAutoFillBackground(true);

    ui->textEdit->installEventFilter(this);
    ui->textEdit->viewport()->installEventFilter(this);
    setWindowIcon(QIcon(":/images/ncfile.png"));

    //fileChangeMonitor.clear();

    ui->splitterH->setBackgroundRole(QPalette::Base);
    ui->marginWidget->setBackgroundRole(QPalette::Base);


    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
    m_Helper = new LongJobHelper(parent);
}

MdiChild::~MdiChild()
{
    if (highlighter != nullptr) {
        delete highlighter;
    }

    delete m_Helper;
}

QPlainTextEdit *MdiChild::textEdit()
{
    return ui->textEdit;
}

void MdiChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("program%1.nc").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    curFileInfo = curFile;
    ui->textEdit->document()->setModified(false);

    connect(ui->textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

void MdiChild::newFile(const QString &fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString tex = in.readAll();
        ui->textEdit->setPlainText(tex);
        file.close();
        QApplication::restoreOverrideCursor();
        fileChangeMonitorAddPath(file.fileName());

    } else {
        QMessageBox::warning(this, tr("EdytorNC"),
                             tr("Cannot read file \"%1\".\n %2")
                             .arg(fileName)
                             .arg(file.errorString()));
    }

    newFile();
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString tex = in.readAll();
        ui->textEdit->setPlainText(tex);
        file.close();
        QApplication::restoreOverrideCursor();

        setCurrentFile(fileName, tex);
        connect(ui->textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
        fileChangeMonitorAddPath(fileName);
        return true;
    } else {
        QMessageBox::warning(this, tr("EdytorNC"),
                             tr("Cannot read file \"%1\".\n %2")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
}

bool MdiChild::save()
{
    bool result = false;

    setFocus();

    if (isUntitled) {
        result = saveAs();
    } else {
        result = saveFile(curFile);
    }

    if (result) {
        if (mdiWindowProperites.clearUndoHistory) {
            ui->textEdit->setUndoRedoEnabled(false);  //clear undo/redo history
            ui->textEdit->setUndoRedoEnabled(true);
        }

        if (mdiWindowProperites.clearUnderlineHistory) {

            QTextCursor cursorPos = ui->textEdit->textCursor();
            ui->textEdit->blockSignals(true);
            ui->textEdit->selectAll();

            if (mdiWindowProperites.underlineChanges) {
                QTextCursor cr = ui->textEdit->textCursor(); // Clear underline
                QTextCharFormat format = cr.charFormat();
                format.setUnderlineStyle(QTextCharFormat::NoUnderline);
                cr.setCharFormat(format);

                ui->textEdit->setTextCursor(cr);
            }

            ui->textEdit->setTextCursor(cursorPos);

            ui->textEdit->document()->setModified(false);
            documentWasModified();
            ui->textEdit->blockSignals(false);
        }
    }

    detectHighligthMode();
    return result;
}

bool MdiChild::saveAs()
{
    QString fileName, filters, saveExt;

#ifdef Q_OS_LINUX
    QString extText = tr("CNC programs files %1 (%1);;");
#elif defined Q_OS_WIN32
    QString extText = tr("CNC programs files (%1);;");
#elif defined Q_OS_MACX
    QString extText = tr("CNC programs files %1 (%1);;");
#endif

    filters = extText.arg(mdiWindowProperites.saveExtension);

    foreach (const QString ext, mdiWindowProperites.extensions) {
        saveExt = extText.arg(ext);

        if (ext != mdiWindowProperites.saveExtension) {
            filters.append(saveExt);
        }
    }

    filters.append(tr("Text files (*.txt);;"
                      "All files (*.* *)"));

    if (isUntitled) {
        fileName = guessFileName();
    } else {
        fileName = curFile;
    }

    if (QFileInfo(fileName).suffix() ==
            "") { // sometimes when file has no extension QFileDialog::getSaveFileName will no apply choosen filter (extension)
        fileName.append(".nc");
    }

    QString file = QFileDialog::getSaveFileName(
                       this,
                       tr("Save file as..."),
                       fileName,
                       filters, &saveFileFilter, QFileDialog::DontConfirmOverwrite);

    if (file.isEmpty() || file.isNull()) {
        return false;
    }

    if (QFileInfo(file).suffix() == "") {
    }

    if ((QFile(file).exists())) {
        QMessageBox msgBox;
        msgBox.setText(tr("<b>File \"%1\" exists.</b>").arg(file));
        msgBox.setInformativeText(tr("Do you want overwrite it ?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
        msgBox.setDefaultButton(QMessageBox::Discard);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Save:
            break;

        case QMessageBox::Discard:
            return false;
            break;

        default:
            return false;
            break;
        }
    }

    fileChangeMonitorRemovePath(fileName);
    return saveFile(file);
}

bool MdiChild::saveFile(const QString &fileName)
{
    int curPos;
    QTextCursor cursor;
    QFile file(fileName);
    fileChangeMonitorRemovePath(file.fileName());
    curPos = ui->textEdit->textCursor().position();

    if (file.open(QIODevice::WriteOnly)) {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        changeDateInComment();

        QTextStream out(&file);

        QString tex = ui->textEdit->toPlainText();

        if (!tex.contains(QLatin1String("\r\n"))) {
            tex.replace(QLatin1String("\n"), QLatin1String("\r\n"));
        }

        out << tex;
        file.close();
        QApplication::restoreOverrideCursor();

        cursor = ui->textEdit->textCursor();
        cursor.setPosition(curPos);
        ui->textEdit->setTextCursor(cursor);

        setCurrentFile(fileName, tex);
        fileChangeMonitorAddPath(file.fileName());
        return true;
    } else {
        QMessageBox::warning(this, tr("EdytorNC"),
                             tr("Cannot write file \"%1\".\n %2")
                             .arg(fileName)
                             .arg(file.errorString()));
    }

    return false;
}

void MdiChild::changeDateInComment()
{
    if (!mdiWindowProperites.changeDateInComment) {
        return;
    }

    QRegularExpression regex;
    QString strDate = QLocale().toString(QDate::currentDate(), QLocale::ShortFormat);
    regex.setPattern(tr("(DATE)") + "[:\\s]*[\\d]{1,4}(\\.|-|/)[\\d]{1,2}(\\.|-|/)[\\d]{2,4}");
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);

    cursor = ui->textEdit->document()->find(regex, cursor);

    if (!cursor.isNull()) {
        ui->textEdit->setUpdatesEnabled(false);
        cursor.beginEditBlock();
        cursor.removeSelectedText();
        cursor.insertText(tr("DATE") + ": " + strDate);
        cursor.endEditBlock();

        ui->textEdit->setUpdatesEnabled(true);
        ui->textEdit->repaint();
    } else {
        cursor = ui->textEdit->textCursor();

        regex.setPattern("(\\(){1,1}[\\s]{0,}[\\d]{1,4}(\\.|-|/)[\\d]{1,2}(\\.|-|/)[\\d]{2,4}[\\s]{0,5}(\\)){1,1}");
        cursor.setPosition(0);
        cursor = ui->textEdit->document()->find(regex, cursor);

        if (cursor.isNull()) {
            regex.setPattern("(;){1,1}[\\s]{0,}[\\d]{1,4}(\\.|-|/)[\\d]{1,2}(\\.|-|/)[\\d]{2,4}[\\s]{0,5}");
            cursor.setPosition(0);
            cursor = ui->textEdit->document()->find(regex, cursor);
        }

        if (!cursor.isNull()) {
            ui->textEdit->setUpdatesEnabled(false);
            cursor.beginEditBlock();
            QString text = cursor.selectedText();
            cursor.removeSelectedText();

            if (text.contains('(')) {
                text = "(" + strDate + ")";
            } else {
                text = ";" + strDate;
            }

            cursor.insertText(text);
            cursor.endEditBlock();

            ui->textEdit->setUpdatesEnabled(true);
            ui->textEdit->repaint();
        }
    }
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        fileChangeMonitorRemovePath(currentFile());
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::documentWasModified()
{
    setWindowModified(ui->textEdit->document()->isModified());
}

bool MdiChild::maybeSave()
{
    if (ui->textEdit->document()->isModified()) {
        QMessageBox msgBox;
        msgBox.setText(tr("<b>File: \"%1\"\n has been modified.</b>").arg(curFile));
        msgBox.setInformativeText(tr("Do you want to save your changes ?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setIcon(QMessageBox::Warning);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Save:
            return save();
            break;

        case QMessageBox::Discard:
            ui->textEdit->document()->setModified(false);
            return true;
            break;

        case QMessageBox::Cancel:
            return false;
            break;

        default:
            return true;
            break;
        } ;
    }

    return true;
}

void MdiChild::setCurrentFile(const QString &fileName, const QString &text)
{
    QRegularExpression regex;
    QString f_tx;

    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    ui->textEdit->document()->setModified(false);
    setWindowModified(false);

    regex.setPattern("\\([^\\n\\r]*\\)|;[^\\n\\r]*"); //find first comment and set it in window tilte
    auto match = regex.match(text);

    while (match.hasMatch()) {
        f_tx = match.captured();

        if (!(f_tx.mid(0, 2) == QLatin1String(";$"))) {
            f_tx.remove(QLatin1Char('('));
            f_tx.remove(QLatin1Char(')'));
            f_tx.remove(QLatin1Char(';'));
            break;
        }

        match = regex.match(text, match.capturedEnd());
    }

    if (f_tx.isEmpty()) {
        curFileInfo = QFileInfo(curFile).fileName();
    } else {
        curFileInfo = f_tx.simplified();
    }

    updateWindowTitle();
}

void MdiChild::updateWindowTitle()
{
    QString title = "";

    if ((mdiWindowProperites.windowMode & SHOW_PROGTITLE)) {
        title = curFileInfo;
    }

    if (!title.isEmpty() && ((mdiWindowProperites.windowMode & SHOW_FILEPATH)
                             || (mdiWindowProperites.windowMode & SHOW_FILENAME))) {
        title += " ---> ";
    }

    if ((mdiWindowProperites.windowMode & SHOW_FILEPATH)) {
        title += (QFileInfo(curFile).canonicalPath().isEmpty() ? "" : (QDir::toNativeSeparators(
                      QFileInfo(curFile).canonicalPath() + "/")));
    }

    if ((mdiWindowProperites.windowMode & SHOW_FILENAME)) {
        title += QFileInfo(curFile).fileName();
    }

    if (title.isEmpty()) {
        title += QFileInfo(curFile).fileName();
    }

    title += "[*]";
    setWindowTitle(title);
}

//QString MdiChild::strippedName(const QString &fullFileName)
//{
//   return QFileInfo(fullFileName).fileName();
//}

QString MdiChild::filePath()
{
    return QFileInfo(curFile).absolutePath();
}

QString MdiChild::fileName()
{
    return QFileInfo(curFile).fileName();
}

_editor_properites MdiChild::getMdiWindowProperites()
{
    mdiWindowProperites.isRedo = ui->textEdit->document()->isRedoAvailable();
    mdiWindowProperites.isUndo = ui->textEdit->document()->isUndoAvailable();
    mdiWindowProperites.ins = ui->textEdit->overwriteMode();
    mdiWindowProperites.modified = ui->textEdit->document()->isModified();
    mdiWindowProperites.readOnly = ui->textEdit->isReadOnly();
    mdiWindowProperites.isSel = ui->textEdit->textCursor().hasSelection();
    mdiWindowProperites.cursorPos = ui->textEdit->textCursor().position(); //textCursor().blockNumber();
    mdiWindowProperites.geometry = parentWidget()->saveGeometry();

    mdiWindowProperites.fileName = curFile;
    return (mdiWindowProperites);
}

void MdiChild::setMdiWindowProperites(_editor_properites opt)
{
    disconnect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    mdiWindowProperites = opt;
    ui->textEdit->setReadOnly(mdiWindowProperites.readOnly);
    setFont(QFont(mdiWindowProperites.fontName, mdiWindowProperites.fontSize, QFont::Normal));

    QPalette pal;

    if (mdiWindowProperites.hColors.backgroundColor != 0xFFFFFF) {
        pal.setColor(QPalette::Base, QColor(mdiWindowProperites.hColors.backgroundColor));
    }

    pal.setColor(QPalette::Text, QColor(mdiWindowProperites.hColors.defaultColor));

    setPalette(pal);

    if (mdiWindowProperites.syntaxH) {
        if (highlighter == nullptr) {
            highlighter = new Highlighter(ui->textEdit->document());
        }

        if (highlighter != nullptr) {
            detectHighligthMode();
        }
    } else {
        if (highlighter != nullptr) {
            delete (highlighter);
        }

        highlighter = nullptr;
    }

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(mdiWindowProperites.cursorPos);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->centerCursor();
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    updateWindowTitle();
}

bool MdiChild::eventFilter(QObject *obj, QEvent *ev)
{
    //qDebug() << "E" << ev->type() << obj->objectName();

    //better word selection
    if ((obj == ui->textEdit->viewport()) && (ev->type() == QEvent::MouseButtonDblClick)) {
        QString key = "";
        QString wordDelimiters = "()[]=,;:/ ";
        bool wasLetter = false;
        int posStart, posEnd;
        QTextCursor cursor = ui->textEdit->textCursor();

        while (true) {
            if (cursor.atBlockStart() || cursor.atStart()) {
                break;
            }

            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            key = cursor.selectedText();

            if (cursor.atBlockStart() || cursor.atStart()) {
                break;
            }

            if (key.isEmpty()) {
                break;
            }

            if (key.at(0).isSpace()) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                break;
            }

            if (key.at(0).isLetter()) {
                wasLetter = true;
            }

            if ((key.at(0).isDigit() || (key.at(0) == '.')) && wasLetter) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                break;
            }

            if (wordDelimiters.contains(key.at(0))) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                break;
            }
        }

        posStart = cursor.position();

        if (!cursor.atEnd() && !cursor.atBlockEnd()) {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
        }

        wasLetter = true;

        while (true) {
            if (cursor.atEnd() || cursor.atBlockEnd()) {
                break;
            }

            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            key = cursor.selectedText();

            if (cursor.atEnd()) {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                break;
            }

            if (key.at(key.length() - 1).isSpace()) {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                break;
            }

            if (key.at(key.length() - 1).isDigit()) {
                wasLetter = false;
            }

            if (key.at(key.length() - 1).isLetter() && !wasLetter) {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                break;
            }

            if (wordDelimiters.contains(key.at(key.length() - 1))) {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                break;
            }
        }

        posEnd = cursor.position();

        cursor.setPosition(posStart, QTextCursor::MoveAnchor);
        cursor.setPosition(posEnd, QTextCursor::KeepAnchor);
        ui->textEdit->setTextCursor(cursor);

        QKeyEvent *k = (QKeyEvent *) ev;

        if (k->modifiers() == Qt::ControlModifier) {
            showInLineCalc();
        }

        return true;
    }

    if ((obj == ui->textEdit) && !(ui->textEdit->isReadOnly())) {
        if (ev->type() == QEvent::KeyPress) {
            QKeyEvent *k = (QKeyEvent *) ev;

            if (k->key() == Qt::Key_Insert) {
                ui->textEdit->setOverwriteMode(!ui->textEdit->overwriteMode());
            }

            if (mdiWindowProperites.underlineChanges) {
                if ((k->text()[0].isPrint()) && !(k->text()[0].isSpace())) {
                    QTextCursor cr = ui->textEdit->textCursor(); //Underline changes
                    QTextCharFormat format = cr.charFormat();
                    format.setUnderlineStyle(QTextCharFormat::DotLine);
                    format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
                    cr.setCharFormat(format);
                    ui->textEdit->setTextCursor(cr);
                }
            }

            if (k->key() == Qt::Key_Comma) { //Keypad comma should always prints period
                if ((k->modifiers() == Qt::KeypadModifier)
                        || (k->nativeScanCode() == 0x53)) { // !!! Qt::KeypadModifier - Not working for keypad comma !!!
                    QApplication::sendEvent(ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period,
                                            Qt::NoModifier, ".", false, 1));
                    return true;
                }

            }

            if (mdiWindowProperites.intCapsLock) {
                if (k->text()[0].isLower() && (k->modifiers() == Qt::NoModifier)) {
                    QApplication::sendEvent(ui->textEdit, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::NoModifier,
                                            k->text().toUpper(), false, 1));
                    return true;

                }

                if (k->text()[0].isUpper() && (k->modifiers() == Qt::ShiftModifier)) {
                    QApplication::sendEvent(ui->textEdit, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::ShiftModifier,
                                            k->text().toLower(), false, 1));
                    return true;
                }
            }
        }

        return false;
    } else {
        //return ui->textEdit->eventFilter(obj, ev);
        return false;
    }
}

int MdiChild::doRenumber(int &mode, int &startAt, int &from, int &prec, int &inc, int &to,
                         bool &renumEmpty, bool &renumComm, bool &renumMarked)
{
    int pos, count, lineCount;
    long int i, num, it;
    QString tx, f_tx, line, i_tx, new_tx;
    QRegularExpression regex;
    bool ok, selection, insertSpace;
    QTextCursor cursor;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    cursor = ui->textEdit->textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setUnderlineStyle(QTextCharFormat::NoUnderline);
    cursor.mergeCharFormat(format);

    if (cursor.hasSelection()) {
        tx = ui->textEdit->textCursor().selectedText();
        tx.replace(QChar::ParagraphSeparator, "\n");
        selection = true;
    } else {
        tx = ui->textEdit->toPlainText();
        selection = false;
    }

    count = 0;

    while (1) {
        if (mode == 4) { //renumber lines without N
            num = startAt;
            lineCount = ui->textEdit->document()->lineCount();
            regex.setPattern("^[0-9]{1,9}\\s\\s");

            for (i = 0; i < lineCount; i++) {
                line = tx.section(QLatin1Char('\n'), i, i);

                i_tx = QString("%1").arg(num, prec);
                i_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                i_tx += "  ";

                auto match = regex.match(line);

                if (match.hasMatch()) {
                    line.replace(match.capturedStart(), match.capturedLength(), i_tx);
                    num += inc;
                    count++;
                } else {
                    if (renumEmpty) {
                        line.insert(0, i_tx);
                        num += inc;
                        count++;
                    }
                }

                new_tx += line + '\n';
            }

            tx = new_tx;
            break;
        }

        if (mode == 3) { //remove all
            num = 0;
            regex.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
            auto match = regex.match(tx);

            while (match.hasMatch()) {
                int pos = match.capturedStart();
                f_tx = match.captured();

                //qDebug() << f_tx;

                if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
                        && !f_tx.contains(QLatin1Char(';'))) {
                    tx.remove(pos, match.capturedLength());
                    num++;
                } else {
                    pos = match.capturedEnd();
                }

                match = regex.match(tx, pos);
            }

            break;
        }

        if (mode == 1) { //renumber all with N
            pos = 0;
            num = startAt;
            regex.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
            auto match = regex.match(tx);

            while (match.hasMatch()) {
                pos = match.capturedStart();
                f_tx = match.captured();

                //qDebug() << f_tx;

                if (pos > 0)
                    if (tx[pos - 1].isLetterOrNumber()) {
                        pos = match.capturedEnd();
                        continue;
                    }

                insertSpace = true;

                if (f_tx.endsWith(QLatin1Char(' '))) {
                    insertSpace = false;
                }

                if (!f_tx.contains(QLatin1Char(' ')) && !f_tx.contains(QLatin1Char('\n'))) {
                    i = match.capturedLength();
                } else {
                    i = match.capturedLength() - 1;
                }

                if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
                        && !f_tx.contains(QLatin1Char(';'))) {
                    f_tx.remove(0, 1);
                    f_tx.remove(QLatin1Char(' '));

                    if (!f_tx.isEmpty()) {
                        it = f_tx.toInt(&ok);
                    } else {
                        it = 0;
                    }

                    if (((it >= from) || (renumMarked && it == 0)) && (it < to)) {
                        f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                        f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));

                        if (insertSpace) {
                            f_tx.append(QLatin1String(" "));
                        }

                        tx.replace(pos, i, f_tx);
                        num += inc;
                        count++;
                    }
                }

                pos = match.capturedEnd();
            }

            break;
        }

        if (mode == 2) { //renumber all
            num = startAt;
            regex.setPattern("[Nn]{1,1}[0-9]+[\\s]{0,}|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");

            if (selection) {
                lineCount = tx.count('\n');
                lineCount++;
            } else {
                lineCount = ui->textEdit->document()->lineCount();
                //lineCount--;
            }

            for (i = 0; i < lineCount; i++) {
                line = tx.section(QLatin1Char('\n'), i, i, QString::SectionIncludeTrailingSep);

                //qDebug() << line;

                pos = 0;

                while (1) {
                    if (line.isEmpty()) {
                        if (!renumEmpty) {
                            break;
                        }

                        f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                        f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                        num += inc;
                        count++;
                        line.insert(0, f_tx);
                        break;
                    }

                    auto match = regex.match(line, pos);

                    if (match.hasMatch() && (line.at(0) != QLatin1Char('$'))) {
                        pos = match.capturedStart();
                        i_tx = match.captured();
                        i_tx.remove('\n');

                        if ((!i_tx.contains(QLatin1Char('(')) && !i_tx.contains(QLatin1Char('\''))
                                && !i_tx.contains(QLatin1Char(';')))) {
                            f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                            f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                            num += inc;
                            count++;
                            f_tx.append(QLatin1String(" "));
                            line.replace(i_tx, f_tx);
                            qDebug() << line << i_tx << f_tx;
                            break;
                        } else if (renumComm) {
                            break;
                        }
                    }

                    if ((line.at(0) == QLatin1Char('N')) && (!line.at(1).isLetter())) {
                        f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                        f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                        num += inc;
                        count++;
                        f_tx.append(QLatin1String(" "));
                        line.replace(0, 1, f_tx);
                        break;
                    }

                    if (((line.at(0) != QLatin1Char('%')) && (line.at(0) != QLatin1Char(':'))
                            && (line.at(0) != QLatin1Char('O')) && (line.at(0) != QLatin1Char('$')))) {
                        f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                        f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                        num += inc;
                        count++;
                        f_tx.append(QLatin1String(" "));
                        line.insert(0, f_tx);
                        break;
                    }

                    break;
                }

                new_tx += line; // + '\n';
            }

            tx = new_tx;
            break;
        }

        break;
    }

    if (selection) {
        ui->textEdit->textCursor().insertText(tx);
    } else {
        ui->textEdit->selectAll();
        ui->textEdit->insertPlainText(tx);
        cursor.setPosition(0);
    }

    ui->textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
    return count;
}

void MdiChild::doRemoveSpace()
{
    enum {
        NORMAL_FLOW,
        AFTER_APOSTROPHE,
        AFTER_PARETHESIS,
        AFTER_SEMICOLON
    } state = NORMAL_FLOW;

    QString *updatedText = new QString();
    QString tx = ui->textEdit->toPlainText();
    int pos = 0;
    bool wasLetter = false;
    bool skipChar;
    bool openAddress = false;
    bool hasChanged = false;
    QChar currentChar = QChar();

    m_Helper->begin(tx.length(), tr("Remove space", "Slow operation title in MDIChild"));

    while (pos < tx.length()) {
        if (m_Helper->check(pos) == LongJobHelper::CANCEL) {
            return;
        }

        currentChar = tx.at(pos++);
        skipChar = false;

        switch (state) {
        case AFTER_APOSTROPHE:
            if (currentChar == '\'' || currentChar == '\n') {
                state = NORMAL_FLOW;
            }

            break;

        case AFTER_PARETHESIS:
            if (currentChar == ')' || currentChar == '\n') {
                state = NORMAL_FLOW;
            }

            break;

        case AFTER_SEMICOLON:
            if (currentChar == '\n') {
                state = NORMAL_FLOW;
            }

            break;

        default:
            if (currentChar == '\'') {
                state = AFTER_APOSTROPHE;
            } else if (currentChar == '(') {
                state = AFTER_PARETHESIS;
            } else if (currentChar == ';') {
                state = AFTER_SEMICOLON;
            } else {
                // Some spaces are not removed to avoid merging words.
                // If there are letters before and after spaces, then one space is left.
                if (currentChar == ' ' || currentChar == '\t') {
                    skipChar = true;
                    hasChanged = true;

                    if (wasLetter) {
                        openAddress = true;
                    }
                } else if ((currentChar >= 'A' && currentChar <= 'Z') ||
                           (currentChar >= 'a' && currentChar <= 'z') ||
                           currentChar == '#') {
                    if (openAddress) {
                        updatedText->append(' ');
                    }

                    wasLetter = true;
                    openAddress = false;
                } else {
                    wasLetter = false;
                    openAddress = false;
                }
            }
        }

        if (!skipChar) {
            updatedText->append(currentChar);
        }
    }

    m_Helper->end();

    if (hasChanged) {
        ui->textEdit->selectAll();
        ui->textEdit->insertPlainText(*updatedText);
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(0);
        ui->textEdit->setTextCursor(cursor);
    }

    delete updatedText;
}

void MdiChild::doRemoveEmptyLines()
{
    int i;
    QString tx;
    QRegularExpression regex;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    tx = ui->textEdit->toPlainText();

    regex.setPattern("[\\n]{2,}");
    i = 0;

    while (i >= 0) {
        i = tx.indexOf(regex, 0);

        if (i >= 0) {
            tx.replace(regex, "\r\n");
        }
    }

    ui->textEdit->selectAll();
    ui->textEdit->insertPlainText(tx);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
// Add empty line after each block
//**************************************************************************************************

void MdiChild::doInsertEmptyLines()
{
    QString tx;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    tx = ui->textEdit->toPlainText();

    if (tx.contains(QLatin1String("\r\n"))) {
        tx.replace(QLatin1String("\r\n"), QLatin1String("\r\n\r\n"));
    } else {
        tx.replace(QLatin1String("\n"), QLatin1String("\n\n"));
    }

    ui->textEdit->selectAll();
    ui->textEdit->insertPlainText(tx);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
}

void MdiChild::doInsertSpace()
{
    enum {
        NORMAL_FLOW,
        AFTER_APOSTROPHE,
        AFTER_PARETHESIS,
        AFTER_SEMICOLON
    } state = NORMAL_FLOW;

    int pos = 0;
    QChar currentChar = QChar();
    QChar previosChar = QChar();
    bool hasChanged = false;

    QString *updatedText = new QString();
    QString tx = ui->textEdit->toPlainText();

    m_Helper->begin(tx.length(), tr("Insert space", "Slow operation title in MDIChild"));

    while (pos < tx.length()) {
        if (m_Helper->check(pos) == LongJobHelper::CANCEL) {
            return;
        }

        previosChar = currentChar;
        currentChar = tx.at(pos++);
        bool insert = false;

        switch (state) {
        case AFTER_APOSTROPHE:
            if (currentChar == '\'' || currentChar == '\n') {
                state = NORMAL_FLOW;
            }

            break;

        case AFTER_PARETHESIS:
            if (currentChar == ')' || currentChar == '\n') {
                state = NORMAL_FLOW;
            }

            break;

        case AFTER_SEMICOLON:
            if (currentChar == '\n') {
                state = NORMAL_FLOW;
            }

            break;

        default:
            if (currentChar == '\'') {
                state = AFTER_APOSTROPHE;

                if (previosChar != ' ' && previosChar != '\n') {
                    insert = true;
                }
            } else if (currentChar == '(') {
                state = AFTER_PARETHESIS;

                if (previosChar != ' ' && previosChar != '\n') {
                    insert = true;
                }
            } else if (currentChar == ';') {
                state = AFTER_SEMICOLON;

                if (previosChar != ' ' && previosChar != '\n') {
                    insert = true;
                }
            } else if (previosChar == ' ' || currentChar == ' ') {
                // do nothing
            } else if (previosChar == '\n') {
                // do nothing
            } else if (currentChar == '#' && previosChar != '#' && previosChar != '['
                       && previosChar != '-' && previosChar != '+' && previosChar != '*'
                       && previosChar != '/' && previosChar != '=') {
                insert = true;
            } else if ((previosChar >= 'A' && previosChar <= 'Z') ||
                       (previosChar >= 'a' && previosChar <= 'z') ||
                       previosChar == ',') {
                // do nothing
            } else if ((currentChar >= 'A' && currentChar <= 'Z') ||
                       (currentChar >= 'A' && currentChar <= 'Z') ||
                       currentChar == ',') {
                insert = true;
            }
        }

        if (insert) {
            hasChanged = true;
            updatedText->append(' ');
        }

        updatedText->append(currentChar);
    }

    m_Helper->end();

    if (hasChanged) {
        ui->textEdit->selectAll();
        ui->textEdit->insertPlainText(*updatedText);
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(0);
        ui->textEdit->setTextCursor(cursor);
    }

    delete updatedText;
}

void MdiChild::doInsertDot()
{
    int pos, count;
    QString tx, f_tx;
    QRegularExpression regex;
    double it;
    bool ok;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    regex.setPattern(QString("[%1]{1,1}[-.+0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$").arg(
                         mdiWindowProperites.dotAdr));

    count = 0;
    tx = ui->textEdit->toPlainText();
    auto match = regex.match(tx);
    pos = 0;

    while (match.hasMatch()) {
        f_tx = match.captured();
        pos = match.capturedEnd();

        if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
                && !f_tx.contains(QLatin1Char(';'))) {
            if (mdiWindowProperites.dotAfter && !f_tx.contains(QLatin1Char('.'))) {
                f_tx.remove(0, 1);

                //f_tx.remove('+');
                it = f_tx.toDouble(&ok);

                if (ok) {
                    it = it / mdiWindowProperites.dotAftrerCount;
                    tx.replace(match.capturedStart() + 1, match.capturedLength() - 1,
                               QString("%1").arg(it, 0, 'f', 3));
                    count++;
                }
            }

            if (mdiWindowProperites.atEnd && !f_tx.contains(QLatin1Char('.'))) {
                tx.insert(match.capturedEnd(), QLatin1Char('.'));
                pos++;
                count++;
            }
        }

        match =  regex.match(tx, pos);
    }

    emit message(QString(tr("Inserted : %1 dots.")).arg(count), 6000);
    cleanUp(&tx);
    ui->textEdit->selectAll();
    ui->textEdit->insertPlainText(tx);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
}

void MdiChild::cleanUp(QString *str)  //remove not needed zeros
{
    QRegularExpression regex;
    regex.setPattern("([\\d]+[.][-+.0-9]+)|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$");
    auto match = regex.match(*str);

    // FIXME: This slow code deletes non-significant zeros one at a time.
    while (match.hasMatch()) {
        int pos = match.capturedEnd();

        if (match.capturedLength(1) != 0 && str->at(match.capturedEnd() - 1) == '0') {
            str->remove(match.capturedEnd() - 1, 1);
            pos = match.capturedStart();
        }

        match = regex.match(*str, pos);
    }
}

void MdiChild::doI2M()
{
    int count;
    QString tx, f_tx;
    QRegularExpression regex;
    double it;
    bool ok;

    QApplication::setOverrideCursor(Qt::BusyCursor);

    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    regex.setPattern(
        QString("[%1]{1,1}([+-]?\\d*\\.?\\d*)|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$").arg(
            mdiWindowProperites.i2mAdr));

    count = 0;
    tx = ui->textEdit->toPlainText();
    auto match = regex.match(tx);

    while (match.hasMatch()) {
        int pos = match.capturedEnd();

        if (match.capturedLength(1) > 0) {
            f_tx = match.captured(1);
            it = f_tx.toDouble(&ok);

            if (ok && it != 0) {
                if (!mdiWindowProperites.inch) {
                    it /= 25.4;
                } else {
                    it *= 25.4;
                }

                QString conv = QString("%1").arg(it, 0, 'f', mdiWindowProperites.i2mprec);
                tx.replace(match.capturedStart(), match.capturedLength(), conv);
                pos += conv.length() - match.capturedLength();
                count++;
            }
        }

        match = regex.match(tx, pos);
    }

    emit message(QString(tr("Converted : %1 numbers.")).arg(count), 6000);
    cleanUp(&tx);
    ui->textEdit->selectAll();
    ui->textEdit->insertPlainText(tx);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
}

bool MdiChild::event(QEvent *event)
{
    QString group, key, text;
    QTextCursor cursor;
    QString fileName;

    if ((event->type() == QEvent::ToolTip) && mdiWindowProperites.editorToolTips) {

        switch (mdiWindowProperites.hColors.highlightMode) {
        case MODE_OKUMA:
            group = QLatin1String("OKUMA");
            break;

        case MODE_FANUC:
            group = QLatin1String("FANUC");
            break;

        case MODE_SINUMERIK_840:
            group = QLatin1String("SINUMERIK_840");
            break;

        case MODE_PHILIPS:
        case MODE_SINUMERIK:
            group = QLatin1String("SINUMERIK");
            break;

        case MODE_HEIDENHAIN:
            group = QLatin1String("HEIDENHAIN");
            break;

        case MODE_HEIDENHAIN_ISO:
            group = QLatin1String("HEIDENHAIN_ISO");
            break;

        case MODE_LINUXCNC:
            group = QLatin1String("LinuxCNC");
            break;

        case MODE_TOOLTIPS:
            group = QLatin1String("TOOLTIP");
            break;

        default:
            event->accept();
            return true;
        }

        key = "";

        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        cursor = ui->textEdit->cursorForPosition(helpEvent->pos());
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor,
                            2);  //fix cursor position

        if (mdiWindowProperites.hColors.highlightMode == MODE_FANUC) {
            do {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                key = cursor.selectedText();

            } while (key.at(0).isLetter() && !key.isEmpty() && !cursor.atStart());

            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);

            do {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                key = cursor.selectedText();

            } while ((key.length() > 0 ? ((key.at(key.length() - 1).isLetter())

                                          || (key.at(key.length() - 1) == QLatin1Char('.'))) : false) && !key.isEmpty()
                     && !cursor.atBlockEnd());

            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

            if (key.length() < 3) {
                cursor = ui->textEdit->cursorForPosition(helpEvent->pos());
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor,
                                    2);  //fix cursor position

                do {
                    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                    key = cursor.selectedText();

                } while (!((key.at(0) == QLatin1Char('#')) || key.at(0).isLetter()) && !key.isEmpty()

                         && !cursor.atStart());

                cursor.clearSelection();
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

                do {
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                    key = cursor.selectedText();
                } while ((key.at(key.length() - 1).isDigit()

                          || (key.at(key.length() - 1) ==  QLatin1Char('.'))) && !key.isEmpty() && !cursor.atEnd());

                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

            }

            key = cursor.selectedText();

        } else {
            if (cursor.atEnd()) {
                return true;
            }

            cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);

            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            key = cursor.selectedText();

            if (key.at(0) != QLatin1Char('@')) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
                key = "";
            } else {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            }

            cursor.movePosition(QTextCursor::EndOfWord,  QTextCursor::KeepAnchor);

            key = key + cursor.selectedText();
        }

        key = key.simplified();

        if (key.length() == 2) {
            if ((key.at(0) == QLatin1Char('G')) || (key.at(0) == QLatin1Char('M')))
                if (!key.at(1).isLetter()) {
                    key.insert(1, "0");
                }
        }

        if (key.isEmpty()) {
            text = "";
        } else {
            fileName = QFileInfo(curFile).canonicalPath() + "/" + "cnc_tips.txt";

            if (QFile::exists(fileName)) {
                QSettings settings(fileName, QSettings::IniFormat);
                settings.beginGroup(group);
                text = settings.value(key, "").toString();
                settings.endGroup();
            }

            if (text.isEmpty() || text.isNull()) {
                QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "EdytorNC", "EdytorNC");
                QString config_dir = QFileInfo(cfg.fileName()).absolutePath() + "/";

                fileName = config_dir + "cnc_tips_" + QLocale::system().name() + ".txt";

                if (QFile::exists(fileName)) {
                    QSettings settings(fileName, QSettings::IniFormat);
                    settings.beginGroup(group);
                    text = settings.value(key, "").toString();
                    settings.endGroup();
                } else {
                    event->accept();
                    return true;
                }
            }
        }

        if (!text.isEmpty()) {
            key = QLatin1String("<p style='white-space:pre'>");

            if (text.length() > 128) {
                key = QLatin1String("<p style='white-space:normal'>");
            }

            QToolTip::showText(helpEvent->globalPos(), key + text, this, QRect());
        } else {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }

    return QWidget::event(event);
}

int MdiChild::compileMacro()
{
    int error;
    QTextCursor cursor;
    QString text = ui->textEdit->toPlainText();

    QRegularExpression regexBegin("\\{BEGIN\\}");
    regexBegin.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    QRegularExpression regexEnd("\\{END\\}");
    regexEnd.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    auto matchBegin = regexBegin.match(text);
    auto matchEnd = regexEnd.match(text, matchBegin.capturedEnd());

    if (!matchBegin.hasMatch() || !matchEnd.hasMatch()) {
        QMessageBox::warning(this, tr("EdytorNc - compile macro"),
                             tr("No constant definition .\n{BEGIN}\n...\n{END}\n No macro ?"));
        return -1;
    }

    //defEnd -= exp.matchedLength();

    QApplication::setOverrideCursor(Qt::BusyCursor);

    QRegularExpression regex("\\{\\$[A-Z0-9\\s]*\\b[=\\n\\r]");
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    auto match = regex.match(text, matchBegin.capturedEnd());

    while (match.hasMatch() && match.capturedEnd() <= matchEnd.capturedStart()) {
        int pos = match.capturedEnd();
        QString param = match.captured();
        param = param.remove(' ');
        param = param.remove('{');
        param = param.remove('=');

        QString val = "";

        do {
            val = val + text.at(pos);
            pos++;

            if (text.at(pos) == '\n' || text.at(pos) == '{') {
                cursor = ui->textEdit->textCursor();
                cursor.setPosition(0);
                ui->textEdit->setTextCursor(cursor);
                QMessageBox::warning(this, tr("EdytorNC - compile macro"),
                                     tr("Param list: no bracket \'}\' !"));
                QApplication::restoreOverrideCursor();
                return -1;
            }
        } while ((text.at(pos) != '}'));

        val = val.remove(' ');

        int i = matchEnd.capturedStart();

        while ((i = text.indexOf(param, i)) >= 0) {
            text.replace(i, param.length(), val);
        }

        match = regex.match(text, pos);
    }

    text.remove(matchBegin.capturedStart(), matchEnd.capturedEnd() - matchBegin.capturedStart());

    regex.setPattern("\\{[-+*=.,()$/0-9A-Z\\s]*\\b[-+*=.,()$/0-9A-Z\\s]*[}]");
    match = regex.match(text);

    while (match.hasMatch()) {
        QString param = match.captured();
        param = param.simplified();
        param = param.remove(QChar(' '));
        param = param.replace(',', '.');

        if (!param.isEmpty()) {
            QString paramTmp = param;
            error = Utils::processBrc(&param);

            if (error < 0) {
                cursor = ui->textEdit->textCursor();
                cursor.setPosition(0);
                ui->textEdit->setTextCursor(cursor);
                ui->textEdit->find(paramTmp);
                macroShowError(error, paramTmp);
                QApplication::restoreOverrideCursor();
                return -1;
            }

            if (!param.isEmpty()) {
                paramTmp = param;
                error = Utils::processBrc(&param);

                if (error < 0) {
                    cursor = ui->textEdit->textCursor();
                    cursor.setPosition(0);
                    ui->textEdit->setTextCursor(cursor);
                    ui->textEdit->find(paramTmp);
                    macroShowError(error, paramTmp);
                    QApplication::restoreOverrideCursor();
                    return -1;
                }
            }

            QString val = param;
            val = val.remove('{');
            val = val.remove('}');
            text.replace(match.capturedStart(), match.capturedLength(), val);
        }

        match = regex.match(text);
    }

    regexBegin.setPattern("\\{BEGIN_SUBS\\}");
    matchBegin = regexBegin.match(text);
    regex.setPattern("\\{END_SUBS\\}");
    matchEnd = regex.match(text, matchBegin.capturedEnd());
    QString basicSubs;

    if (matchBegin.hasMatch() && matchEnd.hasMatch()) {
        basicSubs = text.mid(matchBegin.capturedEnd(),
                             matchEnd.capturedStart() - matchBegin.capturedEnd());
        text.remove(matchBegin.capturedStart(),
                    matchEnd.capturedEnd() - matchBegin.capturedStart() + 1);
    }

    regexBegin.setPattern("\\{BEGIN_BASIC\\}");
    regexEnd.setPattern("\\{END_BASIC\\}");
    matchBegin = regexBegin.match(text);
    matchEnd = regexEnd.match(text, matchBegin.capturedEnd());

    while (matchBegin.hasMatch() && matchEnd.hasMatch()) {
        QString basicCode = text.mid(matchBegin.capturedEnd(),
                                     matchEnd.capturedStart() - matchBegin.capturedEnd());
        text.remove(matchBegin.capturedStart(),
                    matchEnd.capturedEnd() - matchBegin.capturedStart() + 1);
        basicCode.append(basicSubs);

        error = BasicInterpreter().interpretBasic(basicCode);

        if (error > 0) {
            macroShowBasicError(error);
            QApplication::restoreOverrideCursor();
            return -1;
        }

        text.insert(matchBegin.capturedStart(), basicCode);
        matchBegin = regexBegin.match(text, matchBegin.capturedStart());
        matchEnd = regexEnd.match(text, matchBegin.capturedEnd());
    }

    cleanUp(&text);
    ui->textEdit->selectAll();
    ui->textEdit->insertPlainText(text);
    cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);
    ui->textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
    return 1;
}

void MdiChild::macroShowError(int error, QString tx)
{
    QString errText;

    if (error < 0) {
        switch (error) {
        case ERR_NO_BRAC:
            errText = tr("No ( or ) !");
            break;

        case ERR_NO_PARAM:
            errText = tr("Function parameter not found ! \n Check +-*/.\n\"%1\"").arg(tx);
            break;

        case ERR_CONVERT:
            errText = tr("Wrong number !");
            break;

        case ERR_UNKNOWN_FUNC:
            errText = tr("Unknown math function !\n\"%1\"").arg(tx);
            break;

        case ERR_DOUBLE_DOT:
            errText = tr("Decimal point or minus writed two times !\n\"%1\"").arg(tx);
            break;

        default:
            errText = tr("Unknown error !");
        }

        QMessageBox::warning(this, tr("EdytorNc - compile macro"), errText);
    }
}

void MdiChild::macroShowBasicError(int error)
{
    QString errText;

    switch (error) {
    case 0:
        errText = tr("Syntax error");
        break;

    case 1:
        errText = tr("Unbalanced parentheses");
        break;

    case 2:
        errText = tr("No expression present");
        break;

    case 3:
        errText = tr("Equals sign expected");
        break;

    case 4:
        errText = tr("Not a variable");
        break;

    case 5:
        errText = tr("Label table full");
        break;

    case 6:
        errText = tr("Duplicate label");
        break;

    case 7:
        errText = tr("Undefined label");
        break;

    case 8:
        errText = tr("THEN expected");
        break;

    case 9:
        errText = tr("TO expected");
        break;

    case 10:
        errText = tr("Too many nested FOR loops");
        break;

    case 11:
        errText = tr("NEXT without FOR");
        break;

    case 12:
        errText = tr("Too many nested GOSUBs");
        break;

    case 13:
        errText = tr("RETURN without GOSUBs");
        break;

    default:
        errText = tr("Unknown error");
    }

    QMessageBox::warning(this, tr("EdytorNc - compile basic"), errText);
}

void MdiChild::highlightCurrentLine()
{
    QString openBrace;
    QString closeBrace;
    bool proceed;
    QList<QTextEdit::ExtraSelection> tmpSelections;
    QTextDocument::FindFlags findOptions;

    tmpSelections.clear();
    extraSelections.clear();
    tmpSelections.append(blockExtraSelections);
    tmpSelections.append(findTextExtraSelections);
    ui->textEdit->setExtraSelections(tmpSelections);

    if (!ui->textEdit->isReadOnly()) {
        selection.format.setBackground(QColor(mdiWindowProperites.lineColor));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = ui->textEdit->textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    QColor lineColor = QColor(mdiWindowProperites.lineColor).darker(108);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = ui->textEdit->document();
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextCursor beforeCursor = cursor;

    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    QString brace = cursor.selectedText();

    beforeCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    QString beforeBrace = beforeCursor.selectedText();

    proceed = true;
    findOptions = QTextDocument::FindFlags();

    if ((brace != QLatin1String("{")) && (brace != QLatin1String("}"))
            && (brace != QLatin1String("[")) && (brace != QLatin1String("]"))
            && (brace != QLatin1String("("))
            && (brace != QLatin1String(")")) && (brace != QLatin1String("\""))
            && (((brace != QLatin1String("<")) && (brace != QLatin1String(">"))))) {
        if ((beforeBrace == QLatin1String("{")) || (beforeBrace == QLatin1String("}"))
                || (beforeBrace == QLatin1String("["))
                || (beforeBrace == QLatin1String("]"))
                || (beforeBrace == QLatin1String("("))
                || (beforeBrace == QLatin1String(")"))
                || (beforeBrace == QLatin1String("\""))
                || (((beforeBrace == QLatin1String("<"))
                     || (beforeBrace == QLatin1String(">"))))) {

            cursor = beforeCursor;
            brace = cursor.selectedText();
            proceed = true;
        } else {
            proceed = false;

            if (mdiWindowProperites.hColors.highlightMode == MODE_LINUXCNC) {
                cursor = ui->textEdit->textCursor();

                cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                brace = cursor.selectedText().toUpper();

                findOptions = QTextDocument::FindWholeWords;

                if (brace.length() > 1) {
                    if (brace[0] == QLatin1Char('O')) {
                        beforeCursor = cursor;
                        openBrace = brace;
                        closeBrace = brace;
                        proceed = true;
                    }

                    if ((brace == QLatin1String("IF")) || (brace == QLatin1String("ENDIF"))) {
                        openBrace = QLatin1String("IF");
                        closeBrace = QLatin1String("ENDIF");
                        proceed = true;
                    }

                    if ((brace == QLatin1String("SUB")) || (brace == QLatin1String("ENDSUB"))) {
                        openBrace = QLatin1String("SUB");
                        closeBrace = QLatin1String("ENDSUB");
                        proceed = true;
                    }

                    if (brace == QLatin1String("WHILE") || (brace == QLatin1String("ENDWHILE"))) {
                        openBrace = QLatin1String("WHILE");
                        closeBrace = QLatin1String("ENDWHILE");
                        proceed = true;
                    }
                }
            }


            if (mdiWindowProperites.hColors.highlightMode == MODE_SINUMERIK_840) {
                cursor = ui->textEdit->textCursor();

                cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                brace = cursor.selectedText().toUpper();

                findOptions = QTextDocument::FindWholeWords;

                if (brace.length() > 1) {
                    if ((brace == QLatin1String("IF")) || (brace == QLatin1String("ENDIF"))) {
                        openBrace = QLatin1String("IF");
                        closeBrace = QLatin1String("ENDIF");
                        proceed = true;
                    }

                    if ((brace == QLatin1String("REPEAT")) || (brace == QLatin1String("UNTIL"))) {
                        openBrace = QLatin1String("REPEAT");
                        closeBrace = QLatin1String("UNTIL");
                        proceed = true;
                    }

                    if (brace == QLatin1String("WHILE") || (brace == QLatin1String("ENDWHILE"))) {
                        openBrace = QLatin1String("WHILE");
                        closeBrace = QLatin1String("ENDWHILE");
                        proceed = true;
                    }
                }
            }
        }
    }

    if (!proceed) {
        tmpSelections.append(extraSelections);
        ui->textEdit->setExtraSelections(tmpSelections);
        return;
    }

    QTextCharFormat format;
    format.setForeground(Qt::red);
    format.setFontWeight(QFont::Bold);

    if ((brace == QLatin1String("{")) || (brace == QLatin1String("}"))) {
        openBrace = QLatin1String("{");
        closeBrace = QLatin1String("}");
    }

    if ((brace == QLatin1String("[")) || (brace == QLatin1String("]"))) {
        openBrace = QLatin1String("[");
        closeBrace = QLatin1String("]");
    }

    if ((brace == QLatin1String("(")) || (brace == QLatin1String(")"))) {
        openBrace = QLatin1String("(");
        closeBrace = QLatin1String(")");
    }

    if (mdiWindowProperites.hColors.highlightMode == MODE_LINUXCNC) {
        if ((brace == QLatin1String("<")) || (brace == QLatin1String(">"))) {
            openBrace = QLatin1String("<");
            closeBrace = QLatin1String(">");
        }
    }

    if ((brace == QLatin1String("\""))) {
        selection.cursor = cursor;
        extraSelections.append(selection);
        QTextCursor cursor1 = doc->find(QLatin1String("\""), cursor);

        if (!cursor1.isNull() && (cursor1 != cursor)) {
            selection.cursor = cursor1;
            extraSelections.append(selection);
        } else {
            QTextCursor cursor2 = doc->find(QLatin1String("\""), cursor, QTextDocument::FindBackward);

            if (!cursor2.isNull()) {
                selection.cursor = cursor2;
                extraSelections.append(selection);
            }
        }

        tmpSelections.append(extraSelections);
        ui->textEdit->setExtraSelections(tmpSelections);
        return;
    }

    if (brace == openBrace) {
        QTextCursor cursor1 = doc->find(closeBrace, cursor, findOptions);
        QTextCursor cursor2 = doc->find(openBrace, cursor, findOptions);

        if (cursor2.isNull()) {
            selection.cursor = cursor;
            extraSelections.append(selection);
            selection.cursor = cursor1;
            extraSelections.append(selection);
        } else {

            while (cursor1.position() > cursor2.position()) {
                cursor1 = doc->find(closeBrace, cursor1, findOptions);
                cursor2 = doc->find(openBrace, cursor2, findOptions);

                if (cursor2.isNull()) {
                    break;
                }
            }

            selection.cursor = cursor;
            extraSelections.append(selection);
            selection.cursor = cursor1;
            extraSelections.append(selection);
        }
    } else {
        if (brace == closeBrace) {
            QTextCursor cursor1 = doc->find(openBrace, cursor, QTextDocument::FindBackward | findOptions);
            QTextCursor cursor2 = doc->find(closeBrace, cursor, QTextDocument::FindBackward | findOptions);

            if (cursor2.isNull()) {
                selection.cursor = cursor;
                extraSelections.append(selection);
                selection.cursor = cursor1;
                extraSelections.append(selection);
            } else {
                while (cursor1.position() < cursor2.position()) {
                    cursor1 = doc->find(openBrace, cursor1, QTextDocument::FindBackward | findOptions);
                    cursor2 = doc->find(closeBrace, cursor2, QTextDocument::FindBackward | findOptions);

                    if (cursor2.isNull()) {
                        break;
                    }
                }

                selection.cursor = cursor;
                extraSelections.append(selection);
                selection.cursor = cursor1;
                extraSelections.append(selection);
            }
        }
    }

    tmpSelections.append(extraSelections);
    ui->textEdit->setExtraSelections(tmpSelections);
}

void MdiChild::highlightFindText(const QString &searchString, QTextDocument::FindFlags options,
                                 bool ignoreComments)
{
    QList<QTextEdit::ExtraSelection> tmpSelections;
    bool inComment = false;
    bool isRegExp = false;
    bool isRegExpMinMax = false;
    QRegularExpression regex;
    double min = 0;
    double max = 0;
    Qt::CaseSensitivity caseSensitivity;

    tmpSelections.clear();
    findTextExtraSelections.clear();
    tmpSelections.append(extraSelections);
    QColor lineColor = QColor(Qt::yellow).lighter(155);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = ui->textEdit->document();
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.setPosition(0);

    QString addr = searchString;

    if (options & QTextDocument::FindCaseSensitively) {
        caseSensitivity = Qt::CaseSensitive;
    } else {
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        caseSensitivity = Qt::CaseInsensitive;
    }

    // TODO Dedicate the search pattern parsing into a separate method.
    // This can also be useful in the foundTextMatched and findText methods.
    // Try the following pattern for parsing.
    // [A-Z]((\$\$)|(\$([-+]?\d*\.?\d+))(\$([-+]?\d*\.?\d+))?)
    if (addr.contains(QRegularExpression("\\$\\$"))) {
        addr.remove("$$");
        isRegExp = true;
    } else {
        regex.setPattern(QString("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"));
        auto match = regex.match(addr);

        if (match.hasMatch()) {
            isRegExp = true;
            isRegExpMinMax = true;
            QString value = match.captured();
            addr.remove(match.capturedStart(), match.capturedLength());

            value.remove("$");
            bool ok;
            max = value.toDouble(&ok);

            if (!ok) {
                max = 0;
            }

            match = regex.match(addr);

            if (match.hasMatch()) {
                value = match.captured();
                addr.remove(match.capturedStart(), match.capturedLength());
                value.remove("$");
                min = value.toDouble(&ok);

                if (!ok) {
                    min = 0;
                }
            }
        }
    }

    cursor.setPosition(0);

    do {
        if (isRegExp) {
            if (addr.isEmpty()) {
                return;
            }

            regex.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(addr));

            cursor = doc->find(regex, cursor, options);
        } else {
            cursor = doc->find(searchString, cursor, options);
        }

        if (!cursor.isNull()) {
            QString cur_line = cursor.block().text();
            int cur_line_column = cursor.columnNumber();
            int commentPos;

            if (ignoreComments) {
                int id = highligthMode();

                if ((id == MODE_SINUMERIK_840) || (id == MODE_HEIDENHAIN_ISO) || (id == MODE_HEIDENHAIN)) {
                    commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
                } else {
                    if ((id == MODE_AUTO) || (id == MODE_OKUMA) || (id == MODE_SINUMERIK) || (id == MODE_PHILIPS)) {
                        commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
                    } else {
                        commentPos  = cur_line.indexOf(QLatin1Char('('), 0);

                        if (commentPos > cur_line_column) {
                            commentPos = -1;
                        }

                        if (commentPos < 0) {
                            commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
                        }
                    }
                }

                if (commentPos < 0) {
                    commentPos = cur_line_column + 1;
                }

                inComment = (commentPos < cur_line_column);
            } else {
                inComment = false;
            }

            if (!inComment) {
                if (isRegExpMinMax) {
                    QString sval = cursor.selectedText();
                    bool ok;
                    double val = QString(sval).remove(addr, caseSensitivity).toDouble(&ok);

                    if ((val >= min) && (val <= max)) {
                        selection.cursor = cursor;
                        findTextExtraSelections.append(selection);
                    }
                } else {
                    selection.cursor = cursor;
                    findTextExtraSelections.append(selection);
                }
            }
        }
    } while (!cursor.isNull());

    tmpSelections.append(findTextExtraSelections);
    ui->textEdit->setExtraSelections(tmpSelections);
}

void MdiChild::doUndo()
{
    ui->textEdit->undo();
    ui->textEdit->ensureCursorVisible();
    highlightCurrentLine();
}

void MdiChild::doRedo()
{
    ui->textEdit->redo();
    ui->textEdit->ensureCursorVisible();
    highlightCurrentLine();
}

void MdiChild::detectHighligthMode()
{
    QString text;

    if (!mdiWindowProperites.syntaxH) {
        return;
    }

    if (highlighter == nullptr) {
        return;
    }

    bool mod =
        ui->textEdit->document()->isModified();  // something below clears document modified state

    if (mdiWindowProperites.hColors.highlightMode == MODE_AUTO) {
        text = ui->textEdit->toPlainText();
        mdiWindowProperites.hColors.highlightMode = autoDetectHighligthMode(text.toUpper());

        if (mdiWindowProperites.hColors.highlightMode == MODE_AUTO) {
            mdiWindowProperites.hColors.highlightMode = mdiWindowProperites.defaultHighlightMode;
        }
    }

    highlighter->setHColors(mdiWindowProperites.hColors, QFont(mdiWindowProperites.fontName,
                            mdiWindowProperites.fontSize, QFont::Normal));
    highlighter->rehighlight();

    ui->textEdit->document()->setModified(mod);
}

void MdiChild::setHighligthMode(int mod)
{
    mdiWindowProperites.hColors.highlightMode = mod;
    detectHighligthMode();
}

int MdiChild::highligthMode()
{
    return mdiWindowProperites.hColors.highlightMode;
}

void MdiChild::doDiff()
{
    //   setUpdatesEnabled(false);
    //
    ////   if(diffSplitter > 0)
    ////      return;
    ////
    ////   diffSplitter = new QSplitter(Qt::Horizontal, this);
    ////   diffSplitter->setBackgroundRole(QPalette::Base);
    //
    //   //splitterV->setEnabled(true);
    //
    //   diffApp = new KDiff3App(splitterV, "DiffApp");
    //
    //   diffApp->completeInit(curFile, QFileInfo(curFile).canonicalPath());
    //
    ////   QList<int> list;
    ////   list << 200 << 200;
    ////   splitterV->setSizes(list);
    ////   splitterV->adjustSize();
    //
    //   //diffSplitter->addWidget(splitter);
    //
    //   //diffSplitter->show();
    //
    //
    //   //diffApp->resize(width()/2, height());
    //   //diffApp->show();
    //   //diffApp->adjustSize();
    //   //diffApp->resize(800,600);
    //   //splitter->adjustSize();
    //
    //   setUpdatesEnabled(true);
}

QString MdiChild::currentFileInfo()
{
    return curFileInfo;
}

bool MdiChild::foundTextMatched(const QString &pattern, QString text)
{
    bool matched = false;
    bool isRegExp = false;
    bool isRegExpMinMax = false;
    QRegularExpression regex;
    double min = 0;
    double max = 0;

    QString addr = pattern;

    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    if (addr.contains(QRegularExpression("\\$\\$"))) {
        addr.remove("$$");
        isRegExp = true;
    } else {
        regex.setPattern(QString("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"));
        auto match = regex.match(addr);

        if (match.hasMatch()) {
            isRegExp = true;
            isRegExpMinMax = true;
            QString value = match.captured();
            addr.remove(match.capturedStart(), match.capturedLength());
            value.remove("$");
            bool ok;
            max = value.toDouble(&ok);

            if (!ok) {
                max = 0;
            }

            match = regex.match(addr);

            if (match.hasMatch()) {
                value = match.captured();
                addr.remove(match.capturedStart(), match.capturedLength());
                value.remove("$");
                min = value.toDouble(&ok);

                if (!ok) {
                    min = 0;
                }
            }
        }
    }

    if (isRegExp) {
        regex.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(addr));

        if (text.contains(regex)) {
            if (isRegExpMinMax) {
                bool ok;
                double val = QString(text).remove(addr, Qt::CaseInsensitive).toDouble(&ok);

                if (ok && (val >= min) && (val <= max)) {
                    matched = true;
                }
            } else {
                matched = true;
            }
        }
    } else {
        matched = (addr == text);
    }

    return matched;
}

bool MdiChild::findText(const QString &text, QTextDocument::FindFlags options,
                        bool ignoreComments)
{
    bool inComment = false;
    bool found = false;
    bool isRegExp = false;
    bool isRegExpMinMax = false;
    QTextCursor cursor;
    QRegularExpression regex;
    double min = 0;
    double max = 0;
    Qt::CaseSensitivity caseSensitivity;

    if (options & QTextDocument::FindCaseSensitively) {
        caseSensitivity = Qt::CaseSensitive;
    } else {
        caseSensitivity = Qt::CaseInsensitive;
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    QString addr = text;

    if (addr.contains(QRegularExpression("\\$\\$"))) {
        addr.remove("$$");
        isRegExp = true;
    } else {
        regex.setPattern(QString("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"));
        auto match = regex.match(addr);

        if (match.hasMatch()) {
            isRegExp = true;
            isRegExpMinMax = true;
            QString value = match.captured();
            addr.remove(match.capturedStart(), match.capturedLength());
            value.remove("$");
            bool ok;
            max = value.toDouble(&ok);

            if (!ok) {
                max = 0;
            }

            match = regex.match(addr);

            if (match.hasMatch()) {
                value = match.captured();
                addr.remove(match.capturedStart(), match.capturedLength());
                value.remove("$");
                min = value.toDouble(&ok);

                if (!ok) {
                    min = 0;
                }
            }
        }
    }

    ui->textEdit->setUpdatesEnabled(false);

    if (addr.isEmpty()) {
        return false;
    }

    cursor = ui->textEdit->textCursor();

    do {
        if (isRegExp) {
            regex.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(addr));

            cursor = ui->textEdit->document()->find(regex, cursor, options);

            found = !cursor.isNull();

            if (found) {
                if (!isRegExpMinMax) {
                    ui->textEdit->setTextCursor(cursor);
                }
            } else {
                break;
            }
        } else {
            found = ui->textEdit->find(addr, options);
            cursor = ui->textEdit->textCursor();
        }

        QString cur_line = cursor.block().text();
        int cur_line_column = cursor.columnNumber();

        if (found && ignoreComments) {
            int id = highligthMode();
            int commentPos;

            if ((id == MODE_SINUMERIK_840) || (id == MODE_HEIDENHAIN_ISO) || (id == MODE_HEIDENHAIN)) {
                commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
            } else {
                if ((id == MODE_AUTO) || (id == MODE_OKUMA) || (id == MODE_SINUMERIK) || (id == MODE_PHILIPS)) {
                    commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
                } else {
                    commentPos  = cur_line.indexOf(QLatin1Char('('), 0);

                    if (commentPos > cur_line_column) {
                        commentPos = -1;
                    }

                    if (commentPos < 0) {
                        commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
                    }
                }
            }

            if (commentPos < 0) {
                commentPos = cur_line_column + 1;
            }

            inComment = (commentPos < cur_line_column);
        } else {
            inComment = false;
        }

        if ((isRegExpMinMax && found) && !inComment) {
            QString sValue = cursor.selectedText();
            bool ok;
            double val = QString(sValue).remove(addr, caseSensitivity).toDouble(&ok);

            if (((val >= min) && (val <= max))) {
                inComment = false;
                ui->textEdit->setTextCursor(cursor);
            } else {
                inComment = true;
            }
        }

    } while (inComment);

    ui->textEdit->setUpdatesEnabled(true);
    return found;
}

QString MdiChild::guessFileName()
{
    //QTextCursor cursor;
    QString fileName;
    QString text;
    QRegularExpression regex;

    //cursor = ui->textEdit->textCursor();
    text = ui->textEdit->toPlainText();

    if (mdiWindowProperites.guessFileNameByProgNum) {
        forever {
            regex.setPattern(FILENAME_SINU840);
            auto match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove(QLatin1String("%_N_"));
                fileName.remove(QRegularExpression("_(MPF|SPF|TEA|COM|PLC|DEF|INI)"));
                break;
            }

            regex.setPattern(FILENAME_OSP);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove(QLatin1String("$"));
                fileName.remove(QRegularExpression("\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"));
                break;
            }

            regex.setPattern(FILENAME_SINU);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove(QRegularExpression("%(MPF|SPF|TEA)[\\s]{0,3}"));
                break;
            }

            regex.setPattern(FILENAME_PHIL);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove(QRegularExpression("%PM[\\s]{1,}[N]{1,1}"));
                break;
            }

            regex.setPattern(FILENAME_FANUC);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.replace(':', 'O');

                //                if(fileName.at(0)!='O')
                //                    fileName[0]='O';
                //                if(fileName.at(0)=='O' && fileName.at(1)=='O')
                //                    fileName.remove(0,1);
                break;
            }

            regex.setPattern(FILENAME_HEID1);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove(QLatin1String("%"));
                fileName.remove(QRegularExpression("\\s"));
                break;
            }

            regex.setPattern(FILENAME_HEID2);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove(QRegularExpression("(BEGIN)(\\sPGM\\s)"));
                fileName.remove(QRegularExpression("(\\sMM|\\sINCH)"));
                break;
            }

            regex.setPattern(FILENAME_FADAL);
            match = regex.match(text);

            if (match.hasMatch()) {
                fileName = match.captured();
                fileName.remove("N1");
                break;
            }

            fileName = "";
            break;
        }
    } else {
        forever {
            regex.setPattern("(;)[\\w:*=+ -]{4,64}");
            auto match = regex.match(text);

            if (match.capturedStart() >= 2) {
                fileName = match.captured();
                fileName.remove(";");
                break;
            }

            regex.setPattern("(\\()[\\w:*=+ -]{4,64}(\\))");
            match = regex.match(text);

            if (match.capturedStart() >= 2) {
                fileName = match.captured();
                fileName.remove("(");
                fileName.remove(")");
                break;
            }

            fileName = "";
            break;
        }
    }

    fileName = fileName.simplified();
    fileName = fileName.trimmed();

    //fileName.append(mdiWindowProperites.saveExtension);
    if (mdiWindowProperites.saveDirectory.isEmpty()) {
        fileName.prepend(mdiWindowProperites.lastDir + "/");
    } else {
        fileName.prepend(mdiWindowProperites.saveDirectory + "/");
    }

    fileName = QDir::cleanPath(fileName);

    //ui->textEdit->setTextCursor(cursor);

    return fileName;
}

bool MdiChild::findNext(QString textToFind, QTextDocument::FindFlags options,
                        bool ignoreComments)
{
    bool found = false;
    QTextCursor cursor, cursorOld;

    if (textToFind.isEmpty()) {
        return false;
    }

    ui->textEdit->blockSignals(true);

    found = findText(textToFind, options, ignoreComments);

    if (!found) {
        cursor = ui->textEdit->textCursor();
        cursorOld = cursor;

        if (options & QTextDocument::FindBackward) {
            cursor.movePosition(QTextCursor::End);
        } else {
            cursor.movePosition(QTextCursor::Start);
        }

        ui->textEdit->setTextCursor(cursor);

        found = findText(textToFind, options, ignoreComments);

        if (!found) {
            cursorOld.clearSelection();
            ui->textEdit->setTextCursor(cursorOld);
        }
    }

    ui->textEdit->blockSignals(false);
    highlightCurrentLine();

    return found;
}

bool MdiChild::replaceNext(QString textToFind, QString replacedText,
                           QTextDocument::FindFlags options, bool ignoreComments)
{
    QString foundText;
    double val, val1;
    bool ok;
    QRegularExpression regExp;
    QChar op;

    if (ui->textEdit->isReadOnly()) {
        return false;
    }

    if (textToFind.isEmpty()) {
        return false;
    }

    bool found = false;

    ui->textEdit->blockSignals(true);

    if (foundTextMatched(textToFind, ui->textEdit->textCursor().selectedText())) {
        found = true;
    } else {
        found = findNext(textToFind, options, ignoreComments);
    }

    if (found) {
        QTextCursor cr = ui->textEdit->textCursor();
        cr.beginEditBlock();

        if (mdiWindowProperites.underlineChanges) {
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
            cr.setCharFormat(format);
        }

        regExp.setPattern(QString("\\$\\$[\\/*+\\-]{1,1}[-]{0,1}[0-9.]{1,}"));

        if (replacedText.contains(regExp)) {
            replacedText.remove("$$");
            op = replacedText.at(0);
            replacedText.remove(0, 1);
            val = replacedText.toDouble(&ok);

            foundText = cr.selectedText();
            foundText.remove(QRegularExpression("[A-Za-z#]{1,}"));
            val1 = foundText.toDouble(&ok);
            replacedText = cr.selectedText();
            replacedText.remove(foundText);

            if ((val == 0) && (op == '/')) { //divide by 0
                val = 1;
            }

            if (op == '+') {
                val = val1 + val;
            }

            if (op == '-') {
                val = val1 - val;
            }

            if (op == '*') {
                val = val1 * val;
            }

            if (op == '/') {
                val = val1 / val;
            }

            if (replacedText == "#" || replacedText == "O" || replacedText == "o" || replacedText == "N"
                    || replacedText == "n") {
                replacedText = replacedText + Utils::removeZeros(QString("%1").arg(val, 0, 'f', 3));

                if (replacedText[replacedText.length() - 1] == '.') {
                    replacedText = replacedText.remove((replacedText.length() - 1), 1);
                }
            } else {
                replacedText = replacedText + Utils::removeZeros(QString("%1").arg(val, 0, 'f', 3));
            }

        }

        cr.insertText(replacedText);
        cr.endEditBlock();
        ui->textEdit->setTextCursor(cr);

        found = findNext(textToFind, options, ignoreComments);
    }

    ui->textEdit->blockSignals(false);
    highlightCurrentLine();
    highlightFindText(textToFind, options, ignoreComments);

    return found;
}

bool MdiChild::replaceAll(QString textToFind, QString replacedText,
                          QTextDocument::FindFlags options, bool ignoreComments)
{
    bool found = false;

    if (ui->textEdit->isReadOnly()) {
        return false;
    }

    if (textToFind.isEmpty()) {
        return false;
    }

    if (ui->textEdit->textCursor().selectedText() == textToFind) {
        found = true;
    } else {
        found = findNext(textToFind, options, ignoreComments);
    }

    QTextCursor startCursor = ui->textEdit->textCursor();

    while (found) {
        found = replaceNext(textToFind, replacedText, options, ignoreComments);

        if (startCursor.blockNumber() == ui->textEdit->textCursor().blockNumber()) {
            break;
        }

        qApp->processEvents();
    }

    return found;
}

bool MdiChild::swapAxes(QString textToFind, QString replacedText, double min, double max,
                        int oper, double modifier, QTextDocument::FindFlags options, bool ignoreComments, int prec)
{
    double val, val1;
    QRegularExpression regex;
    bool found = false;
    bool ok, inSelection;
    QString newText, foundText;
    bool inComment;
    int commentPos, id;
    QTextDocument *document;
    int cursorStart, cursorEnd;

    cursorStart = 0;
    cursorEnd = 0;

    if (ui->textEdit->isReadOnly()) {
        return false;
    }

    if (textToFind.isEmpty()) {
        return false;
    }

    if (!(options & QTextDocument::FindCaseSensitively)) {
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    ui->textEdit->blockSignals(true);

    inSelection = ui->textEdit->textCursor().hasSelection();

    if (inSelection) {
        cursorStart = ui->textEdit->textCursor().selectionStart();
        cursorEnd = ui->textEdit->textCursor().selectionEnd();
        document = new QTextDocument(ui->textEdit->textCursor().selectedText(), this);
    } else {
        document = ui->textEdit->document();
    }

    QTextCursor cursor(document);
    cursor.setPosition(0);

    do {
        if ((oper == -1) && (min == -999999)) {
            regex.setPattern(QString("(%1)(?=[-=#<.0-9]{0,1}[0-9]{0,}[.]{0,1}[0-9]{0,})(?![A-Z$ ])").arg(
                                 textToFind));
        } else {
            regex.setPattern(QString("(%1)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(textToFind));
        }

        cursor = document->find(regex, cursor, options);
        found = !cursor.isNull();

        if (found && ignoreComments) {
            QString cur_line = cursor.block().text();
            int cur_line_column = cursor.columnNumber();

            id = highligthMode();

            if ((id == MODE_SINUMERIK_840) || (id == MODE_HEIDENHAIN_ISO) || (id == MODE_HEIDENHAIN)) {
                commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
            } else {
                if ((id == MODE_AUTO) || (id == MODE_OKUMA) || (id == MODE_SINUMERIK) || (id == MODE_PHILIPS)) {
                    commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
                } else {
                    commentPos  = cur_line.indexOf(QLatin1Char('('), 0);

                    if (commentPos > cur_line_column) {
                        commentPos = -1;
                    }

                    if (commentPos < 0) {
                        commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
                    }
                }
            }

            if (commentPos < 0) {
                commentPos = cur_line_column + 1;
            }

            inComment = (commentPos < cur_line_column);
        } else {
            inComment = false;
        }

        if (found && !inComment) {
            foundText = cursor.selectedText();
            foundText.remove(textToFind);

            val = 0;
            ok = false;

            val1 = foundText.toDouble(&ok);

            if (min > -999999) {
                if (!ok) {
                    continue;
                }

                if (!((val1 >= min) && (val1 <= max))) {
                    continue;
                }
            }

            if ((modifier == 0) && (oper == 3)) { //divide by 0
                modifier = 1;
            }

            switch (oper) {
            case 0:
                val = val1 + modifier;
                break;

            case 1:
                val = val1 - modifier;
                break;

            case 2:
                val = val1 * modifier;
                break;

            case 3:
                val = val1 / modifier;
                break;

            default:
                val = val1;
                break;
            }

            if (ok) {
                if (replacedText == "#" || replacedText == "O" || replacedText == "o" || replacedText == "N"
                        || replacedText == "n") {
                    newText = replacedText + Utils::removeZeros(QString("%1").arg(val, 0, 'f', prec));

                    if (newText[newText.length() - 1] == '.') {
                        newText = newText.remove((newText.length() - 1), 1);
                    }
                } else {
                    newText = replacedText + Utils::removeZeros(QString("%1").arg(val, 0, 'f', prec));
                }
            } else {
                newText = replacedText;
            }


            if (mdiWindowProperites.underlineChanges) {
                QTextCharFormat format = cursor.charFormat();
                format.setUnderlineStyle(QTextCharFormat::DotLine);
                format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
                cursor.mergeCharFormat(format);
            }

            cursor.insertText(newText);
        }

    } while (found);


    if (inSelection) {
        cursor = QTextCursor(document);
        cursor.select(QTextCursor::Document);

        if (cursorStart > cursorEnd) {
            cursorStart = cursorEnd;
        }

        cursorEnd = cursorStart + cursor.selectedText().length();
        ui->textEdit->textCursor().insertFragment(cursor.selection());
        delete (document);

        cursor = ui->textEdit->textCursor();  //restore selection
        cursor.setPosition(cursorStart, QTextCursor::MoveAnchor);
        cursor.setPosition(cursorEnd, QTextCursor::KeepAnchor);
        ui->textEdit->setTextCursor(cursor);
    }

    ui->textEdit->blockSignals(false);

    return found;
}

void MdiChild::doSwapAxes(QString textToFind, QString replacedText, double min, double max,
                          int oper, double modifier, QTextDocument::FindFlags options, bool ignoreComments, int precision)
{
    if (ui->textEdit->isReadOnly()) {
        return;
    }

    if (textToFind.isEmpty()) {
        return;
    }

    QTextCursor startCursor = ui->textEdit->textCursor();
    startCursor.beginEditBlock();

    if (textToFind != replacedText) {
        swapAxes(replacedText, QString("~%1").arg(replacedText), min, max, -1, modifier, options,
                 ignoreComments, precision);
        swapAxes(textToFind, replacedText, min, max, oper, modifier, options, ignoreComments,
                 precision);
        swapAxes(QString("~%1").arg(replacedText), textToFind, -999999, 0, -1, 0, options,
                 ignoreComments, precision);
    } else {
        swapAxes(textToFind, replacedText, min, max, oper, modifier, options, ignoreComments,
                 precision);
    }

    startCursor.movePosition(QTextCursor::StartOfLine);
    startCursor.endEditBlock();
    ui->textEdit->setTextCursor(startCursor);
}

void MdiChild::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
    printer.setPageMargins(15, 10, 10, 10, QPrinter::Millimeter);
#else
    printer.setPageMargins(QMarginsF(15, 10, 10, 10), QPageLayout::Millimeter);
#endif
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags(Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
#endif
}

void MdiChild::printPreview(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    ui->textEdit->print(printer);
#endif
}

bool MdiChild::isModified()
{
    return ui->textEdit->document()->isModified();
}

void MdiChild::setModified(bool mod)
{
    ui->textEdit->document()->setModified(mod);
}

bool MdiChild::isReadOnly()
{
    return ui->textEdit->isReadOnly();
}

bool MdiChild::hasSelection()
{
    return ui->textEdit->textCursor().hasSelection();
}

bool MdiChild::isUndoAvailable()
{
    return ui->textEdit->document()->isUndoAvailable();
}

bool MdiChild::isRedoAvailable()
{
    return ui->textEdit->document()->isRedoAvailable();
}

bool MdiChild::overwriteMode()
{
    return ui->textEdit->overwriteMode();
}

QTextCursor MdiChild::textCursor()
{
    return ui->textEdit->textCursor();
}

//void MdiChild::createContextMenuActions()
//{
//}

void MdiChild::showContextMenu(const QPoint &pt)
{
    QMenu *menu = ui->textEdit->createStandardContextMenu();
    menu->addSeparator();

    Addons::Actions *addonsActions = EdytorNc::instance()->addonsActions();
    menu->addAction(addonsActions->semiComment());
    menu->addAction(addonsActions->paraComment());
    menu->addSeparator();
    menu->addAction(addonsActions->blockSkipIncrement());
    menu->addAction(addonsActions->blockSkipDecrement());
    menu->addAction(addonsActions->blockSkipRemove());
    menu->addSeparator();

    QAction *inLineCalcAct = new QAction(QIcon(":/images/inlinecalc.png"), tr("Inline calculator"),
                                         this);
    inLineCalcAct->setShortcut(tr("Ctrl+0"));
    connect(inLineCalcAct, SIGNAL(triggered()), this, SLOT(showInLineCalc()));
    menu->addAction(inLineCalcAct);

    menu->exec(ui->textEdit->mapToGlobal(pt));

    delete inLineCalcAct;
    delete menu;
}

//**************************************************************************************************
//  Returns filename with full path
//**************************************************************************************************

QString MdiChild::currentFile()
{
    QString path = curFile;

    if (isUntitled) {
        path = mdiWindowProperites.lastDir + "/" + path;
    }

    path = QDir::cleanPath(path);

    return QDir::toNativeSeparators(path);
}

//void MdiChild::setFileChangeMonitor(QFileSystemWatcher *monitor)
//{
//    fileChangeMonitor = monitor;
//}

void MdiChild::fileChangeMonitorAddPath(QString fileName)
{
    //if(fileChangeMonitor != nullptr)
    //fileChangeMonitor->addPath(fileName);

    emit addRemoveFileWatch(fileName, true);
}

void MdiChild::fileChangeMonitorRemovePath(QString fileName)
{
    //if(fileChangeMonitor != nullptr)
    //fileChangeMonitor->removePath(fileName);

    emit addRemoveFileWatch(fileName, false);
}

void MdiChild::showInLineCalc()
{
    if (!calcLineEdit) {
        calcLineEdit = new QLineEdit(this);
        calcLineEdit->setClearButtonEnabled(true);
        calcLineEdit->setMinimumWidth(100);
        calcLineEdit->adjustSize();
        calcLineEdit->setAttribute(Qt::WA_DeleteOnClose);
        calcLineEdit->setToolTip(tr("You can use:\n") +
                                 "+ - * / ()\n" +
                                 "SIN(x)\n" +
                                 "COS(x)\n" +
                                 "TAN(x)\n" +
                                 "SQRT(x)\n" +
                                 "SQR(x)\n" +
                                 "ABS(x)\n" +
                                 "TRUNC(x)\n" +
                                 "PI\n" +
                                 tr("Press Enter to accept or click anywere to canacel"));

        connect(calcLineEdit, SIGNAL(editingFinished()), this, SLOT(inLineCalcEditingFinished()));
        connect(calcLineEdit, SIGNAL(returnPressed()), this, SLOT(inLineCalcReturnPressed()));

        //calcLineEditWordList << "COS(" << "PI";
        QCompleter *completer = new QCompleter(calcLineEditWordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        calcLineEdit->setCompleter(completer);
    }

    if (calcLineEdit) {
        inLineCalcChar = '0';

        if (ui->textEdit->textCursor().hasSelection()) {
            QString selText = ui->textEdit->textCursor().selectedText();

            if (selText.length() > 1)
                if (selText.at(0).isLetter()) {
                    inLineCalcChar = selText.at(0);
                }

            if (inLineCalcChar.isLetter()) {
                selText.remove(inLineCalcChar);
                selText.remove(" ");
                calcLineEdit->setText(selText);
            }
        }

        QRect rect = ui->textEdit->cursorRect();
        int h = (calcLineEdit->height() - rect.height()) / 2;
        calcLineEdit->move(rect.x() + rect.height(), rect.top() - h);
        calcLineEdit->setFocus();
        calcLineEdit->show();
    }
}

void MdiChild::inLineCalcEditingFinished()
{
    if (calcLineEdit) {
        calcLineEdit->close();
    }
}

void MdiChild::inLineCalcReturnPressed()
{
    if (calcLineEdit) {
        QString text = calcLineEdit->text();
        text.replace(',', '.');

        calcLineEditWordList.append(text);
        calcLineEditWordList.removeDuplicates();

        int result = Utils::processBrc(&text);
        text = Utils::removeZeros(text);

        if (inLineCalcChar.isLetter()) {
            text.prepend(inLineCalcChar);
        }

        qDebug() << "Text" << text << result;

        if (result >= 0) {
            ui->textEdit->insertPlainText(text);
        }

        calcLineEdit->close();
    }
}
