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

#include  <typeinfo>    // for bad_cast

#include <QtGlobal>         // for Q_OS_WIN32 // IWYU pragma: keep
#include <QLatin1String>    // for QLatin1String
#include <QSettings>        // for QSettings
#include <QString>          // for QString
#include <QVariant>         // for QVariant

#include <documentstyle.h>  // for DocumentStyle
#include "gcoder.h"
#include <gcoderstyle.h>    // IWYU pragma: associated


#define CFG_SECTION_GCODESTYLE  "GCoderStyle"
#define CFG_SECTION_COLOR       "Colors"

#define CFG_KEY_COLOR_LINE          "Line"
#define CFG_KEY_COLOR_A             "A"
#define CFG_KEY_COLOR_B             "B"
#define CFG_KEY_COLOR_BACKGROUND    "Background"
#define CFG_KEY_COLOR_COMMENT       "Comment"
#define CFG_KEY_COLOR_DEFAULT       "Default"
#define CFG_KEY_COLOR_DHT           "DHT"
#define CFG_KEY_COLOR_FS            "FS"
#define CFG_KEY_COLOR_G             "G"
#define CFG_KEY_COLOR_KEYWORD       "Keyword"
#define CFG_KEY_COLOR_L             "L"
#define CFG_KEY_COLOR_M             "M"
#define CFG_KEY_COLOR_MACRO         "Macro"
#define CFG_KEY_COLOR_N             "N"
#define CFG_KEY_COLOR_OPERATOR      "Operator"
#define CFG_KEY_COLOR_PROGNAME      "ProgName"
#define CFG_KEY_COLOR_R             "R"
#define CFG_KEY_COLOR_UNDERLINE     "UnderLine"
#define CFG_KEY_COLOR_Z             "Z"
#define CFG_KEY_FONT_NAME           "FontName"
#define CFG_KEY_FONT_SIZE           "FontSize"


GCoderStyle::GCoderStyle()
{
#ifdef Q_OS_WIN32
    fontName = "Courier New";
#else
    fontName = "Monospace";
#endif
    fontSize =  12;
    lineColor =  0xFEFFB6;
    underlineColor =  0x00FF00;
    hColors.commentColor = 0xde0020;
    hColors.gColor = 0x1600ee;
    hColors.mColor = 0x80007d;
    hColors.nColor = 0x808080;
    hColors.lColor = 0x535b5f;
    hColors.fsColor = 0x516600;
    hColors.dhtColor = 0x660033;
    hColors.rColor = 0x24576f;
    hColors.macroColor = 0x000080;
    hColors.keyWordColor = 0x1d8000;
    hColors.progNameColor = 0x000000;
    hColors.operatorColor = 0x9a2200;
    hColors.zColor = 0x000080;
    hColors.aColor = 0x000000;
    hColors.bColor = 0x000000;
    hColors.defaultColor = 0x000000;
    hColors.backgroundColor = 0xFFFFFF;
}

GCoderStyle::GCoderStyle(const GCoderStyle &that) : DocumentStyle()
{
    GCoderStyle::operator=(that);
}

QString GCoderStyle::documentType() const
{
    return GCoder::DOCUMENT_TYPE;
}

DocumentStyle *GCoderStyle::clone() const
{
    GCoderStyle *info = new GCoderStyle();
    info->operator=(*this);
    return info;
}

DocumentStyle &GCoderStyle::operator=(const DocumentStyle &that)
{
    try {
        const GCoderStyle &style = dynamic_cast<const GCoderStyle &>(that);
        return GCoderStyle::operator=(style);
    }  catch (std::bad_cast &e) {
    }

    return DocumentStyle::operator=(that);
}

