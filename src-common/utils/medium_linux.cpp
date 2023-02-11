/**
 * @file
 * @brief The Medium class provides various information about application environment.
 *
 * @date 19.04.2020 (2020-04-19) created by Nick Egorrov
 * @author Nick Egorrov
 * @copyright Copyright (C) 2019  Nick Egorrov
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
#include <QSettings>

#include "utils/medium.h"


int Medium::checkLaunch()
{
    QString dir = QApplication::applicationDirPath();

    if (dir.startsWith(QLatin1String("/usr/"))
            && dir.endsWith(QLatin1String("/bin"))) {
        return LAUNCH_SYSTEM;
    }

    if (dir.endsWith(QLatin1String(".local/bin"))) {
        return LAUNCH_USER;
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

    shareDir = QApplication::applicationDirPath();

    switch (lauchType) {
    case LAUNCH_SYSTEM:
    case LAUNCH_USER:
        shareDir.remove(SLASH_BIN);

        shareDir.append(SLASH_SHARE).append(SLASH).append(APP_NAME);
        langDir = shareDir;
        mSettingsDir = QDir::homePath();
        mSettingsDir.append(QLatin1String("/.config")).append(SLASH).append(APP_NAME);
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
