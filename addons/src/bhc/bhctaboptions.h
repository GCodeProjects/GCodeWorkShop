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

#ifndef BHCTABOPTIONS_H
#define BHCTABOPTIONS_H

class QSettings;


struct BHCTabOptions {
	BHCTabOptions();

	double  x;
	double  y;
	double  diam;
	double  start;
	double  step;
	int     count;
	int     rotate;
	bool    mirrorX;
	bool    mirrorY;
	bool    common;

	void load(QSettings* settings);
	void save(QSettings* settings);
};

#endif // BHCTABOPTIONS_H
