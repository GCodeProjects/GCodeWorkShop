/*
 * 25.01.2020 created by Nick Egorrov, nicegorov@yandex.ru
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

#include <QApplication>

#include "ui/longjobhelper.h"


LongJobHelper::LongJobHelper(QWidget *parent,
                             const QString &title,
                             int lazyCheck,
                             int deadLine):
    m_ProgressDialog(parent, Qt::Dialog)
{
    m_LazyCheck = lazyCheck;
    m_DeadLine = deadLine;
    m_ProgressDialog.setWindowModality(Qt::WindowModal);
    m_ProgressDialog.setWindowTitle(title);
    m_ProgressDialog.cancel();
}

void LongJobHelper::begin(int maximum, const QString &message)
{
    m_Maximum = maximum;
    m_Message = message;
    m_IsLongJob = false;
    m_StartPoint = clock();
    m_TotalTime = 0;
    m_CheckCount = 0;
    QApplication::setOverrideCursor(Qt::BusyCursor);
}

void LongJobHelper::end()
{
    m_TotalTime = time_ms();

    if (m_IsLongJob) {
        m_IsLongJob = false;
        m_ProgressDialog.hide();
    }

    QApplication::restoreOverrideCursor();
    return;
}

int LongJobHelper::real_check(int progress)
{
    m_TotalTime = time_ms();

    if (!m_IsLongJob) {
        clock_t remaining = m_TotalTime * m_Maximum / (m_Maximum - progress);

        if (remaining > m_DeadLine) {
            m_IsLongJob = true;
            m_ProgressDialog.setLabelText(m_Message);
            m_ProgressDialog.setRange(0, m_Maximum);
            m_ProgressDialog.reset();
            m_ProgressDialog.show();
        }
    }

    if (m_IsLongJob) {
        m_ProgressDialog.setValue(progress);
        QApplication::processEvents();

        if (m_ProgressDialog.wasCanceled()) {
            m_IsLongJob = false;
            m_ProgressDialog.hide();
            QApplication::restoreOverrideCursor();
            return CANCEL;
        }
    }

    return CONTINUE;
}

int LongJobHelper::time()
{
    return m_TotalTime;
}

int LongJobHelper::time_ms()
{
    return (clock() - m_StartPoint) / (CLOCKS_PER_SEC / 1000);
}
