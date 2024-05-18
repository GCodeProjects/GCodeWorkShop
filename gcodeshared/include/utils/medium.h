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

#include <QList>        // for QList
#include <QObject>      // for QObject, Q_OBJECT, signals
#include <QString>      // for QString
#include <QStringList>  // for QStringList

class QLocale;
class QSettings;
class QTranslator;


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

	static Medium& instance();
	~Medium();

	/**
	 * @brief
	 * @param dir
	 */
	void addTranslationDir(const QString& dir);

	/**
	 * @brief
	 * @param dir
	 */
	void removeTranslationDir(const QString& dir);

	/**
	 * @brief
	 * @param unit
	 */
	void addTranslationUnit(const QString& unit);

	/**
	 * @brief
	 * @param unit
	 */
	void removeTranslationUnit(const QString& unit);

	QList<QLocale> findTranslation(bool skipQtDir = true);

	void updateTranslation();

	QString settingsDir()
	{
		return mSettingsDir;
	}

	QSettings* settings()
	{
		return mSettings;
	}

signals:
	void onRetranslateUI();

protected:
	explicit Medium(QObject* parent = 0);

	QStringList mTranslationDirs;
	QStringList mTranslationUnits;
	QList<QTranslator*> mTranslators;

	QStringList mShareDirs;
	QString mSettingsDir;

	QSettings* mSettings;

private:
	int checkLaunch();
	void setupDirs();

protected:
	static const QString SLASH;
	static const QString COLON;
	static const QString BIN;
	static const QString SLASH_BIN;
	static const QString LANG;
	static const QString SLASH_LANG;
	static const QString DEBUG;
	static const QString RELEASE;
	static const QString SANDBOX_SHARE;
	static const QString SHARE;
	static const QString SLASH_SHARE;
};

#endif // MEDIUM_H
