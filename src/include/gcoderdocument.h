/*
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

#ifndef GCODERDOCUMENT_H
#define GCODERDOCUMENT_H

#include <QByteArray>       // for QByteArray
#include <QList>            // for QList
#include <QObject>          // for slots, signals, Q_OBJECT
#include <QString>          // for QString
#include <QTextCursor>      // for QTextCursor
#include <QTextEdit>        // for QTextEdit::ExtraSelection, QTextEdit

class QCloseEvent;
template <class Key, class T> class QHash;
class QMenu;
class QPlainTextEdit;
class QPoint;
class QPrinter;
class QTextDocument;

#include <document.h>
#include <documentinfo.h>              // for DocumentInfo, DocumentInfo::Ptr
#include <documentstyle.h>             // for DocumentStyle, DocumentStyle::Ptr
#include <documentwidgetproperties.h>  // for DocumentWidgetProperties, DocumentWidgetProperties::Ptr
#include <gcoderstyle.h>
#include <gcoderwidgetproperties.h>

class CapsLockEventFilter;
class GCoderEventFilter;
class Highlighter;
class InLineCalc;


class GCoderDocument : public Document
{
	Q_OBJECT

public:
	GCoderDocument();

	QString type() const override;
	QString guessFileName() const override;

	void loadTemplate(const QString &fileName = QString()) override;
	bool load() override;
	bool save() override;

	QByteArray rawData() const override;
	void setRawData(const QByteArray &data) override;

	QMenu *createStandardContextMenu(const QPoint &pos) override;

	DocumentInfo::Ptr documentInfo() const override;
	void setDocumentInfo(const DocumentInfo::Ptr &info) override;

	DocumentStyle::Ptr documentStyle() const override;
	void setDocumentStyle(const DocumentStyle::Ptr &style) override;

	DocumentWidgetProperties::Ptr documentWidgetProperties() const override;
	void setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr &properties) override;

	void redo() override;
	void undo() override;
	void clearUndoRedoStacks() override;

	bool isReadOnly() const override;
	void setReadOnly(bool ro) override;

	QString text(bool addCR = false) const;
	void setText(const QString &text);

	bool hasSelection() const;
	void selectAll();
	void clearSelection(bool toAnchor = false);
	QString selectedText() const;
	void insertText(const QString &text);
	void cut();
	void copy();
	void paste();

	void setHighlightMode(int mod);
	int highlightMode() const;
	bool foundTextMatched(const QString &pattern, QString text);
	bool findNext(QString textToFind,
	              bool wholeWords = false,
	              bool ignoreCase = true,
	              bool ignoreComments = true,
	              bool backward = false);
	bool replaceNext(QString textToFind,
	                 QString replacedText,
	                 bool wholeWords = false,
	                 bool ignoreCase = true,
	                 bool ignoreComments = true,
	                 bool backward = false);
	bool replaceAll(QString textToFind,
	                QString replacedText,
	                bool wholeWords = false,
	                bool ignoreCase = true,
	                bool ignoreComments = true,
	                bool backward = false);
	void highlightFindText(const QString &searchString,
	                       bool wholeWords = false,
	                       bool ignoreCase = true,
	                       bool ignoreComments = true,
	                       bool backward = false);
	bool overwriteMode();
	void centerCursor();
	QString wordUnderCursor() const;
	void removeSelectedText();
	void clear();
	void print(QPrinter *printer);
	int currentLine() const;
	int currentColumn() const;
	void goToLine(int line);

	QPlainTextEdit *textEdit() const;

public slots:
	void showInLineCalc();

protected:
	QPlainTextEdit *m_textEdit;
	Highlighter *m_highlighter;
	int m_highlightMode;
	int m_preLoadCursorPosition;
	GCoderStyle m_codeStyle;
	GCoderWidgetProperties m_widgetProperties;
	CapsLockEventFilter *m_capsLockEventFilter;
	GCoderEventFilter *m_gCoderEventFilter;
	QList<QTextEdit::ExtraSelection> m_extraSelections;
	QList<QTextEdit::ExtraSelection> m_findTextExtraSelections;
	QTextEdit::ExtraSelection m_selection;
	QList<QTextEdit::ExtraSelection> m_blockExtraSelections;
	InLineCalc *m_inLineCalc;

	QTextDocument *document() const;
	QTextCursor textCursor() const;
	void setTextCursor(const QTextCursor &cursor);
	void changeDateInComment();
	void closeEvent(QCloseEvent *event);
	void updateBrief();
	void updateWindowTitle();
	void updateToolTips();
	void loadToolTips(QHash<QString, QString> &tips, const QString &fileName, const QString &group);
	bool maybeSave();
	void rehighlight();
	void detectHighlightMode();
	bool findText(const QString &text,  bool findBackward, bool wholeWords, bool ignoreCase, bool ignoreComments = true);

private slots :
	void underLine();
	void highlightCurrentLine();
	void inLineCalcComplete(const QString &text);
};

#endif // GCODERDOCUMENT_H
