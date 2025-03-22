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

#ifndef UTILS_COMMENT_H
#define UTILS_COMMENT_H

#include <functional>   // for function

class QString;


namespace Utils {

enum CommentMode {
	ParenthesisComments,
	SemicolonComments
};

bool autoComments(QString& text, int mode, const std::function<bool(int)>& interrupt);
bool parenthesisComments(QString& line, bool remove);
bool semicolonComments(QString& line, bool remove);

} // namespace Utils

#endif // UTILS_COMMENT_H
