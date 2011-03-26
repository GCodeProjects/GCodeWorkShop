/***************************************************************************
 *   Copyright (C) 2006-2011 by Artur Kozioł                               *
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

/***************************************************************************
 * Code from:                                                              *
 * A tiny BASIC interpreter                                                *
 * http://www.programmersheaven.com/download/55979/download.aspx           *
 *                                                                         *
 ***************************************************************************/


#include "math.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

#include <QString>



#ifndef BASIC_INTERPRETER_H
#define BASIC_INTERPRETER_H




#define NUM_LAB      100
#define LAB_LEN      10
#define FOR_NEST     25
#define SUB_NEST     25


#define DELIMITER    1
#define VARIABLE     2
#define NUMBER       3
#define COMMAND      4
#define STRING	     5
#define QUOTE	     6

#define PRINT        1
#define SIN          2
#define IF           3
#define THEN         4
#define FOR          5
#define NEXT         6
#define TO           7
#define GOTO         8
#define EOL          9
#define FINISHED     10
#define GOSUB        11
#define RETURN       12
#define END          13
#define COS          14
#define TAN          15
#define SQRT         16
#define SQR          17
#define ABS          18
#define TRUNC        19
#define PI           20


struct for_stack
{
   int var; /* counter variable */
   char *loc;
   double target;  /* target value */
};


class BasicInterpreter
{

private:

   QString result;
   int error;

   struct commands /* keyword lookup table */
   {
      char command[20];
      char tok;
   }table[32]; /* 20 Commands must be entered lowercase */

   double variables[26];   /* 26 user variables,  A-Z */
   char *prog;  /* holds expression to be analyzed */


   char token[1024];
   char token_type, tok;

   struct label
   {
      char name[LAB_LEN];
      char *p;
   };
   struct label label_table[NUM_LAB];


   struct for_stack fstack[FOR_NEST];
   //char *find_label(), *gpop();


   //struct for_stack fpop();
   char *gstack[SUB_NEST];
   int ftos;
   int gtos;


   void gpush(char *s);
   char *gpop();
   void fpush(struct for_stack i);
   void exec_for();
   void find_eol();

   void label_init();
   void get_exp(double *result);

   int look_up(char *s);

   int isdelim(char c);
   int iswhite(char c);
   void putback();
   int get_next_label(char *s);
   void serror(int err);
   int get_token();
   void assignment();
   void print();
   void scan_labels();
   char *find_label(char *s);


   void exec_goto();
   void exec_if();
   void next();

   struct for_stack fpop();

   void exec_sin();
   void gosub();
   void greturn();
   double find_var(char *s);
   void primitive(double *result);
   void level6(double *result);
   void level5(double *result);
   void level4(double *result);
   void level3(double *result);
   void level2(double *result);
   void arith(char o, double *r, double *h);
   void unary(char o, double *r);


public:

   BasicInterpreter();

   int interpretBasic(QString &code);


};






#endif // BASIC_INTERPRETER_H
