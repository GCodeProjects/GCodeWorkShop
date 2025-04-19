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

#include "chamferoptions.h"


#define CFG_KEY_ANGLE_VALUE     "Angle"
#define CFG_KEY_ANGLE_IN        "AngleIn"
#define CFG_KEY_HEIGHT_VALUE    "Height"
#define CFG_KEY_HEIGHT_IN       "HeightIn"
#define CFG_KEY_MAX_DIAM_VALUE  "MaxDiam"
#define CFG_KEY_MAX_DIAM_IN     "MaxDiamIn"
#define CFG_KEY_MIN_DIAM_VALUE  "MinDiam"
#define CFG_KEY_MIN_DIAM_IN     "MinDiamIn"
#define CFG_KEY_WIDTH_VALUE     "Width"
#define CFG_KEY_WIDTH_IN        "WidthIn"


ChamferOptions::ChamferOptions() :
	angle{true, 45.0},
	width{true, 1.0},
	height{false, 0.0},
	minDiam{false, 0.0},
	maxDiam{true, 32.0}
{
}

void ChamferOptions::load(QSettings* settings)
{
	angle.value = settings->value(CFG_KEY_ANGLE_VALUE, angle.value).toDouble();
	angle.in = settings->value(CFG_KEY_ANGLE_IN, angle.in).toBool();
	height.value = settings->value(CFG_KEY_HEIGHT_VALUE, height.value).toDouble();
	height.in = settings->value(CFG_KEY_HEIGHT_IN, height.in).toBool();
	maxDiam.value = settings->value(CFG_KEY_MAX_DIAM_VALUE, maxDiam.value).toDouble();
	maxDiam.in = settings->value(CFG_KEY_MAX_DIAM_IN, maxDiam.in).toBool();
	minDiam.value = settings->value(CFG_KEY_MIN_DIAM_VALUE, minDiam.value).toDouble();
	minDiam.in = settings->value(CFG_KEY_MIN_DIAM_IN, minDiam.in).toBool();
	width.value = settings->value(CFG_KEY_WIDTH_VALUE, width.value).toDouble();
	width.in = settings->value(CFG_KEY_WIDTH_IN, width.in).toBool();
}

void ChamferOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_ANGLE_VALUE, angle.value);
	settings->setValue(CFG_KEY_ANGLE_IN, angle.in);
	settings->setValue(CFG_KEY_HEIGHT_VALUE, height.value);
	settings->setValue(CFG_KEY_HEIGHT_IN, height.in);
	settings->setValue(CFG_KEY_MAX_DIAM_VALUE, maxDiam.value);
	settings->setValue(CFG_KEY_MAX_DIAM_IN, maxDiam.in);
	settings->setValue(CFG_KEY_MIN_DIAM_VALUE, minDiam.value);
	settings->setValue(CFG_KEY_MIN_DIAM_IN, minDiam.in);
	settings->setValue(CFG_KEY_WIDTH_VALUE, width.value);
	settings->setValue(CFG_KEY_WIDTH_IN, width.in);
}
