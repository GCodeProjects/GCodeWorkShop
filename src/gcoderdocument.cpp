/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of EdytorNC.
 *
 *  EdytorNC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <memory>   // for shared_ptr<>::element_type
#include <typeinfo> // for bad_cast

#include <QApplication>             // for QApplication, qApp
#include <QChar>                    // for operator==, QChar
#include <QColor>                   // for QColor
#include <QDateTime>                // for QDate
#include <QFile>                    // for QFile
#include <QFont>                    // for QFont, QFont::Normal, QFont::Bold
#include <QHash>                    // for QHash
#include <QIcon>                    // for QIcon
#include <QLatin1Char>              // for QLatin1Char
#include <QLatin1String>            // for QLatin1String
#include <QLocale>                  // for QLocale, QLocale::ShortFormat
#include <QPalette>                 // for QPalette, QPalette::Base, QPalette::Text
#include <QPlainTextEdit>           // for QPlainTextEdit
#ifndef QT_NO_PRINTER
	#include <QPrinter>             // for QPrinter
#endif
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QSettings>                // for QSettings, QSettings::IniFormat
#include <QStringList>              // for QStringList
#include <QTextBlock>               // for QTextBlock
#include <QTextCharFormat>          // for QTextCharFormat, QTextCharFormat::DotLine, QTextCharFormat::NoUnderline
#include <QTextDocument>            // for QTextDocument, QTextDocument::FindFlags, operator|, QTextDocument::FindB...
#include <QTextFormat>              // for QTextFormat, QTextFormat::FullWidthSelection
#include <QTextOption>              // for QTextOption, QTextOption::NoWrap
#include <QVariant>                 // for QVariant
#include <QWidget>                  // for QWidget
#include <Qt>                       // for CaseInsensitive, CaseSensitive, CaseSensitivity, WA_DeleteOnClose, Custo...
#include <QtGlobal>                 // for QFlags, QFlags<>::enum_type, qDebug

class QMenu;

#include <document.h>
#include <documentinfo.h>              // for DocumentInfo, DocumentInfo::Ptr
#include <documentstyle.h>             // for DocumentStyle, DocumentStyle::Ptr
#include <documentwidgetproperties.h>  // for DocumentWidgetProperties, DocumentWidgetProperties::Ptr
#include <gcoderdocument.h>         // IWYU pragma: associated
#include <utils/guessfilename.h>    // for guessFileNameByComments, guessFileNameByProgNum, FileExt
#include <utils/medium.h>           // for Medium
#include <utils/removezeros.h>      // for removeZeros

#include "capslockeventfilter.h"
#include "gcoder.h"
#include "gcodereventfilter.h"      // for GCoderEventFilter
#include "gcoderinfo.h"
#include "gcoderstyle.h"            // for GCoderStyle
#include "gcoderwidgetproperties.h" // for GCoderWidgetProperties, SHOW_FILENAME, SHOW_FILEPATH, SHOW_PROGTITLE
#include "highlighter.h"            // for Highlighter, autoDetectHighligthMode
#include "highlightmode.h"          // for MODE_LINUXCNC, MODE_AUTO, MODE_SINUMERIK_840, MODE_FANUC, MODE_HEIDENHAIN
#include "inlinecalc.h"


