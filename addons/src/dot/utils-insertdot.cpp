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

#include <QLatin1Char>              // for QLatin1Char
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString

#include "utils-insertdot.h"


int Utils::insertDot(QString& tx, const QString& addr, bool convert, int divider)
{
	int count = 0;
	int pos = 0;
	QString f_tx;
	QRegularExpression regex;

	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern(QString("[%1]{1,1}[-.+0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$").arg(addr));
	auto match = regex.match(tx);

	while (match.hasMatch()) {
		f_tx = match.captured();
		pos = match.capturedEnd();

		if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
		        && !f_tx.contains(QLatin1Char(';'))) {
			if (convert && !f_tx.contains(QLatin1Char('.'))) {
				f_tx.remove(0, 1);

				//f_tx.remove('+');
				bool ok;
				double it = f_tx.toDouble(&ok);

				if (ok) {
					it = it / divider;
					tx.replace(match.capturedStart() + 1, match.capturedLength() - 1,
					           QString("%1").arg(it, 0, 'f', 3));
					count++;
				}
			}

			if (!convert && !f_tx.contains(QLatin1Char('.'))) {
				tx.insert(match.capturedEnd(), QLatin1Char('.'));
				pos++;
				count++;
			}
		}

		match =  regex.match(tx, pos);
	}

	return count;
}
