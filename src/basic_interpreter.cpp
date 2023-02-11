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

/***************************************************************************
 * Code from:                                                              *
 * A tiny BASIC interpreter                                                *
 * http://www.programmersheaven.com/download/55979/download.aspx           *
 *                                                                         *
 ***************************************************************************/

// Enable the M_PI constant in MSVC
// see https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/math-constant
#define _USE_MATH_DEFINES

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <QString>

#include "basic_interpreter.h" // BasicInterpreter


BasicInterpreter::BasicInterpreter()
{
    memset(variables, 0, sizeof(variables));

    /* 20 Commands must be entered lowercase in this table*/
    int x = 0;
    strcpy(table[x].command, "print");
    table[x++].tok = PRINT;

    strcpy(table[x].command, "sin");
    table[x++].tok = SIN;
    strcpy(table[x].command, "cos");
    table[x++].tok = COS;
    strcpy(table[x].command, "tan");
    table[x++].tok = TAN;
    strcpy(table[x].command, "sqrt");
    table[x++].tok = SQRT;
    strcpy(table[x].command, "sqr");
    table[x++].tok = SQR;
    strcpy(table[x].command, "abs");
    table[x++].tok = ABS;
    strcpy(table[x].command, "trunc");
    table[x++].tok = TRUNC;
    strcpy(table[x].command, "pi");
    table[x++].tok = PI;

    strcpy(table[x].command, "if");
    table[x++].tok = IF;
    strcpy(table[x].command, "then");
    table[x++].tok = THEN;
    strcpy(table[x].command, "goto");
    table[x++].tok = GOTO;
    strcpy(table[x].command, "for");
    table[x++].tok = FOR;
    strcpy(table[x].command, "next");
    table[x++].tok = NEXT;
    strcpy(table[x].command, "to");
    table[x++].tok = TO;
    strcpy(table[x].command, "gosub");
    table[x++].tok = GOSUB;
    strcpy(table[x].command, "return");
    table[x++].tok = RETURN;
    strcpy(table[x].command, "end");
    table[x++].tok = END;
    strcpy(table[x].command, "");
    table[x++].tok = END + 1;
}

/* GOSUB stack push function. */
void BasicInterpreter::gpush(char *s)
{
    gtos++;

    if (gtos == SUB_NEST) {
        serror(12);
        return;
    }

    gstack[gtos] = s;
}

/* GOSUB stack pop function. */
char *BasicInterpreter::gpop()
{
    if (gtos == 0) {
        serror(13);
        return 0;
    }

    return (gstack[gtos--]);
}

/* Push function for the FOR stack. */
void BasicInterpreter::fpush(struct for_stack i)
{
    if (ftos > FOR_NEST) {
        serror(10);
    }

    fstack[ftos] = i;
    ftos++;
}

/* Execute a FOR loop. */
void BasicInterpreter::exec_for()
{
    struct for_stack i;
    double value;

    get_token(); /* read the control variable */

    if (!isalpha(*token)) {
        serror(4);
        return;
    }

    i.var = toupper(*token) - 'A'; /* save its index */

    get_token(); /* read the equals sign */

    if (*token != '=') {
        serror(3);
        return;
    }

    get_exp(&value); /* get initial value */

    variables[i.var] = value;

    get_token();

    if (tok != TO) {
        serror(9);    /* read and discard the TO */
    }

    get_exp(&i.target); /* get target value */

    /* if loop can execute at least once, push info on stack */
    if (value >= variables[i.var]) {
        i.loc = prog;
        fpush(i);
    } else {
        /* otherwise, skip loop code altogether */
        while (tok != NEXT) {
            get_token();
        }
    }
}

/* Find the start of the next line. */
void BasicInterpreter::find_eol()
{
    while (*prog != '\n'  && *prog != '\0') {
        ++prog;
    }

    if (*prog) {
        prog++;
    }
}

/* Initialize the array that holds the labels.
           By convention, a null label name indicates that
           array position is unused.
        */
void BasicInterpreter::label_init()
{
    int t;

    for (t = 0; t < NUM_LAB; ++t) {
        label_table[t].name[0] = '\0';
    }
}

/* Entry point into parser. */
void BasicInterpreter::get_exp(double *result)
{
    get_token();

    if (!*token) {
        serror(2);
        return;
    }

    level2(result);
    putback(); /* return last token read to input stream */
}

/* Look up a a token's internal representation in the
           token table.
        */
int BasicInterpreter::look_up(char *s)
{
    int i;
    char *p;

    /* convert to lowercase */
    p = s;

    while (*p) {
        *p = tolower(*p);
        p++;
    }

    /* see if token is in table */
    for (i = 0; *table[i].command; i++) {
        if (!strcmp(table[i].command, s)) {
            return table[i].tok;
        }
    }

    return 0; /* unknown command */
}

