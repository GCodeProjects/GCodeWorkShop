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

#ifndef CAPSLOCKEVENTFILTER_H
#define CAPSLOCKEVENTFILTER_H

#include <QObject>  // for QObject, Q_OBJECT, slots
#include <QString>  // for QString

class QEvent;
class QKeyEvent;


class CapsLockEventFilter : public QObject
{
	Q_OBJECT

public:
	explicit CapsLockEventFilter(QObject* parent = nullptr);

public slots:
	void setCapsLockEnable(bool enable);

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
	bool keyEvent(QKeyEvent* event);

protected:
	bool m_capsLockEnable;
};

#endif // CAPSLOCKEVENTFILTER_H
