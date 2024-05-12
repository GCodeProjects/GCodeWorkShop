/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of EdytorNC.
 *
 *  EdytorNC is free software: you can redistribute it and/or modify
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

#ifndef BHCDDRAW_H
#define BHCDDRAW_H

#include <QColor>   // for QColor
#include <QObject>  // for slots, Q_OBJECT
#include <QString>  // for QString
#include <QWidget>  // for QWidget
#include <Qt>       // for operator|, Dialog, MSWindowsFixedSizeDialogHint
#include <QtGlobal> // for qreal, QFlags

class QFocusEvent;
class QMouseEvent;
class QPaintEvent;
class QPixmap;


/**
 * @brief The BHCDraw class
 */
class BHCDraw : public QWidget
{
	Q_OBJECT

public:
	BHCDraw(QWidget* parent = 0,
	        Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::Tool);
	~BHCDraw();

public slots:
	void clear();
	void printText(int x, int y, int line, const QString& text, QColor color);
	void drawHole(qreal ang, qreal dia, qreal xcenter, qreal ycenter, bool first = false,
	              bool last = false, QColor color = Qt::green);
	void init(int w, int h);
	void drawLines(qreal dia, qreal ang, QColor cl);
	void setScale(double sc);

private slots:
	void drawLines();

protected:
	void paintEvent(QPaintEvent*);
	void focusOutEvent(QFocusEvent*);
	void mousePressEvent(QMouseEvent* event);

private:
	double scale;
	QPixmap* pm;
};

#endif // BHCDDRAW_H
