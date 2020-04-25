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

#include <QLibraryInfo>
#include <QDebug>
#include <QDir>

#include "utils/medium.h"
#include "edytornc.h"

Medium::Medium(QObject *parent) :
    QObject(parent),
    lauchType(checkLaunch())
{
    addTranslationDir(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    addTranslationUnit("qtbase"); // For Qt base module;
    addTranslationUnit(APP_NAME);

    setupDirs();

    QString settingFile = m_settingsDir;
    settingFile.append("/").append(APP_NAME).append(".ini");
    m_settings = new QSettings(settingFile, QSettings::IniFormat);
    m_generalConfig = new GeneralConfig(m_settings);
    m_generalConfig->sync();

    updateTranslation();

    m_mainWindow = 0;
}

Medium::~Medium()
{
    m_settings->sync();
    delete m_settings;
}

Medium &Medium::instance()
{
    static Medium enc;
    return enc;
}

void Medium::addTranslationDir(const QString &dir)
{
    if (!m_translationDirs.contains(dir)) {
        m_translationDirs.append(dir);
    }
}

void Medium::removeTranslationDir(const QString &dir)
{
    int i = m_translationDirs.indexOf(dir);

    if (i >= 0) {
        m_translationDirs.removeAt(i);
    }
}

void Medium::addTranslationUnit(const QString &unit)
{
    if (!m_translationUnits.contains(unit)) {
        m_translationUnits.append(unit);
    }
}

void Medium::removeTranslationUnit(const QString &unit)
{
    int i = m_translationUnits.indexOf(unit);

    if (i >= 0) {
        m_translationUnits.removeAt(i);
    }
}

QList<QLocale> Medium::findTranslation(bool skipQtDir)
{
    QList<QLocale> localeList;

    foreach (QString dir, m_translationDirs) {
        QStringList fileList = QDir(dir).entryList(QDir::Files);

        // Skip Qt translation directory.
        if (dir == QLibraryInfo::location(QLibraryInfo::TranslationsPath) && skipQtDir) {
            continue;
        }

        foreach (QString file, fileList) {
            QRegExp rxName("([^a-zA-Z0-9])([a-zA-Z]{2,2}(_[a-zA-Z]{2,2})?)([^a-zA-Z0-9]|$)");
            int pos = rxName.indexIn(file);

            // The locales name is the text of the second capturing parentheses.
            if (pos > -1 && rxName.captureCount() > 1) {
                QLocale locale = QLocale(rxName.cap(2));

                if (!localeList.contains(locale) && locale != QLocale::c()) {
                    localeList.append(locale);
                }
            }
        }
    }

    return localeList;
}

QLocale Medium::uiLocale()
{
    return m_generalConfig->localeUI();
}

void Medium::setUiLocale(const QLocale &locale)
{
    m_generalConfig->localeUI = locale;
}

void Medium::updateTranslation()
{
    QLocale locale = uiLocale();
    QTranslator *qtTranslator = 0;
    QCoreApplication *app = QCoreApplication::instance();

    foreach (QTranslator *trans, m_translators) {
        app->removeTranslator(trans);
        trans->deleteLater();
    }

    m_translators.clear();

    foreach (QString dir, m_translationDirs) {
        foreach (QString file, m_translationUnits) {
            if (qtTranslator == 0) {
                qtTranslator = new QTranslator();
            }

            qDebug() << "loading translation " << file << " from " << dir;
            file.append("_").append(locale.name());

            if (qtTranslator->load(file, dir)) {
                m_translators.append(qtTranslator);
                qtTranslator = 0;
                qDebug() << "  OK";
            } else {
                qDebug() << "  fail";
            }
        }
    }

    if (qtTranslator != 0) {
        delete qtTranslator;
    }

    foreach (QTranslator *trans, m_translators) {
        app->installTranslator(trans);
    }

    emit translationChanged();
}

MainWindow *Medium::mainWindow()
{
    if (m_mainWindow == 0) {
        m_mainWindow = new MainWindow(this);
        connect(m_mainWindow, SIGNAL(destroyed(QObject *)), this,
                SLOT(onMainWindowDestroed(QObject *)));
    }

    return m_mainWindow;
}

void Medium::onMainWindowDestroed(QObject *)
{
    if (m_mainWindow != 0) {
        disconnect(m_mainWindow, SIGNAL(destroyed(QObject *)), this,
                   SLOT(onMainWindowDestroed(QObject *)));
        m_mainWindow = 0;
        qDebug() << "Main window destroyed";
    }
}
