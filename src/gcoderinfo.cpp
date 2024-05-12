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

#include  <typeinfo>    // for bad_cast

#include <QLatin1String>    // for QLatin1String
#include <QSettings>        // for QSettings
#include <QString>          // for QString
#include <QVariant>         // for QVariant

#include <documentinfo.h>   // for DocumentInfo

#include "gcoder.h"
#include "gcoderinfo.h"

#define CFG_KEY_INFO_CURSOR_POS     "CursorPosition"
#define CFG_KEY_INFO_HIGHLIGHT_MODE "HighlightMode"


GCoderInfo::GCoderInfo()
{
}

GCoderInfo::GCoderInfo(const GCoderInfo& that)
{
	GCoderInfo::operator=(that);
}

QString GCoderInfo::documentType() const
{
	return GCoder::DOCUMENT_TYPE;
}

DocumentInfo* GCoderInfo::clone() const
{
	GCoderInfo* info = new GCoderInfo();
	info->operator=(*this);
	return info;
}

DocumentInfo& GCoderInfo::operator=(const DocumentInfo& that)
{
	try {
		const GCoderInfo& gcoderInfo = dynamic_cast<const GCoderInfo&>(that);
		return GCoderInfo::operator=(gcoderInfo);
	}  catch (std::bad_cast& e) {
	}

	return DocumentInfo::operator=(that);
}

GCoderInfo& GCoderInfo::operator=(const GCoderInfo& that)
{
	DocumentInfo::operator=(that);
	cursorPos = that.cursorPos;
	highlightMode = that.highlightMode;
	return *this;
}

void GCoderInfo::loadChild(QSettings* cfg)
{
	if (cfg->value(DOCUMENTINFO_CFG_KEY_TYPE).toString() == documentType()) {
		cursorPos = cfg->value(CFG_KEY_INFO_CURSOR_POS, cursorPos).toInt();
		highlightMode = cfg->value(CFG_KEY_INFO_HIGHLIGHT_MODE, highlightMode).toInt();
	}
}

void GCoderInfo::saveChild(QSettings* cfg) const
{
	cfg->setValue(CFG_KEY_INFO_CURSOR_POS, cursorPos);
	cfg->setValue(CFG_KEY_INFO_HIGHLIGHT_MODE, highlightMode);
}
