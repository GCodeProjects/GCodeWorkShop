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

#include <QLatin1String>        // for QLatin1String
#include <QRegularExpression>   // for QRegularExpression
#include <QString>              // for QString

#include "utils-emptylines.h"


void Utils::insertEmptyLines(QString& tx)
{
	if (tx.contains(QLatin1String("\r\n"))) {
		tx.replace(QLatin1String("\r\n"), QLatin1String("\r\n\r\n"));
	} else {
		tx.replace(QLatin1String("\n"), QLatin1String("\n\n"));
	}
}

void Utils::removeEmptyLines(QString& tx)
{
	int i;
	QRegularExpression regex;

	regex.setPattern("[\\n]{2,}");
	i = 0;

	while (i >= 0) {
		i = tx.indexOf(regex, 0);

		if (i >= 0) {
			tx.replace(regex, "\r\n");
		}
	}
}
