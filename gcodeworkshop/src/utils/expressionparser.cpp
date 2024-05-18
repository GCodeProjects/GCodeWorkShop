/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
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

// Enable the M_PI constant in MSVC
// see https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/math-constant
#define _USE_MATH_DEFINES

#include <cmath>     // M_PI, abs, cos, pow, sin, sqrt, tan, trunc

#include <QChar>                    // for operator==, QChar
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString
#include <Qt>                       // for CaseInsensitive
#include <QtDebug>                  // for QNoDebug, qDebug
#include <QtGlobal>                 // for qDebug

#include <utils/expressionparser.h> // IWYU pragma: associated


int Utils::processBrc(QString* str)
{
	QRegularExpression regex;
	QString par, partmp;
	int pos, err;

	if (str->contains(')') != str->contains('(')) {
		return (ERR_NO_BRAC);
	}

	pos = 0;
	regex.setPattern("\\([-+/*.0-9A-Z]*\\b[.]*\\)");
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	auto match = regex.match(*str);

	while (match.hasMatch()) {
		pos = match.capturedEnd();
		par = match.captured();
		partmp = par;

		par.remove(' ');

		qDebug() << "147" << par << pos;

		err = compute(&par);

		if (err < 0) {
			return (err);
		}

		str->replace(partmp, par, Qt::CaseInsensitive);
		par.remove(' ');
		err = processBrc(str);

		if (err < 0) {
			return (err);
		}

		match = regex.match(*str);
	}

	qDebug() << "852" << *str;
	err = compute(str);
	return (err);
}

