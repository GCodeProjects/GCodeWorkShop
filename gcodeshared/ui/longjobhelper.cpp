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

#include <QApplication> // for QApplication
#include <Qt>           // for CursorShape, WindowModality, WindowType

class QWidget;

#include <ui/longjobhelper.h>   // IWYU pragma: associated


LongJobHelper::LongJobHelper(QWidget* parent,
                             const QString& title,
                             int deadLine):
	mProgressDialog(parent, Qt::Dialog)
{
	mDeadLine = deadLine;
	mProgressDialog.setWindowModality(Qt::WindowModal);
	mProgressDialog.setWindowTitle(title);
	mProgressDialog.cancel();
}

void LongJobHelper::begin(int maximum, const QString& message, int lazyCheck)
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

	if (!mIsLongJob && progress > 0) {
		int remaining = mTotalTime * (mMaximum - progress) / progress;

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
