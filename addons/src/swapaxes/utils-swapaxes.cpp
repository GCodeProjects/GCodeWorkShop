/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
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

#include <QChar>                    // for operator==, QChar
#include <QColor>                   // for QColor
#include <QLatin1Char>              // for QLatin1Char
#include <QPlainTextEdit>           // for QPlainTextEdit
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QString>                  // for QString, operator+, operator!=, QCharRef
#include <QTextBlock>               // for QTextBlock
#include <QTextCharFormat>          // for QTextCharFormat, QTextCharFormat::DotLine
#include <QTextCursor>              // for QTextCursor, QTextCursor::Document, QTextCursor::KeepAnchor, QTextCursor::M...
#include <QTextDocument>            // for QTextDocument, QTextDocument::FindFlags, QTextDocument::FindCaseSensitively
#include <QTextDocumentFragment>    // for QTextDocumentFragment
#include <QtGlobal>                 // for QFlags

#include <utils/removezeros.h>  // for Utils::removeZeros

#include "swapaxesoptions.h"
#include "utils-swapaxes.h"


void Utils::swapAxes(QPlainTextEdit* textEdit,
                     bool highlightChanges,
                     int highlightColor,
                     int commentId,
                     const SwapAxesOptions& opt)
{
	QString textToFind = opt.from;
	QString replacedText = opt.to;
	double min = opt.limit.enable ? opt.limit.min : -999999;
	double max = opt.limit.enable ? opt.limit.max : 999999;
	int oper = opt.convert.enable ? opt.convert.operation : -1;
	double modifier = opt.convert.enable ? opt.convert.value : 0;
	int precision = opt.precision;
	bool ignoreComments = true;
	QTextDocument::FindFlags findOptions = QTextDocument::FindFlags();

	// Original code from MdiChild::doSwapAxes()
	QTextCursor startCursor = textEdit->textCursor();
	startCursor.beginEditBlock();

	if (textToFind != replacedText) {
		Utils::swapAxes(textEdit, highlightChanges, highlightColor, commentId,
		                replacedText, QString("~%1").arg(replacedText), min, max, -1, modifier, findOptions,
		                ignoreComments, precision);
		Utils::swapAxes(textEdit, highlightChanges, highlightColor, commentId,
		                textToFind, replacedText, min, max, oper, modifier, findOptions, ignoreComments, precision);
		Utils::swapAxes(textEdit, highlightChanges, highlightColor, commentId,
		                QString("~%1").arg(replacedText), textToFind, -999999, 0, -1, 0, findOptions, ignoreComments,
		                precision);
	} else {
		Utils::swapAxes(textEdit, highlightChanges, highlightColor, commentId,
		                textToFind, replacedText, min, max, oper, modifier, findOptions, ignoreComments, precision);
	}

	startCursor.movePosition(QTextCursor::StartOfLine);
	startCursor.endEditBlock();
	textEdit->setTextCursor(startCursor);
}

bool Utils::swapAxes(QPlainTextEdit* textEdit,
                     bool underlineChanges,
                     int underlineColor,
                     int commentId,
                     const QString& textToFind,
                     const QString& replacedText,
                     double min,
                     double max,
                     int oper,
                     double modifier,
                     QTextDocument::FindFlags options,
                     bool ignoreComments,
                     int prec)
{
	double val, val1;
	QRegularExpression regex;
	bool found = false;
	bool ok, inSelection;
	QString newText, foundText;
	bool inComment;
	int commentPos;
	QTextDocument* document;
	int cursorStart, cursorEnd;

	cursorStart = 0;
	cursorEnd = 0;

	if (textEdit->isReadOnly()) {
		return false;
	}

	if (textToFind.isEmpty()) {
		return false;
	}

	if (!(options & QTextDocument::FindCaseSensitively)) {
		regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	}

	textEdit->blockSignals(true);

	inSelection = textEdit->textCursor().hasSelection();

	if (inSelection) {
		cursorStart = textEdit->textCursor().selectionStart();
		cursorEnd = textEdit->textCursor().selectionEnd();
		document = new QTextDocument(textEdit->textCursor().selectedText(), textEdit);
	} else {
		document = textEdit->document();
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

			if (commentId == COMMENT_ID_SEMYCOLON) {
				commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
			} else if (commentId == COMMENT_ID_BRACES) {
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
					newText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', prec));

					if (newText[newText.length() - 1] == '.') {
						newText = newText.remove((newText.length() - 1), 1);
					}
				} else {
					newText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', prec));
				}
			} else {
				newText = replacedText;
			}


			if (underlineChanges) {
				QTextCharFormat format = cursor.charFormat();
				format.setUnderlineStyle(QTextCharFormat::DotLine);
				format.setUnderlineColor(QColor(underlineColor));
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
		textEdit->textCursor().insertFragment(cursor.selection());
		delete (document);

		cursor = textEdit->textCursor();  //restore selection
		cursor.setPosition(cursorStart, QTextCursor::MoveAnchor);
		cursor.setPosition(cursorEnd, QTextCursor::KeepAnchor);
		textEdit->setTextCursor(cursor);
	}

	textEdit->blockSignals(false);

	return found;
}
