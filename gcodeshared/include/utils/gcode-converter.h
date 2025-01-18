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

#ifndef UTILS_GCODE_CONVERTER_H
#define UTILS_GCODE_CONVERTER_H

#include <QByteArray>   // for QByteArray

class QSettings;
class QString;


namespace GCode {

struct Converter {
	Converter();

	QString fromRawData(const QByteArray& data) const;
	QByteArray toRawData(const QString& text) const;

	struct Options {
		QByteArray codecName = QByteArray{};
		// When loading files, discard "extra" empty lines.
		bool dropEmptyLine = true;
		// When loading files, discard the upper 128 code points commonly used for national alphabets.
		bool dropExtented = false;
		// When loading files, discard control characters (0x00-0x1f) other than '\n' and '\r'.
		bool dropControll = true;

		void load(QSettings* settings);
		void save(QSettings* settings);
	};

	Options options;

	static Options defaultOptions();
	static void setDefaultOptions(const Options& options);
};

} // namespace GCode

#endif // UTILS_GCODE_CONVERTER_H
