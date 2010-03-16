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

   loadRules();
}

//**************************************************************************************************
//
//**************************************************************************************************

void Highlighter::loadRules()
{
   QStringList keywords;
   QStringList keywordPatterns;
   HighlightingRule rule;
   ProgNameHighlightingRule pogRule;
   QString pattern;


   keywordPatterns.clear();
   highlightRules.clear();
   commentHighlightRules.clear();
   progNameHighlightRules.clear();

   progNameFormat.setForeground(QColor(highlightColors.progNameColor));
   progNameFormat.setFontWeight(QFont::Bold);
   pogRule.format = progNameFormat;

   commentFormat.setForeground(QColor(highlightColors.commentColor));
   commentFormat.setFontWeight(QFont::Normal);


   if(highlightColors.highlightMode == MODE_SINUMERIK_840 || highlightColors.highlightMode == MODE_OKUMA)  // variables names
   {
      keywordFormat.setForeground(QColor(highlightColors.keyWordColor));
      keywordFormat.setFontWeight(QFont::Normal);
      rule.pattern = QRegExp("\\b[A-Z_]{1,1}[A-Z0-9_]{1,}[A-Z_]{0,1}[A-Z0-9_]{0,}\\b");
      rule.format = keywordFormat;
      highlightRules.append(rule);
   };


//   if(highlightColors.highlightMode == MODE_SINUMERIK_840 || highlightColors.highlightMode == MODE_HEIDENHAIN)  // ; comment
//   {
//      QString pattern = ";(?!\\$).*$";
//      rule.pattern = QRegExp(pattern);
//
//      keywordFormat.setForeground(QColor(highlightColors.commentColor));
//      keywordFormat.setFontWeight(QFont::Normal);
//      rule.format = keywordFormat;
//      commentHighlightRules.append(rule);
//   };

   if(highlightColors.highlightMode == MODE_OKUMA || highlightColors.highlightMode == MODE_PHILIPS || highlightColors.highlightMode == MODE_FANUC || highlightColors.highlightMode == MODE_SINUMERIK)  // () comment
   {
      commentStartExpression = QRegExp("\\s\\(|^\\(");
      commentEndExpression = QRegExp("\\)");
   }
   else
   {
      commentStartExpression = QRegExp("][");
      commentEndExpression = QRegExp("][");
   };

   if(highlightColors.highlightMode == MODE_FANUC)  //FANUC
   {
      keywords << "(OR|XOR|AND|NOT|EOR|EQ|NE|GT|LT|GE|LE|IF|THEN|ELSE)"
               << "(SIN|COS|ATAN|TAN|TRUNC|ROUND|ABS|FIX|SQRT|SQR|BCD|BIN|ROUND|FUP|MOD|DROUND|DFIX|DFUP|DIV)"
               << "(GOTO|END|DO)[0-9]{1,5}"
               << "(DPRINT|WHILE)";
      keywordPatterns.append(keywords);

      pattern = "\\b^O[0-9]{1,}\\b"; // FANUC
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

      pattern = "^(:)[0-9]{1,}";  // FANUC
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);
   };


   if(highlightColors.highlightMode == MODE_OKUMA)  //OKUMA
   {
      keywords << "\\b(ABS|DGRDX|GRDY|NOT|RTMDI)\\b"
               << "\\b(AND|DGRDY|GRER|OR|RTI)\\b"
               << "\\b(AG|DIN|GROF|PCIR|RTS)\\b"
               << "\\b(ARC|DRAW|GRON|PMIL|SAVE)\\b"
               << "\\b(ATAN|DROUND|GRSK|PMILR|SIN)\\b"
               << "\\b(ATAN2|EIN|GRST|PRINT|SPRINT)\\b"
               << "\\b(BCD|EMPTY|GT|RCIRI|SQRT)\\b"
               << "\\b(BHC|EQ|HA|RCIRO|SQRX)\\b"
               << "\\b(BIN|EOR|HB|RMILI|SQRY)\\b"
               << "\\b(CALL|FA|HC|RMILO|TAN)\\b"
               << "\\b(CLEAR|FB|IF|RH|TLCO)\\b"
               << "\\b(COPY|FC|LAA|ROUND|TLFR)\\b"
               << "\\b(COPYE|FIX|LE|RP|TLFOFF)\\b"
               << "\\b(COS|FMILF|LPRINT|RQNFL|TLFON)\\b"
               << "\\b(DA|FMILR|LT|RS|TLOK)\\b"
               << "\\b(DB|FUP|MITCAN|RSQCO|TN)\\b"
               << "\\b(DC|GE|MOD|RSQRI)\\b"
               << "\\b(DEF|GOTO|MODIN|RSQRO)\\b"
               << "\\b(DELETE|GRCI|MODOUT|RSTRT)\\b"
               << "\\b(DFIX|GRCV|MSG|RT)\\b"
               << "\\b(DFUP|GRDX|NE|RTMCR)\\b"
               << "\\b(VC)[0-9]{1,3}\\b"
               << "\\b(TG|OG)\\b"
               << "\\b(CLEAR|DRAW|DEF WORK|DIREC|CYLNDR|PT|LF|LC)\\b"
               << "\\b(PSELECT)\\b"
               << "\\bV[A-Z]{1,4}|V[A-Z]{1,3}[0-9]{1,1}\\b";
      keywordPatterns.append(keywords);

      pattern = "^\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}";  // OKUMA OSP
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

      pattern = "\\b(^O| O)[A-Z0-9]{2,}\\b";  // OKUMA
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

      pattern = "[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)($|\\s)";  // OKUMA
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);
   };


   if(highlightColors.highlightMode == MODE_SINUMERIK_840)  //SINUMERIK 840D
   {
      keywords << "\\b(OR|XOR|AND|NOT|EOR|IF|THEN|ELSE|ENDIF|END|BEGIN|DEFINE|AS)\\b"
               << "\\b(SIN|COS|ATAN|TAN|TRUNC|ROUND|ABS|FIX|SQRT|SQR|BCD|BIN|ROUND|FUP|MOD|DROUND|DFIX|DFUP|DIV)\\b"
               << "\\b(DO|WHILE|FOR|TO|NEXT|REPEAT|UNTIL|WHEN)\\b"
               << "\\b(GOSUB|RETURN|RETURN|RET|GET|PUT|READ|WRITE)\\b"
               << "\\b(PROC|SUPA|STOPRE|MSG|SAVE|DISPLOF|SBLOF|PGM|ANG|AR|CHR|RND|AC)\\b"
               << "\\b(VAR|MCALL|CALL|AP|RP|PRINT)\\b"
               << "\\b(DEF ){0,1}(REAL |INT |AXIS |BOOL |CHAR |STRING |FRAME|STRING\\[[0-9]{1,3}\\] )\\b"
               << "\\bDIAMO(N|F)|GOTO(B|F)\\b"
               << "\\b[A]{0,1}(MIRROR|SCALE|ROT|TRANS)\\b"
               << "\\b[AC]{0,1}(ROTS)\\b"
               << "\\$\\b[MSTPAV]{1,1}[NCADM]{0,1}(_)[A-Z0-9_]{1,}\\b";
      keywordPatterns.append(keywords);

      pattern = "^(%_N_)[A-Z]{1,1}[A-Z0-9_]{0,30}_(MPF|SPF|TEA|COM|INI|PLC|DEF)$"; // SINUMERIK >=810D
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

      pattern = "^;\\$PATH=/[A-Z0-9_//]{1,}$";  // SINUMERIK >=810D path
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

   };


   if(highlightColors.highlightMode == MODE_SINUMERIK)  //SINUMERIK OLD 810-880
   {
      keywords << "@[0-9]{3,3}";
      keywordPatterns.append(keywords);

      pattern = "%\\b(MPF|SPF|TEA)[\\s]{0,3}[0-9]{1,4}$\\b";
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

   };


   if(highlightColors.highlightMode == MODE_PHILIPS)  // PHILIPS SYSTEM 2000
   {
      keywords << "\\b(@714)\\b";
      keywordPatterns.append(keywords);;

      pattern = "^%PM$";
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

   };


   if(highlightColors.highlightMode == MODE_HEIDENHAIN_ISO)  //HEIDENHAIN ISO
   {
      keywords << "\\b(@714)\\b";
      keywordPatterns.append(keywords);

      pattern = "%F[0-9]{1,4}";
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);

   };

   if(highlightColors.highlightMode == MODE_HEIDENHAIN)  //HEIDENHAIN DIALOG
   {
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

      pattern = "(BEGIN|END)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)";  // HEIDENHAIN
      pogRule.pattern = QRegExp(pattern);
      progNameHighlightRules.append(pogRule);
   };



   keywordFormat.setForeground(QColor(highlightColors.macroColor));
   keywordFormat.setFontWeight(QFont::Bold);
   foreach(const QString &pattern, keywordPatterns)
   {
      rule.pattern = QRegExp(pattern);
      rule.format = keywordFormat;
      highlightRules.append(rule);
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void Highlighter::highlightBlock(const QString &tx)
{
   switch(highlightColors.highlightMode)
   {
      case MODE_OKUMA            : highlightBlockOkumaRule(tx);
                                   break;
      case MODE_FANUC            : highlightBlockFanucRule(tx);
                                   break;
      case MODE_SINUMERIK_840    : highlightBlockSinuRule(tx);
                                   break;
      case MODE_PHILIPS          :
      case MODE_SINUMERIK        : highlightBlockSinuOldRule(tx);
                                   break;
      case MODE_HEIDENHAIN       : highlightBlockHeidRule(tx);
                                   break;
      case MODE_HEIDENHAIN_ISO   : highlightBlockHeidIsoRule(tx);
                                   break;
      default                    : ;

   };

}

//**************************************************************************************************
// FANUC
//**************************************************************************************************

void Highlighter::highlightBlockFanucRule(const QString &tx)
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

   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
   {
      QRegExp expression(progRule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, progRule.format);
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
            setFormat(pos, sellen, highlightColors.commentColor);
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
               break;
            }
            else
            {
               if(tx.isEmpty())
                  break;
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
                  case 'R'         :
                  case 'Q'         :
                  case '#'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'C'         :                              
                  case '@'         :
                  case 'V'         :
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

            format.setForeground(QColor(highlightColors.operatorColor));
            setFormat(pos, sellen, format);
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
         break;

      };

      format.setForeground(Qt::black);
      format.setFontWeight(QFont::Normal);
      pos = pos + sellen;
      format.setFontWeight(QFont::Normal);

   };

}

