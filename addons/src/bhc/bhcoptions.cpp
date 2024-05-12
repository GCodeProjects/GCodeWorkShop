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

#include <QSettings>

#include "bhcoptions.h"
#include "bhctaboptions.h"  // for BHCTabOptions


#define CFG_SECTION_GREEN   "GreenTab"
#define CFG_SECTION_BLUE    "BlueTab"
#define CFG_SECTION_RED     "RedTab"
#define CFG_SECTION_YELLOW  "YellowTab"


void BHCOptions::load(QSettings* settings, const BHCOptions& defaultOptions)
{
	settings->beginGroup(CFG_SECTION_GREEN);
	green.load(settings, defaultOptions.green);
	settings->endGroup();

	settings->beginGroup(CFG_SECTION_BLUE);
	blue.load(settings, defaultOptions.blue);
	settings->endGroup();

	settings->beginGroup(CFG_SECTION_RED);
	red.load(settings, defaultOptions.red);
	settings->endGroup();

	settings->beginGroup(CFG_SECTION_YELLOW);
	yellow.load(settings, defaultOptions.yellow);
	settings->endGroup();
}

void BHCOptions::save(QSettings* settings)
{
	settings->beginGroup(CFG_SECTION_GREEN);
	green.save(settings);
	settings->endGroup();

	settings->beginGroup(CFG_SECTION_BLUE);
	blue.save(settings);
	settings->endGroup();

	settings->beginGroup(CFG_SECTION_RED);
	red.save(settings);
	settings->endGroup();

	settings->beginGroup(CFG_SECTION_YELLOW);
	yellow.save(settings);
	settings->endGroup();
}
