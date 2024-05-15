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

// Enable the M_PI constant in MSVC
// see https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/math-constant
#define _USE_MATH_DEFINES

#include <cmath>        // for cos, sin, M_PI

#include <QBrush>       // for QBrush
#include <QFont>        // for QFont
#include <QFontMetrics> // for QFontMetrics
#include <QMouseEvent>  // for QMouseEvent
#include <QPainter>     // for QPainter
#include <QPalette>     // for QPalette
#include <QPen>         // for QPen
#include <QPixmap>      // for QPixmap
#include <QPointF>      // for QPointF
#include <QRect>        // for QRect
#include <QString>      // for QString
#include <Qt>           // for PenStyle, GlobalColor, BrushStyle, MouseButton, WidgetAttribute, WindowFlags
#include <QtGlobal>     // for qMin, qreal

class QFocusEvent;
class QMouseEvent;
class QPaintEvent;

#include "bhcdraw.h"


BHCDraw::BHCDraw(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setWindowTitle(tr("Bolt circle - preview"));
	setAttribute(Qt::WA_DeleteOnClose);

	setToolTip(tr("Click to close"));

	setBackgroundRole(QPalette::Shadow);
}

BHCDraw::~BHCDraw()
{
}

void BHCDraw::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		close();
	}
}

void BHCDraw::init(int w, int h)
{
	scale = 1;

	resize(w, h);
	setMaximumSize(width(), height());
	setMinimumSize(width(), height());

	pm = new QPixmap(width(), height());
	pm->fill(Qt::black);
	drawLines();
}

void BHCDraw::setScale(double sc)
{
	scale = sc;
}

void BHCDraw::paintEvent(QPaintEvent*)
{
	if (pm->isNull()) {
		return;
	}

	QPainter painter(this);
	painter.drawPixmap(0, 0, *pm);
}

void BHCDraw::focusOutEvent(QFocusEvent*)
{
}

void BHCDraw::drawHole(qreal ang, qreal dia, qreal xcenter, qreal ycenter, bool first,
                       bool last, QColor color)
{
	qreal x, y, x1, y1, sca, d;

	QPainter* paint = new QPainter(pm);
	QFont font = paint->font();
	font.setPointSize(10);
	paint->setFont(font);
	QFontMetrics fm = paint->fontMetrics();

	paint->save();
	int c = qMin(geometry().width(), geometry().height());
	c = c + (fm.lineSpacing() * 8);

	paint->setWindow(-(c / 2), -(c / 2), c, c);
	QRect v = paint->viewport();
	c = qMin(v.width(), v.height());

	paint->setViewport(v.left() + (v.width() - c) / 2, v.top() + (v.height() - c) / 2, c, c);

	sca = ((c - 20) / 2) / (scale / 2);

	paint->scale(sca, sca);

	d = 12 / sca;
	x = xcenter + (dia + (d)) * std::cos((M_PI / 180) * ang);
	y = ycenter + (dia + (d)) * std::sin((M_PI / 180) * ang);

	x1 = xcenter + (dia - (d)) * std::cos((M_PI / 180) * ang);
	y1 = ycenter + (dia - (d)) * std::sin((M_PI / 180) * ang);


	paint->setPen(QPen(Qt::gray, 0, Qt::DotLine));
	paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

	if (last) {
		QBrush brush(Qt::green, Qt::SolidPattern);
		brush.setColor(color);
		paint->setBrush(brush);
	} else {
		paint->setBrush(Qt::NoBrush);
	}

	if (first) {
		// circle center point
		paint->setPen(QPen(Qt::gray, 0, Qt::DashDotLine));
		paint->drawLine(QPointF(xcenter + v.x() / 2, -(ycenter - d)), QPointF(xcenter + v.x() / 2,
		                -(ycenter + d)));
		paint->drawLine(QPointF(xcenter - d, -(ycenter + v.y() / 2)), QPointF(xcenter + d,
		                -(ycenter + v.y() / 2)));

		//circle radius line
		x = xcenter + 0 * std::cos((M_PI / 180) * ang);
		y = ycenter + (0) * std::sin((M_PI / 180) * ang);
		x1 = xcenter + (dia - (d)) * std::cos((M_PI / 180) * ang);
		y1 = ycenter + (dia - (d)) * std::sin((M_PI / 180) * ang);
		paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

		//diameter circle
		paint->setPen(QPen(Qt::gray, 0, Qt::DotLine));
		paint->setBrush(Qt::NoBrush);
		paint->drawEllipse(QPointF(xcenter + v.x() / 2, - ycenter - v.y() / 2), dia, dia);

		paint->setPen(QPen(color.darker(65), 0, Qt::SolidLine));
	} else {
		paint->setPen(QPen(color, 0, Qt::SolidLine));
	}

	x = xcenter + dia * std::cos((M_PI / 180) * ang);
	y = ycenter + dia * std::sin((M_PI / 180) * ang);

	d = 8 / sca;

	paint->drawEllipse(QPointF(x, -y), d, d);

	paint->restore();
	paint->end();
}