GCoderStyle &GCoderStyle::operator=(const GCoderStyle &that)
{
    DocumentStyle::operator=(that);

    fontName = that.fontName;
    fontSize = that.fontSize;

    lineColor = that.lineColor;
    underlineColor = that.underlineColor;

    hColors.commentColor = that.hColors.commentColor;
    hColors.gColor = that.hColors.gColor;
    hColors.mColor = that.hColors.mColor;
    hColors.nColor = that.hColors.nColor;
    hColors.lColor = that.hColors.lColor;
    hColors.fsColor = that.hColors.fsColor;
    hColors.dhtColor = that.hColors.dhtColor;
    hColors.rColor = that.hColors.rColor;
    hColors.macroColor = that.hColors.macroColor;
    hColors.keyWordColor = that.hColors.keyWordColor;
    hColors.progNameColor = that.hColors.progNameColor;
    hColors.operatorColor = that.hColors.operatorColor;
    hColors.zColor = that.hColors.zColor;
    hColors.aColor = that.hColors.aColor;
    hColors.bColor = that.hColors.bColor;
    hColors.defaultColor = that.hColors.defaultColor;
    hColors.backgroundColor = that.hColors.backgroundColor;

    return *this;
}

void GCoderStyle::loadChild(QSettings *cfg)
{
    cfg->beginGroup(CFG_SECTION_GCODESTYLE);
    fontName = cfg->value(CFG_KEY_FONT_NAME, fontName).toString();
    fontSize = cfg->value(CFG_KEY_FONT_SIZE, fontSize).toInt();

    cfg->beginGroup(CFG_SECTION_COLOR);
    lineColor = cfg->value(CFG_KEY_COLOR_LINE, lineColor).toInt();
    underlineColor = cfg->value(CFG_KEY_COLOR_UNDERLINE, underlineColor).toInt();

    hColors.commentColor = cfg->value(CFG_KEY_COLOR_COMMENT, hColors.commentColor).toInt();
    hColors.gColor = cfg->value(CFG_KEY_COLOR_G, hColors.gColor).toInt();
    hColors.mColor = cfg->value(CFG_KEY_COLOR_M, hColors.mColor).toInt();
    hColors.nColor = cfg->value(CFG_KEY_COLOR_N, hColors.nColor).toInt();
    hColors.lColor = cfg->value(CFG_KEY_COLOR_L, hColors.lColor).toInt();
    hColors.fsColor = cfg->value(CFG_KEY_COLOR_FS, hColors.fsColor).toInt();
    hColors.dhtColor = cfg->value(CFG_KEY_COLOR_DHT, hColors.dhtColor).toInt();
    hColors.rColor = cfg->value(CFG_KEY_COLOR_R, hColors.rColor).toInt();
    hColors.macroColor = cfg->value(CFG_KEY_COLOR_MACRO, hColors.macroColor).toInt();
    hColors.keyWordColor = cfg->value(CFG_KEY_COLOR_KEYWORD, hColors.keyWordColor).toInt();
    hColors.progNameColor = cfg->value(CFG_KEY_COLOR_PROGNAME, hColors.progNameColor).toInt();
    hColors.operatorColor = cfg->value(CFG_KEY_COLOR_OPERATOR, hColors.operatorColor).toInt();
    hColors.zColor = cfg->value(CFG_KEY_COLOR_Z, hColors.zColor).toInt();
    hColors.aColor = cfg->value(CFG_KEY_COLOR_A, hColors.aColor).toInt();
    hColors.bColor = cfg->value(CFG_KEY_COLOR_B, hColors.bColor).toInt();
    hColors.defaultColor = cfg->value(CFG_KEY_COLOR_DEFAULT, hColors.defaultColor).toInt();
    hColors.backgroundColor = cfg->value(CFG_KEY_COLOR_BACKGROUND, hColors.backgroundColor).toInt();
    cfg->endGroup();
    cfg->endGroup();
}