int Utils::compute(QString* str)
{
	QRegularExpression regex;
	QString val1, val2, partmp;
	QString oper;
	int pos, i, j, err;
	double result = 0;
	bool ok, ok1, dot, minus;

	pos = 0;
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[$A-Z]+");
	auto match = regex.match(*str);

	while (match.hasMatch()) {
		pos = match.capturedEnd();
		j = match.capturedStart();
		oper = match.captured().toUpper();
		val1 = "";
		dot = false;
		minus = false;

		while ((str->at(pos) == '-') || (str->at(pos) == '.') || (str->at(pos).isDigit())) {
			if (str->at(pos) == '.') {
				if ((dot)) {
					return (ERR_DOUBLE_DOT);
				}

				dot = true;
			}

			if (str->at(pos) == '-') {
				if (minus || dot) {
					break;
				}

				if (pos == 0) {
					minus = true;
				} else if ((str->at(pos - 1) == '*') || (str->at(pos - 1) == '/') || (str->at(pos - 1) == '+')
				           || (str->at(pos - 1) == '-') || (str->at(pos - 1) == '(')) {
					minus = true;
				}
			}

			val1 += str->at(pos);
			pos++;
		}

		qDebug() << "123" << val1 << oper << oper << *str;

		if (val1.isEmpty()) {
			if (oper == "PI") {
				val1 = "(1)";
			} else {
				return (ERR_NO_PARAM);
			}
		}

		err = processBrc(&val1);

		if (err < 0) {
			return (err);
		}

		if (val1.isEmpty()) {
			val1 = "0";
		}

		result = val1.toDouble(&ok);

		if (!ok) {
			return (ERR_CONVERT);
		}

		while (1) {
			if (oper == "SIN") {
				result = std::sin((M_PI / 180) * result);
				break;
			}

			if (oper == "COS") {
				result = std::cos((M_PI / 180) * result);
				break;
			}

			if (oper == "TAN") {
				result = std::tan((M_PI / 180) * result);
				break;
			}

			if (oper == "SQRT") {
				result = std::sqrt(result);
				break;
			}

			if (oper == "SQR") {
				result = std::pow(result, 2);
				break;
			}

			if (oper == "ABS") {
				result = std::fabs(result);
				break;
			}

			if (oper == "TRUNC") {
				result = std::trunc(result);
				break;
			}

			if (oper == "PI") {
				result = M_PI;
				break;
			}

			return (ERR_UNKNOWN_FUNC);
		}

		partmp.number(result, 'g', 3);
		str->replace(j, pos - j, QString("%1").arg(result, 0, 'f', 3));
		match = regex.match(*str, pos);
	}

	pos = 0;
	regex.setPattern("[/*]{1,1}");

	while ((pos = str->indexOf(regex, pos)) >= 0) {
		oper = str->mid(pos, 1);

		val2 = "";
		dot = false;
		minus = false;

		for (i = pos + 1; i < str->length(); i++) {
			qDebug() << "456,123" << val2 << str->at(i);

			if ((str->at(i) == '.')) {
				if ((dot)) {
					return (ERR_DOUBLE_DOT);
				}

				dot = true;
			}

			if ((str->at(i) == '-')) {
				if (minus || dot) {
					break;
				}

				if (i == 0) {
					minus = true;
				} else if ((str->at(i - 1) == '*') || (str->at(i - 1) == '/') || (str->at(i - 1) == '+')
				           || (str->at(i - 1) == '-') || (str->at(i - 1) == '(')) {
					minus = true;
				}

				//            if(minus && val2.length() > 0)
				//                break;
			}

			if (!((str->at(i).isDigit() || (str->at(i) == '.') || (str->at(i) == '-')))) {
				break;
			}

			val2 += str->at(i);
		}

		i--;

		val1 = "";

		dot = false;
		minus = false;

		for (j = pos - 1; j >= 0; j--) {
			if ((str->at(j) == '.')) {
				if ((dot)) {
					return (ERR_DOUBLE_DOT);
				}

				dot = true;
			}

			if ((str->at(j) == '-')) {
				if ((minus)) {
					break;
				}

				if (j == 0) {
					minus = true;
				} else if ((str->at(j - 1) == '*') || (str->at(j - 1) == '/') || (str->at(j - 1) == '+')
				           || (str->at(j - 1) == '-') || (str->at(j - 1) == '(')) {
					minus = true;
				}
			}

			if (!((str->at(j).isDigit() || (str->at(j) == '.') || (str->at(j) == '-')))) {
				break;
			}

			val1.prepend(str->at(j));
		}

		j++;

		qDebug() << "456" << val1 << oper << val2 << *str;

		if (val1.isEmpty()) {
			return (ERR_NO_PARAM);    // val1 = "0";
		}

		if (val2.isEmpty()) {
			return (ERR_NO_PARAM);    // val2 = "0";
		}

		while (1) {
			if (oper.at(0) == '*') {
				result = val1.toDouble(&ok) * val2.toDouble(&ok1);
				break;
			}

			if (oper.at(0) == '/') {
				result = val1.toDouble(&ok) / val2.toDouble(&ok1);
				break;
			}

			break;
		}

		if (!ok || !ok1) {
			return (ERR_CONVERT);
		}

		pos++;

		partmp.number(result, 'g', 3);
		str->replace(j, (i - j) + 1, QString("%1").arg(result, 3, 'f', 3));
	}

	qDebug() << "9857" << val1 << val2 << *str;

	pos = 1;
	regex.setPattern("[+-]{1,1}");

	while ((pos = str->indexOf(regex, pos)) >= 0) {
		oper = str->mid(pos, 1);

		qDebug() << "789,000" << oper << pos;

		val2 = "";
		dot = false;
		minus = false;

		for (i = pos + 1; i <= str->length(); i++) {
			qDebug() << "789,123" << val2 << str->at(i);

			if ((str->at(i) == '.')) {
				if ((dot)) {
					return (ERR_DOUBLE_DOT);
				}

				dot = true;
			}

			if ((str->at(i) == '-')) {
				if (minus || dot) {
					break;
				}

				if (i == 0) {
					minus = true;
				} else {
					if ((str->at(i - 1) == '*') || (str->at(i - 1) == '/') || (str->at(i - 1) == '+')
					        || (str->at(i - 1) == '-') || (str->at(i - 1) == '(')) {
						minus = true;
					} else {
						break;
					}
				}

				//            if(minus && val2.length() > 0)
				//                break;
			}

			if (!((str->at(i).isDigit() || (str->at(i) == '.') || (str->at(i) == '-')))) {
				break;
			}

			val2 += str->at(i);
		}

		i--;

		val1 = "";

		dot = false;
		minus = false;

		for (j = pos - 1; j >= 0; j--) {
			qDebug() << "789,456" << val1 << str->at(j);

			if ((str->at(j) == '.')) {
				if ((dot)) {
					return (ERR_DOUBLE_DOT);
				}

				dot = true;
			}

			if ((str->at(j) == '-')) {
				if ((minus)) {
					break;
				}

				if (j == 0) {
					minus = true;
				} else if ((str->at(j - 1) == '*') || (str->at(j - 1) == '/') || (str->at(j - 1) == '+')
				           || (str->at(j - 1) == '-') || (str->at(j - 1) == '(')) {
					minus = true;
				}
			}

			if (!((str->at(j).isDigit() || (str->at(j) == '.') || (str->at(j) == '-')))) {
				break;
			}

			val1.prepend(str->at(j));
		}

		j++;

		if ((val1 == "-") && (oper == "-")) {
			val1 = "0";
			oper = "+";
		}

		qDebug() << "789" << val1 << oper << val2 << *str;

		if (val1.isEmpty()) {
			val1 = "0";    //return(ERR_NO_PARAM);
		}

		if (val2.isEmpty()) {
			return (ERR_NO_PARAM);    //val2 = "0";
		}

		while (1) {
			if (oper.at(0) == '-') {
				result = val1.toDouble(&ok) - val2.toDouble(&ok1);
				break;
			}

			result = val1.toDouble(&ok) + val2.toDouble(&ok1);
			break;
		}

		if (!ok || !ok1) {
			return (ERR_CONVERT);
		}

		pos++;

		partmp.number(result, 'g', 3);
		str->replace(j, (i - j) + 1, QString("%1").arg(result, 0, 'f', 3));
	}

	str->remove('(');
	str->remove(')');
	return (0);
}
