/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
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

#include <QCoreApplication>         // for 
#include <QChar>                    // for operator==, QChar, operator!=
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch

#include <basic_interpreter.h>
#include <utils/expressionparser.h>  // for processBrc, ERR_CONVERT, ERR_DOUBLE_DOT, ERR_NO_BRAC, ERR_NO_PARAM

#include "utils-compilemacro.h"


#define tr(s) (QCoreApplication::translate("CompileMacro", s))


int Utils::CompileMacro::compile(const QString& tx)
{
	int error;
	m_result = tx;

	QRegularExpression regexBegin("\\{BEGIN\\}");
	regexBegin.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	QRegularExpression regexEnd("\\{END\\}");
	regexEnd.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	auto matchBegin = regexBegin.match(m_result);
	auto matchEnd = regexEnd.match(m_result, matchBegin.capturedEnd());

	if (!matchBegin.hasMatch() || !matchEnd.hasMatch()) {
		m_status = tr("No constant definition .\n{BEGIN}\n...\n{END}\n No macro ?");
		return -1;
	}

	QRegularExpression regex("\\{\\$[A-Z0-9\\s]*\\b[=\\n\\r]");
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	auto match = regex.match(m_result, matchBegin.capturedEnd());

	while (match.hasMatch() && match.capturedEnd() <= matchEnd.capturedStart()) {
		int pos = match.capturedEnd();
		QString param = match.captured();
		param = param.remove(' ');
		param = param.remove('{');
		param = param.remove('=');

		QString val = "";

		do {
			val = val + m_result.at(pos);
			pos++;

			if (m_result.at(pos) == '\n' || m_result.at(pos) == '{') {
				m_status = tr("Param list: no bracket \'}\' !");
				return -1;
			}
		} while ((m_result.at(pos) != '}'));

		val = val.remove(' ');

		int i = matchEnd.capturedStart();

		while ((i = m_result.indexOf(param, i)) >= 0) {
			m_result.replace(i, param.length(), val);
		}

		match = regex.match(m_result, pos);
	}

	m_result.remove(matchBegin.capturedStart(), matchEnd.capturedEnd() - matchBegin.capturedStart());

	regex.setPattern("\\{[-+*=.,()$/0-9A-Z\\s]*\\b[-+*=.,()$/0-9A-Z\\s]*[}]");
	match = regex.match(m_result);

	while (match.hasMatch()) {
		QString param = match.captured();
		param = param.simplified();
		param = param.remove(QChar(' '));
		param = param.replace(',', '.');

		if (!param.isEmpty()) {
			QString paramTmp = param;
			error = Utils::processBrc(&param);

			if (error < 0) {
				setError(error, paramTmp);
				return -1;
			}

			if (!param.isEmpty()) {
				paramTmp = param;
				error = Utils::processBrc(&param);

				if (error < 0) {
					setError(error, paramTmp);
					return -1;
				}
			}

			QString val = param;
			val = val.remove('{');
			val = val.remove('}');
			m_result.replace(match.capturedStart(), match.capturedLength(), val);
		}

		match = regex.match(m_result);
	}

	regexBegin.setPattern("\\{BEGIN_SUBS\\}");
	matchBegin = regexBegin.match(m_result);
	regex.setPattern("\\{END_SUBS\\}");
	matchEnd = regex.match(m_result, matchBegin.capturedEnd());
	QString basicSubs;

	if (matchBegin.hasMatch() && matchEnd.hasMatch()) {
		basicSubs = m_result.mid(matchBegin.capturedEnd(),
		                         matchEnd.capturedStart() - matchBegin.capturedEnd());
		m_result.remove(matchBegin.capturedStart(),
		                matchEnd.capturedEnd() - matchBegin.capturedStart() + 1);
	}

	regexBegin.setPattern("\\{BEGIN_BASIC\\}");
	regexEnd.setPattern("\\{END_BASIC\\}");
	matchBegin = regexBegin.match(m_result);
	matchEnd = regexEnd.match(m_result, matchBegin.capturedEnd());

	while (matchBegin.hasMatch() && matchEnd.hasMatch()) {
		QString basicCode = m_result.mid(matchBegin.capturedEnd(),
		                                 matchEnd.capturedStart() - matchBegin.capturedEnd());
		m_result.remove(matchBegin.capturedStart(),
		                matchEnd.capturedEnd() - matchBegin.capturedStart() + 1);
		basicCode.append(basicSubs);

		error = BasicInterpreter().interpretBasic(basicCode);

		if (error > 0) {
			setBasicError(error);
			return -1;
		}

		m_result.insert(matchBegin.capturedStart(), basicCode);
		matchBegin = regexBegin.match(m_result, matchBegin.capturedStart());
		matchEnd = regexEnd.match(m_result, matchBegin.capturedEnd());
	}

	cleanUp(m_result);
	return 1;
}

const QString& Utils::CompileMacro::result()
{
	return m_result;
}

const QString& Utils::CompileMacro::status()
{
	return m_status;
}

void Utils::CompileMacro::setError(int error, const QString& tx)
{
	if (error < 0) {
		switch (error) {
		case ERR_NO_BRAC:
			m_status = tr("No ( or ) !");
			break;

		case ERR_NO_PARAM:
			m_status = tr("Function parameter not found ! \n Check +-*/.\n\"%1\"").arg(tx);
			break;

		case ERR_CONVERT:
			m_status = tr("Wrong number !");
			break;

		case ERR_UNKNOWN_FUNC:
			m_status = tr("Unknown math function !\n\"%1\"").arg(tx);
			break;

		case ERR_DOUBLE_DOT:
			m_status = tr("Decimal point or minus writed two times !\n\"%1\"").arg(tx);
			break;

		default:
			m_status = tr("Unknown error !");
		}

		m_status = tr("BRC error:\n%1\nCode=%2").arg(m_status).arg(error);
	}
}

void Utils::CompileMacro::setBasicError(int error)
{
	switch (error) {
	case 0:
		m_status = tr("Syntax error");
		break;

	case 1:
		m_status = tr("Unbalanced parentheses");
		break;

	case 2:
		m_status = tr("No expression present");
		break;

	case 3:
		m_status = tr("Equals sign expected");
		break;

	case 4:
		m_status = tr("Not a variable");
		break;

	case 5:
		m_status = tr("Label table full");
		break;

	case 6:
		m_status = tr("Duplicate label");
		break;

	case 7:
		m_status = tr("Undefined label");
		break;

	case 8:
		m_status = tr("THEN expected");
		break;

	case 9:
		m_status = tr("TO expected");
		break;

	case 10:
		m_status = tr("Too many nested FOR loops");
		break;

	case 11:
		m_status = tr("NEXT without FOR");
		break;

	case 12:
		m_status = tr("Too many nested GOSUBs");
		break;

	case 13:
		m_status = tr("RETURN without GOSUBs");
		break;

	default:
		m_status = tr("Unknown error");
	}

	m_status = tr("Basic interpreter error:\n%1\nCode=%2").arg(m_status).arg(error);
}

void Utils::CompileMacro::cleanUp(QString& str)  //remove not needed zeros
{
	QRegularExpression regex;
	regex.setPattern("([\\d]+[.][-+.0-9]+)|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$");
	auto match = regex.match(str);

	// FIXME: This slow code deletes non-significant zeros one at a time.
	while (match.hasMatch()) {
		int pos = match.capturedEnd();

		if (match.capturedLength(1) != 0 && str.at(match.capturedEnd() - 1) == '0') {
			str.remove(match.capturedEnd() - 1, 1);
			pos = match.capturedStart();
		}

		match = regex.match(str, pos);
	}
}