void GCoderStyle::saveChild(QSettings *cfg) const
{
    cfg->beginGroup(CFG_SECTION_GCODESTYLE);
    cfg->setValue(CFG_KEY_FONT_NAME, fontName);
    cfg->setValue(CFG_KEY_FONT_SIZE, fontSize);

    cfg->beginGroup(CFG_SECTION_COLOR);
    cfg->setValue(CFG_KEY_COLOR_LINE, lineColor);
    cfg->setValue(CFG_KEY_COLOR_UNDERLINE, underlineColor);

    cfg->setValue(CFG_KEY_COLOR_COMMENT, hColors.commentColor);
    cfg->setValue(CFG_KEY_COLOR_G, hColors.gColor);
    cfg->setValue(CFG_KEY_COLOR_M, hColors.mColor);
    cfg->setValue(CFG_KEY_COLOR_N, hColors.nColor);
    cfg->setValue(CFG_KEY_COLOR_L, hColors.lColor);
    cfg->setValue(CFG_KEY_COLOR_FS, hColors.fsColor);
    cfg->setValue(CFG_KEY_COLOR_DHT, hColors.dhtColor);
    cfg->setValue(CFG_KEY_COLOR_R, hColors.rColor);
    cfg->setValue(CFG_KEY_COLOR_MACRO, hColors.macroColor);
    cfg->setValue(CFG_KEY_COLOR_KEYWORD, hColors.keyWordColor);
    cfg->setValue(CFG_KEY_COLOR_PROGNAME, hColors.progNameColor);
    cfg->setValue(CFG_KEY_COLOR_OPERATOR, hColors.operatorColor);
    cfg->setValue(CFG_KEY_COLOR_B, hColors.bColor);
    cfg->setValue(CFG_KEY_COLOR_A, hColors.aColor);
    cfg->setValue(CFG_KEY_COLOR_Z, hColors.zColor);
    cfg->setValue(CFG_KEY_COLOR_DEFAULT, hColors.defaultColor);
    cfg->setValue(CFG_KEY_COLOR_BACKGROUND, hColors.backgroundColor);
    cfg->endGroup();
    cfg->endGroup();
}

void GCoderStyle::importFromV0(QSettings *cfg)
{
    fontName = cfg->value("FontName", fontName).toString();
    fontSize = cfg->value("FontSize", fontSize).toInt();
    lineColor = cfg->value("LineColor", lineColor).toInt();
    underlineColor = cfg->value("UnderlineColor", underlineColor).toInt();

    cfg->beginGroup("Highlight");
    hColors.commentColor = cfg->value("CommentColor", hColors.commentColor).toInt();
    hColors.gColor = cfg->value("GColor", hColors.gColor).toInt();
    hColors.mColor = cfg->value("MColor", hColors.mColor).toInt();
    hColors.nColor = cfg->value("NColor", hColors.nColor).toInt();
    hColors.lColor = cfg->value("LColor", hColors.lColor).toInt();
    hColors.fsColor = cfg->value("FsColor", hColors.fsColor).toInt();
    hColors.dhtColor = cfg->value("DhtColor", hColors.dhtColor).toInt();
    hColors.rColor = cfg->value("RColor", hColors.rColor).toInt();
    hColors.macroColor = cfg->value("MacroColor", hColors.macroColor).toInt();
    hColors.keyWordColor = cfg->value("KeyWordColor", hColors.keyWordColor).toInt();
    hColors.progNameColor = cfg->value("ProgNameColor", hColors.progNameColor).toInt();
    hColors.operatorColor = cfg->value("OperatorColor", hColors.operatorColor).toInt();
    hColors.zColor = cfg->value("ZColor", hColors.zColor).toInt();
    hColors.aColor = cfg->value("AColor", hColors.aColor).toInt();
    hColors.bColor = cfg->value("BColor", hColors.bColor).toInt();
    hColors.defaultColor = cfg->value("DefaultColor", hColors.defaultColor).toInt();
    hColors.backgroundColor = cfg->value("BackgroundColor", hColors.backgroundColor).toInt();
    cfg->endGroup();
}
