/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
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

#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString

#include "utils-i2mprog.h"


int Utils::i2mprog(QString tx, const QString& addr, bool toInch, int prec)
{
	int count = 0;
	QRegularExpression regex;

	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern(
	    QString("[%1]{1,1}([+-]?\\d*\\.?\\d*)|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$").arg(addr));
	auto match = regex.match(tx);

	while (match.hasMatch()) {
		int pos = match.capturedEnd();

		if (match.capturedLength(1) > 0) {
			QString f_tx = match.captured(1);
			bool ok;
			double it = f_tx.toDouble(&ok);

			if (ok && it != 0) {
				if (toInch) {
					it /= 25.4;
				} else {
					it *= 25.4;
				}

				QString conv = QString("%1").arg(it, 0, 'f', prec);
				tx.replace(match.capturedStart(), match.capturedLength(), conv);
				pos += conv.length() - match.capturedLength();
				count++;
			}
		}

		match = regex.match(tx, pos);
	}

	return count;
}
