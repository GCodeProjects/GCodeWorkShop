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

#include <QSettings>    // for QSettings
#include <QVariant>     // for QVariant
#include <QtGlobal>     // for Q_UNUSED

#include <documentinfo.h>   // IWYU pragma: associated


#define CFG_KEY_INFO_FILE_PATH      "FilePath"
#define CFG_KEY_INFO_GEOMETRY       "Geometry"
#define CFG_KEY_INFO_READ_ONLY      "ReadOnly"


DocumentInfo::DocumentInfo()
{
}

DocumentInfo::~DocumentInfo()
{
}

QString DocumentInfo::documentType() const
{
	return "Document";
}

DocumentInfo *DocumentInfo::clone() const
{
	DocumentInfo *info = new DocumentInfo();
	info->operator=(*this);
	return info;
}

DocumentInfo &DocumentInfo::operator =(const DocumentInfo &that)
{
	filePath = that.filePath;
	geometry = that.geometry;
	readOnly = that.readOnly;
	return *this;
}

void DocumentInfo::load(QSettings *cfg)
{
	filePath = cfg->value(CFG_KEY_INFO_FILE_PATH, filePath).toString();
	geometry = cfg->value(CFG_KEY_INFO_GEOMETRY, geometry).toByteArray();
	readOnly = cfg->value(CFG_KEY_INFO_READ_ONLY, readOnly).toBool();
	loadChild(cfg);
}

void DocumentInfo::save(QSettings *cfg) const
{
	cfg->setValue(DOCUMENTINFO_CFG_KEY_TYPE, documentType());
	cfg->setValue(CFG_KEY_INFO_FILE_PATH, filePath);
	cfg->setValue(CFG_KEY_INFO_GEOMETRY, geometry);
	cfg->setValue(CFG_KEY_INFO_READ_ONLY, readOnly);
	saveChild(cfg);
}

void DocumentInfo::loadChild(QSettings *cfg)
{
	Q_UNUSED(cfg);
}

void DocumentInfo::saveChild(QSettings *cfg) const
{
	Q_UNUSED(cfg);
}
