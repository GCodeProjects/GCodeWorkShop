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
                             int deadLine):
	mProgressDialog(parent, Qt::Dialog)
{
	mDeadLine = deadLine;
	mProgressDialog.setWindowModality(Qt::WindowModal);
	mProgressDialog.setWindowTitle(title);
	mProgressDialog.cancel();
}

void LongJobHelper::begin(int maximum, const QString &message, int lazyCheck)
{
	mMaximum = maximum;
	mMessage = message;
	mIsLongJob = false;
	mStartPoint = clock();
	mTotalTime = 0;
	mCheckCount = 0;
	mLazyCheck = lazyCheck;
	QApplication::setOverrideCursor(Qt::BusyCursor);
}

void LongJobHelper::end()
{
	mTotalTime = time_ms();

	if (mIsLongJob) {
		mIsLongJob = false;
		mProgressDialog.hide();
	}

	QApplication::restoreOverrideCursor();
	return;
}

int LongJobHelper::real_check(int progress)
{
	mTotalTime = time_ms();

	if (!mIsLongJob) {
		int remaining = mTotalTime * mMaximum / (mMaximum - progress);

		if (mTotalTime > mDeadLine || remaining > mDeadLine) {
			mIsLongJob = true;
			mProgressDialog.setLabelText(mMessage);
			mProgressDialog.setRange(0, mMaximum);
			mProgressDialog.reset();
			mProgressDialog.show();
		}
	}

	if (mIsLongJob) {
		mProgressDialog.setValue(progress);
		QApplication::processEvents();

		if (mProgressDialog.wasCanceled()) {
			mIsLongJob = false;
			mProgressDialog.hide();
			QApplication::restoreOverrideCursor();
			return CANCEL;
		}
	}

	return CONTINUE;
}

int LongJobHelper::time()
{
	return mTotalTime;
}

int LongJobHelper::time_ms()
{
	return (clock() - mStartPoint) / (CLOCKS_PER_SEC / 1000);
}
