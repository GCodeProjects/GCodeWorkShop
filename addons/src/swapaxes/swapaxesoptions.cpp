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

#include "swapaxesoptions.h"


#define CFG_KEY_CONVERT_ENABLE      "ConvertEnable"
#define CFG_KEY_CONVERT_OPERATION   "ConvertOperation"
#define CFG_KEY_CONVERT_VALUE       "ConvertValue"
#define CFG_KEY_FROM                "From"
#define CFG_KEY_FROM_LIST           "FromList"
#define CFG_KEY_LIMIT_ENABLE        "LimitEnable"
#define CFG_KEY_LIMIT_MAX           "LimitMax"
#define CFG_KEY_LIMIT_MIN           "LimitMin"
#define CFG_KEY_TO                  "To"
#define CFG_KEY_TO_LIST             "ToList"
#define CFG_KEY_PRECISION           "Precision"


SwapAxesOptions::SwapAxesOptions() :
	from{"C"},
	to{"Y"},
	precision{2},
	fromList{"X", "Y", "Z", "U", "V", "W", "C"},
	toList{"X", "Y", "Z", "U", "V", "W", "C"},
	limit{false, -100.0, 100.0},
	convert{false, Add, 0.0}
{
}

void SwapAxesOptions::load(QSettings* settings)
{
	convert.enable = settings->value(CFG_KEY_CONVERT_ENABLE, convert.enable).toBool();
	convert.operation = settings->value(CFG_KEY_CONVERT_OPERATION, convert.operation).toInt();
	convert.value = settings->value(CFG_KEY_CONVERT_VALUE, convert.value).toDouble();
	from = settings->value(CFG_KEY_FROM, from).toString();
	fromList = settings->value(CFG_KEY_FROM_LIST, fromList).toStringList();
	limit.enable = settings->value(CFG_KEY_LIMIT_ENABLE, limit.enable).toBool();
	limit.max = settings->value(CFG_KEY_LIMIT_MAX, limit.max).toDouble();
	limit.min = settings->value(CFG_KEY_LIMIT_MIN, limit.min).toDouble();
	to = settings->value(CFG_KEY_TO, to).toString();
	toList = settings->value(CFG_KEY_TO_LIST, toList).toStringList();
	precision = settings->value(CFG_KEY_PRECISION, precision).toInt();
}

void SwapAxesOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_CONVERT_ENABLE, convert.enable);
	settings->setValue(CFG_KEY_CONVERT_OPERATION, convert.operation);
	settings->setValue(CFG_KEY_CONVERT_VALUE, convert.value);
	settings->setValue(CFG_KEY_FROM, from);
	settings->setValue(CFG_KEY_FROM_LIST, fromList);
	settings->setValue(CFG_KEY_LIMIT_ENABLE, limit.enable);
	settings->setValue(CFG_KEY_LIMIT_MAX, limit.max);
	settings->setValue(CFG_KEY_LIMIT_MIN, limit.min);
	settings->setValue(CFG_KEY_TO, to);
	settings->setValue(CFG_KEY_TO_LIST, toList);
	settings->setValue(CFG_KEY_PRECISION, precision);
}
