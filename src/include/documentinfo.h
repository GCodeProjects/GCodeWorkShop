/*
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

#ifndef DOCUMENTINFO_H
#define DOCUMENTINFO_H

#include <memory>
#include <QByteArray>
#include <QString>

#define DOCUMENTINFO_CFG_KEY_TYPE       "Type"

class QSettings;


class DocumentInfo
{
public:
	typedef std::shared_ptr<DocumentInfo> Ptr;

	DocumentInfo();
	virtual ~DocumentInfo();

	virtual QString documentType() const;

	virtual DocumentInfo* clone() const;

	virtual DocumentInfo& operator=(const DocumentInfo& that);

	void load(QSettings* cfg);
	void save(QSettings* cfg) const;

	QString filePath;
	bool readOnly = false;
	QByteArray geometry = QByteArray();

protected:
	virtual void loadChild(QSettings* cfg);
	virtual void saveChild(QSettings* cfg) const;
};

#endif // DOCUMENTINFO_H
