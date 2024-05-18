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

#ifndef TRIANGLEDIALOG_H
#define TRIANGLEDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for slots, Q_OBJECT
#include <QPointer> // for QPointer
#include <QString>  // for QString
#include <QtGlobal> // for uint

class QDoubleValidator;
class QEvent;
class QPixmap;
class QSettings;
class QWidget;

struct TriangleOptions;

#include "ui_triangledialog.h"  // for Ui::TriangleDialog


/**
 * @brief The TriangleDialog class
 */
class TriangleDialog : public QDialog, private Ui::TriangleDialog
{
	Q_OBJECT

public:
	TriangleDialog(QWidget* parent, QSettings* settings);
	~TriangleDialog();

	void setOptions(const TriangleOptions& options);
	TriangleOptions options();

	void loadSettings(const TriangleOptions& defaultOptions);
	void saveSettings(bool saveOptions = true);

protected:
	bool eventFilter(QObject* obj, QEvent* ev);

protected slots:
	void inputChanged();
	void checkBoxToggled();
	void rightTriangleCheckBoxToggled();

private slots:
	void computeButtonClicked();
	int option1();
	int option2();
	int option3();
	void option4();
	void onFinished(int result);

private:
	uint mode;
	QPixmap* pic1;
	QPixmap* pic2;
	QDoubleValidator* aAInputValid;
	QDoubleValidator* aBInputValid;
	QDoubleValidator* aCInputValid;

protected:
	QPointer<QSettings> mSettings;
};

#endif // TRIANGLEDIALOG_H
