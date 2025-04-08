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

#ifndef RENUMBERDIALOG_H
#define RENUMBERDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for slots, Q_OBJECT
#include <QPointer> // for QPointer
#include <QString>  // for QString

class QSettings;
class QWidget;

struct RenumberOptions;

#include "ui_renumberdialog.h"  // for Ui::RenumberDialog


/**
 * @brief The RenumberDialog class
 */
class RenumberDialog : public QDialog, private Ui::RenumberDialog
{
	Q_OBJECT

public:
	RenumberDialog(QWidget* parent, QSettings* settings);
	~RenumberDialog();

	void setOptions(const RenumberOptions& options);
	RenumberOptions options();

	void loadSettings();
	void saveSettings(bool saveOptions = true);

private slots:
	void atEndClicked();
	void divideClicked();
	void renumClicked();
	void allLinesClicked();
	void removeAllClicked();
	void mRenumHeClicked();
	void onFinished(int result);

protected:
	QPointer<QSettings> mSettings;
};

#endif // RENUMBERDIALOG_H
