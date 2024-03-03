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

#include <QAction>
#include <QApplication>
#include <QChar>
#include <QCloseEvent>
#include <QColor>
#include <QCompleter>
#include <QDateTime>
#include <QDir>
#include <QEvent>              // QEvent::KeyPress
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHash>
#include <QIODevice>
#include <QIcon>
#include <QKeyEvent>
#include <QLatin1Char>
#include <QLatin1String>
#include <QLineEdit>
#include <QLocale>
#include <QMarginsF>
#include <QMenu>
#include <QMessageBox>
#include <QPageLayout>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QRect>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>
#include <QSplitter>
#include <QString>
#include <QStringList>
#include <Qt>                  // Qt::WindowFlags
#include <QtDebug>             // qDebug()
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFormat>
#include <QTextOption>
#include <QTextStream>
#include <QtGlobal>            // QT_VERSION QT_VERSION_CHECK
#include <QVariant>

#include <addons-actions.h>
#include <edytornc.h>
#include <mdichild.h>               // IWYU pragma: associated
#include <utils/expressionparser.h>
#include <utils/guessfilename.h>    // Utils::guessFileName()
#include <utils/medium.h>
#include <utils/removezeros.h>      // Utils::removeZeros()

#include "capslockeventfilter.h"
#include "gcodereventfilter.h"
#include "gcoderinfo.h"        // GCoderInfo
#include "gcoderstyle.h"
#include "gcoderwidgetproperties.h"
#include "highlighter.h"       // Highlighter
#include "highlightmode.h"
#include "ui_mdichildform.h"

class QPoint;


MdiChild::MdiChild(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    ui = new Ui::MdiChild();
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_isUntitled = true;
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
    document()->setDocumentMargin(8);
    highlighter = nullptr;
    m_highlightMode = MODE_AUTO;
    setFocusProxy(ui->textEdit);

    ui->marginWidget->setAutoFillBackground(true);

    m_capsLockEventFilter = new CapsLockEventFilter(ui->textEdit);
    m_capsLockEventFilter->setCapsLockEnable(m_widgetProperties.intCapsLock);
    ui->textEdit->installEventFilter(m_capsLockEventFilter);
    m_gCoderEventFilter = new GCoderEventFilter(ui->textEdit, this);
    ui->textEdit->installEventFilter(m_gCoderEventFilter);
    ui->textEdit->viewport()->installEventFilter(m_gCoderEventFilter);
    connect(m_gCoderEventFilter, SIGNAL(requestInLineCalc()), this, SLOT(showInLineCalc()));
    connect(m_gCoderEventFilter, SIGNAL(requestUnderLine()), this, SLOT(underLine()));

    setWindowIcon(QIcon(":/images/ncfile.png"));

    //fileChangeMonitor.clear();

    ui->splitterH->setBackgroundRole(QPalette::Base);
    ui->marginWidget->setBackgroundRole(QPalette::Base);

    ui->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(textEditCursorPositionChanged()));
    connect(ui->textEdit, SIGNAL(modificationChanged(bool)), this, SLOT(textEditModificationChanged(bool)));
    connect(ui->textEdit, SIGNAL(redoAvailable(bool)), this, SLOT(textEditRedoAvailable(bool)));
    connect(ui->textEdit, SIGNAL(undoAvailable(bool)), this, SLOT(textEditUndoAvailable(bool)));
}

MdiChild::~MdiChild()
{
    if (highlighter != nullptr) {
        delete highlighter;
    }
}

QPlainTextEdit *MdiChild::textEdit() const
{
    return ui->textEdit;
}

void MdiChild::newFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }

    m_isUntitled = true;
    m_fileName = tr("program%1.nc").arg(sequenceNumber++);
    document()->setModified(false);
    setWindowTitle(m_fileName + "[*]");
    m_brief = m_fileName;
    updateWindowTitle();
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

