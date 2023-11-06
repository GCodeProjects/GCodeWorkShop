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

#ifndef UTILS_EXPRESSIONPARSER_H
#define UTILS_EXPRESSIONPARSER_H

class QString;

#define ERR_NO_BRAC       -1
#define ERR_NO_PARAM      -2
#define ERR_CONVERT       -3
#define ERR_UNKNOWN_FUNC  -4
#define ERR_DOUBLE_DOT    -5


namespace Utils {
int processBrc(QString *str);
int compute(QString *str);
}

#endif // UTILS_EXPRESSIONPARSER_H
