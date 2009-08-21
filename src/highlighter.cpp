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

#include <QtGui>
#include <QString>

#include "highlighter.h"


//**************************************************************************************************
//
//**************************************************************************************************

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    
    
}

//**************************************************************************************************
//
//**************************************************************************************************

void Highlighter::setHColors(const _h_colors hColors, const QFont fnt)
{
   font = fnt;
   highlightColors = hColors;

   HighlightingRule rule;

   keywordFormat.setForeground(QColor(highlightColors.macroColor));
   keywordFormat.setFontWeight(QFont::Bold);
   QStringList keywordPatterns;
   keywordPatterns << "\\b(OR|XOR|AND|NOT|EOR|EQ|NE|GT|LT|GE|LE)\\b"
                   << "\\b(SIN|COS|ATAN|TAN|TRUNC|ROUND|ABS|FIX|SQRT|SQR)\\b"
                   << "\\b(BCD|BIN|ROUND|FUP|MOD|DROUND|DFIX|DFUP|DIV)\\b"
                   << "\\b(IF|THEN|ELSE|ENDIF|END|BEGIN)\\b"
                   << "\\bGOTO[BCF]{0,1}\\b"
                   << "\\b(FORM|CLEAR|DRAW|CHUCK|WORK|BLK|CYCL|MAX|LBL)\\b"
                   << "\\b(GOSUB|RETURN|RETURN|RET|RTS|MODIN|MODOUT|GET|PUT|READ|WRITE|NOEX)\\b"
                   << "\\b(DO|WHILE|FOR|TO|NEXT|REPEAT|UNTIL|WHEN)\\b"
                   << "\\b(VC)[0-9]{1,3}\\b"
                   << "\\b(PROC|SUPA|STOPRE|MSG|SAVE|DISPLOF|SBLOF|PGM)\\b"
                   << "\\b(DEF|VAR|REAL|INT|AXIS|BOOL|CHAR|STRING|FRAME)\\b"
                   << "\\b(MCALL|CALL|TLID|TLFON|AP|RP|PSELECT|PRINT)\\b"
                   << "\\bV[A-Z]{1,4}\\b" 
                   << "\\bDIAMO(N|F)\\b" 
                   << "\\b[A]{0,1}(MIRROR|SCALE|ROT|TRANS)\\b"
                   << "\\b[AC]{0,1}(ROTS)\\b";
   foreach(const QString &pattern, keywordPatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.format = keywordFormat;
      highlightingRules.append(rule);
   };


   progNameFormat.setForeground(QColor(highlightColors.progNameColor));
   progNameFormat.setFontWeight(QFont::Bold);
   QStringList progNamePatterns;
   progNamePatterns << "%\\b(MPF|SPF)[\\s]{0,3}[0-9]{1,4}$\\b"
                    << "^\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB)"
                    << "^(%_N_)[A-Z]{1,1}[A-Z0-9_]{0,30}_(MPF|SPF)$"
                    << "\\b^O[0-9]{1,}\\b"
                    << "^%PM$"
                    << "^;\\$PATH=/[A-Z0-9_]{1,}$"
                    << "^(:)[0-9]{1,}"
                    << "\\b^O[A-Z0-9]{2,}\\b"
                    << "(%M)[0-9]{1,4}"
                    << "(BEGIN|END)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)"
                    << "[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|LIB|SUB)($|\\s)";
   foreach(const QString &pattern, progNamePatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.format = progNameFormat;
      progNameHighlightingRules.append(rule);
   };


}

//**************************************************************************************************
//
//**************************************************************************************************

