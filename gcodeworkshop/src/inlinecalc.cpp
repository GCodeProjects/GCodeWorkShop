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

#include <QCompleter>   // for QCompleter
#include <QKeyEvent>    // for QKeyEvent
#include <QLineEdit>    // for QLineEdit
#include <QRect>        // for QRect
#include <Qt>           // for CaseSensitivity, Key
#include <QWidget>      // for QWidget

#include <utils/expressionparser.h> // for processBrc
#include <utils/removezeros.h>      // for removeZeros

#include "inlinecalc.h"


InLineCalc::InLineCalc(QWidget* parent) : QLineEdit(parent)
{
	setHidden(true);
	setClearButtonEnabled(true);
	setMinimumWidth(100);
	adjustSize();
	setToolTip(tr("You can use:\n") +
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

	connect(this, SIGNAL(editingFinished()), this, SLOT(done()));

	QCompleter* completer = new QCompleter(m_wordList, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	setCompleter(completer);
}

void InLineCalc::showCalc(const QString& address, const QString& value, const QRect& cursorRect)
{
	m_address = address;
	setText(value);

	int h = (height() - cursorRect.height()) / 2;
	move(cursorRect.x() + cursorRect.height(), cursorRect.top() - h);
	show();
	setFocus();
}

void InLineCalc::keyPressEvent(QKeyEvent* event)
{
	if (event->key() != Qt::Key_Return) {
		QLineEdit::keyPressEvent(event);
	} else {
		event->accept();
		evalute();
		done();
	}
}

void InLineCalc::evalute()
{
	QString result = text();
	result.replace(',', '.');

	m_wordList.append(result);
	m_wordList.removeDuplicates();

	int status = Utils::processBrc(&result);

	if (status >= 0) {
		emit complete(Utils::removeZeros(result).prepend(m_address));
	}
}

void InLineCalc::done()
{
	close();
	parentWidget()->setFocus();
}
