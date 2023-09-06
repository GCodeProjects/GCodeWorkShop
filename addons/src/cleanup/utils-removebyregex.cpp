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

#include <QRegularExpression>   // for QRegularExpression
#include <QString>              // for QString
#include <QStringList>          // for QStringList

#include "utils-removebyregex.h"


bool Utils::removeTextByRegExp(QString &tx, QStringList expList, bool replaceDollar)
{
    if (expList.isEmpty()) {
        return false;
    }

    for (QString regexp : expList) {
        if (replaceDollar && regexp.contains('$') && !regexp.contains("\\$")) {
            regexp.replace('$', "\\n");
        }

        tx.remove(QRegularExpression(regexp));
    }

    return true;
}
