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

#ifndef BHCDIALOG_H
#define BHCDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for Q_OBJECT, slots
#include <QPointer> // for QPointer
#include <QString>  // for QString

class QSettings;
class QTabWidget;
class QWidget;

class BHCDraw;
struct BHCOptions;

#include "ui_bhcdialog.h"   // for Ui::BHCDialog


/**
 * @brief The BHCDialog class
 */
class BHCDialog : public QDialog, private Ui::BHCDialog
{
	Q_OBJECT

public:
	BHCDialog(QWidget* parent, QSettings* settings);
	~BHCDialog();

	void setOptions(const BHCOptions& options);
	BHCOptions options();

	void loadSettings();
	void saveSettings(bool saveOptions = true);

protected:
	//void windowActivationChange(bool oldActive);

private slots:
	void comChk();
	void computeButtonClicked();
	void clearAll();
	void onFinished(int result);

private:
	QTabWidget* tabBar;
	BHCDraw* drawing;

	int parentHeight;
	int parentWidth;

protected:
	QPointer<QSettings> mSettings;
};

#endif // BHCDIALOG_H
