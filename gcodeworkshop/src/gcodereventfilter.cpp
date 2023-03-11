/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of GCodeWorkShop.
 *
 *  GCodeWorkShop is free software: you can redistribute it and/or modify
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

#include <QChar>                    // for QChar, operator==, operator!=
#include <QDragEnterEvent>          // for QDragEnterEvent
#include <QDropEvent>               // for QDropEvent
#include <QEvent>                   // for QEvent, QEvent::KeyPress, QEvent::MouseButtonDblClick, QEvent::ToolTip
#include <QHelpEvent>               // for QHelpEvent
#include <QKeyEvent>                // for QKeyEvent
#include <QLatin1Char>              // for QLatin1Char
#include <QLatin1String>            // for QLatin1String
#include <QMimeData>                // for QMimeData
#include <QMouseEvent>              // for QMouseEvent
#include <QPlainTextEdit>           // for QPlainTextEdit
#include <QRect>                    // for QRect
#include <QRegularExpression>       // for QRegularExpression
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QTextCursor>              // for QTextCursor, QTextCursor::KeepAnchor, QTextCursor::NextCharacter, QTextCu...
#include <QToolTip>                 // for QToolTip
#include <QWidget>                  // for QWidget
#include <Qt>                       // for KeyboardModifiers, NoModifier, ShiftModifier, ControlModifier, Key_Comma

#include "gcodereventfilter.h"


GCoderEventFilter::GCoderEventFilter(QPlainTextEdit* textEdit, QObject* parent) : QObject(parent),
	m_toolTipRegex("(,[a-zA-Z]|#|@|[a-zA-Z]+(\\d+=)?)([-+]?\\d*(\\.\\d*)?)")
{
	m_textEdit = textEdit;
	m_toolTipEnable = true;
	m_toolTipRegex = QRegularExpression("(,[a-zA-Z]|#|@|[a-zA-Z]+(\\d+=)?)([-+]?\\d*(\\.\\d*)?)");
}

bool GCoderEventFilter::isToolTipEnable() const
{
	return m_toolTipEnable;
}

void GCoderEventFilter::setToolTipEnable(bool enable)
{
	m_toolTipEnable = enable;
}

void GCoderEventFilter::setToolTipRegExp(const QRegularExpression& regexp)
{
	m_toolTipRegex = regexp;
}

QHash<QString, QString> GCoderEventFilter::tips() const
{
	return m_tips;
}

void GCoderEventFilter::setTips(const QHash<QString, QString>& tips)
{
	m_tips = tips;
}

bool GCoderEventFilter::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == m_textEdit && event->type() == QEvent::ToolTip && m_toolTipEnable) {
		QHelpEvent* he = dynamic_cast<QHelpEvent*>(event);
		QWidget* widget = dynamic_cast<QWidget*>(obj);
		return toolTipEvent(widget, he);
	}

	if (obj == m_textEdit->viewport() && event->type() == QEvent::MouseButtonDblClick) {
		QMouseEvent* me = dynamic_cast<QMouseEvent*>(event);
		return mouseButtonDblClickEvent(me);
	}

	if (obj == m_textEdit && event->type() == QEvent::KeyPress) {
		QKeyEvent* ke = dynamic_cast<QKeyEvent*>(event);
		return keyEvent(ke);
	}

	if (event->type() == QEvent::DragEnter) {
		QDragEnterEvent* dee = dynamic_cast<QDragEnterEvent*>(event);
		return dragEnterEvent(dee);
	}

	if (event->type() == QEvent::Drop) {
		QDropEvent* de = dynamic_cast<QDropEvent*>(event);
		return dropEvent(de);
	}

	return false;
}

bool GCoderEventFilter::toolTipEvent(QWidget* widget, QHelpEvent* event)
{
	QString word = wordForPosition(event->pos());

	if (word.length() == 2) {
		if ((word.at(0) == QLatin1Char('G')) || (word.at(0) == QLatin1Char('M')))
			if (!word.at(1).isLetter()) {
				word.insert(1, "0");
			}
	}

	QString text = m_tips.value(word, QString());

	if (!text.isEmpty()) {
		QString header = QLatin1String("<p style='white-space:pre'>");

		if (text.length() > 128) {
			header = QLatin1String("<p style='white-space:normal'>");
		}

		QToolTip::showText(event->globalPos(), header + text, widget, QRect());
	} else {
		QToolTip::hideText();
		event->ignore();
	}

	return true;
}

QString GCoderEventFilter::wordForPosition(const QPoint& pos) const
{
	QTextCursor cursor = m_textEdit->cursorForPosition(pos);
	int positionInBlock = cursor.positionInBlock();
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
	QString text = cursor.selectedText();
	QRegularExpressionMatch match = m_toolTipRegex.match(text);

	while (match.hasMatch()) {
		if (match.capturedStart() < positionInBlock && match.capturedEnd() > positionInBlock) {
			return match.captured().simplified();
		}

		match = m_toolTipRegex.match(text, match.capturedEnd());
	}

	return QString();
}

// Better word selection
bool GCoderEventFilter::mouseButtonDblClickEvent(QMouseEvent* event)
{
	QString key;
	QString wordDelimiters = "()[]=,;:/ ";
	bool wasLetter = false;
	int posStart, posEnd;
	QTextCursor cursor = m_textEdit->textCursor();

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
	m_textEdit->setTextCursor(cursor);

	if (event->modifiers() == Qt::ControlModifier) {
		emit requestInLineCalc();
	}

	return true;
}

bool GCoderEventFilter::keyEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Insert) {
		m_textEdit->setOverwriteMode(!m_textEdit->overwriteMode());
	}

	if (!event->text().isEmpty() && event->text()[0].isPrint() && !event->text()[0].isSpace()) {
		emit requestUnderLine();
	}

	return false;
}

bool GCoderEventFilter::dragEnterEvent(QDragEnterEvent* event)
{
	return event->mimeData()->hasUrls();
}

bool GCoderEventFilter::dropEvent(QDropEvent* event)
{
	return event->mimeData()->hasUrls();
}
