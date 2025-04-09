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

#include "i2mprogoptions.h"


#define CFG_KEY_AXES      "Axes"
#define CFG_KEY_PRECISION "Precision"
#define CFG_KEY_TO_INCH   "ToInch"


I2MProgOptions::I2MProgOptions() :
	toInch{false},
	prec{3},
	axes{"XYZUVW"}
{
}

void I2MProgOptions::load(QSettings* settings, const I2MProgOptions& defaultOptions)
{
	axes = settings->value(CFG_KEY_AXES, defaultOptions.axes).toString();
	prec = settings->value(CFG_KEY_PRECISION, defaultOptions.prec).toInt();
	toInch = settings->value(CFG_KEY_TO_INCH, defaultOptions.toInch).toBool();
}

void I2MProgOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_AXES, axes);
	settings->setValue(CFG_KEY_PRECISION, prec);
	settings->setValue(CFG_KEY_TO_INCH, toInch);
}