GCoderDocument::GCoderDocument() : Document(nullptr)
{
	m_highlighter = nullptr;
	m_highlightMode = MODE_AUTO;
	m_preLoadCursorPosition = 0;

	m_textEdit = new QPlainTextEdit();
	setWidget(m_textEdit);
	m_textEdit->setWordWrapMode(QTextOption::NoWrap);

	m_inLineCalc = new InLineCalc(m_textEdit);
	connect(m_inLineCalc, SIGNAL(complete(const QString&)), this, SLOT(inLineCalcComplete(const QString&)));


	m_textEdit->setWindowIcon(QIcon(":/images/ncfile.png"));
	m_textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_textEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this,
	        SLOT(customContextMenuRequest(const QPoint&)));
	connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
	connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorMoved()));
	connect(m_textEdit, SIGNAL(selectionChanged()), this, SLOT(selectionUpdated()));
	connect(m_textEdit, SIGNAL(modificationChanged(bool)), this, SLOT(setModified(bool)));
	connect(m_textEdit, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoAvailable(bool)));
	connect(m_textEdit, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoAvailable(bool)));

	m_capsLockEventFilter = new CapsLockEventFilter(m_textEdit);
	m_capsLockEventFilter->setCapsLockEnable(m_widgetProperties.intCapsLock);
	m_textEdit->installEventFilter(m_capsLockEventFilter);
	m_gCoderEventFilter = new GCoderEventFilter(m_textEdit, this);
	m_textEdit->installEventFilter(m_gCoderEventFilter);
	m_textEdit->viewport()->installEventFilter(m_gCoderEventFilter);
	connect(m_gCoderEventFilter, SIGNAL(requestInLineCalc()), this, SLOT(showInLineCalc()));
	connect(m_gCoderEventFilter, SIGNAL(requestUnderLine()), this, SLOT(underLine()));

	document()->setDocumentMargin(8);
}

QString GCoderDocument::type() const
{
	return GCoder::DOCUMENT_TYPE;
}

QString GCoderDocument::guessFileName() const
{
	QString fileName;

	if (m_widgetProperties.guessFileNameByProgNum) {
		fileName = Utils::guessFileNameByProgNum(text()).name;
	} else {
		fileName = Utils::guessFileNameByComments(text());
	}

	return fileName.simplified();
}

void GCoderDocument::loadTemplate(const QString& fileName)
{
	static int sequenceNumber = 1;

	if (!fileName.isEmpty()) {
		loadFile(fileName, false);
	}

	setUntitled(true);
	setFileName(tr("program%1.nc").arg(sequenceNumber++));
	document()->setModified(false);
	setBrief(this->fileName());
	updateWindowTitle();
}

bool GCoderDocument::load()
{
	if (!loadFile(filePath())) {
		return false;
	}

	if (isUntitled()) {
		QTextCursor cursor = textCursor();
		cursor.setPosition(m_preLoadCursorPosition);
		setTextCursor(cursor);
	}

	setUntitled(false);
	document()->setModified(false);
	updateBrief();
	updateWindowTitle();
	detectHighlightMode();

	return true;
}

bool GCoderDocument::save()
{
	changeDateInComment();

	if (!saveFile(filePath())) {
		return false;
	}

	setUntitled(false);
	document()->setModified(false);
	updateBrief();
	updateWindowTitle();

	if (m_widgetProperties.clearUndoHistory) {
		clearUndoRedoStacks();
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
		textEdit()->blockSignals(false);
	}

	detectHighlightMode();
	return true;
}

QByteArray GCoderDocument::rawData() const
{
	return text(true).toLocal8Bit();
}

void GCoderDocument::setRawData(const QByteArray& data)
{
	setText(QString::fromLocal8Bit(data));
}

QMenu* GCoderDocument::createStandardContextMenu(const QPoint& pos)
{
	return textEdit()->createStandardContextMenu(pos);
}

DocumentInfo::Ptr GCoderDocument::documentInfo() const
{
	GCoderInfo* info = new GCoderInfo();
	*info = *Document::documentInfo();
	info->cursorPos = textCursor().position();
	info->highlightMode = highlightMode();
	return DocumentInfo::Ptr(info);
}

void GCoderDocument::setDocumentInfo(const DocumentInfo::Ptr& info)
{
	Document::setDocumentInfo(info);

	GCoderInfo* gci = dynamic_cast<GCoderInfo*>(info.get());

	if (gci) {
		setHighlightMode(gci->highlightMode);

		if (isUntitled()) {
			m_preLoadCursorPosition = gci->cursorPos;
		} else {
			QTextCursor cursor = textCursor();
			cursor.setPosition(gci->cursorPos);
			setTextCursor(cursor);
		}
	}
}

DocumentStyle::Ptr GCoderDocument::documentStyle() const
{
	return DocumentStyle::Ptr(new GCoderStyle(m_codeStyle));
}