//**************************************************************************************************
// SINUMERIK 810D-840D
//**************************************************************************************************

void Highlighter::highlightBlockSinuRule(const QString &tx)
{
   int pos;
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

   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
   {
      QRegExp expression(progRule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, progRule.format);
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
            if(tx.length() >= (pos + 2))
               if(tx.at(pos + 1) == '$')
                  break;
            sellen = (tx.length() - pos);
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };

         //***********************************************************************

         if((ch >= 'A' && ch <= 'Z') || (ch == '_'))
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
            while((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
            {
               val.append(ch);
               if((sellen + val.length() + pos) >= tx.length())
                  break;
               ch = tx.at(sellen + val.length() + pos);
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  sellen = adrress.length() + 1;

                  break;
               };
            };
            adrress = adrress + val;
            sellen = adrress.length();

            if(adrress.contains(QRegExp("[A-Z_]{2,}")))
            {
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  setFormat(pos, sellen, format);
               };

               if(ch == '(')
               {
                  format.setForeground(QColor(highlightColors.macroColor));
                  format.setFontWeight(QFont::Normal);
                  setFormat(pos, sellen, format);
               };

               pos = pos + sellen;
               sellen = 0;
               break;

            }
            else
            {
               if(tx.isEmpty())
                  break;
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
                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                     setFormat(pos, sellen, format);
                                     break;

                  case 'R'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case '#'         :
                  case 'Q'         :
                  case 'V'         :
                  case '@'         :
                  case 'C'         :
                  case 'E'         :
                  case 'H'         :
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
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };
         break;
      };

      format.setForeground(Qt::black);
      format.setFontWeight(QFont::Normal);
      pos = pos + sellen;
   };

}

