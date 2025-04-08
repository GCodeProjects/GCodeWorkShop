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

#include <QCoreApplication> // for translate
#include <QDialog>          // for QDialog
#include <QString>          // for QString
#include <QWidget>          // for QWidget

#include <ui/longjobhelper.h>   // for LongJobHelper, LongJobHelper::CANCEL

#include "addons-renumber.h"
#include "renumberdialog.h"     // for RenumberDialog
#include "renumberoptions.h"    // for RenumberOptions
#include "utils-renumber.h"     // for renumber


bool Addons::doRenumber(QWidget* parent, QSettings* settings, QString& tx)
{
	QString key = "RenumberDialog";
	RenumberDialog* dlg;
	dlg = parent->findChild<RenumberDialog*>(key);

	if (!dlg) {
		dlg = new RenumberDialog(parent, settings);
		dlg->setObjectName(key);
		dlg->loadSettings();
	}

	if (dlg->exec() != QDialog::Accepted) {
		return false;
	}

	LongJobHelper helper{parent};
	helper.begin(tx.length(), QCoreApplication::translate("Addons::Actions", "Renumbering"), 20);

	bool changed = Utils::renumber(tx, dlg->options(), [&helper](int pos) -> bool{
		return helper.check(pos) == LongJobHelper::CANCEL;
	});

	helper.end();
	return changed;
}

