/*
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

#ifndef UTILS_COMPILEMACRO_H
#define UTILS_COMPILEMACRO_H

#include <QString>  // for QString


namespace Utils {

class CompileMacro
{
public:
	int compile(const QString& text);
	const QString& result();
	const QString& status();

protected:
	QString m_status;
	QString m_result;

	void setError(int error, const QString& tx);
	void setBasicError(int error);
	void cleanUp(QString& str) ;
};
}

#endif // UTILS_COMPILEMACRO_H
