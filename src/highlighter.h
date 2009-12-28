/***************************************************************************
 *   Copyright (C) 2006-2009 by Artur Kozioł                               *
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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

#include "commoninc.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE



#define MODE_ALL                 0xFFFF
#define MODE_OKUMA               0xFFFF
#define MODE_FANUC               0xFFFF
#define MODE_HEIDENHAIN          0xFFFF
#define MODE_SINUMERIK           0xFFFF
#define MODE_SINUMERIK_840       0xFFFF




class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent);
    void setHColors(const _h_colors hColors, const QFont fnt);

protected:
    void highlightBlock(const QString &text);

private:
    void highlightInside(const QString &tx, int pos, int maxlen);
    _h_colors highlightColors;
    QFont font;




    struct HighlightingRule
     {
         QRegExp pattern;
         QTextCharFormat format;
     };
     QVector<HighlightingRule> highlightRules;
     QVector<HighlightingRule> progNameHighlightRules;
     QVector<HighlightingRule> commentHighlightRules;

     QRegExp commentStartExpression;
     QRegExp commentEndExpression;


     QTextCharFormat keywordFormat;

     QTextCharFormat commentFormat;


     QTextCharFormat operatorFormat;
     QTextCharFormat progNameFormat;
     //QTextCharFormat adressFormat;




};

#endif
