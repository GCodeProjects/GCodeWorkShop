/*
 *  Copyright (C) 2023-2025 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of GCodeWorkShop.
 *
 *  GCodeWorkShop is free software: you can redistribute it and/or modify
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

#include <QChar>                    // for operator!=, QChar, operator==
#include <QLatin1Char>              // for QLatin1Char
#include <QLatin1String>            // for QLatin1String
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString, operator+, QCharRef, QString::SectionIncludeTrailingSep

#include "renumberoptions.h"    // for RenumberOptions, RenumberOptions::RemoveAll, RenumberOptions::RenumberAll
#include "utils-renumber.h"


bool Utils::renumber(QString& text, const RenumberOptions& opt, const std::function<bool(int)>& interrupt)
{
	bool changed = false;
	QString result;
	int num = opt.startAt;
	int substr_start = 0;
	int substr_end = 0;

	while (substr_end >= 0) {
		substr_end = text.indexOf('\n', substr_start);
		QString line = text.mid(substr_start, substr_end - substr_start + 1);
		substr_start = substr_end + 1;
		bool localChange = false;

		if (interrupt(substr_start)) {
			return false;
		}

		switch (opt.mode) {
		case RenumberOptions::RenumberWithN:
			localChange = renumberWithN(line, num, opt);
			break;

		case RenumberOptions::RenumberAll:
			localChange = renumberAll(line, num, opt);
			break;

		case RenumberOptions::RemoveAll:
			localChange = renumberRemoveAll(line);
			break;

		case RenumberOptions::RenumberWithoutN:
			localChange = renumberWithoutN(line, num, opt);
			break;

		default:
			;
		}

		if (localChange) {
			changed = true;
			num += opt.inc;
		}

		result += line;
	}

	text = result;
	return changed;
}

bool Utils::renumberWithoutN(QString& line, int num, const RenumberOptions& opt)
{
	bool changed = false;
	QString i_tx;
	QRegularExpression regex;

	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("^[0-9]{1,9}\\s\\s");

	i_tx = QString("%1").arg(num, opt.width);
	i_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
	i_tx += "  ";

	auto match = regex.match(line);

	if (match.hasMatch()) {
		line.replace(match.capturedStart(), match.capturedLength(), i_tx);
		changed = true;
	} else {
		if (opt.renumEmpty) {
			line.insert(0, i_tx);
			changed = true;
		}
	}

	return changed;
}

bool Utils::renumberWithN(QString& line, int num, const RenumberOptions& opt)
{
	bool changed = false;
	int pos;
	long int i, it;
	QString f_tx;
	QRegularExpression regex;
	bool ok, insertSpace;

	pos = 0;
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
	auto match = regex.match(line);

	while (match.hasMatch()) {
		pos = match.capturedStart();
		f_tx = match.captured();

		//qDebug() << f_tx;

		if (pos > 0)
			if (line[pos - 1].isLetterOrNumber()) {
				pos = match.capturedEnd();
				continue;
			}

		insertSpace = true;

		if (f_tx.endsWith(QLatin1Char(' '))) {
			insertSpace = false;
		}

		if (!f_tx.contains(QLatin1Char(' ')) && !f_tx.contains(QLatin1Char('\n'))) {
			i = match.capturedLength();
		} else {
			i = match.capturedLength() - 1;
		}

		if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
		        && !f_tx.contains(QLatin1Char(';'))) {
			f_tx.remove(0, 1);
			f_tx.remove(QLatin1Char(' '));

			if (!f_tx.isEmpty()) {
				it = f_tx.toInt(&ok);
			} else {
				it = 0;
			}

			if (((it >= opt.from) || (opt.renumMarked && it == 0)) && (it < opt.to)) {
				f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
				f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));

				if (insertSpace) {
					f_tx.append(QLatin1String(" "));
				}

				line.replace(pos, i, f_tx);
				changed = true;
			}
		}

		match = regex.match(line, match.capturedEnd());
	}

	return changed;
}

bool Utils::renumberAll(QString& line, int num, const RenumberOptions& opt)
{
	int pos;
	QString f_tx, i_tx;
	QRegularExpression regex;

	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[Nn]{1,1}[0-9]+[\\s]{0,}|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");

	pos = 0;

	if (line.isEmpty()) {
		if (!opt.renumEmpty) {
			return false;
		}

		f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
		f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
		line.insert(0, f_tx);
		return true;
	}

	auto match = regex.match(line, pos);

	if (match.hasMatch() && (line.at(0) != QLatin1Char('$'))) {
		pos = match.capturedStart();
		i_tx = match.captured();
		i_tx.remove('\n');

		if ((!i_tx.contains(QLatin1Char('(')) && !i_tx.contains(QLatin1Char('\''))
		        && !i_tx.contains(QLatin1Char(';')))) {
			f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
			f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
			f_tx.append(QLatin1String(" "));
			line.replace(i_tx, f_tx);
			return true;
		} else if (opt.renumComm) {
			return false;
		}
	}

	if ((line.at(0) == QLatin1Char('N')) && (!line.at(1).isLetter())) {
		f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
		f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
		f_tx.append(QLatin1String(" "));
		line.replace(0, 1, f_tx);
		return true;
	}

	if (((line.at(0) != QLatin1Char('%')) && (line.at(0) != QLatin1Char(':'))
	        && (line.at(0) != QLatin1Char('O')) && (line.at(0) != QLatin1Char('$')))) {
		f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
		f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
		f_tx.append(QLatin1String(" "));
		line.insert(0, f_tx);
		return true;
	}

	return false;
}

bool Utils::renumberRemoveAll(QString& line)
{
	bool changed = false;
	QString f_tx;
	QRegularExpression regex;

	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
	auto match = regex.match(line);

	while (match.hasMatch()) {
		int pos = match.capturedStart();
		f_tx = match.captured();

		//qDebug() << f_tx;

		if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
		        && !f_tx.contains(QLatin1Char(';'))) {
			line.remove(pos, match.capturedLength());
			changed = true;
		} else {
			pos = match.capturedEnd();
		}

		match = regex.match(line, pos);
	}

	return changed;
}
