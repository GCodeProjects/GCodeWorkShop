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
   
   QStringList keywords;
   QStringList keywordPatterns;
   HighlightingRule rule;
   
   font = fnt;
   highlightColors = hColors;
   
   keywordPatterns.clear();

   commentFormat.setForeground(QColor(highlightColors.commentColor));
   commentFormat.setFontWeight(QFont::Normal);


   keywordFormat.setForeground(QColor(highlightColors.keyWordColor));
   rule.pattern = QRegExp("\\b[A-Z_]{1,1}[A-Z0-9_]{1,}[A-Z_]{0,1}[A-Z0-9_]{0,}\\b");
   rule.format = keywordFormat;
   highlightRules.append(rule);



//   keywordPatterns << "\\b(OR|XOR|AND|NOT|EOR|EQ|NE|GT|LT|GE|LE|IF|THEN|ELSE|ENDIF|END|BEGIN)\\b"
//         << "\\b(SIN|COS|ATAN|TAN|TRUNC|ROUND|ABS|FIX|SQRT|SQR|BCD|BIN|ROUND|FUP|MOD|DROUND|DFIX|DFUP|DIV)\\b"
//         << "\\bGOTO[BCF]{0,1}\\b"
//         << "\\b(FORM|CLEAR|DRAW|CHUCK|WORK|BLK|CYCL|MAX|LBL|DO|WHILE|FOR|TO|NEXT|REPEAT|UNTIL|WHEN)\\b"
//         << "\\b(GOSUB|RETURN|RETURN|RET|RTS|MODIN|MODOUT|GET|PUT|READ|WRITE|NOEX)\\b"
//         << "\\b(VC)[0-9]{1,3}\\b"
//         << "\\b(PROC|SUPA|STOPRE|MSG|SAVE|DISPLOF|SBLOF|PGM|ANG|AR|CHR|RND|AC)\\b"
//         << "\\b(BHC|SQRX|LAA|TG|OG)\\b"
//         << "\\b(DEF|TOOL DEF|TOOL CALL|VAR|REAL|INT|AXIS|BOOL|CHAR|STRING|FRAME|MCALL|CALL|TLID|TLFON|AP|RP|PSELECT|PRINT|EMPTY)\\b"
//         << "\\bV[A-Z]{1,4}|V[A-Z]{1,3}[0-9]{1,1}\\b"
//         << "\\bDIAMO(N|F)\\b"
//         << "\\b[A]{0,1}(MIRROR|SCALE|ROT|TRANS)\\b"
//         << "\\b[AC]{0,1}(ROTS)\\b";



   //OKUMA
   keywords << "\\b(OR|XOR|AND|NOT|EOR|EQ|NE|GT|LT|GE|LE|IF|THEN|ELSE|ENDIF|END|BEGIN)\\b"
         << "\\b(SIN|COS|ATAN|TAN|TRUNC|ROUND|ABS|FIX|SQRT|SQR|BCD|BIN|ROUND|FUP|MOD|DROUND|DFIX|DFUP|DIV)\\b"
         << "\\bGOTO\\b"
         << "\\b(RTS|MODIN|MODOUT|NOEX|NOCYL)\\b"
         << "\\b(VC)[0-9]{1,3}\\b"
         << "\\b(BHC |SQRX |LAA |TG|OG)\\b"
         << "\\b(CLEAR|DRAW|DEF WORK|DIREC|CYLNDR|PT|LF|LC)\\b"
         << "\\b(CALL|TLID|TLFON|TLFOFF|PSELECT|PRINT|EMPTY)\\b"
         << "\\bV[A-Z]{1,4}|V[A-Z]{1,3}[0-9]{1,1}\\b";
   keywordPatterns.append(keywords);
   
   //SINUMERIK 840D
   keywords << "\\b(OR|XOR|AND|NOT|EOR|IF|THEN|ELSE|ENDIF|END|BEGIN)\\b"
         << "\\b(SIN|COS|ATAN|TAN|TRUNC|ROUND|ABS|FIX|SQRT|SQR|BCD|BIN|ROUND|FUP|MOD|DROUND|DFIX|DFUP|DIV)\\b"
         << "\\b(DO|WHILE|FOR|TO|NEXT|REPEAT|UNTIL|WHEN)\\b"
         << "\\b(GOSUB|RETURN|RETURN|RET|GET|PUT|READ|WRITE)\\b"
         << "\\b(PROC|SUPA|STOPRE|MSG|SAVE|DISPLOF|SBLOF|PGM|ANG|AR|CHR|RND|AC)\\b"
         << "\\b(VAR|MCALL|CALL|AP|RP|PRINT)\\b"
         << "\\b(DEF )(REAL |INT |AXIS |BOOL |CHAR |STRING |FRAME )\\b"
         << "\\bDIAMO(N|F)\\b"
         << "\\b[A]{0,1}(MIRROR|SCALE|ROT|TRANS)\\b"
         << "\\b[AC]{0,1}(ROTS)\\b";
   keywordPatterns.append(keywords);



   //HEIDENHAIN
   keywords << "\\b(CALL PGM |CALL LBL |TOOL CALL |BLK FORM |LBL )\\b"
         << "\\b(CYCL DEF|SET UP|DEPTH|PECKG|RADIUS|DWELL|PECKING|CIRCULAR|POCKET|POS.-CYCLE CIRCLE)\\b"
         << "\\b(CONTOUR|GEOM|MILLG|LABEL|SLOT|ROUGH-OUT|ALLOW|ANGLE|ROTATION|IROT|REP|TRAIN|IMAGE)\\b"
         << "\\b(CYLINDER SURFACE|DATUM|SHIFT)\\b"
         << "\\b(TOOL DEF|CC|FN|CT|LP|PR|IPA|PA|RR|RL|DR|DL|CR|LEN|DEP|LN|APPR|CP|FL)\\b"
         << "\\b(FC|FSELECT|FPOL|CTP|CHF|LCT|DEP|FCT|AN|ERROR)\\b"
         << "\\b(EQU|NE|GT|LT|GE|LE|GOTO|IF)\\b"
         //<< "\\b(DR|DL)[+- ]{1,1}\\b"
         << "\\b(MAX)\\b";
   keywordPatterns.append(keywords);
   
   
   
   keywordFormat.setForeground(QColor(highlightColors.macroColor));
   keywordFormat.setFontWeight(QFont::Bold);
   foreach(const QString &pattern, keywordPatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.format = keywordFormat;
      highlightRules.append(rule);
   };


   progNameFormat.setForeground(QColor(highlightColors.progNameColor));
   progNameFormat.setFontWeight(QFont::Bold);
   QStringList progNamePatterns;
   progNamePatterns << "%\\b(MPF|SPF)[\\s]{0,3}[0-9]{1,4}$\\b"
         << "^\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"
         << "^(%_N_)[A-Z]{1,1}[A-Z0-9_]{0,30}_(MPF|SPF)$"
         << "\\b^O[0-9]{1,}\\b"
         << "^%PM$"
         << "^;\\$PATH=/[A-Z0-9_//]{1,}$"
         << "^(:)[0-9]{1,}"
         << "\\bO[A-Z0-9]{2,}\\b"
         << "(%M)[0-9]{1,4}"
         << "(BEGIN|END)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)"
         << "[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|LIB|SUB)($|\\s)";
   foreach(const QString &pattern, progNamePatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.format = progNameFormat;
      progNameHighlightRules.append(rule);
   };

   commentStartExpression = QRegExp("\\s\\(|^\\(");
   commentEndExpression = QRegExp("\\)");
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


   foreach(HighlightingRule rule, highlightRules)
   {
      QRegExp expression(rule.pattern);
      int index = tx.indexOf(expression);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, rule.format);
         index = tx.indexOf(expression, index + length);
      }
   };

   foreach(HighlightingRule rule, commentHighlightRules)
   {
      QRegExp expression(rule.pattern);
      int index = tx.indexOf(expression);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, rule.format);
         if((index + length) >= tx.length())
           return;
         index = tx.indexOf(expression, index + length);
      }
   };

   foreach(const HighlightingRule &rule, progNameHighlightRules)
   {
      QRegExp expression(rule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, rule.format);
         //if((index + length) >= tx.length())
           //return;

         index = expression.indexIn(tx, index + length);
      };
   };


   setCurrentBlockState(0);
   int startIndex = 0;
   if(previousBlockState() != 1)
      startIndex = tx.indexOf(commentStartExpression);

   while(startIndex >= 0)
   {
      int endIndex = tx.indexOf(commentEndExpression, startIndex);
      int commentLength;
      if(endIndex == -1)
      {
         setCurrentBlockState(1);
         commentLength = tx.length() - startIndex;
         setFormat(startIndex, commentLength, commentFormat);
         return;
      }
      else
      {
         commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
      }
      setFormat(startIndex, commentLength, commentFormat);
      //pos = startIndex + commentLength;
      startIndex = tx.indexOf(commentStartExpression, startIndex + commentLength);
   }

   while(pos < tx.length())
   {

      ch = tx.at(pos);
      sellen = 1;
      while(true)
      {
         if(ch == ';')
         {
            if(tx.at(pos + 1) == '$') 
              break;
            sellen = (tx.length() - pos);
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
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
            break;
         };

         //***********************************************************************

         if((ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@') || (ch == '_'))
         {
            adrress = "";
            do
            {
               adrress.append(ch);
               if((adrress.length() + pos) >= tx.length())
                  break;
               ch = tx.at(adrress.length() + pos);
            }while((ch >= 'A' && ch <= 'Z') || (ch == '_'));
            sellen = adrress.length();

            val = "";
            while((ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
            {
               val.append(ch);
               if((sellen + val.length() + pos) >= tx.length())
                  break;
               ch = tx.at(sellen + val.length() + pos);
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
               break;

            }
            else
            {
               ch = adrress.at(0);
               switch(ch.toAscii())
               {
                  case 'G'         : format.setForeground(QColor(highlightColors.gColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'M'         : format.setForeground(QColor(highlightColors.mColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'N'         : format.setForeground(QColor(highlightColors.nColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'L'         : format.setForeground(QColor(highlightColors.lColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'A'         : format.setForeground(QColor(highlightColors.aColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'B'         : format.setForeground(QColor(highlightColors.bColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'Z'         : format.setForeground(QColor(highlightColors.zColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'F'         :
                  case 'S'         : format.setForeground(QColor(highlightColors.fsColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'D'         :
                  case 'H'         :
                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case '#'         :
                  case 'Q'         :
                  case 'V'         :
                  case '@'         :
                  case 'R'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'C'         :
                  case 'E'         :
                  case 'I'         :
                  case 'J'         :
                  case 'K'         :
                  case 'P'         :
                  case 'X'         :
                  case 'Y'         :
                  case 'U'         :
                  case 'W'         : format.setForeground(Qt::black);
                                     setFormat(pos, sellen, format);
                                     break;
                  default          : ;
               };


               break;
            };
            break;
         };

         //***********************************************************************

         if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '<') || (ch == '>')
            || (ch == '='))
            {

            if((pos + sellen) >= tx.length())
               break;
            ch = tx.at(pos + sellen);
            if(ch == '(')
            {
               setFormat(pos, 1, highlightColors.operatorColor);
               pos++;
               count = 1;
               do
               {
                  if((pos + sellen) >= tx.length())
                  {
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
            {
               format.setForeground(QColor(highlightColors.operatorColor));
               setFormat(pos, sellen, format);
            };
            //setFormat(pos, sellen, format);
            break;

         };

         //***********************************************************************

         if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}'))
         {
            format.setForeground(QColor(highlightColors.operatorColor));
            setFormat(pos, sellen, format);
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
               ch = tx.at(sellen + pos);

            }while(!((ch == '"') || (ch == '\'') || ((pos + sellen) >= tx.length())));
            sellen++;
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };


         //***********************************************************************

         format.setForeground(Qt::black);
         format.setFontWeight(QFont::Normal);
         //setFormat(pos, sellen, format);
         break;

      };

      pos = pos + sellen;
      format.setFontWeight(QFont::Normal);

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
      ch = tx.at(pos);
      sellen = 1;
      while(TRUE)
      {
         if((ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@') || (ch == '_'))
         {
            adrress = "";
            do
            {
               adrress.append(ch);
               if((adrress.length() + pos) >= tx.length())
                  break;
               ch = tx.at(adrress.length() + pos);
            }while((ch >= 'A' && ch <= 'Z') || (ch == '_'));
            sellen = adrress.length();


            val = "";
            while((ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
            {
               val.append(ch);
               if((sellen + val.length() + pos) >= tx.length())
                  break;
               ch = tx.at(sellen + val.length() + pos);
            };

            sellen = adrress.length() + val.length();



            if(adrress.length() > 1)
            {
               format.setForeground(QColor(highlightColors.keyWordColor));
               format.setFontWeight(QFont::Normal);

               foreach(const HighlightingRule &rule, highlightRules)
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
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'M'         : format.setForeground(QColor(highlightColors.mColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'N'         : format.setForeground(QColor(highlightColors.nColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'L'         : format.setForeground(QColor(highlightColors.lColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'A'         : format.setForeground(QColor(highlightColors.aColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'B'         : format.setForeground(QColor(highlightColors.bColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'Z'         : format.setForeground(QColor(highlightColors.zColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'F'         :
                  case 'S'         : format.setForeground(QColor(highlightColors.fsColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'D'         :
                  case 'H'         :
                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case '#'         :
                  case 'Q'         :
                  case 'V'         :
                  case '@'         :
                  case 'R'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'C'         :
                  case 'E'         :
                  case 'I'         :
                  case 'J'         :
                  case 'K'         :
                  case 'P'         :
                  case 'X'         :
                  case 'Y'         :
                  case 'U'         :
                  case 'W'         : format.setForeground(Qt::black);
                                     setFormat(pos, sellen, format);
                                     break;
                  default          : ;
               };
               break;

            };

            break;
         };

         //***********************************************************************

         if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '$') || (ch == '<') || (ch == '>')
            || (ch == '='))
            {
            format.setForeground(QColor(highlightColors.operatorColor));
            setFormat(pos, sellen, format);
            break;

         };

         //***********************************************************************

         if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}') || (ch == '(') || (ch == ')'))
         {
            format.setForeground(QColor(highlightColors.operatorColor));
            setFormat(pos, sellen, format);
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
               ch = tx.at(sellen + pos);

            }while(!((ch == '"') || (ch == '\'') || ((pos + sellen) >= tx.length())));
            sellen++;
            format.setForeground(QColor( highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };

         //***********************************************************************

         format.setForeground(Qt::black);
         setFormat(pos, sellen, format);
         break;

      };

      pos = pos + sellen;

   };

}

 //**************************************************************************************************
//
//**************************************************************************************************

