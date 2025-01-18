/*
 *  Copyright (C) 2025 Nick Egorrov, nicegorov@yandex.ru
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

#include <vector>       // for vector

#include <QByteArray>   // for QByteArray
#include <QSettings>    // for QSettings
#include <QString>      // for QString
#include <QVariant>     // for QVariant
#include <QTextCodec>   // for QTextCodec

#include <utils/gcode-converter.h>  // IWYU pragma: associated


namespace GCode {
namespace Private {

// see https://en.wikipedia.org/wiki/Euclidean_algorithm
int greatest_common_divisor(int a, int b)
{
	while (a != b) {
		if (a > b) {
			a -= b;
		} else {
			b -= a;
		}
	}

	return a;
}

int greatest_common_divisor(const std::vector<int>& values)
{
	if (values.empty()) {
		return 1;
	}

	int result = values.front();

	for (int a : values) {
		result = greatest_common_divisor(a, result);
	}

	return result;
}

void add_unique(std::vector<int>& values, int v)
{
	for (int i : values) {
		if (i == v) {
			return;
		}
	}

	values.push_back(v);
}

QByteArray filterData(const QByteArray& data,
                      bool dropEmptyLine,
                      bool dropExtented,
                      bool dropControll)
{
	int newLineCount = 0;
	int mergedLine = 1;

	if (dropEmptyLine) {
		std::vector<int> values;
		newLineCount = 0;

		for (char c : data) {
			if (c == '\n' || c == '\r') {
				++newLineCount;
			} else {
				if (newLineCount != 0) {
					add_unique(values, newLineCount);
					newLineCount = 0;
				}
			}
		}

		mergedLine = greatest_common_divisor(values);
	}

	QByteArray handled;
	newLineCount = 0;

	for (char c : data) {
		if (c == '\n' || c == '\r') {
			if (++newLineCount > mergedLine) {
				newLineCount = 1;
			}

			if (newLineCount > 1) {
				continue;
			}
		} else if (c < 0) {
			newLineCount = 0;

			if (dropExtented) {
				continue;
			}
		} else if (c < 0x20) {
			newLineCount = 0;

			if (dropControll) {
				continue;
			}
		} else {
			newLineCount = 0;
		}

		handled.push_back(c);
	}

	return handled;
}

QString fromRawData(const QByteArray& data,
                    bool dropEmptyLine,
                    bool dropExtented,
                    bool dropControll,
                    const QByteArray& codec)
{
	const QByteArray& handled = filterData(data,
	                                       dropEmptyLine,
	                                       dropExtented,
	                                       dropControll);
	QTextCodec* textCodec = QTextCodec::codecForName(codec);

	if (textCodec != nullptr) {
		return textCodec->toUnicode(handled);
	} else {
		return QString::fromLocal8Bit(handled);
	}
}

QByteArray toRawData(const QString& text, const QByteArray& codec)
{
	QTextCodec* textCodec = QTextCodec::codecForName(codec);

	if (textCodec != nullptr) {
		return textCodec->fromUnicode(text);
	} else {
		return text.toLocal8Bit();
	}
}

Converter::Options s_defaultOptions{};

} // namespase Private
} // namespase GCode


#define CFG_KEY_CODEC_NAME          "CodecName"
#define CFG_KEY_DROP_CONTROLL       "DropControll"
#define CFG_KEY_DROP_EMPTY_LINE     "DropEmptyLine"
#define CFG_KEY_DROP_EXTENDED       "DropExtented"

void GCode::Converter::Options::load(QSettings* settings)
{
	codecName = settings->value(CFG_KEY_CODEC_NAME, codecName).toByteArray();
	dropControll = settings->value(CFG_KEY_DROP_CONTROLL, dropControll).toBool();
	dropEmptyLine = settings->value(CFG_KEY_DROP_EMPTY_LINE, dropEmptyLine).toBool();
	dropExtented = settings->value(CFG_KEY_DROP_EXTENDED, dropExtented).toBool();
}

void GCode::Converter::Options::save(QSettings* settings)
{
	settings->setValue(CFG_KEY_CODEC_NAME, codecName);
	settings->setValue(CFG_KEY_DROP_CONTROLL, dropControll);
	settings->setValue(CFG_KEY_DROP_EMPTY_LINE, dropEmptyLine);
	settings->setValue(CFG_KEY_DROP_EXTENDED, dropExtented);
}

GCode::Converter::Options GCode::Converter::defaultOptions()
{
	return Private::s_defaultOptions;
}

void GCode::Converter::setDefaultOptions(const Options& options)
{
	Private::s_defaultOptions = options;
}

GCode::Converter::Converter() :
	options{defaultOptions()}
{
}

QString GCode::Converter::fromRawData(const QByteArray& data) const
{
	return Private::fromRawData(data,
	                            options.dropEmptyLine,
	                            options.dropExtented,
	                            options.dropControll,
	                            options.codecName);
}

QByteArray GCode::Converter::toRawData(const QString& text) const
{
	return Private::toRawData(text, options.codecName);
}
