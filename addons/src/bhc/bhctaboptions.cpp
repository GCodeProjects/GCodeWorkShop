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

#include <QSettings>    // for QSettings
#include <QVariant>     // for QVariant

#include "bhctaboptions.h"


#define CFG_KEY_COMMON  "Common"
#define CFG_KEY_COUNT   "Count"
#define CFG_KEY_DIAM    "Diameter"
#define CFG_KEY_MIRRORX "MirrorX"
#define CFG_KEY_MIRRORY "MirrorY"
#define CFG_KEY_ROTATE  "Rotate"
#define CFG_KEY_START   "Start"
#define CFG_KEY_STEP    "Step"
#define CFG_KEY_X       "XCenter"
#define CFG_KEY_Y       "YCenter"


BHCTabOptions::BHCTabOptions() :
	x{0},
	y{0},
	diam{0},
	start{0},
	step{0},
	count{1},
	rotate{0},
	mirrorX{false},
	mirrorY{false},
	common{false}
{
}

void BHCTabOptions::load(QSettings* settings)
{
	x = settings->value(CFG_KEY_X, x).toDouble();
	y = settings->value(CFG_KEY_Y, y).toDouble();
	diam = settings->value(CFG_KEY_DIAM, diam).toDouble();
	start = settings->value(CFG_KEY_START, start).toDouble();
	step = settings->value(CFG_KEY_STEP, step).toDouble();
	count = settings->value(CFG_KEY_COUNT, count).toInt();
	rotate = settings->value(CFG_KEY_ROTATE, rotate).toInt();
	mirrorX = settings->value(CFG_KEY_MIRRORX, mirrorX).toBool();
	mirrorY = settings->value(CFG_KEY_MIRRORY, mirrorY).toBool();
	common = settings->value(CFG_KEY_COMMON, common).toBool();
}

void BHCTabOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_X, x);
	settings->setValue(CFG_KEY_Y, y);
	settings->setValue(CFG_KEY_DIAM, diam);
	settings->setValue(CFG_KEY_START, start);
	settings->setValue(CFG_KEY_STEP, step);
	settings->setValue(CFG_KEY_COUNT, count);
	settings->setValue(CFG_KEY_ROTATE, rotate);
	settings->setValue(CFG_KEY_MIRRORX, mirrorX);
	settings->setValue(CFG_KEY_MIRRORY, mirrorY);
	settings->setValue(CFG_KEY_COMMON, common);
}