//**************************************************************************************************
// OKUMA OSP
//**************************************************************************************************

void Highlighter::highlightBlockOkumaRule(const QString &tx)
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



   while(pos < tx.length())
   {

      ch = tx.at(pos);
      sellen = 1;
      while(true)
      {
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

            setFormat(pos, sellen, highlightColors.commentColor);
            break;
         };

         //***********************************************************************

         if((ch >= 'A' && ch <= 'Z') || (ch == '_'))
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
            while((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
            {
               val.append(ch);
               if((sellen + val.length() + pos) >= tx.length())
                  break;
               ch = tx.at(sellen + val.length() + pos);
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  sellen = adrress.length() + 1;

                  break;
               };
            };
            adrress = adrress + val;
            sellen = adrress.length();

            if(adrress.contains(QRegExp("[A-Z_]{2,}")))
            {
               pos = pos + sellen;
               sellen = 0;
               break;

            }
            else
            {
               if(tx.isEmpty())
                  break;
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

                  case 'Q'         :
                  case 'R'         :
                  case 'V'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'C'         :
                  case '#'         :
                  case '@'         :
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
            format.setForeground(QColor(highlightColors.operatorColor));
            setFormat(pos, sellen, format);
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
         break;

      };

      format.setForeground(Qt::black);
      format.setFontWeight(QFont::Normal);
      pos = pos + sellen;
   };

   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
   {
      QRegExp expression(progRule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, progRule.format);
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

}

//**************************************************************************************************
// SINUMERIK OLD 810-880
//**************************************************************************************************

void Highlighter::highlightBlockSinuOldRule(const QString &tx)
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

   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
   {
      QRegExp expression(progRule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, progRule.format);
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
            setFormat(pos, sellen, highlightColors.commentColor);
            break;
         };

         //***********************************************************************

         if((ch >= 'A' && ch <= 'Z') || (ch == '_'))
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
               break;
            }
            else
            {
               if(tx.isEmpty())
                  break;
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
                  case 'K'         :
                  case 'R'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'C'         :
                  case 'Q'         :
                  case '#'         :
                  case 'V'         :
                  case 'E'         :
                  case 'I'         :
                  case 'J'         :   
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

            format.setForeground(QColor(highlightColors.operatorColor));
            setFormat(pos, sellen, format);
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
         break;

      };

      format.setForeground(Qt::black);
      format.setFontWeight(QFont::Normal);
      pos = pos + sellen;
      format.setFontWeight(QFont::Normal);

   };

}