void GCoderDocument::setDocumentStyle(const DocumentStyle::Ptr& style)
{
	try {
		m_codeStyle = dynamic_cast<const GCoderStyle&>(*style);
	}  catch (std::bad_cast& e) {
		return;
	}

	QFont font = QFont(m_codeStyle.fontName, m_codeStyle.fontSize, QFont::Normal);
	document()->setDefaultFont(font);

	QPalette pal;

	if (m_codeStyle.hColors.backgroundColor != 0xFFFFFF) {
		pal.setColor(QPalette::Base, QColor(m_codeStyle.hColors.backgroundColor));
	}

	pal.setColor(QPalette::Text, QColor(m_codeStyle.hColors.defaultColor));
	widget()->setPalette(pal);
	rehighlight();
	highlightCurrentLine();
}

DocumentWidgetProperties::Ptr GCoderDocument::documentWidgetProperties() const
{
	return DocumentWidgetProperties::Ptr(new GCoderWidgetProperties(m_widgetProperties));
}

void GCoderDocument::setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr& properties)
{
	try {
		m_widgetProperties = dynamic_cast<const GCoderWidgetProperties&>(*properties);
	}  catch (std::bad_cast& e) {
		return;
	}

	m_capsLockEventFilter->setCapsLockEnable(m_widgetProperties.intCapsLock);

	if (m_widgetProperties.syntaxH) {
		if (m_highlighter == nullptr) {
			m_highlighter = new Highlighter(document());
			rehighlight();
		}
	} else {
		if (m_highlighter != nullptr) {
			delete (m_highlighter);
		}

		m_highlighter = nullptr;
	}
}

QString GCoderDocument::text(bool addCR) const
{
	QString text = document()->toPlainText();

	if (addCR && !text.contains(QLatin1String("\r\n"))) {
		text.replace(QLatin1String("\n"), QLatin1String("\r\n"));
	}

	return text;
}

void GCoderDocument::setText(const QString& text)
{
	document()->setPlainText(text);
}

void GCoderDocument::insertText(const QString& text)
{
	textEdit()->insertPlainText(text);
}

bool GCoderDocument::isReadOnly() const
{
	return textEdit()->isReadOnly();
}

void GCoderDocument::setReadOnly(bool ro)
{
	textEdit()->setReadOnly(ro);
}

void GCoderDocument::redo()
{
	document()->redo();
}

void GCoderDocument::undo()
{
	document()->undo();
}

void GCoderDocument::clearUndoRedoStacks()
{
	document()->clearUndoRedoStacks();
}

bool GCoderDocument::overwriteMode()
{
	return textEdit()->overwriteMode();
}

void GCoderDocument::centerCursor()
{
	textEdit()->centerCursor();
}

QString GCoderDocument::wordUnderCursor() const
{
	QTextCursor cursor = textEdit()->textCursor();
	cursor.select(QTextCursor::WordUnderCursor);
	return cursor.selectedText();
}

bool GCoderDocument::hasSelection() const
{
	return textCursor().hasSelection();
}

void GCoderDocument::selectAll()
{
	textEdit()->selectAll();
}

void GCoderDocument::clearSelection(bool toAnchor)
{
	QTextCursor cursor = textEdit()->textCursor();
	cursor.setPosition(toAnchor ? cursor.anchor() : cursor.position());
	setTextCursor(cursor);
}

QString GCoderDocument::selectedText() const
{
	return textEdit()->textCursor().selectedText();
}

void GCoderDocument::removeSelectedText()
{
	textEdit()->textCursor().removeSelectedText();
}

void GCoderDocument::clear()
{
	textEdit()->clear();
}

void GCoderDocument::copy()
{
	textEdit()->copy();
}

void GCoderDocument::cut()
{
	textEdit()->cut();
}

void GCoderDocument::paste()
{
	if (m_widgetProperties.underlineChanges) {
		QTextCharFormat format = textEdit()->currentCharFormat();
		format.setUnderlineStyle(QTextCharFormat::DotLine);
		format.setUnderlineColor(QColor(m_codeStyle.underlineColor));
		textEdit()->setCurrentCharFormat(format);
	}

	textEdit()->paste();
}

void GCoderDocument::print(QPrinter* printer)
{
#ifndef QT_NO_PRINTER
	document()->print(printer);
#endif
}

