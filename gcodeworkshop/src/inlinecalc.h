/*
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
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

#ifndef INLINECALC_H
#define INLINECALC_H

#include <QLineEdit>    // for QLineEdit
#include <QObject>      // for Q_OBJECT, signals, slots
#include <QString>      // for QString
#include <QStringList>  // for QStringList

class QKeyEvent;
class QRect;
class QWidget;


class InLineCalc : public QLineEdit
{
	Q_OBJECT
public:
	explicit InLineCalc(QWidget* parent);

	void showCalc(const QString& address, const QString& selectedText, const QRect& cursorRect);

protected:
	virtual void keyPressEvent(QKeyEvent* event) override ;
	void evalute();

protected slots :
	void done();

signals:
	void complete(const QString& text);

protected:
	QStringList m_wordList;
	QString m_address;
};

#endif // INLINECALC_H
