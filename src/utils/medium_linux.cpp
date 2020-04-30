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

#include <QDebug>
#include <QDir>
#include <QApplication>

#include "utils/medium.h"

int Medium::checkLaunch()
{
    QString dir = QApplication::applicationDirPath();

    if (dir.endsWith(QLatin1String("usr/local/bin"))
            || dir.endsWith(QLatin1String("usr/bin"))) {
        return LAUNCH_SYSTEM;
    } else if (dir.endsWith(QLatin1String(".local/bin"))) {
        return LAUNCH_USER;
    } else if (dir.endsWith(QLatin1String("bin/debug"))
               || dir.endsWith(QLatin1String("bin/release"))) {
        return LAUNCH_SANDBOX;
    }

    return LAUNCH_PORTABLE;
}

void Medium::setupDirs()
{
    QString shareDir;
    QString langDir;

    shareDir = QApplication::applicationDirPath();
    mSettingsDir = QDir::homePath();
    mSettingsDir.append(QLatin1String("/.config/"));
    mSettingsDir.append(APP_NAME);

    switch (lauchType) {
    case LAUNCH_SYSTEM:
    case LAUNCH_USER:
        shareDir.remove(QLatin1String("/bin"));
        shareDir.append(QLatin1String("/share/"));
        shareDir.append(APP_NAME);
        langDir = shareDir;
        break;

    case LAUNCH_SANDBOX:
        shareDir.remove(QLatin1String("/bin/release"));
        shareDir.remove(QLatin1String("/bin/debug"));
        langDir = shareDir;
        mSettingsDir = shareDir;
        mSettingsDir.append(QLatin1String("/bin"));
        shareDir.append(QLatin1String("/install/share"));
        break;

    case LAUNCH_PORTABLE:
    default:
        shareDir.remove("/bin");
        langDir = shareDir;
        mSettingsDir = shareDir;
        shareDir.append(QLatin1String("/share"));
    }

    langDir.append(QLatin1String("/lang"));
    addTranslationDir(QLatin1String(":/lang"));
    addTranslationDir(langDir);

    mShareDirs.append(shareDir);
    mShareDirs.append(QLatin1String(":"));

    qInfo() << "Set of directoryes :";
    qInfo() << "  translation   " << mTranslationDirs;
    qInfo() << "  config        " << mSettingsDir;
    qInfo() << "  programm data " << mShareDirs;
    qInfo() << "";
}
