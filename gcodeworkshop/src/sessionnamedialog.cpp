/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of GCodeWorkShop.                                   *
 *                                                                         *
 *   GCodeWorkShop is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QDialogButtonBox> // for QDialogButtonBox
#include <QLineEdit>        // for QLineEdit
#include <QString>          // for QString
#include <Qt>               // for WindowFlags

class QWidget;

#include "sessionnamedialog.h"


SessionNameDialog::SessionNameDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	setWindowTitle(tr("New session..."));
	setModal(true);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

SessionNameDialog::~SessionNameDialog()
{
}

QString SessionNameDialog::getName()
{
	return lineEdit->text();
}

void SessionNameDialog::setName(QString name)
{
	lineEdit->setText(name);
}
