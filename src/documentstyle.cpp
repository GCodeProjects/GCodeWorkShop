/*
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

#include <QtGlobal>     // for Q_UNUSED

#include <documentstyle.h>  // IWYU pragma: associated


DocumentStyle::DocumentStyle()
{
}

DocumentStyle::DocumentStyle(const DocumentStyle& that)
{
	DocumentStyle::operator=(that);
}

DocumentStyle::~DocumentStyle()
{
}

QString DocumentStyle::documentType() const
{
	return "Document";
}

DocumentStyle* DocumentStyle::clone() const
{
	DocumentStyle* style = new DocumentStyle();
	style->operator=(*this);
	return style;
}

DocumentStyle& DocumentStyle::operator =(const DocumentStyle& that)
{
	Q_UNUSED(that);
	return *this;
}

void DocumentStyle::load(QSettings* cfg)
{
	loadChild(cfg);
}

void DocumentStyle::save(QSettings* cfg) const
{
	saveChild(cfg);
}

void DocumentStyle::loadChild(QSettings* cfg)
{
	Q_UNUSED(cfg);
}

void DocumentStyle::saveChild(QSettings* cfg) const
{
	Q_UNUSED(cfg);
}
