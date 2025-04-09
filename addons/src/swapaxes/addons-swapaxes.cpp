/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023-2025 Nick Egorrov, nicegorov@yandex.ru
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

#include <functional>   // for function

#include <QDialog>  // for QDialog, QDialog::Accepted
#include <QString>  // for QString
#include <QWidget>  // for QWidget

#include <ui/longjobhelper.h>   // for LongJobHelper, LongJobHelper::CANCEL

#include "addons-swapaxes.h"
#include "swapaxesdialog.h"     // for SwapAxesDialog
#include "swapaxesoptions.h"    // for SwapAxesOptions
#include "utils-swapaxes.h"     // for swapAxes, COMMENT_ID_UNKNOWN


bool Addons::doSwapAxes(QWidget* parent, QSettings* settings, QString& text)
{
	QString key = "SwapAxesDialog";
	SwapAxesDialog* dlg;
	dlg = parent->findChild<SwapAxesDialog*>(key);

	if (!dlg) {
		dlg = new SwapAxesDialog(parent, settings);
		dlg->setObjectName(key);
		dlg->loadSettings(SwapAxesOptions());
	}

	if (dlg->exec() != QDialog::Accepted) {
		return false;
	}


	LongJobHelper helper{parent};
	helper.begin(text.length(), QCoreApplication::translate("Addons::Actions", "Axis exchange"), 20);

	bool changed = Utils::swapAxes(text, dlg->options(), [&helper](int pos) -> bool{
		return helper.check(pos) == LongJobHelper::CANCEL;
	});

	helper.end();
	dlg->deleteLater();
	return changed;
}
