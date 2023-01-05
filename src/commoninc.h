/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of EdytorNC.                                        *
 *                                                                         *
 *   EdytorNC is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMONINC_H
#define COMMONINC_H

#include<QString>
#include<QRegExp>


#define TABBED_MODE              0x01
#define SHOW_FILENAME            0x02
#define SHOW_FILEPATH            0x04
#define SHOW_PROGTITLE           0x08

#define MODE_AUTO                0x00
#define MODE_OKUMA               0x02
#define MODE_FANUC               0x03
#define MODE_HEIDENHAIN          0x04
#define MODE_SINUMERIK           0x05
#define MODE_SINUMERIK_840       0x06
#define MODE_PHILIPS             0x07
#define MODE_HEIDENHAIN_ISO      0x08
#define MODE_TOOLTIPS            0x09
#define MODE_LINUXCNC            0x0A
//#define MODE_FADAL               0x0B


#define FILENAME_SINU840     "%_N_[a-zA-Z0-9_]{1,31}_(MPF|SPF|TEA|COM|PLC|DEF|INI)"
#define FILENAME_OSP         "\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"
#define FILENAME_FANUC       "(?:\\n|\\r)(O|:)[0-9]{1,}"
#define FILENAME_SINU        "%\\b(MPF|SPF|TEA)[\\s]{0,3}[0-9]{1,4}\\b"
#define FILENAME_HEID1       "%[a-zA-Z0-9_]{1,30}(\\s)"
#define FILENAME_HEID2       "(BEGIN)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)"
#define FILENAME_PHIL        "%PM[\\s]{1,}N[0-9]{4,4}"
#define FILENAME_FADAL       "(N1)(O|:)[0-9]{1,}"


struct _h_colors {
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
    int highlightMode;
};

//**************************************************************************************************
//
//**************************************************************************************************

struct _editor_properites {
    bool isRedo, isUndo, isSel;
    bool readOnly;
    bool ins;
    bool modified;
    int cursorPos;
    QString lastDir, dotAdr, i2mAdr;
    QString fileName;
    bool atEnd, dotAfter;
    int dotAftrerCount;
    int i2mprec;
    bool inch;
    QString fontName;
    int fontSize;
    bool intCapsLock;
    bool syntaxH;
    _h_colors hColors;
    QByteArray geometry;
    bool maximized;
    bool underlineChanges;
    int lineColor;
    int underlineColor;
    int windowMode;
    QString calcBinary;
    bool clearUndoHistory;
    bool clearUnderlineHistory;
    bool editorToolTips;
    bool defaultReadOnly;
    QStringList extensions;
    QString saveExtension;
    QString saveDirectory;
    int defaultHighlightMode;
    bool startEmpty;
    QString curFile;  // replace by filename
    QString curFileInfo;
    bool guessFileNameByProgNum;
    bool disableFileChangeMonitor;
    bool changeDateInComment;
};

#endif // COMMONINC_H
