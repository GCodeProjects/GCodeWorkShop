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

#include <QLatin1Char>                      // for QLatin1Char
#include <QRegularExpression>               // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>			// for QRegularExpressionMatch
#include <QRegularExpressionMatchIterator>  // for QRegularExpressionMatchIterator
#include <QString>                          // for QString

#include "utils-insertdot.h"


#define APOSTROPHE_COMMENT_REGEXPR  "\\'[^\\n\\r]*\\'"
#define GCODE_COMMENT_REGEXPR       "\\([^\\n\\r]*\\)"
#define SINUMERIK_COMMENT_REGEXPR   ";[^\\n\\r]*$"
//                            |<     sign      >| |<     digits-dot-digits     >| |<  digits-dot  >| |<  dot-digits  >| |<   digits  >|
#define WORD_REGEXPR    "[%1]([ \\t]*[-+]?[ \\t]*((\\d[ \\t\\d]*\\.[ \\t\\d]*\\d)|(\\d[ \\t\\d]*\\.)|(\\.[ \\t\\d]*\\d)|([ \\t\\d]*\\d)))"


int Utils::insertDot(QString& tx,
                     const QString& addr,
                     bool convert,
                     int divider,
                     const std::function<bool (int)>& interrupt)
{
	int count = 0;
	int pos = 0;
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
	QString result;

	while (iterator.hasNext()) {
		if (interrupt(pos)) {
			return 0;
		}

		QRegularExpressionMatch match = iterator.next();

		if (match.capturedLength(1) > 0) {
			QString f_tx = match.captured(1);
			f_tx.remove(' ');
			f_tx.remove('\t');

			if (convert && !f_tx.contains(QLatin1Char('.'))) {
				bool ok;
				double it = f_tx.toDouble(&ok);

				if (ok) {
					it = it / divider;
					QString conv = QString("%1").arg(it, 0, 'f', 3);
					result.append(tx.mid(pos, match.capturedStart(1) - pos));
					result.append(conv);
					pos = match.capturedEnd(1);
					count++;
				}
			}

			if (!convert && !f_tx.contains(QLatin1Char('.'))) {
				result.append(tx.mid(pos, match.capturedEnd(1) - pos));
				result.append(QLatin1Char('.'));
				pos = match.capturedEnd(1);
				count++;
			}
		}
	}

	result.append(tx.mid(pos));
	tx = result;
	return count;
}
