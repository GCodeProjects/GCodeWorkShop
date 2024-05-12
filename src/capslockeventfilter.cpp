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

#include <QApplication> //for QApplication
#include <QChar>        // for QChar
#include <QEvent>       // for QEvent, QEvent::KeyPress
#include <QKeyEvent>    // for QKeyEvent
#include <Qt>           // for KeyboardModifier, Key
#include <QtGlobal>     // for Q_UNUSED

#include "capslockeventfilter.h"


CapsLockEventFilter::CapsLockEventFilter(QObject* parent)
	: QObject{parent}
{
	m_capsLockEnable = false;
}

void CapsLockEventFilter::setCapsLockEnable(bool enable)
{
	m_capsLockEnable = enable;
}

bool CapsLockEventFilter::eventFilter(QObject* obj, QEvent* event)
{
	Q_UNUSED(obj);

	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* ke = dynamic_cast<QKeyEvent*>(event);
		return keyEvent(ke);
	}

	return false;
}

bool CapsLockEventFilter::keyEvent(QKeyEvent* event)
{
	//Keypad comma should always prints period
	if (event->key() == Qt::Key_Comma) {
		// !!! Qt::KeypadModifier - Not working for keypad comma !!!
		if ((event->modifiers() == Qt::KeypadModifier) || (event->nativeScanCode() == 0x53)) {
			QApplication::sendEvent(parent(), new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, ".", false, 1));
			return true;
		}
	}

	if (m_capsLockEnable && !event->text().isEmpty()) {
		if (event->text().at(0).isLower() && (event->modifiers() == Qt::NoModifier)) {
			QApplication::sendEvent(parent(), new QKeyEvent(QEvent::KeyPress, event->key(), Qt::NoModifier, event->text().toUpper(),
			                        false, 1));
			return true;
		}

		if (event->text().at(0).isUpper() && (event->modifiers() == Qt::ShiftModifier)) {
			QApplication::sendEvent(parent(), new QKeyEvent(QEvent::KeyPress, event->key(), Qt::ShiftModifier,
			                        event->text().toLower(), false, 1));
			return true;
		}
	}

	return false;
}
