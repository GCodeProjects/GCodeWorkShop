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

#include <memory>   // for __shared_ptr_access, shared_ptr

#include <QSettings>    // for QSettings
#include <QVariant>     // for QVariant

#include "documentinfo.h"   // for DocumentInfo, DOCUMENTINFO_CFG_KEY_TYPE
#include "gcoderinfo.h"     // for GCoderInfo
#include "sessionmanager.h"


#define CFG_KEY_FILES_LIMIT         "FilesLimit"
#define CFG_KEY_SESSION_NAME        "Name"
#define CFG_KEY_SESSIONS_LIMIT      "SessionsLimit"
#define CFG_SECTION                 "SessionManager"


SessionManager::SessionManager(QObject *parent) : QObject(parent)
{
    m_sessionsLimit = SESSIONS_DEFAULT_LIMIT;
    m_filesLimit = FILES_DEFAULT_LIMIT;
    SessionItem item;
    item.name = tr("default");
    m_sessions.append(item);
}

bool SessionManager::addSession(const QString &name)
{
    const QString &sessionName = name.simplified();

    if (sessionName.isEmpty()) {
        return false;
    }

    if (index(sessionName) >= 0) {
        // A session with the same name already exists.
        return false;
    }

    if (m_sessions.size() >= m_sessionsLimit) {
        return false;
    }

    SessionItem item;
    item.name = sessionName;
    m_sessions.append(item);
    emit sessionListChanged(sessionList());
    emit saveRequest();
    return true;
}

bool SessionManager::removeSession(const QString &name)
{
    int ind = index(name);

    if (ind <= 0) {
        // A session with this name does not exist or it is the current session.
        return false;
    }

    m_sessions.removeAt(ind);
    emit sessionListChanged(sessionList());
    emit saveRequest();
    return true;
}

bool SessionManager::renameSession(const QString &oldName, const QString &newName)
{
    if (newName.isEmpty()) {
        return false;
    }

    if (oldName == newName) {
        return true;
    }

    int ind = index(oldName);

    if (ind < 0) {
        // A session with this name does not exist.
        return false;
    }

    m_sessions[ind].name = newName;
    emit sessionListChanged(sessionList());
    emit saveRequest();
    return true;
}

bool SessionManager::copySession(const QString &from, const QString &to)
{
    if (to.isEmpty()) {
        return false;
    }

    if (from == to) {
        return true;
    }

    int indFrom = index(from);

    if (indFrom < 0) {
        // A session with this name does not exist.
        return false;
    }

    SessionItem item = m_sessions.at(indFrom);
    item.name = to;
    m_sessions.append(item);
    emit sessionListChanged(sessionList());
    emit saveRequest();
    return true;
}

void SessionManager::clearSessions()
{
    if (trim(1)) {
        emit sessionListChanged(sessionList());
        emit saveRequest();
    }
}

QString SessionManager::currentSession() const
{
    return m_sessions.at(0).name;
}

bool SessionManager::setCurrentSession(const QString &name)
{
    int ind = index(name);

    if (ind < 0) {
        // A session with this name does not exist.
        return false;
    }

    if (ind != 0) {
        emit beforeCurrentSessionChanged();
        m_sessions.move(ind, 0);
        emit currentSessionChanged();
        emit sessionListChanged(sessionList());
        emit saveRequest();
    }

    return true;
}

QStringList SessionManager::sessionList() const
{
    QStringList list;

    for (const SessionItem &item : m_sessions) {
        list.append(item.name);
    }

    return list;
}

void SessionManager::setSessionList(const QStringList &list)
{
    QList<SessionItem> newList;

    for (const QString &name : list) {
        SessionItem item;
        int ind = index(name);

        if (ind >= 0) {
            item = m_sessions.at(ind);
        } else {
            item.name = name;
            item.files.clear();
        }

        newList.append(item);
    }

    m_sessions = newList;

    emit currentSessionChanged();
    emit sessionListChanged(sessionList());
    emit saveRequest();
}

int SessionManager::sessionsLimit() const
{
    return m_sessionsLimit;
}

void SessionManager::setSessionsLimit(int lim)
{
    if (lim < 1 || lim > SESSIONS_MAX) {
        // Out of range.
        return;
    }

    if (lim == m_sessionsLimit) {
        // Nothing to do.
        return;
    }

    m_sessionsLimit = lim;

    if (trim(m_sessionsLimit)) {
        emit sessionListChanged(sessionList());
    }

    emit saveRequest();
}

bool SessionManager::addFile(const DocumentInfo::Ptr &fileInfo)
{
    SessionItem &item = m_sessions[0];

    if (item.index(fileInfo->filePath) >= 0) {
        // A file with the same name already exists.
        return true;
    }

    if (item.files.size() >= m_filesLimit) {
        return false;
    }

    item.files.append(fileInfo);
    emit sessionListChanged(sessionList());
    emit saveRequest();
    return true;
}

void SessionManager::removeFile(const QString &filePath)
{
    SessionItem &item = m_sessions[0];
    int index = item.index(filePath);

    if (index < 0) {
        // A file with this name does not exist.
        return;
    }

    item.files.removeAt(index);
    emit sessionListChanged(sessionList());
    emit saveRequest();
}

void SessionManager::clearFiles()
{
    SessionItem &item = m_sessions[0];

    if (item.files.size()) {
        item.files.clear();
        emit sessionListChanged(sessionList());
        emit saveRequest();
    }
}

