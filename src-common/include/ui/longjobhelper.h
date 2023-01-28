/*
 * 25.01.2020 (2020-01-25) created by Nick Egorrov, nicegorov@yandex.ru
 *
 * Copyright 2020 Nick Egorrov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LONGJOBHELPER_H
#define LONGJOBHELPER_H

#include <time.h>

#include <QString>
#include <QProgressDialog>

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
    LongJobHelper(QWidget *parent = 0,
                  const QString &title = "Slow operation",
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
    void begin(int maximum, const QString &message, int lazyCheck = DEFAULT_LAZY_CHECK);

    /**
     * @brief End of checks.
     * @details The operation time is fixed, a cursor is restored and, if necessary, a progress
     * dialog is hided.
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
