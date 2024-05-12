/*
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
#include <QSettings>    // for QSettings
#include <QVariant>     // for QVariant

#include "chamferoptions.h"


#define CFG_KEY_ANGLE_VALUE     "Angle"
#define CFG_KEY_ANGLE_IN        "AngleIn"
#define CFG_KEY_HIGHT_VALUE     "Hight"
#define CFG_KEY_HIGHT_IN        "HightIn"
#define CFG_KEY_MAX_DIAM_VALUE  "MaxDiam"
#define CFG_KEY_MAX_DIAM_IN     "MaxDiamIn"
#define CFG_KEY_MIN_DIAM_VALUE  "MinDiam"
#define CFG_KEY_MIN_DIAM_IN     "MinDiamIn"
#define CFG_KEY_WIDTH_VALUE     "Width"
#define CFG_KEY_WIDTH_IN        "WidthIn"


void ChamferOptions::load(QSettings* settings, const ChamferOptions& defaultOptions)
{
	angle.value = settings->value(CFG_KEY_ANGLE_VALUE, defaultOptions.angle.value).toDouble();
	angle.in = settings->value(CFG_KEY_ANGLE_IN, defaultOptions.angle.in).toBool();
	hight.value = settings->value(CFG_KEY_HIGHT_VALUE, defaultOptions.hight.value).toDouble();
	hight.in = settings->value(CFG_KEY_HIGHT_IN, defaultOptions.hight.in).toBool();
	maxDiam.value = settings->value(CFG_KEY_MAX_DIAM_VALUE, defaultOptions.maxDiam.value).toDouble();
	maxDiam.in = settings->value(CFG_KEY_MAX_DIAM_IN, defaultOptions.maxDiam.in).toBool();
	minDiam.value = settings->value(CFG_KEY_MIN_DIAM_VALUE, defaultOptions.minDiam.value).toDouble();
	minDiam.in = settings->value(CFG_KEY_MIN_DIAM_IN, defaultOptions.minDiam.in).toBool();
	width.value = settings->value(CFG_KEY_WIDTH_VALUE, defaultOptions.width.value).toDouble();
	width.in = settings->value(CFG_KEY_WIDTH_IN, defaultOptions.width.in).toBool();
}

void ChamferOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_ANGLE_VALUE, angle.value);
	settings->setValue(CFG_KEY_ANGLE_IN, angle.in);
	settings->setValue(CFG_KEY_HIGHT_VALUE, hight.value);
	settings->setValue(CFG_KEY_HIGHT_IN, hight.in);
	settings->setValue(CFG_KEY_MAX_DIAM_VALUE, maxDiam.value);
	settings->setValue(CFG_KEY_MAX_DIAM_IN, maxDiam.in);
	settings->setValue(CFG_KEY_MIN_DIAM_VALUE, minDiam.value);
	settings->setValue(CFG_KEY_MIN_DIAM_IN, minDiam.in);
	settings->setValue(CFG_KEY_WIDTH_VALUE, width.value);
	settings->setValue(CFG_KEY_WIDTH_IN, width.in);
}