/* Return true if c is a delimiter. */
int BasicInterpreter::isdelim(char c)
{
    if (strchr(" ;,+-<>/*%^=()", c) || c == 9 || c == '\n' || c == 0) {
        return 1;
    }

    return 0;
}

/* Return 1 if c is space or tab. */
int BasicInterpreter::iswhite(char c)
{
    if (c == ' ' || c == '\t') {
        return 1;
    } else {
        return 0;
    }
}

/* Return a token to input stream. */
void BasicInterpreter::putback()
{
    char *t;

    t = token;

    for (; *t; t++) {
        prog--;
    }
}

/* Return index of next free position in label array.
           A -1 is returned if the array is full.
           A -2 is returned when duplicate label is found.
        */
int BasicInterpreter::get_next_label(char *s)
{
    int t;

    for (t = 0; t < NUM_LAB; ++t) {
        if (label_table[t].name[0] == 0) {
            return t;
        }

        if (!strcmp(label_table[t].name, s)) {
            return -2;    /* dup */
        }
    }

    return -1;
}

/* display an error message */
void BasicInterpreter::serror(int err)
{
    error = err;
}

/* Get a token. */
int BasicInterpreter::get_token()
{
    char *temp;

    token_type = 0;
    tok = 0;
    temp = token;

    if (*prog == '\0') {
        /* end of file */
        *token = 0;
        tok = FINISHED;
        return (token_type = DELIMITER);
    }

    while (iswhite(*prog)) {
        ++prog;    /* skip over white space */
    }

    if (*prog == '\n') {
        /* crlf */
        ++prog; // ++prog;
        tok = EOL;
        *token = '\n';
        token[1] = 0;
        token[2] = 0;
        return (token_type = DELIMITER);
    }

    if (strchr("+-*^/%=;(),><", *prog)) {
        /* delimiter */
        *temp = *prog;
        prog++; /* advance to next position */
        temp++;
        *temp = 0;
        return (token_type = DELIMITER);
    }

    if (*prog == '"') {
        /* quoted string */
        prog++;

        while (*prog != '"' && *prog != '\n') {
            *temp++ = *prog++;
        }

        if (*prog == '\n') {
            serror(1);
        }

        prog++;
        *temp = 0;
        return (token_type = QUOTE);
    }

    if (isdigit(*prog)) {
        /* number */
        while (!isdelim(*prog)) {
            *temp++ = *prog++;
        }

        *temp = '\0';
        return (token_type = NUMBER);
    }

    if (isalpha(*prog)) {
        /* var or command */
        while (!isdelim(*prog)) {
            *temp++ = *prog++;
        }

        token_type = STRING;
    }

    *temp = '\0';

    /* see if a string is a command or a variable */
    if (token_type == STRING) {
        tok = look_up(token); /* convert to internal rep */

        if (!tok) {
            token_type = VARIABLE;
        } else {
            token_type = COMMAND;    /* is a command */
        }
    }

    return token_type;
}

/* Assign a variable a value. */
void BasicInterpreter::assignment()
{
    int var;
    double value;

    /* get the variable name */
    get_token();

    if (!isalpha(*token)) {
        serror(4);
        return;
    }

    var = toupper(*token) - 'A';

    /* get the equals sign */
    get_token();

    if (*token != '=') {
        serror(3);
        return;
    }

    /* get the value to assign to var */
    get_exp(&value);

    /* assign the value */
    variables[var] = value;
}

