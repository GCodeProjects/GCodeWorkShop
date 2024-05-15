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

#ifndef GCODERINFO_H
#define GCODERINFO_H

#include <QString>  // for QString

class QSettings;

#include <documentinfo.h>   // for DocumentInfo


class GCoderInfo: public DocumentInfo
{
public:
	GCoderInfo();
	GCoderInfo(const GCoderInfo& that);

	QString documentType() const override;

	DocumentInfo* clone() const override;

	DocumentInfo& operator=(const DocumentInfo& that) override;
	virtual GCoderInfo& operator=(const GCoderInfo& that);

	int cursorPos = 0;
	int highlightMode = 0;

protected:
	void loadChild(QSettings* cfg) override;
	void saveChild(QSettings* cfg) const override;
};

#endif // GCODERINFO_H
