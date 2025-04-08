/*
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

#ifndef SWAPAXESOPTIONS_H
#define SWAPAXESOPTIONS_H

#include <QString>      // for QString
#include <QStringList>  // for QStringList

class QSettings;


struct SwapAxesOptions {
	SwapAxesOptions();

	QString     from;
	QString     to;
	int         precision;

	QStringList fromList;
	QStringList toList;

	struct Limit {
		bool    enable;
		double  min;
		double  max;
	} limit;

	enum Operation {
		Nop = -1,
		Add,
		Subtract,
		Multiply,
		Divide
	};

	struct Convert {
		bool    enable;
		int     operation;
		double  value;
	} convert;

	void load(QSettings* settings);
	void save(QSettings* settings);
};

#endif // SWAPAXESOPTIONS_H
