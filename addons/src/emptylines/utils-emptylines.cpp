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

#include <QLatin1String>        // for QLatin1String
#include <QRegularExpression>   // for QRegularExpression
#include <QString>              // for QString

#include "utils-emptylines.h"


bool Utils::insertEmptyLines(QString& tx, const std::function<bool (int)>& interrupt)
{
	bool changed = false;
	int pos = 0;
	QRegularExpression regexpr{
		"\\n|\\r\\n"
	};
	QRegularExpressionMatchIterator iterator = regexpr.globalMatch(tx);
	QString result;

	while (iterator.hasNext()) {
		if (interrupt(pos)) {
			return false;
		}

		QRegularExpressionMatch match = iterator.next();
		result.append(tx.mid(pos, match.capturedStart() - pos));
		result.append("\n\n");
		pos = match.capturedEnd();
		changed = true;
	}

	result.append(tx.mid(pos));
	tx = result;
	return changed;
}

bool Utils::removeEmptyLines(QString& tx, const std::function<bool (int)>& interrupt)
{
	bool changed = false;
	int pos = 0;
	QRegularExpression regexpr{
		"[\\n\\r]+"
	};
	QRegularExpressionMatchIterator iterator = regexpr.globalMatch(tx);
	QString result;

	while (iterator.hasNext()) {
		if (interrupt(pos)) {
			return false;
		}

		QRegularExpressionMatch match = iterator.next();
		result.append(tx.mid(pos, match.capturedStart() - pos));
		result.append("\n");
		pos = match.capturedEnd();
		changed = true;
	}

	result.append(tx.mid(pos));
	tx = result;
	return changed;
}