bool MdiChild::load() {
    if (!loadFile(filePath())) {
        return false;
    }

    m_isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    updateBrief();
    updateWindowTitle();
    detectHighligthMode();

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    fileChangeMonitorAddPath(filePath());
    return true;
}

bool MdiChild::save()
{
    if (!saveFile(filePath())) {
        return false;
    }

    m_isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    updateBrief();
    updateWindowTitle();

    if (m_widgetProperties.clearUndoHistory) {
        document()->clearUndoRedoStacks();
    }

    if (m_widgetProperties.clearUnderlineHistory) {

        QTextCursor cursorPos = textCursor();
        textEdit()->blockSignals(true);
        selectAll();

        if (m_widgetProperties.underlineChanges) {
            QTextCursor cr = textCursor(); // Clear underline
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
            cr.setCharFormat(format);
            setTextCursor(cr);
        }

        setTextCursor(cursorPos);

        document()->setModified(false);
        documentWasModified();
        textEdit()->blockSignals(false);
    }

    detectHighligthMode();
    return true;
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        m_ioErrorString = file.errorString();
        return false;
    }

    m_ioErrorString.clear();
    setRawData(file.readAll());
    file.close();
    return true;
}

bool MdiChild::saveFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        m_ioErrorString = file.errorString();
        return false;
    }

    m_ioErrorString.clear();

    changeDateInComment();
    fileChangeMonitorRemovePath(file.fileName());
    file.write(rawData());
    file.close();
    fileChangeMonitorAddPath(fileName);
    return true;
}

QString MdiChild::ioErrorString() const
{
    return m_ioErrorString;
}

QByteArray MdiChild::rawData() const
{
    return text(true).toLocal8Bit();
}

void MdiChild::setRawData(const QByteArray &data)
{
    setText(QString::fromLocal8Bit(data));
}

