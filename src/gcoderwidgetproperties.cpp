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

#include  <typeinfo>    // for bad_cast

#include <QLatin1String>    // for QLatin1String
#include <QSettings>        // for QSettings
#include <QVariant>         // for QVariant

#include <documentwidgetproperties.h>   // for DocumentWidgetProperties
#include <gcoderwidgetproperties.h>     // IWYU pragma: associated

#include "gcoder.h"
#include "highlightmode.h"  // for MODE_AUTO


#define CFG_SECTION  "GCoderWidgetProperties"

#define CFG_KEY_INTCAPSLOCK             "InternalCapsLock"
#define CFG_KEY_UNDERLINECHANGES        "UnderlineChanges"
#define CFG_KEY_WINDOWMODE              "WindowMode"
#define CFG_KEY_CLEARUNDOREDO           "ClearUndoRedoHistoty"
#define CFG_KEY_CLEARUNDERLINE          "ClearUnderlineHistoty"
#define CFG_KEY_EDITORTOOLTIPS          "EditorToolTips"
#define CFG_KEY_DEFAULTHIGHLIGHTMODE    "DefaultHighlightMode"
#define CFG_KEY_GUESSFILENAMEBYPROGNUM  "GuessFileNameByProgNum"
#define CFG_KEY_CHANGEDATEINCOMMENT     "ChangeDateInComment"
#define CFG_KEY_HIGHLIGHTON             "HighlightOn"


GCoderWidgetProperties::GCoderWidgetProperties()
{
	intCapsLock = true;
	underlineChanges = true;
	windowMode = SHOW_FILENAME | SHOW_FILEPATH | SHOW_PROGTITLE;
	clearUndoHistory = false;
	clearUnderlineHistory = false;
	editorToolTips = true;
	defaultHighlightMode = MODE_AUTO;
	guessFileNameByProgNum = true;
	changeDateInComment = false;
	syntaxH = true;
}

GCoderWidgetProperties::GCoderWidgetProperties(const GCoderWidgetProperties &that) : DocumentWidgetProperties()
{
	GCoderWidgetProperties::operator=(that);
}

QString GCoderWidgetProperties::documentType() const
{
	return GCoder::DOCUMENT_TYPE;
}

DocumentWidgetProperties *GCoderWidgetProperties::clone() const
{
	GCoderWidgetProperties *prop = new GCoderWidgetProperties();
	prop->operator=(*this);
	return prop;
}

DocumentWidgetProperties &GCoderWidgetProperties::operator=(const DocumentWidgetProperties &that)
{
	try {
		const GCoderWidgetProperties &style = dynamic_cast<const GCoderWidgetProperties &>(that);
		return GCoderWidgetProperties::operator=(style);
	}  catch (std::bad_cast &e) {
	}

	return DocumentWidgetProperties::operator=(that);
}

GCoderWidgetProperties &GCoderWidgetProperties::operator=(const GCoderWidgetProperties &that)
{
	DocumentWidgetProperties::operator=(that);

	intCapsLock = that.intCapsLock;
	underlineChanges = that.underlineChanges;
	windowMode = that.windowMode;
	clearUndoHistory = that.clearUndoHistory;
	clearUnderlineHistory = that.clearUnderlineHistory;
	editorToolTips = that.editorToolTips;
	defaultHighlightMode = that.defaultHighlightMode;
	guessFileNameByProgNum = that.guessFileNameByProgNum;
	changeDateInComment = that.changeDateInComment;
	syntaxH = that.syntaxH;

	return *this;
}

void GCoderWidgetProperties::loadChild(QSettings *cfg)
{
	cfg->beginGroup(CFG_SECTION);
	intCapsLock = cfg->value(CFG_KEY_INTCAPSLOCK, intCapsLock).toBool();
	underlineChanges = cfg->value(CFG_KEY_UNDERLINECHANGES, underlineChanges).toBool();
	windowMode = cfg->value(CFG_KEY_WINDOWMODE, windowMode).toInt();
	clearUndoHistory = cfg->value(CFG_KEY_CLEARUNDOREDO, clearUndoHistory).toBool();
	clearUnderlineHistory = cfg->value(CFG_KEY_CLEARUNDERLINE, clearUnderlineHistory).toBool();
	editorToolTips = cfg->value(CFG_KEY_EDITORTOOLTIPS, editorToolTips).toBool();
	defaultHighlightMode = cfg->value(CFG_KEY_DEFAULTHIGHLIGHTMODE, defaultHighlightMode).toInt();
	guessFileNameByProgNum = cfg->value(CFG_KEY_GUESSFILENAMEBYPROGNUM, guessFileNameByProgNum).toBool();
	changeDateInComment = cfg->value(CFG_KEY_CHANGEDATEINCOMMENT, changeDateInComment).toBool();
	syntaxH = cfg->value(CFG_KEY_HIGHLIGHTON, syntaxH).toBool();
	cfg->endGroup();
}

void GCoderWidgetProperties::saveChild(QSettings *cfg) const
{
	cfg->beginGroup(CFG_SECTION);
	cfg->setValue(CFG_KEY_INTCAPSLOCK, intCapsLock);
	cfg->setValue(CFG_KEY_UNDERLINECHANGES, underlineChanges);
	cfg->setValue(CFG_KEY_WINDOWMODE, windowMode);
	cfg->setValue(CFG_KEY_CLEARUNDOREDO, clearUndoHistory);
	cfg->setValue(CFG_KEY_CLEARUNDERLINE, clearUnderlineHistory);
	cfg->setValue(CFG_KEY_EDITORTOOLTIPS, editorToolTips);
	cfg->setValue(CFG_KEY_DEFAULTHIGHLIGHTMODE, defaultHighlightMode);
	cfg->setValue(CFG_KEY_GUESSFILENAMEBYPROGNUM, guessFileNameByProgNum);
	cfg->setValue(CFG_KEY_CHANGEDATEINCOMMENT, changeDateInComment);
	cfg->setValue(CFG_KEY_HIGHLIGHTON, syntaxH);
	cfg->endGroup();
}
