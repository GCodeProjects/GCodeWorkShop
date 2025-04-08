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

#include "i2moptions.h"


#define CFG_KEY_INCH_VALUE  "inch"
#define CFG_KEY_INCH_IN     "inchIn"
#define CFG_KEY_MM_VALUE    "mm"
#define CFG_KEY_MM_IN       "mmIn"
#define CFG_KEY_TO_INCH     "ToInch"


I2MOptions::I2MOptions() :
	toInch{true},
	mm{true, 100.0},
	inch{false, 0.0}
{
}

void I2MOptions::load(QSettings* settings)
{
	inch.value = settings->value(CFG_KEY_INCH_VALUE, inch.value).toDouble();
	inch.in = settings->value(CFG_KEY_INCH_IN, inch.in).toBool();
	mm.value = settings->value(CFG_KEY_MM_VALUE, mm.value).toDouble();
	mm.in = settings->value(CFG_KEY_MM_IN, mm.in).toBool();
	toInch = settings->value(CFG_KEY_TO_INCH, toInch).toBool();
}

void I2MOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_INCH_VALUE, inch.value);
	settings->setValue(CFG_KEY_INCH_IN, inch.in);
	settings->setValue(CFG_KEY_MM_VALUE, mm.value);
	settings->setValue(CFG_KEY_MM_IN, mm.in);
	settings->setValue(CFG_KEY_TO_INCH, toInch);
}
