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

#include <documentwidgetproperties.h>  // IWYU pragma: associated


DocumentWidgetProperties::DocumentWidgetProperties()
{
}

DocumentWidgetProperties::DocumentWidgetProperties(const DocumentWidgetProperties &that)
{
    DocumentWidgetProperties::operator=(that);
}

DocumentWidgetProperties::~DocumentWidgetProperties()
{
}

QString DocumentWidgetProperties::documentType() const
{
    return "Document";
}

DocumentWidgetProperties *DocumentWidgetProperties::clone() const
{
    DocumentWidgetProperties *prop = new DocumentWidgetProperties();
    prop->operator=(*this);
    return prop;
}

DocumentWidgetProperties &DocumentWidgetProperties::operator =(const DocumentWidgetProperties &that)
{
    Q_UNUSED(that);
    return *this;
}

void DocumentWidgetProperties::load(QSettings *cfg)
{
    loadChild(cfg);
}

void DocumentWidgetProperties::save(QSettings *cfg) const
{
    saveChild(cfg);
}

void DocumentWidgetProperties::loadChild(QSettings *cfg)
{
    Q_UNUSED(cfg);
}

void DocumentWidgetProperties::saveChild(QSettings *cfg) const
{
    Q_UNUSED(cfg);
}
