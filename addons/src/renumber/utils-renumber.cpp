/*
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
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


void Utils::renumber(const RenumberOptions& opt, QString& tx)
{
	int lineCount = tx.count("\n");

	switch (opt.mode) {
	case RenumberOptions::RenumberWithN:
		renumberWithN(opt, tx);
		break;

	case RenumberOptions::RenumberAll:
		renumberAll(opt, tx, lineCount);
		break;

	case RenumberOptions::RemoveAll:
		removeAll(tx);
		break;

	case RenumberOptions::RenumberWithoutN:
		renumberWithoutN(opt, tx, lineCount);
		break;

	default:
		;
	}
}


void Utils::renumberWithoutN(const RenumberOptions& opt, QString& tx, int lineCount)
{
	QString line, i_tx, new_tx;
	QRegularExpression regex;

	int num = opt.startAt;
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("^[0-9]{1,9}\\s\\s");

	for (int i = 0; i < lineCount; i++) {
		line = tx.section(QLatin1Char('\n'), i, i);

		i_tx = QString("%1").arg(num, opt.width);
		i_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
		i_tx += "  ";

		auto match = regex.match(line);

		if (match.hasMatch()) {
			line.replace(match.capturedStart(), match.capturedLength(), i_tx);
			num += opt.inc;
		} else {
			if (opt.renumEmpty) {
				line.insert(0, i_tx);
				num += opt.inc;
			}
		}

		new_tx += line + '\n';
	}

	tx = new_tx;
}

void Utils::renumberWithN(const RenumberOptions& opt, QString& tx)
{
	int pos;
	long int i, num, it;
	QString f_tx;
	QRegularExpression regex;
	bool ok, insertSpace;

	pos = 0;
	num = opt.startAt;
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
	auto match = regex.match(tx);

	while (match.hasMatch()) {
		pos = match.capturedStart();
		f_tx = match.captured();

		//qDebug() << f_tx;

		if (pos > 0)
			if (tx[pos - 1].isLetterOrNumber()) {
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

				tx.replace(pos, i, f_tx);
				num += opt.inc;
			}
		}

		match = regex.match(tx, match.capturedEnd());
	}
}

void Utils::renumberAll(const RenumberOptions& opt, QString& tx, int lineCount)
{
	int pos;
	long int num;
	QString f_tx, line, i_tx, new_tx;
	QRegularExpression regex;

	num = opt.startAt;
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[Nn]{1,1}[0-9]+[\\s]{0,}|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");

	for (int i = 0; i < lineCount; i++) {
		line = tx.section(QLatin1Char('\n'), i, i, QString::SectionIncludeTrailingSep);

		//qDebug() << line;

		pos = 0;

		while (1) {
			if (line.isEmpty()) {
				if (!opt.renumEmpty) {
					break;
				}

				f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
				f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
				num += opt.inc;
				line.insert(0, f_tx);
				break;
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
					num += opt.inc;
					f_tx.append(QLatin1String(" "));
					line.replace(i_tx, f_tx);
					break;
				} else if (opt.renumComm) {
					break;
				}
			}

			if ((line.at(0) == QLatin1Char('N')) && (!line.at(1).isLetter())) {
				f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
				f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
				num += opt.inc;
				f_tx.append(QLatin1String(" "));
				line.replace(0, 1, f_tx);
				break;
			}

			if (((line.at(0) != QLatin1Char('%')) && (line.at(0) != QLatin1Char(':'))
			        && (line.at(0) != QLatin1Char('O')) && (line.at(0) != QLatin1Char('$')))) {
				f_tx = QString(QLatin1String("N%1")).arg(num, opt.width);
				f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
				num += opt.inc;
				f_tx.append(QLatin1String(" "));
				line.insert(0, f_tx);
				break;
			}

			break;
		}

		new_tx += line; // + '\n';
	}

	tx = new_tx;
}

void Utils::removeAll(QString& tx)
{
	long int num;
	QString f_tx;
	QRegularExpression regex;

	num = 0;
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
	auto match = regex.match(tx);

	while (match.hasMatch()) {
		int pos = match.capturedStart();
		f_tx = match.captured();

		//qDebug() << f_tx;

		if (!f_tx.contains(QLatin1Char('(')) && !f_tx.contains(QLatin1Char('\''))
		        && !f_tx.contains(QLatin1Char(';'))) {
			tx.remove(pos, match.capturedLength());
			num++;
		} else {
			pos = match.capturedEnd();
		}

		match = regex.match(tx, pos);
	}
}