void Highlighter::highlightBlock(const QString &tx)
{
  int pos, count;
  int sellen;
  QChar ch;
  QString adrress, val;
  QTextCharFormat format;


  format.setFontWeight(QFont::Normal);
  pos = 0;

  if(previousBlockState() == 1)
  {
     if((sellen = tx.indexOf(')', pos)) < 0)
     {
        setCurrentBlockState(1);
        sellen = tx.length();
        setFormat(pos, sellen, QColor(highlightColors.commentColor));
        return;
     };

     sellen = sellen + 1;
     setFormat(pos, sellen, QColor(highlightColors.commentColor));
     setCurrentBlockState(0);
     pos = pos + sellen;
  };


  while(pos < tx.length())
  {

     ch = tx.at(pos).toUpper(); 
     sellen = 1;
     while(TRUE)
     {
        setCurrentBlockState(0);
        if(ch == ';')
        {
           sellen = (tx.length() - pos);
           format.setForeground(QColor( highlightColors.commentColor));
           break;
        };

//***********************************************************************

        if(ch == '(')
        {
           count = 1;
           do
           {  
              if((pos + sellen) >= tx.length())
              {
                 setCurrentBlockState(1);
                 sellen = (tx.length() - pos);
                 break;
              };
              ch = tx.at(pos + sellen);
              sellen++;

              if(ch == '(')
                count++;
              else
                if(ch == ')')
                  count--;

           }while(count > 0);

           format.setForeground(QColor( highlightColors.commentColor));
           break;
        };

//***********************************************************************

        if((ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@')|| (ch == '_'))
        {
           adrress = "";
           do
           {
              adrress.append(ch);
              if((adrress.length() + pos) >= tx.length())
                break;
              ch = tx.at(adrress.length() + pos).toUpper(); 
           }while((ch >= 'A' && ch <= 'Z')|| (ch == '_'));
           sellen = adrress.length();

           val = "";
           while((ch >= '0' && ch <= '9') || (ch == '.') || (ch == '-'))
           {
              val.append(ch);
              if((sellen + val.length() + pos) >= tx.length())
                break;
              ch = tx.at(sellen + val.length() + pos).toUpper();
           };

           sellen = adrress.length() + val.length();



           if(adrress.length() > 1)
           {
              if(ch == '(')
              {
                 setFormat(pos, sellen, highlightColors.macroColor);
                 count = 1;
                 pos = pos + sellen;
                 sellen = 1;
                 do
                 {
                    if((pos + sellen) >= tx.length()) 
                    {
                       setCurrentBlockState(2);
                       sellen = (tx.length() - pos);
                       break;
                    };
                    ch = tx.at(pos + sellen);
                    sellen++;

                    if(ch == '(')
                      count++;
                    else
                      if(ch == ')')
                        count--;

                 }while(count > 0);

                 highlightInside(tx, pos, pos + sellen);
                 pos = pos + sellen;
                 sellen = 0;
              };

              do
              {
                 format.setForeground(QColor(highlightColors.keyWordColor)); 
                 format.setFontWeight(QFont::Normal);

                 foreach(const HighlightingRule &rule, progNameHighlightingRules)
                 {
                    QRegExp expression(rule.pattern);
                    expression.setCaseSensitivity(Qt::CaseInsensitive);

                    if(adrress.contains(expression))
                    {
                       format = rule.format;
                       break;
                    };

                 };

                 foreach(const HighlightingRule &rule, highlightingRules)
                 {
                    QRegExp expression(rule.pattern);
                    expression.setCaseSensitivity(Qt::CaseInsensitive);

                    if(adrress.contains(expression))
                    {
                       format = rule.format;
                       break;
                    };

                 };

              }while(FALSE);


           }
           else
           {
              ch = adrress.at(0);
              switch(ch.toAscii())
              {
                 case 'G'         : format.setForeground(QColor(highlightColors.gColor));
                                    break;
                 case 'M'         : format.setForeground(QColor(highlightColors.mColor));
                                    break;
                 case 'N'         : format.setForeground(QColor(highlightColors.nColor));
                                    break;
                 case 'L'         : format.setForeground(QColor(highlightColors.lColor));
                                    break;
                 case 'A'         : format.setForeground(QColor(highlightColors.aColor));
                                    break;
                 case 'B'         : format.setForeground(QColor(highlightColors.bColor));
                                    break;
                 case 'Z'         : format.setForeground(QColor(highlightColors.zColor));
                                    break;
                 case 'F'         :
                 case 'S'         : format.setForeground(QColor(highlightColors.fsColor));
                                    break;
                 case 'D'         :
                 case 'H'         :
                 case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                    break;
                 case '#'         :
                 case 'Q'         :
                 case 'V'         :
                 case '@'         :
                 case 'R'         : format.setForeground(QColor(highlightColors.rColor));
                                    break;
                 default          : format.setForeground(Qt::black);
              };

           };


           break;
        };

//***********************************************************************

        if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '<') || (ch == '>') || (ch == '$')
            || (ch == '='))
        {

           if((pos + sellen) >= tx.length())
             break;
           ch = tx.at(pos + sellen).toUpper();
           if(ch == '(')
           {
              setFormat(pos, 1, highlightColors.operatorColor);
              pos++;
              count = 1;
                 do
                 {  
                    if((pos + sellen) >= tx.length()) 
                    {
                       setCurrentBlockState(2);
                       sellen = (tx.length() - pos);
                       break;
                    };
                    ch = tx.at(pos + sellen);
                    sellen++;

                    if(ch == '(')
                      count++;
                    else
                      if(ch == ')')
                        count--;

                 }while(count > 0);
                 highlightInside(tx, pos, pos + sellen);
                 pos = pos + sellen;
                 sellen = 0;
           }
           else
             format.setForeground(QColor(highlightColors.operatorColor));
           break;

        };

//***********************************************************************

        if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}'))
        {
           format.setForeground(QColor(highlightColors.operatorColor));
           break;
        };

//***********************************************************************

        if((ch == '"') || (ch == '\''))
        {
           do
           {
              sellen++;
              if((pos + sellen) >= tx.length())
                break; 
              ch = tx.at(sellen + pos).toUpper();

           }while(!((ch == '"') || (ch == '\'') || ((pos + sellen) >= tx.length())));
           sellen++;
           format.setForeground(QColor(highlightColors.commentColor));
           break;
        };


//***********************************************************************

        format.setForeground(Qt::black);
        format.setFontWeight(QFont::Normal);
        break;

     };

     setFormat(pos, sellen, format);
     
     pos = pos + sellen;
     format.setFontWeight(QFont::Normal);

  };

  foreach(const HighlightingRule &rule, progNameHighlightingRules)
  {
     QRegExp expression(rule.pattern);
     int index = expression.indexIn(tx);
     while (index >= 0)
     {
        int length = expression.matchedLength();
        setFormat(index, length, rule.format);
        index = expression.indexIn(tx, index + length);
     };
  };
}

