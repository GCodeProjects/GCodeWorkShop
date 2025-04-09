/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
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

#include <algorithm>	// for max, min

#include <QChar>                            // for operator==, QChar
#include <QRegularExpression>               // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>          // for QRegularExpressionMatch
#include <QRegularExpressionMatchIterator>  // for QRegularExpressionMatchIterator
#include <QString>                          // for QString, operator+, operator!=, QCharRef

#include <utils/removezeros.h>  // for Utils::removeZeros

#include "swapaxesoptions.h"    // for SwapAxesOptions
#include "utils-swapaxes.h"


#define APOSTROPHE_COMMENT_REGEXPR  "\\'[^\\n\\r]*\\'"
#define GCODE_COMMENT_REGEXPR       "\\([^\\n\\r]*\\)"
#define SINUMERIK_COMMENT_REGEXPR   ";[^\\n\\r]*$"
// Group 2: an word
// Group 3: an address
// Group 4: an value
//                                         |<     sign      >| |<     digits-dot-digits     >| |<  digits-dot  >| |<  dot-digits  >| |<   digits  >|
#define WORD_REGEXPR    "(\\B|\\b)((%1|%2)([ \\t]*[-+]?[ \\t]*((\\d[ \\t\\d]*\\.[ \\t\\d]*\\d)|(\\d[ \\t\\d]*\\.)|(\\.[ \\t\\d]*\\d)|([ \\t\\d]*\\d))))"


bool Utils::swapAxes(QString& text, const SwapAxesOptions& opt, const std::function<bool(int)>& interrupt)
{
	double min = std::min(opt.limit.min, opt.limit.max);
	double max = std::max(opt.limit.min, opt.limit.max);
	int oper = opt.convert.enable ? opt.convert.operation : SwapAxesOptions::Nop;
	double modifier = opt.convert.enable ? opt.convert.value : 0;
	return Utils::swapAxes(text, opt.from, opt.to,
	                       opt.limit.enable, min, max,
	                       oper, modifier, opt.precision,
	                       interrupt);
}

bool Utils::swapAxes(QString& text,
                     const QString& textToFind,
                     const QString& replacedText,
                     bool checkBound,
                     double min,
                     double max,
                     int oper,
                     double modifier,
                     int prec,
                     const std::function<bool (int)>& interrupt)
{
	if (textToFind.isEmpty()) {
		return false;
	}

	bool changed = false;
	QRegularExpression regex{
		QString(
		    WORD_REGEXPR
		    "|"
		    GCODE_COMMENT_REGEXPR
		    "|"
		    APOSTROPHE_COMMENT_REGEXPR
		    "|"
		    SINUMERIK_COMMENT_REGEXPR
		).arg(textToFind, replacedText),
		QRegularExpression::CaseInsensitiveOption
	};
	QRegularExpressionMatchIterator iterator = regex.globalMatch(text);
	int pos = 0;
	QString result;

	while (iterator.hasNext()) {
		if (interrupt(pos)) {
			return false;
		}

		QRegularExpressionMatch match = iterator.next();

		if (match.capturedLength(2) == 0) {
			continue;
		}

		QString digits = match.captured(4);
		digits.remove(' ');
		digits.remove('\t');
		bool ok;
		double val = digits.toDouble(&ok);

		if (!ok) {
			continue;
		}

		if (checkBound && (val < min || val > max)) {
			continue;
		}

		QString word;

		if (match.captured(3) == textToFind) {
			switch (oper) {
			case SwapAxesOptions::Add:
				val += modifier;
				break;

			case SwapAxesOptions::Subtract:
				val -= modifier;
				break;

			case SwapAxesOptions::Multiply:
				val *= modifier;
				break;

			case SwapAxesOptions::Divide:
				if (modifier != 0) {
					val /= modifier;
				}

				break;

			default:
				;
			}

			word = replacedText;
		} else {
			word = textToFind;
		}

		word.append(removeZeros(QString("%1").arg(val, 0, 'f', prec)));

		if ((word == "#" ||
		        word == "O" ||
		        word == "o" ||
		        word == "N" ||
		        word == "n") && word.endsWith('.')) {
			word.chop(1);
		}

		result.append(text.mid(pos, match.capturedStart(2) - pos));
		result.append(word);
		pos = match.capturedEnd(2);
		changed = true;
	}

	result.append(text.mid(pos));
	text = result;
	return changed;
}