bool SessionManager::documentInfo(DocumentInfo::Ptr &info) const
{
    const SessionItem &item = m_sessions.at(0);
    int index = item.index(info->filePath);

    if (index < 0) {
        // A file with this name does not exist.
        return false;
    }

    info = item.files.at(index);
    return true;
}

bool SessionManager::setDocumentInfo(const DocumentInfo::Ptr &info)
{
    SessionItem &item = m_sessions[0];
    int index = item.index(info->filePath);

    if (index < 0) {
        // A file with this name does not exist.
        return false;
    }

    item.files[index] = info;
    emit sessionListChanged(sessionList());
    emit saveRequest();
    return true;
}

QList<DocumentInfo::Ptr> SessionManager::documentInfoList() const
{
    return m_sessions.at(0).files;
}

void SessionManager::setDocumentInfoList(const QList<DocumentInfo::Ptr> &infoList)
{
    SessionItem &item = m_sessions[0];
    item.files = infoList;
    item.trim(m_filesLimit);
    emit sessionListChanged(sessionList());
    emit saveRequest();
}

int SessionManager::filesLimit() const
{
    return m_filesLimit;
}

void SessionManager::setFilesLimit(int lim)
{
    if (lim < 0 || lim > FILES_MAX) {
        // Out of range.
        return;
    }

    if (lim == m_filesLimit) {
        // Nothing to do.
        return;
    }

    m_filesLimit = lim;
    bool changed = false;

    for (SessionItem &item : m_sessions) {
        if (item.trim(m_filesLimit)) {
            changed = true;
        }
    }

    if (changed) {
        emit sessionListChanged(sessionList());
    }

    emit saveRequest();
}

void SessionManager::load(QSettings *cfg)
{
    cfg->beginGroup(CFG_SECTION);
    m_sessionsLimit = cfg->value(CFG_KEY_SESSIONS_LIMIT, m_sessionsLimit).toInt();

    if (m_sessionsLimit <= 0 || m_sessionsLimit > SESSIONS_MAX) {
        m_sessionsLimit = SESSIONS_MAX;
    }

    m_filesLimit = cfg->value(CFG_KEY_FILES_LIMIT, m_filesLimit).toInt();

    if (m_filesLimit <= 0 || m_filesLimit > FILES_MAX) {
        m_filesLimit = FILES_MAX;
    }

    QList<SessionItem> newSessions;

    for (int i = 1; i <= SESSIONS_MAX; i++) {
        cfg->beginGroup(QString::number(i));

        if (!cfg->contains(CFG_KEY_SESSION_NAME)) {
            cfg->endGroup();
            break;
        }

        SessionItem item;
        item.name = cfg->value(CFG_KEY_SESSION_NAME).toString();
        item.files.clear();

        for (int j = 1; j <= FILES_MAX; j++) {
            cfg->beginGroup(QString::number(j));

            if (!cfg->contains(DOCUMENTINFO_CFG_KEY_TYPE)) {
                cfg->endGroup();
                break;
            }

            DocumentInfo::Ptr info = DocumentInfo::Ptr(new GCoderInfo());
            info->load(cfg);
            item.files.append(info);
            cfg->endGroup();
        }

        newSessions.append(item);
        cfg->endGroup(); // Number i
    }

    cfg->endGroup();

    if (!newSessions.isEmpty()) {
        m_sessions = newSessions;
    }

    emit sessionListChanged(sessionList());
}

void SessionManager::save(QSettings *cfg) const
{
    cfg->beginGroup(CFG_SECTION);
    cfg->remove("");
    int sessionCount = 0;

    for (const SessionItem &item : m_sessions) {
        cfg->beginGroup(QString::number(++sessionCount));
        cfg->setValue(CFG_KEY_SESSION_NAME, item.name);

        int infoCount = 0;

        for (const DocumentInfo::Ptr &info : item.files) {
            cfg->beginGroup(QString::number(++infoCount));
            info->save(cfg);
            cfg->endGroup();
        }

        cfg->endGroup();
    }

    cfg->setValue(CFG_KEY_SESSIONS_LIMIT, m_sessionsLimit);
    cfg->setValue(CFG_KEY_FILES_LIMIT, m_filesLimit);
    cfg->endGroup();
}

int SessionManager::index(const QString &name) const
{
    for (int i = 0; i < m_sessions.size(); i++) {
        if (name == m_sessions.at(i).name) {
            return i;
        }
    }

    return -1;
}

bool SessionManager::trim(int lim)
{
    bool changed = false;

    while (m_sessions.size() > lim) {
        m_sessions.removeLast();
        changed = true;
    }

    return changed;
}

int SessionManager::SessionItem::index(const QString &filePath) const
{
    for (int i = 0; i < files.size(); i++) {
        if (filePath == files.at(i)->filePath) {
            return i;
        }
    }

    return -1;
}

bool SessionManager::SessionItem::trim(int lim)
{
    bool changed = false;

    while (files.size() > lim) {
        files.removeLast();
        changed = true;
    }

    return changed;
}

int SessionManager::SessionItem::add(const DocumentInfo::Ptr &fileInfo, int lim)
{
    int ind = index(fileInfo->filePath);

    if (ind >= 0) {
        // A file with the same name already exists.
        return ind;
    }

    if (files.size() >= lim) {
        return -1;
    }

    files.append(fileInfo);
    return true;
}