//**************************************************************************************************
// HEIDENHAIN DIALOG
//**************************************************************************************************

void Highlighter::highlightBlockHeidRule(const QString &tx)
{
   int pos;
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

   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
   {
      QRegExp expression(progRule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, progRule.format);
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
            if(tx.length() >= (pos + 2))
               if(tx.at(pos + 1) == '$')
                  break;
            sellen = (tx.length() - pos);
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };

         //***********************************************************************

         if((ch >= 'A' && ch <= 'Z') || (ch == '_'))
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
            while((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
            {
               val.append(ch);
               if((sellen + val.length() + pos) >= tx.length())
                  break;
               ch = tx.at(sellen + val.length() + pos);
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  sellen = adrress.length() + 1;

                  break;
               };
            };
            adrress = adrress + val;
            sellen = adrress.length();

            if(adrress.contains(QRegExp("[A-Z_]{2,}")))
            {
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  setFormat(pos, sellen, format);
               };

               if(ch == '(')
               {
                  format.setForeground(QColor(highlightColors.macroColor));
                  format.setFontWeight(QFont::Normal);
                  setFormat(pos, sellen, format);
               };

               pos = pos + sellen;
               sellen = 0;
               break;

            }
            else
            {
               if(tx.isEmpty())
                  break;
               ch = adrress.at(0);
               switch(ch.toAscii())
               {
                  case 'L'         : format.setForeground(QColor(highlightColors.gColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'M'         : format.setForeground(QColor(highlightColors.mColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'N'         : format.setForeground(QColor(highlightColors.nColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case 'R'         : format.setForeground(QColor(highlightColors.lColor));
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
                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                     setFormat(pos, sellen, format);
                                     break;

                  case 'Q'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case '#'         :
                  case 'G'         :
                  case 'V'         :
                  case '@'         :
                  case 'C'         :
                  case 'E'         :
                  case 'H'         :
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
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };
         break;
      };

      format.setForeground(Qt::black);
      format.setFontWeight(QFont::Normal);
      pos = pos + sellen;
   };

}

//**************************************************************************************************
// HEIDENHAIN ISO
//**************************************************************************************************

void Highlighter::highlightBlockHeidIsoRule(const QString &tx)
{
   int pos;
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
            if(tx.length() >= (pos + 2))
               if(tx.at(pos + 1) == '$')
                  break;
            sellen = (tx.length() - pos);
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };

         //***********************************************************************

         if((ch >= 'A' && ch <= 'Z') || (ch == '_'))
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
            while((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
            {
               val.append(ch);
               if((sellen + val.length() + pos) >= tx.length())
                  break;
               ch = tx.at(sellen + val.length() + pos);
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  sellen = adrress.length() + 1;

                  break;
               };
            };
            adrress = adrress + val;
            sellen = adrress.length();

            if(adrress.contains(QRegExp("[A-Z_]{2,}")))
            {
               if(ch == ':')
               {
                  format.setForeground(QColor(highlightColors.keyWordColor));
                  format.setFontWeight(QFont::Bold);
                  setFormat(pos, sellen, format);
               };

               if(ch == '(')
               {
                  format.setForeground(QColor(highlightColors.macroColor));
                  format.setFontWeight(QFont::Normal);
                  setFormat(pos, sellen, format);
               };

               pos = pos + sellen;
               sellen = 0;
               break;

            }
            else
            {
               if(tx.isEmpty())
                  break;
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
                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
                                     setFormat(pos, sellen, format);
                                     break;

                  case 'Q'         : format.setForeground(QColor(highlightColors.rColor));
                                     setFormat(pos, sellen, format);
                                     break;
                  case '#'         :
                  case 'R'         :
                  case 'V'         :
                  case '@'         :
                  case 'C'         :
                  case 'E'         :
                  case 'H'         :
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
            format.setForeground(QColor(highlightColors.commentColor));
            setFormat(pos, sellen, format);
            break;
         };
         break;
      };

      format.setForeground(Qt::black);
      format.setFontWeight(QFont::Normal);
      pos = pos + sellen;
   };

   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
   {
      QRegExp expression(progRule.pattern);
      int index = expression.indexIn(tx);
      while(index >= 0)
      {
         int length = expression.matchedLength();
         setFormat(index, length, progRule.format);
         index = expression.indexIn(tx, index + length);
      };
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

//void Highlighter::highlightInside(const QString &tx, int pos, int maxlen)
//{
//   int sellen;
//   QChar ch;
//   QString adrress, val;
//   QTextCharFormat format;
//
//
//   while(pos < maxlen)
//   {
//      ch = tx.at(pos);
//      sellen = 1;
//      while(TRUE)
//      {
//         if((ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@') || (ch == '_'))
//         {
//            adrress = "";
//            do
//            {
//               adrress.append(ch);
//               if((adrress.length() + pos) >= tx.length())
//                  break;
//               ch = tx.at(adrress.length() + pos);
//            }while((ch >= 'A' && ch <= 'Z') || (ch == '_'));
//            sellen = adrress.length();
//
//
//            val = "";
//            while((ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
//            {
//               val.append(ch);
//               if((sellen + val.length() + pos) >= tx.length())
//                  break;
//               ch = tx.at(sellen + val.length() + pos);
//            };
//
//            sellen = adrress.length() + val.length();
//
//
//
//            if(adrress.length() > 1)
//            {
//               format.setForeground(QColor(highlightColors.keyWordColor));
//               format.setFontWeight(QFont::Normal);
//
//               foreach(const HighlightingRule &rule, highlightRules)
//               {
//                  QRegExp expression(rule.pattern);
//                  expression.setCaseSensitivity(Qt::CaseInsensitive);
//
//                  if(adrress.contains(expression))
//                  {
//                     format = rule.format;
//                     break;
//                  };
//
//               };
//
//            }
//            else
//            {
//               if(tx.isEmpty())
//                  break;
//               ch = adrress.at(0);
//               switch(ch.toAscii())
//               {
//                  case 'G'         : format.setForeground(QColor(highlightColors.gColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'M'         : format.setForeground(QColor(highlightColors.mColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'N'         : format.setForeground(QColor(highlightColors.nColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'L'         : format.setForeground(QColor(highlightColors.lColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'A'         : format.setForeground(QColor(highlightColors.aColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'B'         : format.setForeground(QColor(highlightColors.bColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'Z'         : format.setForeground(QColor(highlightColors.zColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'F'         :
//                  case 'S'         : format.setForeground(QColor(highlightColors.fsColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'D'         :
//                  case 'H'         :
//                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case '#'         :
//                  case 'Q'         :
//                  case 'V'         :
//                  case '@'         :
//                  case 'R'         : format.setForeground(QColor(highlightColors.rColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'C'         :
//                  case 'E'         :
//                  case 'I'         :
//                  case 'J'         :
//                  case 'K'         :
//                  case 'P'         :
//                  case 'X'         :
//                  case 'Y'         :
//                  case 'U'         :
//                  case 'W'         : format.setForeground(Qt::black);
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  default          : ;
//               };
//               break;
//
//            };
//
//            break;
//         };
//
//         //***********************************************************************
//
//         if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '$') || (ch == '<') || (ch == '>')
//            || (ch == '='))
//            {
//            format.setForeground(QColor(highlightColors.operatorColor));
//            setFormat(pos, sellen, format);
//            break;
//
//         };
//
//         //***********************************************************************
//
//         if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}') || (ch == '(') || (ch == ')'))
//         {
//            format.setForeground(QColor(highlightColors.operatorColor));
//            setFormat(pos, sellen, format);
//            break;
//         };
//
//         //***********************************************************************
//
//         if((ch == '"') || (ch == '\''))
//         {
//            do
//            {
//               sellen++;
//               if((pos + sellen) >= tx.length())
//                  break;
//               ch = tx.at(sellen + pos);
//
//            }while(!((ch == '"') || (ch == '\'') || ((pos + sellen) >= tx.length())));
//            sellen++;
//            format.setForeground(QColor( highlightColors.commentColor));
//            setFormat(pos, sellen, format);
//            break;
//         };
//
//         //***********************************************************************
//
//         format.setForeground(Qt::black);
//         setFormat(pos, sellen, format);
//         break;
//
//      };
//
//      pos = pos + sellen;
//
//   };
//
//}

 //**************************************************************************************************
//
//**************************************************************************************************

//**************************************************************************************************
//
//**************************************************************************************************

//void Highlighter::highlightBlockFanucRule(const QString &tx)
//{
//   int pos, count;
//   int sellen;
//   QChar ch;
//   QString adrress, val;
//   QTextCharFormat format;
//
//
//
//   format.setFontWeight(QFont::Normal);
//   pos = 0;
//
//
//   foreach(HighlightingRule rule, highlightRules)
//   {
//      QRegExp expression(rule.pattern);
//      int index = tx.indexOf(expression);
//      while(index >= 0)
//      {
//         int length = expression.matchedLength();
//         setFormat(index, length, rule.format);
//         //if(length >= tx.length())
//           //return;
//         index = tx.indexOf(expression, index + length);
//      }
//   };
//
//   foreach(HighlightingRule rule, commentHighlightRules)
//   {
//      QRegExp expression(rule.pattern);
//      //expression.setPatternSyntax(QRegExp::Wildcard);
//      int index = tx.indexOf(expression);
//      while(index >= 0)
//      {
//         int length = expression.matchedLength();
//         setFormat(index, length, rule.format);
//         if(length >= tx.length())
//           return;
//         index = tx.indexOf(expression, index + length);
//      }
//   };
//
//   foreach(const ProgNameHighlightingRule &progRule, progNameHighlightRules)
//   {
//      QRegExp expression(progRule.pattern);
//      int index = expression.indexIn(tx);
//      while(index >= 0)
//      {
//         int length = expression.matchedLength();
//         setFormat(index, length, progRule.format);
//
//         index = expression.indexIn(tx, index + length);
//      };
//   };
//
//   setCurrentBlockState(0);
//   int startIndex = 0;
//   if(previousBlockState() != 1)
//      startIndex = tx.indexOf(commentStartExpression);
//
//   while(startIndex >= 0)
//   {
//      int endIndex = tx.indexOf(commentEndExpression, startIndex);
//      int commentLength;
//      if(endIndex == -1)
//      {
//         setCurrentBlockState(1);
//         commentLength = tx.length() - startIndex;
//         setFormat(startIndex, commentLength, commentFormat);
//         return;
//      }
//      else
//      {
//         commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
//      }
//      setFormat(startIndex, commentLength, commentFormat);
//      //pos = startIndex + commentLength;
//      startIndex = tx.indexOf(commentStartExpression, startIndex + commentLength);
//   }
//
//   while(pos < tx.length())
//   {
//
//      ch = tx.at(pos);
//      sellen = 1;
//      while(true)
//      {
//         if(ch == ';')
//         {
//            if(tx.length() >= (pos + 2))
//               if(tx.at(pos + 1) == '$')
//                  break;
//            sellen = (tx.length() - pos);
//            format.setForeground(QColor(highlightColors.commentColor));
//            setFormat(pos, sellen, format);
//            break;
//         };
//
//         //***********************************************************************
//
//         if(ch == '(')
//         {
//            count = 1;
//            do
//            {
//               if((pos + sellen) >= tx.length())
//               {
//                  sellen = (tx.length() - pos);
//                  break;
//               };
//               ch = tx.at(pos + sellen);
//               sellen++;
//
//               if(ch == '(')
//                  count++;
//               else
//                  if(ch == ')')
//                     count--;
//
//            }while(count > 0);
//            break;
//         };
//
//         //***********************************************************************
//
//         if((ch >= 'A' && ch <= 'Z') || (ch == '#') || (ch == '@') || (ch == '_'))
//         {
//            adrress = "";
//            do
//            {
//               adrress.append(ch);
//               if((adrress.length() + pos) >= tx.length())
//                  break;
//               ch = tx.at(adrress.length() + pos);
//            }while((ch >= 'A' && ch <= 'Z') || (ch == '_'));
//            sellen = adrress.length();
//
//            val = "";
//            while((ch >= '0' && ch <= '9') || (ch == '.') || (((ch == '-') || (ch == '+')) && (sellen == 1)))
//            {
//               val.append(ch);
//               if((sellen + val.length() + pos) >= tx.length())
//                  break;
//               ch = tx.at(sellen + val.length() + pos);
//            };
//
//            sellen = adrress.length() + val.length();
//
//            if(adrress.length() > 1)
//            {
//               if(ch == '(')
//               {
//                  setFormat(pos, sellen, highlightColors.macroColor);
//                  count = 1;
//                  pos = pos + sellen;
//                  sellen = 1;
//                  do
//                  {
//                     if((pos + sellen) >= tx.length())
//                     {
//                        sellen = (tx.length() - pos);
//                        break;
//                     };
//                     ch = tx.at(pos + sellen);
//                     sellen++;
//
//                     if(ch == '(')
//                        count++;
//                     else
//                        if(ch == ')')
//                           count--;
//
//                  }while(count > 0);
//
//                  highlightInside(tx, pos, pos + sellen);
//                  pos = pos + sellen;
//                  sellen = 0;
//               };
//               break;
//
//            }
//            else
//            {
//               if(tx.isEmpty())
//                  break;
//               ch = adrress.at(0);
//               switch(ch.toAscii())
//               {
//                  case 'G'         : format.setForeground(QColor(highlightColors.gColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'M'         : format.setForeground(QColor(highlightColors.mColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'N'         : format.setForeground(QColor(highlightColors.nColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'L'         : format.setForeground(QColor(highlightColors.lColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'A'         : format.setForeground(QColor(highlightColors.aColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'B'         : format.setForeground(QColor(highlightColors.bColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'Z'         : format.setForeground(QColor(highlightColors.zColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'F'         :
//                  case 'S'         : format.setForeground(QColor(highlightColors.fsColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'D'         :
//                  case 'H'         :
//                  case 'T'         : format.setForeground(QColor(highlightColors.dhtColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case '#'         :
//                  case 'Q'         :
//                  case 'V'         :
//                  case '@'         :
//                  case 'R'         : format.setForeground(QColor(highlightColors.rColor));
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  case 'C'         :
//                  case 'E'         :
//                  case 'I'         :
//                  case 'J'         :
//                  case 'K'         :
//                  case 'P'         :
//                  case 'X'         :
//                  case 'Y'         :
//                  case 'U'         :
//                  case 'W'         : format.setForeground(Qt::black);
//                                     setFormat(pos, sellen, format);
//                                     break;
//                  default          : ;
//               };
//
//               break;
//            };
//            break;
//         };
//
//         //***********************************************************************
//
//         if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '<') || (ch == '>')
//            || (ch == '='))
//            {
//
//            if((pos + sellen) >= tx.length())
//               break;
//            ch = tx.at(pos + sellen);
//            if(ch == '(')
//            {
//               setFormat(pos, 1, highlightColors.operatorColor);
//               pos++;
//               count = 1;
//               do
//               {
//                  if((pos + sellen) >= tx.length())
//                  {
//                     sellen = (tx.length() - pos);
//                     break;
//                  };
//                  ch = tx.at(pos + sellen);
//                  sellen++;
//
//                  if(ch == '(')
//                     count++;
//                  else
//                     if(ch == ')')
//                        count--;
//
//               }while(count > 0);
//               highlightInside(tx, pos, pos + sellen);
//               pos = pos + sellen;
//               sellen = 0;
//            }
//            else
//            {
//               format.setForeground(QColor(highlightColors.operatorColor));
//               setFormat(pos, sellen, format);
//            };
//            //setFormat(pos, sellen, format);
//            break;
//
//         };
//
//         //***********************************************************************
//
//         if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}'))
//         {
//            format.setForeground(QColor(highlightColors.operatorColor));
//            setFormat(pos, sellen, format);
//            break;
//         };
//
//         //***********************************************************************
//
//         if((ch == '"') || (ch == '\''))
//         {
//            do
//            {
//               sellen++;
//               if((pos + sellen) >= tx.length())
//                  break;
//               ch = tx.at(sellen + pos);
//
//            }while(!((ch == '"') || (ch == '\'') || ((pos + sellen) >= tx.length())));
//            sellen++;
//            format.setForeground(QColor(highlightColors.commentColor));
//            setFormat(pos, sellen, format);
//            break;
//         };
//
//
//         //***********************************************************************
//
//
//         //setFormat(pos, sellen, format);
//         break;
//
//      };
//
//      format.setForeground(Qt::black);
//      format.setFontWeight(QFont::Normal);
//      pos = pos + sellen;
//      format.setFontWeight(QFont::Normal);
//
//   };
//
//}


//**************************************************************************************************
//
//**************************************************************************************************

int autoDetectHighligthMode(const QString text)
{

   if(!text.isEmpty())
   {
      if(text.contains(QRegExp("^(%_N_)[A-Z]{1,1}[A-Z0-9_]{0,30}_(MPF|SPF|TEA|COM|PLC|DEF|INI)\\n"))
         || text.contains(QRegExp("^;\\$PATH=/[A-Z0-9_//]{1,}\\n")))
      {
         return MODE_SINUMERIK_840;
      };

      if(text.contains(QRegExp("^\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"))
         || text.contains(QRegExp("[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)($|\\s)")))
      {
         return MODE_OKUMA;
      };

      if(text.contains(QRegExp("(:)[0-9]{1,}"))
         || text.contains(QRegExp("\\bO[0-9]{1,}\\b")))
      {
         return MODE_FANUC;
      };

      if(text.contains(QRegExp("%\\b(MPF|SPF|TEA)[\\s]{0,3}[0-9]{1,4}\\b")))
      {
         return MODE_SINUMERIK;
      };

      if(text.contains(QRegExp("%F[0-9]{1,4}")))
      {
         return MODE_HEIDENHAIN_ISO;
      };

      if(text.contains(QRegExp("(BEGIN|END)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)")))
      {
         return MODE_HEIDENHAIN;
      };

      if(text.contains(QRegExp("^%PM$")))   // PHILIPS SYSTEM 2000
      {
         return MODE_PHILIPS;
      };
   };

   return MODE_SINUMERIK;

//   progNameFormat.setForeground(QColor(highlightColors.progNameColor));
//   progNameFormat.setFontWeight(QFont::Bold);
//   pogRule.format = progNameFormat;
//
//   QString  pattern = "^\\$[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}";  // OKUMA OSP
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_OKUMA;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "\\b(^O| O)[A-Z0-9]{2,}\\b";  // OKUMA
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_OKUMA;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "[A-Z]{1,1}[A-Z0-9_-]{1,}\\.(MIN|SSB|SDF|LIB|SUB)($|\\s)";  // OKUMA
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_OKUMA;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "%\\b(MPF|SPF|TEA)[\\s]{0,3}[0-9]{1,4}$\\b";  // SINUMERIK OLD 810-880
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_SINUMERIK;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "^(%_N_)[A-Z]{1,1}[A-Z0-9_]{0,30}_(MPF|SPF|TEA|COM|PLC|DEF)$"; // SINUMERIK >=810D
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_SINUMERIK_840;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "^;\\$PATH=/[A-Z0-9_//]{1,}$";  // SINUMERIK >=810D path
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_SINUMERIK_840;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "\\b^O[0-9]{1,}\\b"; // FANUC
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_FANUC;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "^(:)[0-9]{1,}";  // FANUC
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_FANUC;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "^%PM$";  // PHILIPS SYSTEM 2000
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_PHILIPS;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "(%M)[0-9]{1,4}";  //
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_ALL;
//   progNameHighlightRules.append(pogRule);
//
//   pattern = "(BEGIN|END)(\\sPGM\\s)[a-zA-Z0-9_-+*]{1,}(\\sMM|\\sINCH)";  // HEIDENHAIN
//   pogRule.pattern = QRegExp(pattern);
//   pogRule.mode = MODE_HEIDENHAIN;
//   progNameHighlightRules.append(pogRule);


}
