/*
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
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

#include <algorithm>    // for max, min

#include <QChar>            // for QChar
#include <QPlainTextEdit>   // for QPlainTextEdit
#include <QTextCursor>      // for QTextCursor, QTextCursor::KeepAnchor, QTextCursor::MoveAnchor, QTextCursor::End

#include <document.h>       // for Document
#include <edytornc.h>       // for EdytorNc
#include <gcoderdocument.h> //

#include "addons-context.h"


bool Addons::Context::pull(int mode)
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(EdytorNc::instance()->activeDocument());

	if (!gdoc) {
		return false;
	}

	m_mode = mode;
	m_edit = gdoc->textEdit();
	QTextCursor cursor(m_edit->document());
	m_selectionStart = m_edit->textCursor().selectionStart();
	m_selectionEnd = m_edit->textCursor().selectionEnd();
	m_fragmentStart = std::min(m_selectionStart, m_selectionEnd);
	m_fragmentEnd = std::max(m_selectionStart, m_selectionEnd);

	switch (m_mode) {
	case Addons::Context::SELECTED:
		if (m_fragmentStart == m_fragmentEnd) {
			return false;
		}

		cursor.setPosition(m_fragmentEnd, QTextCursor::MoveAnchor);
		cursor.setPosition(m_fragmentStart, QTextCursor::KeepAnchor);
		break;

	case Addons::Context::SELECTED_OR_ALL:
		if (m_fragmentStart == m_fragmentEnd) {
			cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
			cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
		} else {
			cursor.setPosition(m_fragmentEnd, QTextCursor::MoveAnchor);
			cursor.setPosition(m_fragmentStart, QTextCursor::KeepAnchor);
		}

		break;

	case Addons::Context::SELECTED_BLOCKS:
		cursor.setPosition(m_fragmentEnd, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
		cursor.setPosition(m_fragmentStart, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);

		break;

	case Addons::Context::ALL:
		cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
		break;

	default:
		return false;
	}

	m_fragmentStart = cursor.position();
	m_fragmentEnd = cursor.anchor();
	m_text = cursor.selectedText();
	m_text.replace(QChar(0x2029), '\n');

	return true;
}

void Addons::Context::push(const QString& text)
{
	QTextCursor cursor(m_edit->document());
	cursor.setPosition(m_fragmentEnd, QTextCursor::MoveAnchor);
	cursor.setPosition(m_fragmentStart, QTextCursor::KeepAnchor);
	cursor.insertText(text);
	cursor.setPosition(m_selectionEnd, QTextCursor::MoveAnchor);
	cursor.setPosition(m_selectionStart, QTextCursor::KeepAnchor);
	m_edit->setTextCursor(cursor);
}

void Addons::Context::push()
{
	push(m_text);
}

