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

#include <QDialog>  // for QDialog, QDialog::Accepted
#include <QString>  // for QString
#include <QWidget>  // for QWidget

#include "addons-swapaxes.h"
#include "swapaxesdialog.h"     // for SwapAxesDialog
#include "swapaxesoptions.h"    // for SwapAxesOptions
#include "utils-swapaxes.h"     // for swapAxes, COMMENT_ID_UNKNOWN


int Addons::doSwapAxes(QWidget *parent, QSettings *settings, QPlainTextEdit *textEdit)
{
	int result = 0;
	QString key = "SwapAxesDialog";
	SwapAxesDialog *dlg;
	dlg = parent->findChild<SwapAxesDialog *>(key);

	if (!dlg) {
		dlg = new SwapAxesDialog(parent, settings);
		dlg->setObjectName(key);
		dlg->loadSettings(SwapAxesOptions());
	}

	if (dlg->exec() == QDialog::Accepted) {
		Utils::swapAxes(textEdit, false, 40000, COMMENT_ID_UNKNOWN, dlg->options());
	}

	dlg->deleteLater();
	return result;
}
