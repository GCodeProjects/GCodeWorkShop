/**
 * @file
 * @brief The ConfigPage class is used to manage settings.
 *
 * @date 04.05.2020 (2020-05-04) created by Nick Egorrov
 * @author Nick Egorrov
 * @copyright Copyright (C) 2020  Nick Egorrov
 * @copyright
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 * @copyright
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.*
 * @copyright
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QFrame>
#include <QString>
#include <QWidget>

#include "utils/configpage.h"


const QString& ConfigPage::toolTip()
{
	static QString tooltip = ("");
	return tooltip;
}

void ConfigPage::add(QSharedPointer<ConfigPage> child)
{
	if (mChild.indexOf(child) >= 0) {
		return;
	}

	mChild.append(child);
}

void ConfigPage::add(ConfigPage* child)
{
	add(QSharedPointer<ConfigPage>(child));
}

QList<QSharedPointer<ConfigPage> > ConfigPage::pages()
{
	return mChild;
}

QWidget* ConfigPage::widget()
{
	if (mWidget == nullptr) {
		mWidget = doWidget();
		connect(mWidget, SIGNAL(destroyed(QObject*)), SLOT(widgetDestroed(QObject*)));
	}

	return mWidget;
}

QWidget* ConfigPage::doWidget()
{
	return new QFrame();
}

void ConfigPage::widgetDestroed(QObject* object)
{
	if (object == mWidget) {
		mWidget = 0;
	}
}