int GCoderDocument::currentLine() const
{
	return textCursor().block().firstLineNumber() + 1;
}

int GCoderDocument::currentColumn() const
{
	QTextCursor cursor = textCursor();
	return cursor.position() - cursor.block().position();
}

void GCoderDocument::goToLine(int line)
{
	QTextBlock block = document()->findBlockByNumber(line);
	setTextCursor(QTextCursor(block));
	centerCursor();
	widget()->setFocus();
}

QPlainTextEdit* GCoderDocument::textEdit() const
{
	return m_textEdit;
}

QTextDocument* GCoderDocument::document() const
{
	return textEdit()->document();
}

QTextCursor GCoderDocument::textCursor() const
{
	return textEdit()->textCursor();
}

void GCoderDocument::setTextCursor(const QTextCursor& cursor)
{
	textEdit()->setTextCursor(cursor);
}

void GCoderDocument::changeDateInComment()
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

void GCoderDocument::updateBrief()
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
		setBrief(fileName());
	} else {
		setBrief(f_tx.simplified());
	}
}

void GCoderDocument::updateWindowTitle()
{
	QString title = "";

	if ((m_widgetProperties.windowMode & SHOW_PROGTITLE)) {
		title = brief();
	}

	if (!title.isEmpty() && ((m_widgetProperties.windowMode & SHOW_FILEPATH)
	                         || (m_widgetProperties.windowMode & SHOW_FILENAME))) {
		title += " ---> ";
	}

	if ((m_widgetProperties.windowMode & SHOW_FILEPATH)) {
		title += path() + "/";
	}

	if ((m_widgetProperties.windowMode & SHOW_FILENAME) || title.isEmpty()) {
		title += fileName();
	}

	setWidgetTitle(title);
}

void GCoderDocument::updateToolTips()
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

void GCoderDocument::loadToolTips(QHash<QString, QString>& tips, const QString& fileName, const QString& group)
{
	if (QFile::exists(fileName)) {
		QSettings settings(fileName, QSettings::IniFormat);
		settings.beginGroup(group);
		const QStringList& keys = settings.childKeys();

		for (const QString& k : keys) {
			QString text = settings.value(k, "").toString();

			if (!text.isEmpty()) {
				tips.insert(k, text);
			} else {
				tips.remove(k);
			}
		}
	}
}

void GCoderDocument::setHighlightMode(int mod)
{
	if (m_highlightMode != mod) {
		m_highlightMode = mod;
		rehighlight();
		updateToolTips();
	}
}

int GCoderDocument::highlightMode() const
{
	return m_highlightMode;
}

void GCoderDocument::rehighlight()
{
	if (m_widgetProperties.syntaxH && m_highlighter) {
		m_highlighter->setHighlightMode(highlightMode());
		m_highlighter->setHColors(m_codeStyle.hColors, QFont(m_codeStyle.fontName, m_codeStyle.fontSize, QFont::Normal));
		m_highlighter->rehighlight();
	}
}

void GCoderDocument::detectHighlightMode()
{
	if (!m_widgetProperties.syntaxH || !m_highlighter) {
		return;
	}

	bool mod = document()->isModified();  // something below clears document modified state

	if (highlightMode() == MODE_AUTO) {
		setHighlightMode(autoDetectHighligthMode(text().toUpper()));
	}

	if (highlightMode() == MODE_AUTO) {
		setHighlightMode(m_widgetProperties.defaultHighlightMode);
	}

	document()->setModified(mod);
}

bool GCoderDocument::foundTextMatched(const QString& pattern, QString text)
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

bool GCoderDocument::findNext(QString textToFind,
                              bool wholeWords,
                              bool ignoreCase,
                              bool ignoreComments,
                              bool backward)
{
	bool found = false;
	QTextCursor cursor, cursorOld;

	if (textToFind.isEmpty()) {
		return false;
	}

	textEdit()->blockSignals(true);

	found = findText(textToFind, backward, wholeWords, ignoreCase, ignoreComments);

	if (!found) {
		cursor = textCursor();
		cursorOld = cursor;

		if (backward) {
			cursor.movePosition(QTextCursor::End);
		} else {
			cursor.movePosition(QTextCursor::Start);
		}

		setTextCursor(cursor);

		found = findText(textToFind, backward, wholeWords, ignoreCase, ignoreComments);

		if (!found) {
			cursorOld.clearSelection();
			setTextCursor(cursorOld);
		}
	}

	textEdit()->blockSignals(false);
	highlightCurrentLine();

	return found;
}

