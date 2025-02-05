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

#include <cstdlib> // getenv()

#include <QApplication>             // for QApplication
#include <QCoreApplication>         // for QCoreApplication
#include <QDir>                     // for QDir
#include <QLatin1String>            // for QLatin1String
#include <QLibraryInfo>             // for QLibraryInfo
#include <QList>                    // for QList
#include <QLocale>                  // for QLocale, QTypeInfo<>::isLarge, QTypeInfo<>::isStatic, QTypeInfo<>::isComplex
#include <QRegularExpression>       // for QRegularExpression
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QSettings>                // for QSettings
#include <QStandardPaths>           // for QStandardPaths
#include <QtDebug>                  // for QNoDebug, qDebug
#include <QtGlobal>                 // for QForeachContainer, qMakeForeachContainer, QTypeInfo<>::isLarge, QTypeInfo...
#include <QTranslator>              // for QTranslator

#include <utils/medium.h>   // IWYU pragma: associated


const QString Medium::SLASH = QLatin1String("/");
const QString Medium::COLON = QLatin1String(":");
const QString Medium::BIN = QLatin1String("bin");
const QString Medium::SLASH_BIN = SLASH + BIN;
const QString Medium::LANG = QLatin1String("lang");
const QString Medium::SLASH_LANG = SLASH + LANG;
const QString Medium::DEBUG = QLatin1String("debug");
const QString Medium::RELEASE = QLatin1String("release");
const QString Medium::SANDBOX_SHARE = QLatin1String("/install/share");
const QString Medium::SHARE = QLatin1String("share");
const QString Medium::SLASH_SHARE = SLASH + QLatin1String("share");

Medium::Medium(QObject* parent) :
	QObject(parent),
	lauchType(checkLaunch())
{
	addTranslationDir(QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	addTranslationDir(COLON + SLASH_LANG);
	addTranslationUnit("qtbase"); // For Qt base module;
	addTranslationUnit(APP_NAME);

	setupDirs();
	mShareDirs.append(COLON);

	qDebug() << "Seting directoryes:";
	qDebug() << "  translation   " << mTranslationDirs;
	qDebug() << "  config        " << mSettingsDir;
	qDebug() << "  programm data " << mShareDirs;

	QString settingFile = mSettingsDir;
	settingFile.append(SLASH).append(APP_NAME).append(".ini");
	mSettings = new QSettings(settingFile, QSettings::IniFormat);

	updateTranslation();
}

Medium::~Medium()
{
	mSettings->sync();
	delete mSettings;
}

int Medium::checkLaunch()
{
	QString dir = QDir::fromNativeSeparators(QApplication::applicationDirPath());

#ifdef Q_OS_WIN

	char* sysProgFile;

	sysProgFile = getenv("ProgramFiles");

	if (sysProgFile != nullptr &&
	        dir.startsWith(QDir::fromNativeSeparators(QString::fromLocal8Bit(sysProgFile)))) {
		return LAUNCH_SYSTEM;
	}

#else // Not Q_OS_WIN

	if (dir.startsWith(QLatin1String("/usr/"))
	        && dir.endsWith(QLatin1String("/bin"))) {
		return LAUNCH_SYSTEM;
	}

	if (dir.endsWith(QLatin1String(".local/bin"))) {
		return LAUNCH_USER;
	}

#endif // Q_OS_WIN

	if (dir.contains("build")) {
		return LAUNCH_SANDBOX;
	}

	return LAUNCH_PORTABLE;
}

void Medium::setupDirs()
{
	QString rootDir;
	QString shareDir;
	QString langDir;

	rootDir = QDir::fromNativeSeparators(QApplication::applicationDirPath());

	switch (lauchType) {
	case LAUNCH_SYSTEM:
	case LAUNCH_USER:
		if (rootDir.endsWith(SLASH_BIN)) {
			rootDir.remove(SLASH_BIN);
		}

		shareDir = rootDir;
#ifndef Q_OS_WIN
		shareDir.append(SLASH_SHARE).append(SLASH).append(APP_NAME);
#endif
		langDir = shareDir;
		mSettingsDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
		mSettingsDir.append(SLASH).append(APP_NAME);
		break;

	case LAUNCH_SANDBOX:
		shareDir = rootDir.section(SLASH, 0, -2);
		langDir = shareDir;
		mSettingsDir = shareDir;
		mSettingsDir.append(SLASH_BIN);
		shareDir.append(SANDBOX_SHARE);
		break;

	case LAUNCH_PORTABLE:
	default:
		if (rootDir.endsWith(SLASH_BIN)) {
			rootDir.remove(SLASH_BIN);
		}

		shareDir = rootDir;
		langDir = shareDir;
		mSettingsDir = shareDir;
	}

	langDir.append(SLASH_LANG);
	addTranslationDir(langDir);

	mShareDirs.append(shareDir);
}

Medium& Medium::instance()
{
	static Medium enc;
	return enc;
}

void Medium::addTranslationDir(const QString& dir)
{
	if (!mTranslationDirs.contains(dir)) {
		mTranslationDirs.append(dir);
	}
}

void Medium::removeTranslationDir(const QString& dir)
{
	int i = mTranslationDirs.indexOf(dir);

	if (i >= 0) {
		mTranslationDirs.removeAt(i);
	}
}

void Medium::addTranslationUnit(const QString& unit)
{
	if (!mTranslationUnits.contains(unit)) {
		mTranslationUnits.append(unit);
	}
}

void Medium::removeTranslationUnit(const QString& unit)
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
			QRegularExpression rxName("([^a-zA-Z0-9])([a-zA-Z]{2,2}(_[a-zA-Z]{2,2})?)([^a-zA-Z0-9]|$)");
			auto match = rxName.match(file);

			// The locales name is the text of the second capturing parentheses.
			if (match.lastCapturedIndex() > 1) {
				QLocale locale = QLocale(match.captured(2));

				if (!localeList.contains(locale) && locale != QLocale::c()) {
					localeList.append(locale);
				}
			}
		}
	}

	return localeList;
}

void Medium::updateTranslation()
{
	QCoreApplication* app = QCoreApplication::instance();
	QLocale locale = QLocale();

	foreach (QTranslator* trans, mTranslators) {
		app->removeTranslator(trans);
		trans->deleteLater();
	}

	mTranslators.clear();

	foreach (QString file, mTranslationUnits) {
		file.append("_").append(locale.name());

		foreach (QString dir, mTranslationDirs) {
			QTranslator* qtTranslator = new QTranslator();
			qDebug() << "loading translation " << file << " from " << dir;

			if (qtTranslator->load(file, dir)) {
				mTranslators.append(qtTranslator);
				qtTranslator = 0;
				qDebug() << "  OK";
				break;
			} else {
				qDebug() << "  fail";
				delete qtTranslator;
			}
		}
	}

	foreach (QTranslator* trans, mTranslators) {
		app->installTranslator(trans);
	}

	emit onRetranslateUI();
}
