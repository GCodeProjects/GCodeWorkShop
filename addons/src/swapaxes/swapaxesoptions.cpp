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


void SwapAxesOptions::load(QSettings *settings, const SwapAxesOptions &defaultOptions)
{
    convert.enable = settings->value(CFG_KEY_CONVERT_ENABLE, defaultOptions.convert.enable).toBool();
    convert.operation = settings->value(CFG_KEY_CONVERT_ENABLE, defaultOptions.convert.operation).toInt();
    convert.value = settings->value(CFG_KEY_CONVERT_ENABLE, defaultOptions.convert.value).toDouble();
    from = settings->value(CFG_KEY_FROM, defaultOptions.from).toString();
    fromList = settings->value(CFG_KEY_FROM_LIST, defaultOptions.fromList).toStringList();
    limit.enable = settings->value(CFG_KEY_LIMIT_ENABLE, defaultOptions.limit.enable).toBool();
    limit.max = settings->value(CFG_KEY_LIMIT_MAX, defaultOptions.limit.max).toDouble();
    limit.min = settings->value(CFG_KEY_LIMIT_MIN, defaultOptions.limit.min).toDouble();
    to = settings->value(CFG_KEY_TO, defaultOptions.to).toString();
    toList = settings->value(CFG_KEY_TO_LIST, defaultOptions.toList).toStringList();
    precision = settings->value(CFG_KEY_PRECISION, defaultOptions.precision).toInt();
}

void SwapAxesOptions::save(QSettings *settings)
{
    settings->setValue(CFG_KEY_CONVERT_ENABLE, convert.enable);
    settings->setValue(CFG_KEY_CONVERT_ENABLE, convert.operation);
    settings->setValue(CFG_KEY_CONVERT_ENABLE, convert.value);
    settings->setValue(CFG_KEY_FROM, from);
    settings->setValue(CFG_KEY_FROM_LIST, fromList);
    settings->setValue(CFG_KEY_LIMIT_ENABLE, limit.enable);
    settings->setValue(CFG_KEY_LIMIT_MAX, limit.max);
    settings->setValue(CFG_KEY_LIMIT_MIN, limit.min);
    settings->setValue(CFG_KEY_TO, to);
    settings->setValue(CFG_KEY_TO_LIST, toList);
    settings->setValue(CFG_KEY_PRECISION, precision);
}
