/*
 *  Copyright (C) 2023-2023 Nick Egorrov, nicegorov@yandex.ru
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

#include <QRegularExpression>               // for QRegularExpression
#include <QRegularExpressionMatch>          // for QRegularExpressionMatch
#include <QRegularExpressionMatchIterator>  // for QRegularExpressionMatchIterator
#include <QString>                          // for QString
#include <QStringList>                      // for QStringList

#include "utils-removebyregex.h"


bool Utils::removeTextByRegExp(QString& tx,
                               const QStringList& expList,
                               bool replaceDollar,
                               const std::function<bool (int)>& interrupt)
{
	if (expList.isEmpty()) {
		return false;
	}

	QString complex;

	// Glue multiple regular expressions into one.
	for (QString exp : expList) {
		if (replaceDollar && exp.contains('$') && !exp.contains("\\$")) {
			exp.replace('$', "\\n");
		}

		if (!complex.isEmpty()) {
			complex.append("|");
		}

		complex.append(exp);
	}

	QRegularExpression regexpr{
		complex,
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatchIterator iterator = regexpr.globalMatch(tx);

	int pos = 0;
	QString result;

	while (iterator.hasNext()) {
		if (interrupt(pos)) {
			return false;
		}

		const QRegularExpressionMatch& match = iterator.next();
		const QString& sub = tx.mid(pos, match.capturedStart() - pos);
		result.append(sub);
		pos = match.capturedEnd();
	}

	result.append(tx.mid(pos));
	tx = result;
	return true;
}
