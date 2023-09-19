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

#ifndef UTILS_SWAPAXES_H
#define UTILS_SWAPAXES_H

#include <QString>          // for QString
#include <QTextDocument>    // for QTextDocument, QTextDocument::FindFlags

class QPlainTextEdit;

struct SwapAxesOptions;


enum {
    COMMENT_ID_UNKNOWN,
    COMMENT_ID_BRACES,
    COMMENT_ID_SEMYCOLON
};


namespace Utils {
void swapAxes(QPlainTextEdit *textEdit,
              bool highlightChanges,
              int highlightColor,
              int commentId,
              const SwapAxesOptions &opt);

bool swapAxes(QPlainTextEdit *textEdit,
              bool underlineChanges,
              int underlineColor,
              int commentId,
              const QString &textToFind,
              const QString &replacedText,
              double min,
              double max,
              int oper,
              double modifier,
              QTextDocument::FindFlags options,
              bool ignoreComments,
              int prec);
}

#endif // UTILS_SWAPAXES_H
