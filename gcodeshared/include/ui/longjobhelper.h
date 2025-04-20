/*
 *  Copyright (C) 2020-2025 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of GCodeWorkShop.
 *
 *  GCodeWorkShop is free software: you can redistribute it and/or modify
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

#ifndef LONGJOBHELPER_H
#define LONGJOBHELPER_H

#include <time.h>   // for clock_t

#include <QProgressDialog>  // for QProgressDialog
#include <QString>          // for QString

class QWidget;


/**
 * @brief The LongJobHelper class help to avoid block GUI in slow operation.
 * @details
 * @code
 * class Foo
 * {
 *     LongJobHelper m_Helper();
 *
 * public:
 *     void myFunc(QString text){
 *         int ln = text.length();
 *         m_Helper.begin(ln, "Some slow operation");
 *
 *         for (int i=0; i < ln; i++) {
 *             if (m_Helper.check(pos) == LongJobHelper::CANCEL) {
 *                 return;
 *             }
 *
 *             QChar c = text.at(i);
 *             // A lot of work with the chars.
 *         }
 *
 *         m_Helper.end();
 *     }
 * };
 * @endcode
 */
class LongJobHelper
{
public:
	constexpr static int CONTINUE = 0;
	constexpr static int CANCEL = 1;
	constexpr static int DEFAULT_LAZY_CHECK = 1000;
	constexpr static int DEFAULT_DEADLINE = 300;

	/**
	 * @brief Construct LongJobHelper
	 * @param parent A parent widget for QProgressDialog.
	 * @param title A title for QProgressDialog.
	 * @param deadLine Allowable time to execute operation without show
	 *        QProgressDialog, in msec.
	 */
	LongJobHelper(QWidget* parent = 0,
	              const QString& title = "Slow operation",
	              int deadLine = DEFAULT_DEADLINE);
	~LongJobHelper() {;}

	/**
	 * @brief Preparing for proper checks.
	 * @details Also changes a application cursor to BusyCursor.
	 * @param maximum Maximum number of iteration. It used to calculate remaining time of
	 *        execute operation and to show progress bar.
	 * @param message
	 * @param lazyCheck The function check() does the real check once from lazyCheck.
	 */
	void begin(int maximum, const QString& message, int lazyCheck = DEFAULT_LAZY_CHECK);

	/**
	 * @brief End of checks.
	 * @details The operation time is fixed, a cursor is restored and, if necessary, a progress
	 * dialog is hiding.
	 * @see time()
	 */
	void end();

	/**
	 * @brief Check of the time to complete the operation.
	 * @details The function makes a lazy check.  This means that the real check occurs once
	 * out of several (see the lazyCheck parameter in the constructor LongJobHelper()).
	 * @param progress Current iteration number.
	 * @return CANCEL if operation must be interrupted.
	 */
	inline int check(int progress)
	{
		if (mCheckCount++ < mLazyCheck) {
			return CONTINUE;
		}

		mCheckCount = 0;

		return real_check(progress);
	}

	/**
	 * @brief Time of operation.
	 * @details Calculate time between call begin() and last call check() or end().
	 * @return Time of operation, in msec.
	 */
	int time();

protected:
	/**
	 * @brief A real check of the time to complete the operation.
	 * @details If the calculated remaining time is longer than allowed (see the deadLine
	 * parameter in the constructor LongJobHelper()), the progress bar is displayed.
	 * @param progress Current iteration number.
	 * @return CANCEL if operation must be interrupted.
	 */
	int real_check(int progress);

	int time_ms();

	int mMaximum;
	clock_t mStartPoint;
	int mTotalTime;
	int mLazyCheck;
	int mDeadLine;
	int mCheckCount;
	bool mIsLongJob;
	QString mMessage;
	QProgressDialog mProgressDialog;
};

#endif // LONGJOBHELPER_H