/* Execute a simple version of the BASIC PRINT statement */
void BasicInterpreter::print()
{
    double answer;
    int len = 0;
    char last_delim = 0;
    QString str;

    do {
        get_token(); /* get next list item */

        if (tok == EOL || tok == FINISHED) {
            break;
        }

        if (token_type == QUOTE) {
            /* is string */
            str = token;
            str.remove("""");
            result.append(str);
            len += str.size();
            get_token();
        } else {
            /* is expression */
            putback();
            get_exp(&answer);
            get_token();
            str = QString("%1").arg(answer, 0, 'f', 3);
            result.append(str);
            len += str.size();
        }

        last_delim = *token;

        if (*token == ';') {
            result.append("\n");
        } else if (*token == ',') {
            /* do nothing */;
        } else if (tok != EOL && tok != FINISHED) {
            serror(0);
        }
    } while (*token == ';' || *token == ',');

    if (tok == EOL || tok == FINISHED) {
        if (last_delim != ';' && last_delim != ',') {
            result.append("\n");
        }
    } else {
        serror(0);    /* error is not , or ; */
    }

}

/* Find all labels. */
void BasicInterpreter::scan_labels()
{
    int addr;
    char *temp;

    label_init();  /* zero all labels */
    temp = prog;   /* save pointer to top of program */

    /* if the first token in the file is a label */
    get_token();

    if (token_type == NUMBER) {
        strcpy(label_table[0].name, token);
        label_table[0].p = prog;
    }

    find_eol();

    do {
        get_token();

        if (token_type == NUMBER) {
            addr = get_next_label(token);

            if (addr == -1 || addr == -2) {
                (addr == -1) ? serror(5) : serror(6);
            }

            strcpy(label_table[addr].name, token);
            label_table[addr].p = prog;  /* current point in program */
        }

        /* if not on a blank line, find next line */
        if (tok != EOL) {
            find_eol();
        }
    } while (tok != FINISHED);

    prog = temp;  /* restore to original */
}

/* Find location of given label.  A null is returned if
           label is not found; otherwise a pointer to the position
           of the label is returned.
        */
char *BasicInterpreter::find_label(char *s)
{
    int t;

    for (t = 0; t < NUM_LAB; ++t) {
        if (!strcmp(label_table[t].name, s)) {
            return label_table[t].p;
        }
    }

    return nullptr; /* error condition */
}

/* Execute a GOTO statement. */
void BasicInterpreter::exec_goto()
{
    char *loc;

    get_token(); /* get label to go to */
    /* find the location of the label */
    loc = find_label(token);

    if (loc == nullptr) {
        serror(7);    /* label not defined */
    }

    else {
        prog = loc;    /* start program running at that loc */
    }
}

/* Execute an IF statement. */
void BasicInterpreter::exec_if()
{
    double x, y;
    int cond;
    char op;

    get_exp(&x); /* get left expression */

    get_token(); /* get the operator */

    if (!strchr("=<>", *token)) {
        serror(0); /* not a legal operator */
        return;
    }

    op = *token;

    get_exp(&y); /* get right expression */

    /* determine the outcome */
    cond = 0;

    switch (op) {
    case '<':
        if (x < y) {
            cond = 1;
        }

        break;

    case '>':
        if (x > y) {
            cond = 1;
        }

        break;

    case '=':
        if (x == y) {
            cond = 1;
        }

        break;
    }

    if (cond) {
        /* is true so process target of IF */
        get_token();

        if (tok != THEN) {
            serror(8);
            return;
        }/* else program execution starts on next line */
    } else {
        find_eol();    /* find start of next line */
    }
}

/* Execute a NEXT statement. */
void BasicInterpreter::next()
{
    struct for_stack i;

    i = fpop(); /* read the loop info */

    variables[i.var]++; /* increment control variable */

    if (variables[i.var] > i.target) {
        return;    /* all done */
    }

    fpush(i);  /* otherwise, restore the info */
    prog = i.loc;  /* loop */
}

struct for_stack BasicInterpreter::fpop()
{
    ftos--;

    if (ftos < 0) {
        serror(11);
    }

    return (fstack[ftos]);
}

/* Execute a simple form of the BASIC INPUT command */
void BasicInterpreter::exec_sin()
{
    struct for_stack i;
    double value;

    get_token(); /* read the control variable */

    if (!isalpha(*token)) {
        serror(4);
        return;
    }

    i.var = toupper(*token) - 'A'; /* save its index */

    get_token(); /* read the equals sign */

    if (*token != '=') {
        serror(3);
        return;
    }

    get_exp(&value); /* get initial value */

    variables[i.var] = value;

    get_token();

    if (tok != TO) {
        serror(9);    /* read and discard the TO */
    }

    get_exp(&i.target); /* get target value */

    /* if loop can execute at least once, push info on stack */
    if (value >= variables[i.var]) {
        i.loc = prog;
        fpush(i);
    } else {
        /* otherwise, skip loop code altogether */
        while (tok != NEXT) {
            get_token();
        }
    }
}

/* Execute a GOSUB command. */
void BasicInterpreter::gosub()
{
    char *loc;

    get_token();
    /* find the label to call */
    loc = find_label(token);

    if (loc == nullptr) {
        serror(7);    /* label not defined */
    } else {
        gpush(prog); /* save place to return to */
        prog = loc;  /* start program running at that loc */
    }
}

/* Return from GOSUB. */
void BasicInterpreter::greturn()
{
    prog = gpop();
}

/* Find the value of a variable. */
double BasicInterpreter::find_var(char *s)
{
    if (!isalpha(*s)) {
        serror(4); /* not a variable */
        return 0;
    }

    return variables[toupper(*token) - 'A'];
}

/* Find value of number or variable. */
void BasicInterpreter::primitive(double *result)
{
    bool ok;

    switch (token_type) {
    case VARIABLE:
        *result = find_var(token);
        get_token();
        return;

    case NUMBER:
        *result = QString(token).toDouble(&ok);
        get_token();
        return;

    default:
        serror(0);
    }
}

/* Process parenthesized expression. */
void BasicInterpreter::level6(double *result)
{
    if ((*token == '(') && (token_type == DELIMITER)) {
        get_token();
        level2(result);

        if (*token != ')') {
            serror(1);
        }

        get_token();
    } else {
        primitive(result);
    }
}

/* Is a unary + or -. */
void BasicInterpreter::level5(double *result)
{
    char op;

    op = 0;

    if ((token_type == DELIMITER) && (*token == '+' || *token == '-')) {
        op = *token;
        get_token();
    }

    level6(result);

    if (op) {
        unary(op, result);
    }
}

/* Process integer exponent. */
void BasicInterpreter::level4(double *result)
{
    double hold;

    level5(result);

    if (*token == '^') {
        get_token();
        level4(&hold);
        arith('^', result, &hold);
    }

    switch (tok) {
    case SIN:
        get_token();
        level4(&hold);
        *result = sin((M_PI / 180) * hold);
        break;

    case COS:
        get_token();
        level4(&hold);
        *result = cos((M_PI / 180) * hold);
        break;

    case TAN:
        get_token();
        level4(&hold);
        *result = tan((M_PI / 180) * hold);
        break;

    case SQRT:
        get_token();
        level4(&hold);
        *result = sqrt(hold);
        break;

    case SQR:
        get_token();
        level4(&hold);
        *result = pow(hold, 2);
        break;

    case ABS:
        get_token();
        level4(&hold);
        *result = abs(hold);
        break;

    case TRUNC:
        get_token();
        level4(&hold);
        *result = trunc(hold);
        break;

    case PI:
        *result = M_PI;
        break;
    }
}

/* Multiply or divide two factors. */
void BasicInterpreter::level3(double *result)
{
    char  op;
    double hold;

    level4(result);

    while ((op = *token) == '*' || op == '/' || op == '%') {
        get_token();
        level4(&hold);
        arith(op, result, &hold);
    }
}

/*  Add or subtract two terms. */
void BasicInterpreter::level2(double *result)
{
    char  op;
    double hold;

    level3(result);

    while ((op = *token) == '+' || op == '-') {
        get_token();
        level3(&hold);
        arith(op, result, &hold);
    }
}

/* Perform the specified arithmetic. */
void BasicInterpreter::arith(char o, double *r, double *h)
{
    double t, ex;

    switch (o) {
    case '-':
        *r = *r - *h;
        break;

    case '+':
        *r = *r + *h;
        break;

    case '*':
        *r = *r * *h;
        break;

    case '/':
        *r = (*r) / (*h);
        break;

    case '%':
        t = (*r) / (*h);
        *r = *r - (t * (*h));
        break;

    case '^':
        ex = *r;

        if (*h == 0) {
            *r = 1;
            break;
        }

        for (t = *h - 1; t > 0; --t) {
            *r = (*r) * ex;
        }

        break;
    }
}

/* Reverse the sign. */
void BasicInterpreter::unary(char o, double *r)
{
    if (o == '-') {
        *r = -(*r);
    }
}

int BasicInterpreter::interpretBasic(QString &code)
{
    result.clear();
    error = 0;

    char *buf = new char[code.toLatin1().size() + 1];

    memcpy(buf, code.toLatin1().data(), code.toLatin1().size());

    prog = buf;
    scan_labels(); /* find the labels in the program */
    ftos = 0; /* initialize the FOR stack index */
    gtos = 0; /* initialize the GOSUB stack index */

    do {
        token_type = get_token();

        /* check for assignment statement */
        if (token_type == VARIABLE) {
            putback(); /* return the var to the input stream */
            assignment(); /* must be assignment statement */
        } else {
            /* is command */
            switch (tok) {
            case PRINT:
                print();
                break;

            case GOTO:
                exec_goto();
                break;

            case IF:
                exec_if();
                break;

            case FOR:
                exec_for();
                break;

            case NEXT:
                next();
                break;

            //case SIN:
            //    sin();
            //    break;

            case GOSUB:
                gosub();
                break;

            case RETURN:
                greturn();
                break;

            case END:
                code = result;
                return error;
            }
        }
    } while (tok != FINISHED);

    code = result;
    return error;
}
