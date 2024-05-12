/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of EdytorNC.                                        *
 *                                                                         *
 *   EdytorNC is free software; you can redistribute it and/or modify      *
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

#include <QCheckBox>        // for QCheckBox
#include <QList>            // for QList
#include <QListWidget>      // for QListWidget
#include <QListWidgetItem>  // for QListWidgetItem
#include <QPushButton>      // for QPushButton
#include <QString>          // for QString
#include <QStringList>      // for QStringList
#include <Qt>               // for Dialog, WindowFlags
class QWidget;

#include "sessiondialog.h"
#include "sessionmanager.h"     // for SessionManager
#include "sessionnamedialog.h"  // for SessionNameDialog


SessionDialog::SessionDialog(QWidget* parent, SessionManager* sessions) : QDialog(parent, Qt::Dialog)
{
	m_sessions = sessions;
	setupUi(this);
	setWindowTitle(tr("Session manager"));
	setModal(true);

	connect(newPushButton, SIGNAL(clicked()), this, SLOT(newButtonClicked()));
	connect(renamePushButton, SIGNAL(clicked()), this, SLOT(renameButtonClicked()));
	connect(clonePushButton, SIGNAL(clicked()), this, SLOT(cloneButtonClicked()));
	connect(deletePushButton, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));
	connect(switchPushButton, SIGNAL(clicked()), this, SLOT(switchButtonClicked()));

	connect(sessionListWidget, SIGNAL(itemSelectionChanged()), this,
	        SLOT(sessionListItemSelectionChanged()));

	connect(m_sessions, SIGNAL(sessionListChanged(QStringList)), this, SLOT(updateSessionList(QStringList)));
	updateSessionList(m_sessions->sessionList());
}

SessionDialog::~SessionDialog()
{
}

void SessionDialog::newButtonClicked()
{
	SessionNameDialog* newSesDialog = new SessionNameDialog(this);
	newSesDialog->setName("");
	int result = newSesDialog->exec();

	if (result == QDialog::Accepted) {
		QString tx = newSesDialog->getName().simplified();

		if (!tx.isEmpty()) {
			m_sessions->addSession(tx);
		}
	}

	delete newSesDialog;
}

void SessionDialog::renameButtonClicked()
{
	QString currName = sessionListWidget->currentItem()->text();
	SessionNameDialog* newSesDialog = new SessionNameDialog(this);
	newSesDialog->setName(currName);
	int result = newSesDialog->exec();

	if (result == QDialog::Accepted) {
		QString newName = newSesDialog->getName().simplified();

		if (!newName.isEmpty()) {
			m_sessions->renameSession(currName, newName);
		}
	}

	delete newSesDialog;
}

void SessionDialog::cloneButtonClicked()
{
	QString currName = sessionListWidget->currentItem()->text();
	SessionNameDialog* newSesDialog = new SessionNameDialog(this);
	newSesDialog->setName(currName);
	int result = newSesDialog->exec();

	if (result == QDialog::Accepted) {
		QString newName = newSesDialog->getName().simplified();

		if (!newName.isEmpty()) {
			m_sessions->copySession(currName, newName);
		}
	}

	delete newSesDialog;
}

void SessionDialog::deleteButtonClicked()
{
	const QList<QListWidgetItem*>& selected = sessionListWidget->selectedItems();

	if (!selected.isEmpty()) {
		m_sessions->removeSession(selected.at(0)->text());
	}
}

void SessionDialog::switchButtonClicked()
{
	m_sessions->setCurrentSession(sessionListWidget->currentItem()->text());
	accept();
}

void SessionDialog::sessionListItemSelectionChanged()
{
	bool hasSelection = sessionListWidget->selectedItems().size() == 1;
	deletePushButton->setEnabled(hasSelection);
	renamePushButton->setEnabled(hasSelection);
	clonePushButton->setEnabled(hasSelection);
	switchPushButton->setEnabled(hasSelection);
}

void SessionDialog::updateSessionList(const QStringList& list)
{
	sessionListWidget->clear();
	sessionListWidget->addItems(list);
}
