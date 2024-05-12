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

#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QObject>
#include <QString>
#include <QStringList>

class QSettings;


class RecentFiles : public QObject
{
	Q_OBJECT

public:
	enum {
		DEFAULT_SIZE = 16,
		MAX_SIZE = 32
	};

	explicit RecentFiles(QObject *parent = nullptr);

	QString file(int id);
	QStringList fileList();

	void add(const QString &file);
	void remove(const QString &file);
	void clear();

	int max();
	void setMax(int max);

	void load(QSettings *cfg);
	void save(QSettings *cfg);

signals:
	void saveRequest();
	void fileListChanged(const QStringList &files);

protected:
	void trim();

	int m_max;
	QStringList m_files;
};

#endif // RECENTFILES_H
