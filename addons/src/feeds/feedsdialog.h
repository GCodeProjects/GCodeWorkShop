/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
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

#ifndef FEEDSDIALOG_H
#define FEEDSDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for Q_OBJECT, slots
#include <QPointer> // for QPointer
#include <QString>  // for QString

class QEvent;
class QSettings;
class QWidget;

struct FeedsOptions;

#include "ui_feedsdialog.h" // for Ui::FeedsDialog


/**
 * @brief The FeedsDialog class
 */
class FeedsDialog : public QDialog, private Ui::FeedsDialog
{
	Q_OBJECT

public:
	FeedsDialog(QWidget* parent, QSettings* settings);
	~FeedsDialog();

	void setOptions(const FeedsOptions& options);
	FeedsOptions options();

	void loadSettings(const FeedsOptions& defaultOptions);
	void saveSettings(bool saveOptions = true);

protected:
	bool eventFilter(QObject* obj, QEvent* ev);

protected slots:
	void inputChanged();

private slots:
	void computeButtonClicked();
	void computeVcButtonClicked();
	void setDefButton();
	void checkBoxChanged();
	void onFinished(int result);

protected:
	QPointer<QSettings> mSettings;
};

#endif // FEEDSDIALOG_H
