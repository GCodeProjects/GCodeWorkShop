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

#include <algorithm>    // for sort

#include <QList>                    // for QList
#include <QRegularExpression>       // for QRegularExpression
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString
#include <QStringList>              // for QStringList
#include <QtGlobal>                 // for QTypeInfo<>::isLarge, QTypeInfo<>::isStatic, QForeachContainer, QTypeInfo...

#include <utils/splitfile.h>    // IWYU pragma: associated

#include "filepatterns.h"  // for FILENAME_FADAL, FILENAME_FANUC, FILENAME_HEID1, FILENAME_HEID2, FILENAME_OSP


QStringList Utils::splitFile(const QString& text)
{
	QStringList progs;
	QStringList regexPatterns;
	QList<int> progBegins;
	QRegularExpression regex;

	progs.clear();

	if (text.isNull() || text.isEmpty()) {
		return progs;
	}

	regexPatterns << FILENAME_SINU840
	              << FILENAME_OSP
	              << FILENAME_FANUC
	              << FILENAME_SINU
	              << FILENAME_HEID1
	              << FILENAME_HEID2
	              << FILENAME_PHIL
	              << FILENAME_FADAL;

	// detect CNC control type
	foreach (const QString pattern, regexPatterns) {
		regex.setPattern(pattern);

		if (text.contains(regex)) {
			regexPatterns.clear();
			regexPatterns.append(pattern);
			break;
		}
	}

	// prepare program list
	foreach (const QString pattern, regexPatterns) {
		regex.setPattern(pattern);
		auto match = regex.match(text);

		while (match.hasMatch()) {
			progBegins.append(match.capturedStart());
			match = regex.match(text, match.capturedEnd());
		}
	}

	std::sort(progBegins.begin(), progBegins.end());

	// split file  TODO: data can be lost if filename detection fails also some garbage are left
	QList<int>::const_iterator it = progBegins.constBegin();

	while (it != progBegins.constEnd()) {
		int progEnd;
		int progBegin = *it;
		++it;

		if (it != progBegins.constEnd()) {
			progEnd = *it;
		} else {
			progEnd = text.size();
		}

		QString tx = text.mid(progBegin, progEnd - progBegin);

		if (!tx.isEmpty()) {
			progs.append(tx);
		}
	}

	return progs;
}
