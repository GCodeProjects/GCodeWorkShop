/*
 *  Copyright (C) 2023-2025 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of GCodeWorkShop.
 *
 *  GCodeWorkShop is free software: you can redistribute it and/or modify
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

#include <functional>   // for function

class QString;

struct RenumberOptions;


namespace Utils {

bool renumber(QString& text, const RenumberOptions& opt, const std::function<bool(int)>& interrupt);
bool renumberWithoutN(QString& line, int num, const RenumberOptions& opt);
bool renumberWithN(QString& line, int num, const RenumberOptions& opt);
bool renumberAll(QString& line, int num, const RenumberOptions& opt);
bool renumberRemoveAll(QString& line);

} // namespace Utils

#endif // UTILSREMOVEBY_REGEX_H
