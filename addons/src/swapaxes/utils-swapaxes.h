/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
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

#ifndef UTILS_SWAPAXES_H
#define UTILS_SWAPAXES_H

#include <functional>   // for function

#include <QString>	// for QString

struct SwapAxesOptions;


namespace Utils {
bool swapAxes(QString& text, const SwapAxesOptions& opt, const std::function<bool (int)>& interrupt);

bool swapAxes(QString& text,
              const QString& textToFind,
              const QString& replacedText,
              bool checkBound,
              double min,
              double max,
              int oper,
              double modifier,
              int prec,
              const std::function<bool(int)>& interrupt);
}

#endif // UTILS_SWAPAXES_H
