/*
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This program is free software: you can redistribute it and/or modify
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

#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

#include <QtGlobal> // QT_VERSION QT_VERSION_CHECK
#include <QFontMetrics>

inline int fontWidth(const QFontMetrics &fm)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    return fm.width('W');
#else
    return fm.horizontalAdvance('W');
#endif
}

inline bool isFontFixed(const QFontMetrics &fm)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    return fm.width('W') == fm.width('i');
#else
    return fm.horizontalAdvance('W') == fm.horizontalAdvance('i');
#endif
}

#endif // COMPATIBILITY_H
