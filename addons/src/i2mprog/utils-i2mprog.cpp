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

#include <QRegularExpression>               // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>          // for QRegularExpressionMatch
#include <QRegularExpressionMatchIterator>  // for QRegularExpressionMatchIterator
#include <QString>                          // for QString

#include "utils-i2mprog.h"


#define APOSTROPHE_COMMENT_REGEXPR  "\\'[^\\n\\r]*\\'"
#define GCODE_COMMENT_REGEXPR       "\\([^\\n\\r]*\\)"
#define SINUMERIK_COMMENT_REGEXPR   ";[^\\n\\r]*$"
//                            |<     sign      >| |<     digits-dot-digits     >| |<  digits-dot  >| |<  dot-digits  >| |<   digits  >|
#define WORD_REGEXPR    "[%1]([ \\t]*[-+]?[ \\t]*((\\d[ \\t\\d]*\\.[ \\t\\d]*\\d)|(\\d[ \\t\\d]*\\.)|(\\.[ \\t\\d]*\\d)|([ \\t\\d]*\\d)))"


int Utils::i2mprog(QString& tx,
                   const QString& addr,
                   bool toInch, int prec,
                   const std::function<bool (int)>& interrupt)
{
	int count = 0;
	QRegularExpression regex{
		QString(
		    WORD_REGEXPR
		    "|"
		    GCODE_COMMENT_REGEXPR
		    "|"
		    APOSTROPHE_COMMENT_REGEXPR
		    "|"
		    SINUMERIK_COMMENT_REGEXPR
		).arg(addr),
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatchIterator iterator = regex.globalMatch(tx);

	int pos = 0;
	QString result;

	while (iterator.hasNext()) {
		if (interrupt(pos)) {
			return 0;
		}

		const QRegularExpressionMatch& match = iterator.next();

		if (match.capturedLength(1) > 0) {
			QString f_tx = match.captured(1);
			f_tx.remove(' ');
			f_tx.remove('\t');
			bool ok;
			double it = f_tx.toDouble(&ok);

			if (ok && it != 0) {
				if (toInch) {
					it /= 25.4;
				} else {
					it *= 25.4;
				}

				QString conv = QString("%1").arg(it, 0, 'f', prec);
				result.append(tx.mid(pos, match.capturedStart(1) - pos));
				result.append(conv);
				pos = match.capturedEnd(1);
				count++;
			}
		}
	}

	result.append(tx.mid(pos));
	tx = result;
	return count;
}
