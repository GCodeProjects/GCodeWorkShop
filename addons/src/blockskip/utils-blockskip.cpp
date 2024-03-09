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

#include <QChar>                    // for QChar, QChar::LineFeed
#include <QRegularExpression>       // for QRegularExpression
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString
#include <QStringList>              // for QStringList
#include <QtGlobal>                 // for QForeachContainer, qMakeForeachContainer, foreach

#include "utils-blockskip.h"


void Utils::blockSkip(QString& tx, bool remove, bool inc)
{
	int num = 0;
	QRegularExpression regex;

	regex.setPattern(QString("/[0-9]{0,1}"));

	if (!remove) {
		auto match = regex.match(tx);

		if (match.hasMatch()) {
			QString captured = match.captured();
			tx.remove(regex);
			captured.remove('/');
			captured.remove(' ');
			bool ok;
			num = captured.toInt(&ok);

			if (!ok) {
				num = 0;
			}

			if (inc) {
				num++;

				if (num > 9) {
					num = 9;
				}
			} else {
				num--;

				if (num < 0) {
					num = 0;
				}
			}
		}
	}

	QStringList list = tx.split(QChar::LineFeed);

	if (list.isEmpty()) {
		list.append(tx);
	}

	tx.clear();

	foreach (QString txLine, list) {
		if (remove) {
			if (txLine.length() > 0) {
				txLine.remove(regex);
			}
		} else {
			int i = txLine.indexOf(QRegularExpression("[;/(]{1,1}"));

			if ((i > 1) || (i < 0)) {
				if (num == 0) {
					txLine.prepend("/");
				} else {
					txLine.prepend(QString("/%1").arg(num));
				}
			}
		}

		txLine.append("\n");
		tx.append(txLine);
	}

	tx.remove(tx.length() - 1, 1);
}