bool GCoderDocument::replaceNext(QString textToFind,
                                 QString replacedText,
                                 bool wholeWords,
                                 bool ignoreCase,
                                 bool ignoreComments,
                                 bool backward)
{
	QString foundText;
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
		found = findNext(textToFind, backward, wholeWords, ignoreCase, ignoreComments);
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
			double val = replacedText.toDouble(&ok);

			foundText = cr.selectedText();
			foundText.remove(QRegularExpression("[A-Za-z#]{1,}"));
			double val1 = foundText.toDouble(&ok);
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

		found = findNext(textToFind, backward, wholeWords, ignoreCase, ignoreComments);
	}

	textEdit()->blockSignals(false);
	highlightCurrentLine();
	highlightFindText(textToFind, backward, wholeWords, ignoreCase, ignoreComments);

	return found;
}

bool GCoderDocument::replaceAll(QString textToFind,
                                QString replacedText,
                                bool wholeWords,
                                bool ignoreCase,
                                bool ignoreComments,
                                bool backward)
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
		found = findNext(textToFind, backward, wholeWords, ignoreCase, ignoreComments);
	}

	QTextCursor startCursor = textCursor();

	while (found) {
		found = replaceNext(textToFind, replacedText, backward, wholeWords, ignoreCase, ignoreComments);

		if (startCursor.blockNumber() == textCursor().blockNumber()) {
			break;
		}

		qApp->processEvents();
	}

	return found;
}

