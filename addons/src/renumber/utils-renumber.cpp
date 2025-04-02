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


#define APOSTROPHE_COMMENT_REGEXPR  "\\'[^\\n\\']*[\\']?"
#define GCODE_COMMENT_REGEXPR       "\\([^\\n\\)]*[\\)]?"
#define N_MARK_REGEXPR              "^([ \\t]*N\\b[ \\t]*)"
#define N_WORD_REGEXPR              "([ \\t]*N([0-9 \\t]*[0-9])[ \\t]*)"
#define SINUMERIK_COMMENT_REGEXPR   ";[^\\n]*"


bool Utils::renumber(QString& text, const RenumberOptions& opt, const std::function<bool(int)>& interrupt)
{
	bool changed = false;
	QString result;
	int num = opt.startAt;
	int width = opt.applyWidth ? opt.width : 0;
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
			localChange = renumberWithN(line, num, width, opt.from, opt.to, opt.renumMarked);
			break;

		case RenumberOptions::RenumberAll:
			localChange = renumberAll(line, num, width, opt.renumEmpty, opt.renumComm, opt.keepExisting);
			break;

		case RenumberOptions::RemoveAll:
			localChange = renumberRemoveAll(line, true);
			break;

		case RenumberOptions::RenumberWithoutN:
			localChange = renumberWithoutN(line, num, width, opt.renumEmpty);
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

bool Utils::renumberWithoutN(QString& line, int num, int width, bool renumAll)
{
	static QRegularExpression regex{
		"^([0-9]{1,9}[ \\t]{2,2})",
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatch&& match = regex.match(line);
	QString n_word = QString("%1  ").arg(num, width, 10, QLatin1Char{' '});

	if (match.hasMatch()) {
		line.replace(match.capturedStart(1), match.capturedLength(1), n_word);
	} else if (renumAll) {
		line.insert(0, n_word);
	} else {
		return false;
	}

	return true;
}

bool Utils::renumberWithN(QString& line, int num, int width, int from, int to, bool renumMarked)
{
	static QRegularExpression check_num{
		N_WORD_REGEXPR              // groups 1 and 2
		"|"
		N_MARK_REGEXPR              // group 3
		"|"
		GCODE_COMMENT_REGEXPR
		"|"
		APOSTROPHE_COMMENT_REGEXPR
		"|"
		SINUMERIK_COMMENT_REGEXPR,
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatch&& match = check_num.match(line);
	bool num_captured = match.capturedLength(2) > 0;
	bool mark_captured = match.capturedLength(3) > 0;

	if (num_captured) {
		QString digits = match.captured(2);
		digits.remove(' ');
		digits.remove('\t');
		int current = digits.toInt();

		if (current < from || current > to) {
			return false;
		}

		line.remove(match.capturedStart(1), match.capturedLength(1));
	} else if (mark_captured && renumMarked) {
		line.remove(match.capturedStart(3), match.capturedLength(3));

		// The regular expression N_MARK_REGEXPR can capture the end-of-line character.
		if (line.isEmpty()) {
			line.append('\n');
		}
	} else {
		return false;
	}

	insertNWord(line, num, width);
	return true;
}

bool Utils::renumberAll(QString& line, int num, int width, bool renumEmpty, bool renumComm, bool keepExisting)
{
	bool changed = false;
	static QRegularExpression check_num{
		"^[ \\t]*(:|%|\\$|O|PM\\W)" // group 1
		"|"
		N_WORD_REGEXPR              // groups 2 and 3
		"|"
		N_MARK_REGEXPR              // group 4
		"|"
		GCODE_COMMENT_REGEXPR
		"|"
		APOSTROPHE_COMMENT_REGEXPR
		"|"
		SINUMERIK_COMMENT_REGEXPR,
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatch&& match = check_num.match(line);
	bool stop_captured = match.capturedLength(1) > 0;
	bool num_captured = match.capturedLength(2) > 0;
	bool mark_captured = match.capturedLength(4) > 0;

	// Exit if line start with ':', '%', '$', 'O' or 'PM'
	// TODO: add signatures for blocks that cannot be numbered
	if (stop_captured) {
		return false;
	}

	if (num_captured) {
		line.remove(match.capturedStart(2), match.capturedLength(2));
		changed = true;
	} else if (mark_captured) {
		line.remove(match.capturedStart(4), match.capturedLength(4));
		changed = true;

		// The regular expression N_MARK_REGEXPR can capture the end-of-line character.
		if (line.isEmpty()) {
			line.append('\n');
		}
	}

	static QRegularExpression check_empty{
		"([A-Z@#])"                         // group 1
		"|"
		"(" GCODE_COMMENT_REGEXPR ")"       // group 2
		"|"
		"(" APOSTROPHE_COMMENT_REGEXPR ")"  // group 3
		"|"
		"(" SINUMERIK_COMMENT_REGEXPR ")",  // group 4
		QRegularExpression::CaseInsensitiveOption
	};
	match = check_empty.match(line);
	bool not_empty = match.capturedLength(1) > 0;
	bool comments_captured = match.capturedLength(2) > 0 || match.capturedLength(3) > 0 || match.capturedLength(4) > 0;
	bool insert = ((num_captured || mark_captured) && keepExisting) || not_empty || renumEmpty || (renumComm
	              && comments_captured);

	if (insert) {
		insertNWord(line, num, width);
		changed = true;
	}

	return changed;
}

bool Utils::renumberRemoveAll(QString& line, bool removeMarked)
{
	static QRegularExpression check_num{
		N_WORD_REGEXPR              // group 1 and 2
		"|"
		N_MARK_REGEXPR              // group 3
		"|"
		GCODE_COMMENT_REGEXPR
		"|"
		APOSTROPHE_COMMENT_REGEXPR
		"|"
		SINUMERIK_COMMENT_REGEXPR,
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatch&& match = check_num.match(line);
	bool num_captured = match.capturedLength(1) > 0;
	bool mark_captured = match.capturedLength(3) > 0;

	if (num_captured) {
		line.remove(match.capturedStart(1), match.capturedLength(1));
	} else if (mark_captured && removeMarked) {
		line.remove(match.capturedStart(3), match.capturedLength(3));
	} else {
		return false;
	}

	return true;
}

void Utils::insertNWord(QString& line, int num, int width)
{
	QString n_word = QString(QLatin1String("N%1")).arg(num, width, 10, QLatin1Char{'0'});

	if (line.size() > 0 && line.at(0) != '\n') {
		n_word.append(' ');
	}

	line.prepend(n_word);
}
