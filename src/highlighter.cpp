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
}

//**************************************************************************************************
//
//**************************************************************************************************

void Highlighter::highlightBlock(const QString &tx)
{
  uint pos, count;
  int sellen;
  QColor color;
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
           if(tx.mid(0, 2) == ";$")
             color = highlightColors.progNameColor;
           else
             color = highlightColors.commentColor;
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

           color = highlightColors.commentColor;
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
              ch = tx.at(adrress.length() + pos).toUpper(); 
           }while((ch >= 'A' && ch <= 'Z') || (ch == '_'));
           sellen = adrress.length();

           val = "";
           while((ch >= '0' && ch <= '9') || (ch == '.'))
           {
              val.append(ch);
              if((sellen + val.length() + pos) >= tx.length())
                break;
              ch = tx.at(sellen + val.length() + pos).toUpper();
           };

           sellen = adrress.length() + val.length();



           if(adrress.length() > 1)
           {
              //ch = tx.at(sellen + pos).upper();
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
                 //color = highlightColors.keyWordColor;
                 pos = pos + sellen;
                 sellen = 0;
              };

              do
              {
                 if(adrress.at(0) == 'N')
                 {
                    color = highlightColors.nColor;
                    break;
                 };
                 if((adrress.at(0) == '_' && adrress.at(1) == 'N' && adrress.at(2) == '_'))
                 {
                    sellen--;
                    do
                    {
                       sellen++;
                       if((sellen + pos) >= tx.length())
                         break;
                       ch = tx.at(sellen + pos).toUpper();
                       if(ch == ' ')
                       {
                          break;
                       };
                    }while(TRUE);
                    color = highlightColors.progNameColor;
                    format.setFontWeight(QFont::Bold);
                    break;
                 };

                 if((adrress == "MPF") || (adrress == "SPF") || (adrress.at(0) == 'O') || (adrress.at(0) == '$') ||
                   (adrress.at(0) == '_' && adrress.at(1) == 'N'&& adrress.at(2) == '_'))
                 {
                    color = highlightColors.progNameColor;
                    format.setFontWeight(QFont::Bold);
                    break;
                 };

                 color = highlightColors.keyWordColor;   //Qt::darkBlue;
              }while(FALSE);


           }
           else
           {
              ch = adrress.at(0);
              switch(ch.toAscii())
              {
                 case 'G'         : color = highlightColors.gColor;
                                    break;
                 case 'M'         : color = highlightColors.mColor;
                                    break;
                 case 'N'         : color = highlightColors.nColor;
                                    break;
                 case 'L'         : color = highlightColors.lColor;
                                    break;
                 case 'A'         : color = highlightColors.aColor;
                                    break;
                 case 'B'         : color = highlightColors.bColor;
                                    break;
                 case 'Z'         : color = highlightColors.zColor;
                                    break;
                 case 'F'         :
                 case 'S'         : color = highlightColors.fsColor;
                                    break;
                 case 'D'         :
                 case 'H'         :
                 case 'T'         : color = highlightColors.dhtColor;
                                    break;
                 case '#'         :
                 case 'V'         :
                 case '@'         :
                 case 'R'         : color = highlightColors.rColor;
                                    break;
                 case '%'         :
                 case 'O'         : color = highlightColors.progNameColor;
                                    format.setFontWeight(QFont::Bold);
                                    break;
                 default          : color = Qt::black;
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
             color = highlightColors.operatorColor;
           break;

        };

//***********************************************************************

        if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}'))
        {
           color = highlightColors.operatorColor;
           break;
        };

//***********************************************************************

        if((ch == ':'))
        {
           sellen--;
           do
           {
              sellen++;
              if((sellen + pos) >= tx.length())
                break;
              ch = tx.at(sellen + pos).toUpper();
           }while((ch >= '0' && ch <= '9') || (ch == '.'));

           color = highlightColors.progNameColor;
           format.setFontWeight(QFont::Bold);
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
           color = highlightColors.commentColor;
           break;
        };


//***********************************************************************

        color = Qt::black;
        format.setFontWeight(QFont::Normal);
        break;

     };

     
     
     format.setForeground(QColor(color));
     setFormat(pos, sellen, format);
     //setFormat(pos, sellen, font);
     //setFormat(pos, sellen, QColor(color));
     

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
  QColor color;
  QChar ch;
  QString adrress, val;


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
           while((ch >= '0' && ch <= '9') || (ch == '.'))
           {
              val.append(ch);
              if((sellen + val.length() + pos) >= tx.length())
                break;
              ch = tx.at(sellen + val.length() + pos).toUpper();
           };

           sellen = adrress.length() + val.length();



           if(adrress.length() > 1)
           {
              color = highlightColors.keyWordColor;   //Qt::darkBlue;
           }
           else
           {
              ch = adrress.at(0);
              switch(ch.toAscii())
              {
                 case 'G'         : color = highlightColors.gColor;
                                    break;
                 case 'M'         : color = highlightColors.mColor;
                                    break;
                 case 'N'         : color = highlightColors.nColor;
                                    break;
                 case 'L'         : color = highlightColors.lColor;
                                    break;
                 case 'A'         : color = highlightColors.aColor;
                                    break;
                 case 'B'         : color = highlightColors.bColor;
                                    break;
                 case 'Z'         : color = highlightColors.zColor;
                                    break;
                 case 'F'         :
                 case 'S'         : color = highlightColors.fsColor;
                                    break;
                 case 'D'         :
                 case 'H'         :
                 case 'T'         : color = highlightColors.dhtColor;
                                    break;
                 case '#'         :
                 case '@'         :
                 case 'R'         : color = highlightColors.rColor;
                                    break;
                 default          : color = Qt::black;
              };

           };


           break;
        };

//***********************************************************************

        if((ch == '/') || (ch == '*') || (ch == '-') || (ch == '+') || (ch == '$') || (ch == '<') || (ch == '>')
            || (ch == '='))
        {
           color = highlightColors.operatorColor;
           break;

        };

//***********************************************************************

        if((ch == '[') || (ch == ']') || (ch == ',') || (ch == '{') || (ch == '}') || (ch == '(') || (ch == ')'))
        {
           color = highlightColors.operatorColor;
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
           color = highlightColors.commentColor;
           break;
        };

//***********************************************************************

        color = Qt::black;
        break;

     };

     setFormat(pos, sellen, color);
     pos = pos + sellen;

  };

}
