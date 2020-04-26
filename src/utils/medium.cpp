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
#include "generalconfig.h"
#include "edytornc.h"

Medium::Medium(QObject *parent) :
    QObject(parent),
    lauchType(checkLaunch())
{
    addTranslationDir(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    addTranslationUnit("qtbase"); // For Qt base module;
    addTranslationUnit(APP_NAME);

    setupDirs();

    QString settingFile = mSettingsDir;
    settingFile.append("/").append(APP_NAME).append(".ini");
    mSettings = new QSettings(settingFile, QSettings::IniFormat);
    mGeneralConfig = new GeneralConfig(mSettings);
    mGeneralConfig->sync();

    updateTranslation();

    mMainWindow = 0;
}

Medium::~Medium()
{
    mSettings->sync();
    delete mSettings;
}

Medium &Medium::instance()
{
    static Medium enc;
    return enc;
}

void Medium::addTranslationDir(const QString &dir)
{
    if (!mTranslationDirs.contains(dir)) {
        mTranslationDirs.append(dir);
    }
}

void Medium::removeTranslationDir(const QString &dir)
{
    int i = mTranslationDirs.indexOf(dir);

    if (i >= 0) {
        mTranslationDirs.removeAt(i);
    }
}

void Medium::addTranslationUnit(const QString &unit)
{
    if (!mTranslationUnits.contains(unit)) {
        mTranslationUnits.append(unit);
    }
}

void Medium::removeTranslationUnit(const QString &unit)
{
    int i = mTranslationUnits.indexOf(unit);

    if (i >= 0) {
        mTranslationUnits.removeAt(i);
    }
}

QList<QLocale> Medium::findTranslation(bool skipQtDir)
{
    QList<QLocale> localeList;

    foreach (QString dir, mTranslationDirs) {
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
    return mGeneralConfig->localeUI();
}

void Medium::setUiLocale(const QLocale &locale)
{
    mGeneralConfig->localeUI = locale;
}

void Medium::updateTranslation()
{
    QLocale locale = uiLocale();
    QTranslator *qtTranslator = 0;
    QCoreApplication *app = QCoreApplication::instance();

    foreach (QTranslator *trans, mTranslators) {
        app->removeTranslator(trans);
        trans->deleteLater();
    }

    mTranslators.clear();

    foreach (QString dir, mTranslationDirs) {
        foreach (QString file, mTranslationUnits) {
            if (qtTranslator == 0) {
                qtTranslator = new QTranslator();
            }

            qDebug() << "loading translation " << file << " from " << dir;
            file.append("_").append(locale.name());

            if (qtTranslator->load(file, dir)) {
                mTranslators.append(qtTranslator);
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

    foreach (QTranslator *trans, mTranslators) {
        app->installTranslator(trans);
    }

    emit translationChanged();
}

MainWindow *Medium::mainWindow()
{
    if (mMainWindow == 0) {
        mMainWindow = new MainWindow(this);
        connect(mMainWindow, SIGNAL(destroyed(QObject *)), this,
                SLOT(onMainWindowDestroed(QObject *)));
    }

    return mMainWindow;
}

void Medium::onMainWindowDestroed(QObject *)
{
    if (mMainWindow != 0) {
        disconnect(mMainWindow, SIGNAL(destroyed(QObject *)), this,
                   SLOT(onMainWindowDestroed(QObject *)));
        mMainWindow = 0;
        qDebug() << "Main window destroyed";
    }
}
