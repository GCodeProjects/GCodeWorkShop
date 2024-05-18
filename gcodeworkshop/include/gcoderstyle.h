/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
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

#ifndef GCODERSTYLE_H
#define GCODERSTYLE_H

#include <QString>  // for QString

class QSettings;

#include <documentstyle.h>  // for DocumentStyle


class HighlightColors
{
public:
	int commentColor;
	int gColor;
	int mColor;
	int nColor;
	int lColor;
	int fsColor;
	int dhtColor;
	int rColor;
	int macroColor;
	int keyWordColor;
	int progNameColor;
	int operatorColor;
	int zColor;
	int aColor;
	int bColor;
	int defaultColor;
	int backgroundColor;
};


class GCoderStyle: public DocumentStyle
{
public:
	GCoderStyle();
	GCoderStyle(const GCoderStyle& that);

	virtual QString documentType() const override;

	DocumentStyle* clone() const override;

	DocumentStyle& operator=(const DocumentStyle& that) override;
	virtual GCoderStyle& operator=(const GCoderStyle& that);

	// Import from old settings
	void importFromV0(QSettings* cfg);

	QString fontName;
	int fontSize;
	int lineColor;
	int underlineColor;
	HighlightColors hColors;

protected:
	void loadChild(QSettings* cfg) override;
	void saveChild(QSettings* cfg) const override;
};

#endif // GCODERSTYLE_H
