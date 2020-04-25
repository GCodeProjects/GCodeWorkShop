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

#ifndef MEDIUM_H
#define MEDIUM_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QLocale>

#include "utils/config.h"
#include "mainwindow.h"

/**
 * @brief The GeneralConfig class store general application settings.
 */
class GeneralConfig : public Config
{
public:
    /** UI locale, used to translate text in widgets. */
    CfgQLocale localeUI {this, "uilocale", QLocale::system()};

protected:
    GeneralConfig(QSettings *setting) :
        Config(setting)
    {
    }

    friend class Medium;
};

/**
 * @brief The Medium class provides various information about application environment.
 */
class Medium : public QObject
{
    Q_OBJECT

public:
    const QString APP_NAME = "edytornc";

    enum {
        LAUNCH_SANDBOX, /**< The application has been run in project directory. */
        LAUNCH_SYSTEM,  /**< The application has been installed for all users. */
        LAUNCH_USER,    /**< The application has been installed for current user. */
        LAUNCH_PORTABLE /**< The application has been launched nor in the system directory
                             of the installation, not in the project directory. */
    };

    const int lauchType;

    static Medium &instance();
    ~Medium();

    /**
     * @brief
     * @param dir
     */
    void addTranslationDir(const QString &dir);

    /**
     * @brief
     * @param dir
     */
    void removeTranslationDir(const QString &dir);

    /**
     * @brief
     * @param unit
     */
    void addTranslationUnit(const QString &unit);

    /**
     * @brief
     * @param unit
     */
    void removeTranslationUnit(const QString &unit);

    QList<QLocale> findTranslation(bool skipQtDir = true);
    QLocale uiLocale();
    void setUiLocale(const QLocale &locale);

    void updateTranslation();

    QString settingsDir()
    {
        return m_settingsDir;
    }

    QSettings *settings()
    {
        return m_settings;
    }

    GeneralConfig *generalConfig()
    {
        return m_generalConfig;
    }

    MainWindow *mainWindow();

signals:
    void translationChanged();

public slots:

protected slots:
    void onMainWindowDestroed(QObject *);

protected:
    explicit Medium(QObject *parent = 0);

    QStringList m_translationDirs;
    QStringList m_translationUnits;
    QList<QTranslator *> m_translators;

    QStringList m_shareDirs;
    QString m_settingsDir;

    QSettings *m_settings;
    GeneralConfig *m_generalConfig;

    MainWindow *m_mainWindow;

private:
    int checkLaunch();
    void setupDirs();
};

#endif // MEDIUM_H