void BHCDraw::drawLines(qreal dia, qreal ang, QColor cl)
{
	qreal x, y, x1, y1;

	dia = dia / 2;

	QPainter* paint = new QPainter(pm);
	QFont font = paint->font();
	font.setPointSize(10);
	paint->setFont(font);
	QFontMetrics fm = paint->fontMetrics();

	paint->save();

	int d = qMin(geometry().width(), geometry().height());
	d = d + (fm.lineSpacing() * 6);
	paint->setWindow(-(d / 2), -(d / 2), d, d);
	QRect v = paint->viewport();
	d = qMin(v.width(), v.height());
	paint->setViewport(v.left() + (v.width() - d) / 2, v.top() + (v.height() - d) / 2, d, d);

	paint->setPen(QPen(cl, 0, Qt::DotLine));

	x = (dia + 4) * std::cos((M_PI / 180) * ang);
	y = (dia + 4) * std::sin((M_PI / 180) * ang);
	x1 = (dia + 4) * std::cos((M_PI / 180) * (ang + 180));
	y1 = (dia + 4) * std::sin((M_PI / 180) * (ang + 180));
	paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

	x = (dia + 4) * std::cos((M_PI / 180) * (ang + 90));
	y = (dia + 4) * std::sin((M_PI / 180) * (ang + 90));
	x1 = (dia + 4) * std::cos((M_PI / 180) * (ang + 270));
	y1 = (dia + 4) * std::sin((M_PI / 180) * (ang + 270));
	paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

	paint->restore();
	paint->end();
}

void BHCDraw::clear()
{
	pm->fill(Qt::black);
	drawLines();
}

void BHCDraw::printText(int x, int y, int line, const QString& text, QColor color)
{
	QPainter* paint = new QPainter(pm);
	QFont font = paint->font();
	font.setPointSize(10);
	paint->setFont(font);
	QFontMetrics fm = paint->fontMetrics();

	paint->save();
	paint->setPen(QPen(color, 0, Qt::SolidLine));
	paint->drawText(x, y + (fm.lineSpacing() * line), text);
	paint->restore();
	paint->end();
}

void BHCDraw::drawLines()
{
	QPainter* paint = new QPainter(pm);
	QFont font = paint->font();
	font.setPointSize(10);
	paint->setFont(font);
	QFontMetrics fm = paint->fontMetrics();

	paint->save();

	int d = qMin(geometry().width(), geometry().height());
	d = d + (fm.lineSpacing() * 6);
	paint->setWindow(-(d / 2), -(d / 2), d, d);
	QRect v = paint->viewport();
	d = qMin(v.width(), v.height());
	paint->setViewport(v.left() + (v.width() - d) / 2, v.top() + (v.height() - d) / 2, d, d);

	paint->setPen(QPen(Qt::gray, 0, Qt::DashDotLine));

	v = paint->viewport();
	paint->drawLine(QPointF(0, (v.height() / 2) - 5), QPointF(0, 5 - (v.height() / 2)));
	paint->drawLine(QPointF((v.width() / 2) - 5, 0), QPointF(5 - (v.width() / 2), 0));

	paint->restore();
	paint->end();
}
