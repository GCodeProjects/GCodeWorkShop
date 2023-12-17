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

#include <QSettings>    // for QSettings
#include <QVariant>     // for QVariant

#include "recentfiles.h"


#define CFG_SECTION     "RecentFiles"
#define CFG_KEY_FILES   "Files"
#define CFG_KEY_MAX     "Max"


RecentFiles::RecentFiles(QObject *parent) : QObject(parent)
{
    m_max = DEFAULT_SIZE;
}

QString RecentFiles::file(int id)
{
    return m_files.at(id);
}

QStringList RecentFiles::fileList()
{
    return m_files;
}

void RecentFiles::add(const QString &file)
{
    m_files.prepend(file);
    m_files.removeDuplicates();
    trim();
    emit saveRequest();
    emit fileListChanged(m_files);
}

void RecentFiles::remove(const QString &file)
{
    if (m_files.removeAll(file)) {
        emit saveRequest();
        emit fileListChanged(m_files);
    }
}

void RecentFiles::clear()
{
    if (!m_files.isEmpty()) {
        m_files.clear();
        emit saveRequest();
        emit fileListChanged(m_files);
    }
}

int RecentFiles::max()
{
    return m_max;
}

void RecentFiles::setMax(int max)
{
    if (max != m_max && max > 0 && max <= MAX_SIZE) {
        m_max = max;
        trim();
        emit saveRequest();
        emit fileListChanged(m_files);
    }
}

void RecentFiles::load(QSettings *cfg)
{
    cfg->beginGroup(CFG_SECTION);
    m_max = cfg->value(CFG_KEY_MAX, DEFAULT_SIZE).toInt();

    if (m_max <= 0 || m_max > MAX_SIZE) {
        m_max = DEFAULT_SIZE;
    }

    cfg->beginGroup(CFG_KEY_FILES);

    for (int i = 1; i <= m_max; i++) {
        const QString &key = QString::number(i);

        if (!cfg->contains(key)) {
            break;
        }

        const QString &file = cfg->value(key).toString();

        if (!file.isEmpty()) {
            m_files.append(file);
        }
    }

    cfg->endGroup();
    cfg->endGroup();
    emit fileListChanged(m_files);
}

void RecentFiles::save(QSettings *cfg)
{
    cfg->beginGroup(CFG_SECTION);
    cfg->remove("");

    cfg->beginGroup(CFG_KEY_FILES);
    int count = 0;

    for (const QString &file : m_files) {
        if (!file.isEmpty()) {
            cfg->setValue(QString::number(++count), file);
        }
    }

    cfg->endGroup();
    cfg->setValue(CFG_KEY_MAX, m_max);
    cfg->endGroup();
}

void RecentFiles::trim()
{
    while (m_files.size() > m_max) {
        m_files.removeLast();
    }
}
