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

#include <QChar>        // for operator==, QChar, QChar::LineFeed
#include <QString>      // for QString, QCharRef
#include <QStringList>  // for QStringList
#include <QtGlobal>     // for QForeachContainer, qMakeForeachContainer, foreach

#include "utils-comment.h"


void Utils::paraComment(QString& tx)
{
	QStringList list = tx.split(QChar::LineFeed);
	bool remove = false;

	if (tx[0] == '(') {
		remove = true;
	}

	tx.clear();

	foreach (QString txLine, list) {
		if (remove) {
			if (txLine.length() > 0) {
				int idx = txLine.indexOf("(");

				if (idx >= 0) {
					txLine.remove(idx, 1);
				}

				idx = txLine.indexOf(")");

				if (idx >= 0) {
					txLine.remove(idx, 1);
				}
			}
		} else {
			txLine.prepend("(");
			txLine.append(")");
		}

		txLine.append("\n");
		tx.append(txLine);
	}

	tx.remove(tx.length() - 1, 1);
}

void Utils::semiComment(QString& tx)
{
	QStringList list = tx.split(QChar::LineFeed);
	bool remove = false;

	if (tx[0] == ';') {
		remove = true;
	}

	tx.clear();

	foreach (QString txLine, list) {
		if (remove) {
			if (txLine.length() > 0) {
				int idx = txLine.indexOf(";");

				if (idx >= 0) {
					txLine.remove(idx, 1);
				}
			}
		} else {
			txLine.prepend(";");
		}

		txLine.append("\n");
		tx.append(txLine);
	}

	tx.remove(tx.length() - 1, 1);
}