bool GCoderDocument::findText(const QString& text, bool findBackward, bool wholeWords, bool ignoreCase,
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

	if (!ignoreCase) {
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
		QTextDocument::FindFlags options{};
		options = (findBackward ? QTextDocument::FindBackward : options) |
		          (wholeWords ? QTextDocument::FindWholeWords : options) |
		          (!ignoreCase ? QTextDocument::FindCaseSensitively : options);

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
			int id = highlightMode();
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

void GCoderDocument::underLine()
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

void GCoderDocument::highlightFindText(const QString& searchString,
                                       bool wholeWords,
                                       bool ignoreCase,
                                       bool ignoreComments,
                                       bool backward)
{
	QList<QTextEdit::ExtraSelection> tmpSelections;
	bool inComment;
	bool isRegExp = false;
	bool isRegExpMinMax = false;
	QRegularExpression regex;
	double min = 0;
	double max = 0;
	Qt::CaseSensitivity caseSensitivity;

	tmpSelections.clear();
	m_findTextExtraSelections.clear();
	tmpSelections.append(m_extraSelections);
	QColor lineColor = QColor(Qt::yellow).lighter(155);
	m_selection.format.setBackground(lineColor);

	QTextDocument* doc = document();
	QTextCursor cursor = textCursor();
	cursor.setPosition(0);

	QString addr = searchString;

	if (!ignoreCase) {
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
		QTextDocument::FindFlags options{};
		options = (backward ? QTextDocument::FindBackward : options) |
		          (wholeWords ? QTextDocument::FindWholeWords : options) |
		          (!ignoreCase ? QTextDocument::FindCaseSensitively : options);

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

			if (ignoreComments) {
				int id = highlightMode();
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

			if (!inComment) {
				if (isRegExpMinMax) {
					QString sval = cursor.selectedText();
					bool ok;
					double val = QString(sval).remove(addr, caseSensitivity).toDouble(&ok);

					if ((val >= min) && (val <= max)) {
						m_selection.cursor = cursor;
						m_findTextExtraSelections.append(m_selection);
					}
				} else {
					m_selection.cursor = cursor;
					m_findTextExtraSelections.append(m_selection);
				}
			}
		}
	} while (!cursor.isNull());

	tmpSelections.append(m_findTextExtraSelections);
	textEdit()->setExtraSelections(tmpSelections);
}

void GCoderDocument::highlightCurrentLine()
{
	QString openBrace;
	QString closeBrace;
	bool proceed;
	QList<QTextEdit::ExtraSelection> tmpSelections;
	QTextDocument::FindFlags findOptions;

	tmpSelections.clear();
	m_extraSelections.clear();
	tmpSelections.append(m_blockExtraSelections);
	tmpSelections.append(m_findTextExtraSelections);
	textEdit()->setExtraSelections(tmpSelections);

	if (!isReadOnly()) {
		m_selection.format.setBackground(QColor(m_codeStyle.lineColor));
		m_selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		m_selection.cursor = textCursor();
		m_selection.cursor.clearSelection();
		m_extraSelections.append(m_selection);
	}

	QColor lineColor = QColor(m_codeStyle.lineColor).darker(108);
	m_selection.format.setBackground(lineColor);

	QTextDocument* doc = document();
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
		tmpSelections.append(m_extraSelections);
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
		m_selection.cursor = cursor;
		m_extraSelections.append(m_selection);
		QTextCursor cursor1 = doc->find(QLatin1String("\""), cursor);

		if (!cursor1.isNull() && (cursor1 != cursor)) {
			m_selection.cursor = cursor1;
			m_extraSelections.append(m_selection);
		} else {
			QTextCursor cursor2 = doc->find(QLatin1String("\""), cursor, QTextDocument::FindBackward);

			if (!cursor2.isNull()) {
				m_selection.cursor = cursor2;
				m_extraSelections.append(m_selection);
			}
		}

		tmpSelections.append(m_extraSelections);
		textEdit()->setExtraSelections(tmpSelections);
		return;
	}

	if (brace == openBrace) {
		QTextCursor cursor1 = doc->find(closeBrace, cursor, findOptions);
		QTextCursor cursor2 = doc->find(openBrace, cursor, findOptions);

		if (cursor2.isNull()) {
			m_selection.cursor = cursor;
			m_extraSelections.append(m_selection);
			m_selection.cursor = cursor1;
			m_extraSelections.append(m_selection);
		} else {

			while (cursor1.position() > cursor2.position()) {
				cursor1 = doc->find(closeBrace, cursor1, findOptions);
				cursor2 = doc->find(openBrace, cursor2, findOptions);

				if (cursor2.isNull()) {
					break;
				}
			}

			m_selection.cursor = cursor;
			m_extraSelections.append(m_selection);
			m_selection.cursor = cursor1;
			m_extraSelections.append(m_selection);
		}
	} else {
		if (brace == closeBrace) {
			QTextCursor cursor1 = doc->find(openBrace, cursor, QTextDocument::FindBackward | findOptions);
			QTextCursor cursor2 = doc->find(closeBrace, cursor, QTextDocument::FindBackward | findOptions);

			if (cursor2.isNull()) {
				m_selection.cursor = cursor;
				m_extraSelections.append(m_selection);
				m_selection.cursor = cursor1;
				m_extraSelections.append(m_selection);
			} else {
				while (cursor1.position() < cursor2.position()) {
					cursor1 = doc->find(openBrace, cursor1, QTextDocument::FindBackward | findOptions);
					cursor2 = doc->find(closeBrace, cursor2, QTextDocument::FindBackward | findOptions);

					if (cursor2.isNull()) {
						break;
					}
				}

				m_selection.cursor = cursor;
				m_extraSelections.append(m_selection);
				m_selection.cursor = cursor1;
				m_extraSelections.append(m_selection);
			}
		}
	}

	tmpSelections.append(m_extraSelections);
	textEdit()->setExtraSelections(tmpSelections);
}

void GCoderDocument::showInLineCalc()
{
	QString value = selectedText();
	QString address;

	if (!value.isEmpty() && value.at(0).isLetter()) {
		address = value.at(0);
		value.remove(address);
		value.remove(" ");
	} else {
		value.clear();
	}

	m_inLineCalc->showCalc(address, value, textEdit()->cursorRect());
}

void GCoderDocument::inLineCalcComplete(const QString& text)
{
	insertText(text);
}
