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

#include "dotoptions.h"


#define CFG_KEY_AXES    "Axes"
#define CFG_KEY_CONVERT "Convert"
#define CFG_KEY_DIVIDER "Divider"


void DotOptions::load(QSettings *settings, const DotOptions &defaultOptions)
{
	axes = settings->value(CFG_KEY_AXES, defaultOptions.axes).toString();
	convert = settings->value(CFG_KEY_CONVERT, defaultOptions.convert).toBool();
	divider = settings->value(CFG_KEY_DIVIDER, defaultOptions.divider).toInt();
}

void DotOptions::save(QSettings *settings)
{
	settings->setValue(CFG_KEY_AXES, axes);
	settings->setValue(CFG_KEY_CONVERT, convert);
	settings->setValue(CFG_KEY_DIVIDER, divider);
}
