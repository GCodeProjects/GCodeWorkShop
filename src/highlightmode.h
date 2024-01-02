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

#ifndef HIGHLIGHTMODE_H
#define HIGHLIGHTMODE_H

enum HighlightMode {
    MODE_AUTO,
    MODE_OKUMA,
    MODE_FANUC,
    MODE_HEIDENHAIN,
    MODE_SINUMERIK,
    MODE_SINUMERIK_840,
    MODE_PHILIPS,
    MODE_HEIDENHAIN_ISO,
    MODE_TOOLTIPS,
    MODE_LINUXCNC
};

#endif // HIGHLIGHTMODE_H
