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

#ifndef TOOLTIPEVENTFILTER_H
#define TOOLTIPEVENTFILTER_H

#include <QHash>                // for QHash
#include <QObject>              // for QObject, Q_OBJECT, signals
#include <QRegularExpression>   // for QRegularExpression
#include <QString>              // for QString

class QDragEnterEvent;
class QDropEvent;
class QEvent;
class QHelpEvent;
class QKeyEvent;
class QMouseEvent;
class QPlainTextEdit;
class QPoint;
class QWidget;


class GCoderEventFilter : public QObject
{
	Q_OBJECT

public:
	explicit GCoderEventFilter(QPlainTextEdit* textEdit, QObject* parent = nullptr);

	bool isToolTipEnable() const;
	void setToolTipEnable(bool enable);

	void setToolTipRegExp(const QRegularExpression& regexp);

	QHash<QString, QString> tips() const;
	void setTips(const QHash<QString, QString>& tips);

	QString wordForPosition(const QPoint& pos) const;

signals:
	void requestInLineCalc();
	void requestUnderLine();

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
	bool toolTipEvent(QWidget* widget, QHelpEvent* event);
	bool mouseButtonDblClickEvent(QMouseEvent* event);
	bool keyEvent(QKeyEvent* event);

	/*
	 * The QPlainTextEdit class already has a drag-n-drop mechanism, but in
	 * the case of files, QPlainTextEdit inserts a text representation of url
	 * into the edited document. So we add event handlers to filter such events
	 * and handle those events in the main window.
	 * see https://doc.qt.io/qt-5/dnd.html#dropping
	 */
	bool dragEnterEvent(QDragEnterEvent* event);
	bool dropEvent(QDropEvent* event);

protected:
	QPlainTextEdit* m_textEdit;
	QHash<QString, QString> m_tips;
	bool m_toolTipEnable;
	QRegularExpression m_toolTipRegex;
};

#endif // TOOLTIPEVENTFILTER_H
