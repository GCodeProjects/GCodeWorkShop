/***************************************************************************
 *   Copyright (C) 2006-2010 by Artur Kozioł                               *
 *   artkoz@poczta.onet.pl                                                 *
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


struct _h_colors
{
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

};

//**************************************************************************************************
//
//**************************************************************************************************

struct _editor_properites
{
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
   bool tabbedMode;
};




#endif
