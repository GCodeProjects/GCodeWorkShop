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
#include <QStringList>  // for QStringList
#include <QVariant>     // for QVariant

#include "cleanupoptions.h"


#define CFG_KEY_COMMENTS    "Comments"
#define CFG_KEY_EXPRESSIONS "Expressions"
#define CFG_KEY_SELECTED    "Selected"


void CleanUpOptions::load(QSettings *settings, const CleanUpOptions &defaultOptions)
{
	comments = settings->value(CFG_KEY_COMMENTS, defaultOptions.comments).toStringList();
	expressions = settings->value(CFG_KEY_EXPRESSIONS, defaultOptions.expressions).toStringList();
	selected = settings->value(CFG_KEY_SELECTED, defaultOptions.selected).toStringList();
}

void CleanUpOptions::save(QSettings *settings)
{
	if (expressions.isEmpty()) {
		settings->remove(CFG_KEY_COMMENTS);
		settings->remove(CFG_KEY_EXPRESSIONS);
		settings->remove(CFG_KEY_SELECTED);
	} else {
		settings->setValue(CFG_KEY_COMMENTS, comments);
		settings->setValue(CFG_KEY_EXPRESSIONS, expressions);
		settings->setValue(CFG_KEY_SELECTED, selected);
	}
}
