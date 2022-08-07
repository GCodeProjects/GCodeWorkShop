/**
 * @file
 * @brief The Medium class provides various information about application environment.
 *
 * @date 02.05.2020 (2020-05-02) created by Nick Egorrov
 * @author Nick Egorrov
 * @copyright Copyright (C) 2020  Nick Egorrov
 * @copyright
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 * @copyright
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.*
 * @copyright
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QDir>
#include <QApplication>

#include <shlobj.h>

#include "utils/medium.h"


int Medium::checkLaunch()
{
    QString dir = QDir::fromNativeSeparators(QApplication::applicationDirPath());
    WCHAR buf[MAX_PATH];

    SHGetFolderPathW(nullptr,  CSIDL_PROGRAM_FILES, nullptr, 0, buf);

    if (dir.startsWith(QDir::fromNativeSeparators(QString::fromWCharArray(buf)))) {
        return LAUNCH_SYSTEM;
    }

    dir = dir.section(SLASH, -2, -1);

    if (dir.startsWith(BIN) && (dir.endsWith(DEBUG) || dir.endsWith(RELEASE))) {
        return LAUNCH_SANDBOX;
    }

    return LAUNCH_PORTABLE;
}

void Medium::setupDirs()
{
    QString shareDir;
    QString langDir;

    shareDir = QDir::fromNativeSeparators(QApplication::applicationDirPath());

    int folderId = CSIDL_LOCAL_APPDATA;

    switch (lauchType) {
    case LAUNCH_SYSTEM:
        folderId = CSIDL_APPDATA;

    case LAUNCH_USER:
        if (shareDir.endsWith(SLASH_BIN)) {
            shareDir.remove(SLASH_BIN);
        }

        langDir = shareDir;
        WCHAR buf[MAX_PATH];
        SHGetFolderPathW(nullptr,  folderId, nullptr, 0, buf);
        mSettingsDir = QDir::fromNativeSeparators(QString::fromWCharArray(buf));
        mSettingsDir.append(SLASH).append(APP_NAME);
        break;

    case LAUNCH_SANDBOX:
        shareDir = shareDir.section(SLASH, 0, -3);
        langDir = shareDir;
        mSettingsDir = shareDir;
        mSettingsDir.append(SLASH_BIN);
        shareDir.append(SANDBOX_SHARE);
        break;

    case LAUNCH_PORTABLE:
    default:
        if (shareDir.endsWith(SLASH_BIN)) {
            shareDir.remove(SLASH_BIN);
        }

        langDir = shareDir;
        mSettingsDir = shareDir;
    }

    langDir.append(SLASH_LANG);
    addTranslationDir(COLON + SLASH_LANG);
    addTranslationDir(langDir);

    mShareDirs.append(shareDir);
    mShareDirs.append(COLON);
}