//**************************************************************************************************
//
//**************************************************************************************************

void Highlighter::highlightInside(const QString &tx, int pos, int maxlen)
{
  int sellen;
  QChar ch;
  QString adrress, val;
  QTextCharFormat format;


  while(pos < maxlen)
  {
     ch = tx.at(pos).toUpper();
     sellen = 1;
     while(TRUE)
     {
        setCurrentBlockState(0);

//***********************************************************************

        if((ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@') || (ch == '_'))
        {
           adrress = "";
           do
           {
              adrress.append(ch);
              if((adrress.length() + pos) >= tx.length())
                break;
              ch = tx.at(adrress.length() + pos).toUpper();
           }while((ch >= 'A' && ch <= 'Z') || (ch == '_'));
           sellen = adrress.length();



           val = "";
           while((ch >= '0' && ch <= '9') || (ch == '.') || (ch == '-'))
           {
              val.append(ch);
              if((sellen + val.length() + pos) >= tx.length())
                break;
              ch = tx.at(sellen + val.length() + pos).toUpper();
           };

           sellen = adrress.length() + val.length();



           if(adrress.length() > 1)
           {
              format.setForeground(QColor(highlightColors.keyWordColor)); 
              format.setFontWeight(QFont::Normal);

              foreach(const HighlightingRule &rule, highlightingRules)
              {
                 QRegExp expression(rule.pattern);
                 expression.setCaseSensitivity(Qt::CaseInsensitive);

                 if(adrress.contains(expression))
                 {
                    format = rule.format;
                    break;
                 };

              };
           }
           else
           {
              ch = adrress.at(0);
              switch(ch.toAscii())
              {
                 case 'G'         : format.setForeground(QColor(highlightColors.gColor));
                                    break;
                 case 'M'         : format.setForeground(QColor(highlightColors.mColor));
                                    break;
                 case 'N'         : format.setForeground(QColor(highlightColors.nColor));
                                    break;
                 case 'L'         : format.setForeground(QColor(highlightColors.lColor));
                                    break;
                 case 'A'         : format.setForeground(QColor(highlightColors.aColor));
                                    break;
                 case 'B'         : format.setForeground(QColor(highlightColors.bColor));
                                    break;
                 case 'Z'         : format.setForeground(QColor(highlightColors.zColor));
                                    break;
                 case 'F'         :
                 case 'S'         : format.setForeground(QColor(highlightColors.fsColor));
                                    break;
                 case 'D'         :
                 case 'H'         :
                 case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                    break;
                 case '#'         :
                 case 'Q'         :
                 case '@'         :
                 case 'R'         : format.setForeground(QColor(highlightColors.rColor));
                                    break;
                 default          : format.setForeground(Qt::black);
              };

           };


           break;
        };

//***********************************************************************

        if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '$') || (ch == '<') || (ch == '>')
            || (ch == '='))
        {
           format.setForeground(QColor(highlightColors.operatorColor));
           break;

        };

//***********************************************************************

        if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}') || (ch == '(') || (ch == ')'))
        {
           format.setForeground(QColor(highlightColors.operatorColor));
           break;
        };

//***********************************************************************

        if((ch == '"') || (ch == '\''))
        {
           do
           {
              sellen++;
              if((pos + sellen) >= tx.length())
                break;
              ch = tx.at(sellen + pos).toUpper();

           }while(!((ch == '"') || (ch == '\'') || ((pos + sellen) >= tx.length())));
           sellen++;
           format.setForeground(QColor( highlightColors.commentColor));
           break;
        };

//***********************************************************************

        format.setForeground(Qt::black);
        break;

     };

     setFormat(pos, sellen, format);
     pos = pos + sellen;

  };

}

 //**************************************************************************************************
//
//**************************************************************************************************

 /*void Highlighter::highlightBlock(const QString &text)
 {
    foreach(const HighlightingRule &rule, highlightingRules)
    {
         QRegExp expression(rule.pattern);
         expression.setCaseSensitivity(Qt::CaseInsensitive);

         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
     setCurrentBlockState(0);

     int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = commentStartExpression.indexIn(text);

     while (startIndex >= 0)
     {
         int endIndex = commentEndExpression.indexIn(text, startIndex);
         int commentLength;
         if (endIndex == -1)
         {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else
         {
             commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
         }
         setFormat(startIndex, commentLength, multiLineCommentFormat);
         startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
     }
 }*/