void MdiChild::changeDateInComment()
{
    if (!m_widgetProperties.changeDateInComment) {
        return;
    }

    QRegularExpression regex;
    QString strDate = QLocale().toString(QDate::currentDate(), QLocale::ShortFormat);
    regex.setPattern(tr("(DATE)") + "[:\\s]*[\\d]{1,4}(\\.|-|/)[\\d]{1,2}(\\.|-|/)[\\d]{2,4}");
    QTextCursor cursor = textCursor();
    cursor.setPosition(0);

    cursor = document()->find(regex, cursor);

    if (!cursor.isNull()) {
        textEdit()->setUpdatesEnabled(false);
        cursor.beginEditBlock();
        cursor.removeSelectedText();
        cursor.insertText(tr("DATE") + ": " + strDate);
        cursor.endEditBlock();

        textEdit()->setUpdatesEnabled(true);
        textEdit()->repaint();
    } else {
        cursor = textCursor();

        regex.setPattern("(\\(){1,1}[\\s]{0,}[\\d]{1,4}(\\.|-|/)[\\d]{1,2}(\\.|-|/)[\\d]{2,4}[\\s]{0,5}(\\)){1,1}");
        cursor.setPosition(0);
        cursor = document()->find(regex, cursor);

        if (cursor.isNull()) {
            regex.setPattern("(;){1,1}[\\s]{0,}[\\d]{1,4}(\\.|-|/)[\\d]{1,2}(\\.|-|/)[\\d]{2,4}[\\s]{0,5}");
            cursor.setPosition(0);
            cursor = document()->find(regex, cursor);
        }

        if (!cursor.isNull()) {
            textEdit()->setUpdatesEnabled(false);
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

            textEdit()->setUpdatesEnabled(true);
            textEdit()->repaint();
        }
    }
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (closeRequest(this)) {
        fileChangeMonitorRemovePath(filePath());
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

void MdiChild::updateBrief()
{
    QRegularExpression regex;
    QString text = this->text();
    QString f_tx;

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
        m_brief = fileName();
    } else {
        m_brief = f_tx.simplified();
    }
}

void MdiChild::updateWindowTitle()
{
    QString title = "";

    if ((m_widgetProperties.windowMode & SHOW_PROGTITLE)) {
        title = m_brief;
    }

    if (!title.isEmpty() && ((m_widgetProperties.windowMode & SHOW_FILEPATH)
                             || (m_widgetProperties.windowMode & SHOW_FILENAME))) {
        title += " ---> ";
    }

    if ((m_widgetProperties.windowMode & SHOW_FILEPATH)) {
        title += m_dir.path() + "/";
    }

    if ((m_widgetProperties.windowMode & SHOW_FILENAME) || title.isEmpty()) {
        title += m_fileName;
    }

    title += "[*]";
    setWindowTitle(title);
}

//QString MdiChild::strippedName(const QString &fullFileName)
//{
//   return QFileInfo(fullFileName).fileName();
//}

QString MdiChild::path() const
{
    return m_dir.path();
}

void MdiChild::setPath(const QString &path)
{
    m_dir.setPath(path);
}

QString MdiChild::fileName() const
{
    return  m_fileName;
}

void MdiChild::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

QString MdiChild::filePath() const
{
    return m_dir.filePath(m_fileName);
}

void MdiChild::setFilePath(const QString &filePath)
{
    QFileInfo fi(filePath);
    m_fileName = fi.fileName();
    m_dir = fi.dir();
}

DocumentInfo::Ptr MdiChild::documentInfo() const
{
    GCoderInfo *info = new GCoderInfo();
    info->cursorPos = textCursor().position();
    info->filePath = filePath();
    info->geometry = parentWidget()->saveGeometry();
    info->highlightMode = highligthMode();
    info->readOnly = isReadOnly();
    return GCoderInfo::Ptr(info);
}

void MdiChild::setDocumentInfo(const DocumentInfo::Ptr &info)
{
    if (!info->geometry.isEmpty()) {
        parentWidget()->restoreGeometry(info->geometry);
    }

    GCoderInfo *gci = dynamic_cast<GCoderInfo *>(info.get());

    if (gci) {
        QTextCursor cursor = textCursor();
        cursor.setPosition(gci->cursorPos);
        setTextCursor(cursor);
        setHighligthMode(gci->highlightMode);
    }
}

DocumentStyle::Ptr MdiChild::codeStyle() const
{
    return DocumentStyle::Ptr(new GCoderStyle(m_codeStyle));
}

void MdiChild::setCodeStyle(const DocumentStyle::Ptr &style)
{
    try {
        m_codeStyle = dynamic_cast<const GCoderStyle &>(*style);
    }  catch (std::bad_cast &e) {
        return;
    }

    QFont font = QFont(m_codeStyle.fontName, m_codeStyle.fontSize, QFont::Normal);
    document()->setDefaultFont(font);

    QPalette pal;

    if (m_codeStyle.hColors.backgroundColor != 0xFFFFFF) {
        pal.setColor(QPalette::Base, QColor(m_codeStyle.hColors.backgroundColor));
    }

    pal.setColor(QPalette::Text, QColor(m_codeStyle.hColors.defaultColor));
    setPalette(pal);
    detectHighligthMode();
    highlightCurrentLine();
}

DocumentWidgetProperties::Ptr MdiChild::widgetProperties() const
{
    return DocumentWidgetProperties::Ptr(new GCoderWidgetProperties(m_widgetProperties));
}

void MdiChild::setWidgetProperties(const DocumentWidgetProperties::Ptr &properties)
{
    try {
        m_widgetProperties = dynamic_cast<const GCoderWidgetProperties &>(*properties);
    }  catch (std::bad_cast &e) {
        return;
    }

    m_capsLockEventFilter->setCapsLockEnable(m_widgetProperties.intCapsLock);

    if (m_widgetProperties.syntaxH) {
        if (highlighter == nullptr) {
            highlighter = new Highlighter(document());
        }
    } else {
        if (highlighter != nullptr) {
            delete (highlighter);
        }

        highlighter = nullptr;
    }

    updateToolTips();
}

void MdiChild::updateToolTips()
{
    QString group;

    switch (m_highlightMode) {
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
        m_gCoderEventFilter->setToolTipEnable(false);
        return;
    }

    m_gCoderEventFilter->setToolTipEnable(m_widgetProperties.editorToolTips);

    QHash<QString, QString> tips;

    QString fileName = Medium::instance().settingsDir() + "/" + "cnc_tips_" + QLocale::system().name() + ".txt";
    loadToolTips(tips, fileName, group);

    fileName = path() + "/" + "cnc_tips.txt";
    loadToolTips(tips, fileName, group);

    m_gCoderEventFilter->setTips(tips);
}

void MdiChild::loadToolTips(QHash<QString, QString> &tips, const QString &fileName, const QString &group)
{
    if (QFile::exists(fileName)) {
        QSettings settings(fileName, QSettings::IniFormat);
        settings.beginGroup(group);
        const QStringList &keys = settings.childKeys();

        for (const QString &k : keys) {
            QString text = settings.value(k, "").toString();

            if (!text.isEmpty()) {
                tips.insert(k, text);
            } else {
                tips.remove(k);
            }
        }
    }
}

void MdiChild::underLine()
{
    if (m_widgetProperties.underlineChanges) {
         QTextCursor cr = textCursor();
         QTextCharFormat format = cr.charFormat();
         format.setUnderlineStyle(QTextCharFormat::DotLine);
         format.setUnderlineColor(QColor(m_codeStyle.underlineColor));
         cr.setCharFormat(format);
         setTextCursor(cr);
    }
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
    textEdit()->setExtraSelections(tmpSelections);

    if (!isReadOnly()) {
        selection.format.setBackground(QColor(m_codeStyle.lineColor));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    QColor lineColor = QColor(m_codeStyle.lineColor).darker(108);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = document();
    QTextCursor cursor = textCursor();
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

            if (m_highlightMode == MODE_LINUXCNC) {
                cursor = textCursor();

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


            if (m_highlightMode == MODE_SINUMERIK_840) {
                cursor = textCursor();

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
        textEdit()->setExtraSelections(tmpSelections);
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

    if (m_highlightMode == MODE_LINUXCNC) {
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
        textEdit()->setExtraSelections(tmpSelections);
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
    textEdit()->setExtraSelections(tmpSelections);
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

    QTextDocument *doc = document();
    QTextCursor cursor = textCursor();
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
    textEdit()->setExtraSelections(tmpSelections);
}

void MdiChild::undo()
{
    textEdit()->undo();
    textEdit()->ensureCursorVisible();
    highlightCurrentLine();
}

void MdiChild::redo()
{
    textEdit()->redo();
    textEdit()->ensureCursorVisible();
    highlightCurrentLine();
}

void MdiChild::detectHighligthMode()
{
    if (!m_widgetProperties.syntaxH) {
        return;
    }

    if (highlighter == nullptr) {
        return;
    }

    bool mod = document()->isModified();  // something below clears document modified state

    if (m_highlightMode == MODE_AUTO) {
        m_highlightMode = autoDetectHighligthMode(text().toUpper());

        if (m_highlightMode == MODE_AUTO) {
            m_highlightMode = m_widgetProperties.defaultHighlightMode;
        }

        updateToolTips();
    }

    highlighter->setHighlightMode(m_highlightMode);
    highlighter->setHColors(m_codeStyle.hColors, QFont(m_codeStyle.fontName, m_codeStyle.fontSize, QFont::Normal));
    highlighter->rehighlight();

    document()->setModified(mod);
}

void MdiChild::setHighligthMode(int mod)
{
    m_highlightMode = mod;
    updateToolTips();
    detectHighligthMode();
}

int MdiChild::highligthMode() const
{
    return m_highlightMode;
}

QString MdiChild::brief()
{
    return m_brief;
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

    textEdit()->setUpdatesEnabled(false);

    if (addr.isEmpty()) {
        return false;
    }

    cursor = textCursor();

    do {
        if (isRegExp) {
            regex.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(addr));

            cursor = document()->find(regex, cursor, options);

            found = !cursor.isNull();

            if (found) {
                if (!isRegExpMinMax) {
                    setTextCursor(cursor);
                }
            } else {
                break;
            }
        } else {
            found = textEdit()->find(addr, options);
            cursor = textCursor();
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
                setTextCursor(cursor);
            } else {
                inComment = true;
            }
        }

    } while (inComment);

    textEdit()->setUpdatesEnabled(true);
    return found;
}

QString MdiChild::guessFileName()
{
    QString fileName;

    if (m_widgetProperties.guessFileNameByProgNum) {
        fileName = Utils::guessFileNameByProgNum(text()).name;
    } else {
        fileName = Utils::guessFileNameByComments(text());
    }

    return fileName.simplified();
}

bool MdiChild::findNext(QString textToFind, QTextDocument::FindFlags options,
                        bool ignoreComments)
{
    bool found = false;
    QTextCursor cursor, cursorOld;

    if (textToFind.isEmpty()) {
        return false;
    }

    textEdit()->blockSignals(true);

    found = findText(textToFind, options, ignoreComments);

    if (!found) {
        cursor = textCursor();
        cursorOld = cursor;

        if (options & QTextDocument::FindBackward) {
            cursor.movePosition(QTextCursor::End);
        } else {
            cursor.movePosition(QTextCursor::Start);
        }

        setTextCursor(cursor);

        found = findText(textToFind, options, ignoreComments);

        if (!found) {
            cursorOld.clearSelection();
            setTextCursor(cursorOld);
        }
    }

    textEdit()->blockSignals(false);
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

    if (isReadOnly()) {
        return false;
    }

    if (textToFind.isEmpty()) {
        return false;
    }

    bool found = false;

    textEdit()->blockSignals(true);

    if (foundTextMatched(textToFind, selectedText())) {
        found = true;
    } else {
        found = findNext(textToFind, options, ignoreComments);
    }

    if (found) {
        QTextCursor cr = textCursor();
        cr.beginEditBlock();

        if (m_widgetProperties.underlineChanges) {
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(m_codeStyle.underlineColor));
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
        setTextCursor(cr);

        found = findNext(textToFind, options, ignoreComments);
    }

    textEdit()->blockSignals(false);
    highlightCurrentLine();
    highlightFindText(textToFind, options, ignoreComments);

    return found;
}

bool MdiChild::replaceAll(QString textToFind, QString replacedText,
                          QTextDocument::FindFlags options, bool ignoreComments)
{
    bool found = false;

    if (isReadOnly()) {
        return false;
    }

    if (textToFind.isEmpty()) {
        return false;
    }

    if (selectedText() == textToFind) {
        found = true;
    } else {
        found = findNext(textToFind, options, ignoreComments);
    }

    QTextCursor startCursor = textCursor();

    while (found) {
        found = replaceNext(textToFind, replacedText, options, ignoreComments);

        if (startCursor.blockNumber() == textCursor().blockNumber()) {
            break;
        }

        qApp->processEvents();
    }

    return found;
}

QString MdiChild::text(bool addCR) const
{
    QString text = document()->toPlainText();

    if (addCR && !text.contains(QLatin1String("\r\n"))) {
        text.replace(QLatin1String("\n"), QLatin1String("\r\n"));
    }

    return text;
}

void MdiChild::setText(const QString &text)
{
    document()->setPlainText(text);
}

void MdiChild::insertText(const QString& text)
{
    textEdit()->insertPlainText(text);
}

bool MdiChild::isModified() const
{
    return document()->isModified();
}

void MdiChild::setModified(bool mod)
{
    document()->setModified(mod);
}

bool MdiChild::isReadOnly() const
{
    return textEdit()->isReadOnly();
}

void MdiChild::setReadOnly(bool ro)
{
    textEdit()->setReadOnly(ro);
}

bool MdiChild::isUntitled() const
{
    return m_isUntitled;
}

bool MdiChild::hasSelection() const
{
    return textCursor().hasSelection();
}

bool MdiChild::isUndoAvailable() const
{
    return document()->isUndoAvailable();
}

bool MdiChild::isRedoAvailable() const
{
    return document()->isRedoAvailable();
}

void MdiChild::clearUndoRedoStacks()
{
    document()->clearUndoRedoStacks();
}

bool MdiChild::overwriteMode()
{
    return textEdit()->overwriteMode();
}

void MdiChild::centerCursor()
{
    textEdit()->centerCursor();
}

QString MdiChild::wordUnderCursor() const
{
    QTextCursor cursor = textEdit()->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void MdiChild::selectAll()
{
    textEdit()->selectAll();
}

void MdiChild::clearSelection(bool toAnchor)
{
    QTextCursor cursor = textEdit()->textCursor();
    cursor.setPosition(toAnchor ? cursor.anchor() : cursor.position());
    setTextCursor(cursor);
}

QString MdiChild::selectedText() const
{
    return textEdit()->textCursor().selectedText();
}

void MdiChild::removeSelectedText()
{
    textEdit()->textCursor().removeSelectedText();
}

void MdiChild::clear()
{
    textEdit()->clear();
}

void MdiChild::copy()
{
    textEdit()->copy();
}

void MdiChild::cut()
{
    textEdit()->cut();
}

void MdiChild::paste()
{
    if (m_widgetProperties.underlineChanges) {
        QTextCharFormat format = textEdit()->currentCharFormat();
        format.setUnderlineStyle(QTextCharFormat::DotLine);
        format.setUnderlineColor(QColor(m_codeStyle.underlineColor));
        textEdit()->setCurrentCharFormat(format);
    }

    textEdit()->paste();
}

void MdiChild::print(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    document()->print(printer);
#endif
}

int MdiChild::currentLine() const
{
    return textCursor().block().firstLineNumber() + 1;
}

int MdiChild::currentColumn() const
{
    QTextCursor cursor = textCursor();
    return cursor.position() - cursor.block().position();
}

void MdiChild::goToLine(int line)
{
    QTextBlock block = document()->findBlockByNumber(line);
    setTextCursor(QTextCursor(block));
    centerCursor();
    setFocus();
}

QTextDocument *MdiChild::document() const
{
    return textEdit()->document();
}

QTextCursor MdiChild::textCursor() const
{
    return textEdit()->textCursor();
}

void MdiChild::setTextCursor(const QTextCursor& cursor)
{
    textEdit()->setTextCursor(cursor);
}

void MdiChild::textEditRedoAvailable(bool available)
{
    emit redoAvailable(available);
}

void MdiChild::textEditUndoAvailable(bool available)
{
    emit undoAvailable(available);
}

void MdiChild::textEditCursorPositionChanged()
{
    emit cursorPositionChanged();
}

void MdiChild::textEditModificationChanged(bool ch)
{
    emit modificationChanged(ch);
}

//void MdiChild::createContextMenuActions()
//{
//}

void MdiChild::showContextMenu(const QPoint &pt)
{
    QMenu *menu = textEdit()->createStandardContextMenu();
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

    menu->exec(textEdit()->mapToGlobal(pt));

    delete inLineCalcAct;
    delete menu;
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

        if (hasSelection()) {
            QString selText = selectedText();

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

        QRect rect = textEdit()->cursorRect();
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
            insertText(text);
        }

        calcLineEdit->close();
    }
}
