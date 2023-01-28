/**
 * @file
 * @brief The GeneralConfig class store general application settings.
 *
 * @date 26.04.2020 (2020-04-26) created by Nick Egorrov
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

#ifndef GENERALCONFIG_H
#define GENERALCONFIG_H

#include "utils/config.h"

/**
 * @brief The GeneralConfig class store general application settings.
 */
class GeneralConfig : public Config
{
public:
    /** UI locale, used to translate text in widgets. */
    CfgQLocale localeUI {this, "uilocale", QLocale::system()};
    CfgQString lastDir {this, "LastDir",  QDir::homePath(), false};
    Cfgbool disableFileChangeMonitor {this, "DisableFileChangeMonitor", false};

protected:
    GeneralConfig(QSettings *setting) : Config(setting){}

    friend class Medium;
};

#endif // GENERALCONFIG_H
