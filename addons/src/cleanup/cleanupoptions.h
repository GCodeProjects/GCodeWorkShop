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

#ifndef CLEANUPOPTIONS_H
#define CLEANUPOPTIONS_H

#include <QStringList>

class QSettings;


struct CleanUpOptions {
	QStringList expressions;
	QStringList comments;
	QStringList selected;

	void load(QSettings* settings, const CleanUpOptions& defaultOptions);
	void save(QSettings* settings);
};

#endif // CLEANUPOPTIONS_H
