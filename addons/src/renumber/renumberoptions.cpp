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

#include "renumberoptions.h"


#define CFG_KEY_APPLY_WIDTH     "ApplyWidth"
#define CFG_KEY_FROM            "From"
#define CFG_KEY_INC             "Inc"
#define CFG_KEY_MODE            "Mode"
#define CFG_KEY_RENUM_EMPTY     "RenumEmpty"
#define CFG_KEY_RENUM_MARKED    "RenumMarked"
#define CFG_KEY_RENUM_WITH_COMM "RenumWithComm"
#define CFG_KEY_START           "Start"
#define CFG_KEY_TO              "To"
#define CFG_KEY_WIDTH           "Width"


void RenumberOptions::load(QSettings* settings, const RenumberOptions& defaultOptions)
{
	applyWidth = settings->value(CFG_KEY_APPLY_WIDTH, defaultOptions.applyWidth).toBool();
	from = settings->value(CFG_KEY_FROM, defaultOptions.from).toInt();
	inc = settings->value(CFG_KEY_INC, defaultOptions.inc).toInt();
	mode = settings->value(CFG_KEY_MODE, defaultOptions.mode).toInt();
	startAt = settings->value(CFG_KEY_START, defaultOptions.startAt).toInt();
	to = settings->value(CFG_KEY_TO, defaultOptions.to).toInt();
	renumComm = settings->value(CFG_KEY_RENUM_WITH_COMM, defaultOptions.renumComm).toBool();
	renumEmpty = settings->value(CFG_KEY_RENUM_EMPTY, defaultOptions.renumEmpty).toBool();
	renumMarked = settings->value(CFG_KEY_RENUM_MARKED, defaultOptions.renumMarked).toBool();
	width = settings->value(CFG_KEY_WIDTH, defaultOptions.width).toInt();
}

void RenumberOptions::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_APPLY_WIDTH, applyWidth);
	settings->setValue(CFG_KEY_FROM, from);
	settings->setValue(CFG_KEY_INC, inc);
	settings->setValue(CFG_KEY_MODE, mode);
	settings->setValue(CFG_KEY_RENUM_WITH_COMM, renumComm);
	settings->setValue(CFG_KEY_RENUM_EMPTY, renumEmpty);
	settings->setValue(CFG_KEY_RENUM_MARKED, renumMarked);
	settings->setValue(CFG_KEY_START, startAt);
	settings->setValue(CFG_KEY_TO, to);
	settings->setValue(CFG_KEY_WIDTH, width);
}
