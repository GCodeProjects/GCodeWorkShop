/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of GCodeWorkShop.                                   *
 *                                                                         *
 *   GCodeWorkShop is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QChar>                    // for operator==, QChar
#include <QRegularExpression>       // for QRegularExpression
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString

#include <utils/removezeros.h>  // IWYU pragma: associated


QString Utils::removeZeros(QString str)
{
	QRegularExpression regex;
	regex.setPattern("[\\d]{0,}[-.]{0,1}[-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$"); //[\\d]+[.][-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$"
	auto match = regex.match(str);

	while (match.hasMatch()) {
		int pos = match.capturedStart();

		if ((str.at(match.capturedEnd() - 1) == '0')) {
			str.remove(match.capturedEnd() - 1, 1);
		} else {
			pos = match.capturedEnd();
		}

		match = regex.match(str, pos);
	}

	if (str == "-0.") {
		str = "0.";
	}

	return (str);
}
