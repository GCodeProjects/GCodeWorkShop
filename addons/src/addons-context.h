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

#ifndef ADDONS_CONTEXT_H
#define ADDONS_CONTEXT_H

#include <QString>

class QPlainTextEdit;


namespace Addons {
class Context
{
public:
	enum Mode {
		// Only selected text.
		SELECTED,
		// From the start of the first selected block to the end of the last
		// selected block; or the block in which the cursor is placed.
		SELECTED_BLOCKS,
		// Selected or whole text.
		SELECTED_OR_ALL,
		// Whole text.
		ALL
	};

    // *INDENT-OFF*
    bool pull(int mode);
    QString &text() {return m_text;}
    int fragmentStart() {return m_fragmentStart;}
    int fragmentEnd() {return m_fragmentEnd;}
    void setSelectionStart(int pos) {m_selectionStart = pos;}
    void setSelectionEnd(int pos) {m_selectionEnd = pos;}
    void push(const QString &text);
    void push();
    QPlainTextEdit *textEdit()  {return m_edit;}
    // *INDENT-ON*

protected:
	int m_mode;
	QPlainTextEdit *m_edit;
	int m_selectionStart;   // QPlainTextEdit::selectionStart()
	int m_selectionEnd;     // QPlainTextEdit::selectionEnd()
	int m_fragmentStart;
	int m_fragmentEnd;
	QString m_text;
};
} // namespace Addons

#endif // ADDONS_CONTEXT_H
