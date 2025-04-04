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

#include <QCoreApplication> // for QCoreApplication
#include <QDialog>          // for QDialog, QDialog::Accepted
#include <QObject>          // for QObject
#include <QString>          // for QString
#include <QStringList>      // for QStringList
#include <QWidget>          // for QWidget

#include <ui/longjobhelper.h>   // for LongJobHelper, LongJobHelper::CANCEL

#include "addons-cleanup.h"
#include "cleanupdialog.h"          // for CleanUpDialog
#include "cleanupoptions.h"         // for CleanUpOptions
#include "utils-removebyregex.h"    // for removeTextByRegExp


bool Addons::doCleanUp(QWidget* parent, QSettings* settings,  QString& tx)
{
	bool result = false;
	QString key = "CleanUpDialog";
	CleanUpDialog* dlg;
	dlg = parent->findChild<CleanUpDialog*>(key);

	if (!dlg) {
		dlg = new CleanUpDialog(parent, settings);
		dlg->setObjectName(key);
		CleanUpOptions opt;
		opt.expressions
		        << "('\\()[\\w,.;:/*+\\\\! $%^&-]{0,}(\\))$"
		        << "(\\()[\\w,.;:/*+\\\\! $%^&-]{0,}(\\))"
		        << "[\n]{2,}";
		opt.comments
		        << QObject::tr("Lines with: '(comment)")
		        << QObject::tr("Any: (comment)")
		        << QObject::tr("Empty lines:");
		dlg->loadSettings(opt);
	}

	dlg->setText(tx);

	if (dlg->exec() != QDialog::Accepted) {
		return false;
	}

	LongJobHelper helper{parent};
	helper.begin(tx.length(), QCoreApplication::translate("Addons::Actions", "Cleaning up"), 20);

	result = Utils::removeTextByRegExp(tx, dlg->options().selected, true, [&helper](int pos) -> bool{
		return helper.check(pos) == LongJobHelper::CANCEL;
	});

	helper.end();
	dlg->deleteLater();
	return result;
}
