/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
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

#include <QRegularExpression>       // for QRegularExpression
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QString>                  // for QString
#include <QtDebug>                  // for qDebug
#include <QtGlobal>                 // for forever

#include <utils/guessfilename.h>    // IWYU pragma: associated

#include "filepatterns.h"   // for FILENAME_FADAL, FILENAME_FANUC, FILENAME_HEID1, FILENAME_HEID2, FILENAME_OSP


Utils::FileExt Utils::guessFileNameByProgNum(const QString& text)
{
	FileExt file;
	QRegularExpression regex;

	if (text.isEmpty()) {
		return file;
	}

	forever { // Detect program name like: O0032, %_N_PR25475002_MPF, $O0004.MIN%...
		regex.setPattern(FILENAME_SINU840);
		auto match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.remove("%_N_");
			//name1.remove(QRegularExpression("_(MPF|SPF|TEA|COM|PLC|DEF|INI)"));

			regex.setPattern("_(MPF|SPF|TEA|COM|PLC|DEF|INI)");
			match = regex.match(file.name);

			if (match.hasMatch()) {
				file.ext = match.captured();
				file.name.remove(file.ext);
				file.ext.remove(" ");
				file.ext.replace('_', '.');;
			} else {
				file.ext.clear();
			}

			qDebug() << "3" << file.name << file.ext;
			break;
		}

		regex.setPattern(FILENAME_OSP);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.remove("$");

			regex.setPattern("\\.(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}");
			match = regex.match(file.name);

			if (match.hasMatch()) {
				file.ext = match.captured();
				file.name.remove(file.ext);
				file.ext.remove(" ");
				file.ext.remove("%");
			} else {
				file.ext.clear();
			}

			file.name.remove(QRegularExpression("[%]{0,1}"));
			//name1.remove(QRegularExpression(".(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"));
			qDebug() << "10" << file.name << file.ext;
			break;
		}

		regex.setPattern(FILENAME_SINU);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			//name1.remove(QRegularExpression("%(MPF|SPF|TEA)[\\s]{0,3}"));

			regex.setPattern("%(MPF|SPF|TEA)[\\s]{0,3}");
			match = regex.match(file.name);

			if (match.hasMatch()) {
				file.ext = match.captured();
				file.name.remove(file.ext);
				file.ext.remove(" ");
				file.ext.remove("%");
				file.ext.prepend('.');
			} else {
				file.ext.clear();
			}


			//name1.append(ext);
			qDebug() << "11" << file.name << file.ext;
			break;
		}

		regex.setPattern(FILENAME_PHIL);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.remove(QRegularExpression("%PM[\\s]{1,}[N]{1,1}"));
			file.ext.clear();
			qDebug() << "12" << file.name << file.ext;
			break;
		}

		regex.setPattern(FILENAME_FANUC);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.replace(':', 'O');

			//                if(name1.at(0)!='O')
			//                    name1[0]='O';
			//                if(name1.at(0)=='O' && name1.at(1)=='O')
			//                    name1.remove(0,1);

			file.ext.clear();
			qDebug() << "13" << file.name << file.ext;
			break;
		}

		regex.setPattern(FILENAME_HEID1);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.remove("%");
			file.name.remove(QRegularExpression("\\s"));
			file.ext.clear();
			qDebug() << "14" << file.name << file.ext;
			break;
		}

		regex.setPattern(FILENAME_HEID2);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.remove(QRegularExpression("(BEGIN)(\\sPGM\\s)"));
			file.name.remove(QRegularExpression("(\\sMM|\\sINCH)"));
			file.ext.clear();

			break;
		}

		regex.setPattern(FILENAME_FADAL);
		match = regex.match(text);

		if (match.hasMatch()) {
			file.name = match.captured();
			file.name.remove("N1");

			file.ext.clear();
			qDebug() << "13" << file.name << file.ext;
			break;
		}

		file.name = "";
		file.ext = "";
		break;
	}

	file.name.remove(".");
	file.name.remove(",");
	file.name = file.name.simplified();
	file.ext = file.ext.simplified();
	return file;
}

QString Utils::guessFileNameByComments(const QString& text)
{
	QString fileName;

	forever {
		QRegularExpression regex;
		regex.setPattern("(;)[\\w:*=+ -]{4,64}");
		auto match = regex.match(text);

		if (match.capturedStart() >= 2) {
			fileName = match.captured();
			fileName.remove(";");
			break;
		}

		regex.setPattern("(\\()[\\w:*=+ -]{4,64}(\\))");
		match = regex.match(text);

		if (match.capturedStart() >= 2) {
			fileName = match.captured();
			fileName.remove("(");
			fileName.remove(")");
			break;
		}

		break;
	}

	return fileName.simplified();
}

Utils::FileExt Utils::guessFileNameByRegExp(const QString& text, const QString& expression)
{
	FileExt file;
	QRegularExpression regex;
	regex.setPattern(expression);
	auto match = regex.match(text);

	if (match.hasMatch()) {
		file.name = match.captured();
		file.name.remove(";");
		file.name.remove("(");
		file.name.remove(")");
		file.name.remove("[");
		file.name.remove("]");

		int pos = file.name.lastIndexOf('.');

		if (pos >= 0) {
			file.ext = file.name.mid(pos);
			file.name.remove(regex);
			file.ext.remove(" ");
		} else {
			file.ext.clear();
		}
	}

	file.name.remove("*");
	file.name.remove(",");
	file.name = file.name.simplified();
	file.ext = file.ext.simplified();

	if (!file.ext.isEmpty()) {
		file.name.remove(file.ext);
	}

	return file;
}
