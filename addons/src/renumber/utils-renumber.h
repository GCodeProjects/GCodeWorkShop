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

#ifndef UTILS_REMOVEBYREGEX_H
#define UTILS_REMOVEBYREGEX_H

class QString;

struct RenumberOptions;


namespace Utils {
void renumber(const RenumberOptions &opt, QString &tx);
void renumberWithoutN(const RenumberOptions &opt, QString &tx, int lineCount);
void renumberWithN(const RenumberOptions &opt, QString &tx);
void renumberAll(const RenumberOptions &opt, QString &tx, int lineCount);
void removeAll(QString &tx);
}

#endif // UTILSREMOVEBY_REGEX_H
