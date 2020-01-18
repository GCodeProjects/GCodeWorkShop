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

#include "sessiondialog.h"


sessionDialog::sessionDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle(tr("Session manager"));
    setModal(true);

    //clonePushButton->setVisible(false);
    //restoreCheckBox->setVisible(false);

    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("Sessions");

    restoreCheckBox->setChecked(settings.value("RestoreLastSession", false).toBool());
    settings.endGroup();

    connect(newPushButton, SIGNAL(clicked()), this, SLOT(newButtonClicked()));
    connect(renamePushButton, SIGNAL(clicked()), this, SLOT(renameButtonClicked()));
    connect(clonePushButton, SIGNAL(clicked()), this, SLOT(cloneButtonClicked()));
    connect(deletePushButton, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));
    connect(switchPushButton, SIGNAL(clicked()), this, SLOT(switchButtonClicked()));

    connect(sessionListWidget, SIGNAL(itemActivated(QListWidgetItem *)), this,
            SLOT(sessionListItemitemActivated(QListWidgetItem *)));

}

sessionDialog::~sessionDialog()
{
    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("Sessions");

    settings.setValue("RestoreLastSession", restoreCheckBox->isChecked());
    settings.endGroup();
}

void sessionDialog::newButtonClicked()
{
    newSessionDialog *newSesDialog = new newSessionDialog(this);
    newSesDialog->setName("");
    int result = newSesDialog->exec();

    if (result == QDialog::Accepted) {
        QString tx = newSesDialog->getName().simplified();

        if (!tx.isEmpty()) {
            sessionListWidget->insertItem(sessionListWidget->count(), tx);
            sessionListWidget->item(sessionListWidget->count() - 1)->setCheckState(Qt::Unchecked);
        }
    }

    delete newSesDialog;
}

void sessionDialog::renameButtonClicked()
{
    QString currName = sessionListWidget->currentItem()->text();

    if (currName == tr("default")) {
        return;
    }

    newSessionDialog *newSesDialog = new newSessionDialog(this);
    newSesDialog->setName(currName);
    int result = newSesDialog->exec();

    if (result == QDialog::Accepted) {
        QString newName = newSesDialog->getName().simplified();

        if (!newName.isEmpty()) {
            sessionListWidget->currentItem()->setText(newName);
            copySession(currName, newName, true);
        }
    }

    delete newSesDialog;
}

void sessionDialog::cloneButtonClicked()
{
    QString currName = sessionListWidget->currentItem()->text();
    newSessionDialog *newSesDialog = new newSessionDialog(this);
    newSesDialog->setName(currName);
    int result = newSesDialog->exec();

    if (result == QDialog::Accepted) {
        QString newName = newSesDialog->getName().simplified();

        if (!newName.isEmpty()) {
            sessionListWidget->insertItem(sessionListWidget->count(), newName);
            sessionListWidget->item(sessionListWidget->count() - 1)->setCheckState(Qt::Unchecked);
            copySession(currName, newName, false);
        }
    }

    delete newSesDialog;
}

void sessionDialog::deleteButtonClicked()
{
    if (sessionListWidget->currentItem()->text() != tr("default")) {
        deleteSession(sessionListWidget->selectedItems().at(0)->text());
        qDeleteAll(sessionListWidget->selectedItems());
        switchButtonClicked();
    }
}

void sessionDialog::switchButtonClicked()
{
    clearChecked();
    sessionListWidget->currentItem()->setCheckState(Qt::Checked);
    accept();
}

void sessionDialog::sessionListItemitemActivated(QListWidgetItem *item)
{
    Q_UNUSED(item);

    bool hasSelection = sessionListWidget->selectedItems().size() == 1;
    deletePushButton->setEnabled(hasSelection);
    renamePushButton->setEnabled(hasSelection);
    clonePushButton->setEnabled(hasSelection);
    switchPushButton->setEnabled(hasSelection);
}

void sessionDialog::setSessionList(QStringList list)
{
    list.removeDuplicates();
    list.sort();
    sessionListWidget->addItems(list);
    clearChecked();
}

QStringList sessionDialog::sessionList()
{
    QStringList sessionList;

    for (int i = 0; i < sessionListWidget->count(); i++) {
        sessionList.append(sessionListWidget->item(i)->text());
        sessionList.removeDuplicates();
        sessionList.sort();
    }

    return sessionList;
}

void sessionDialog::setSelectedSession(QString name)
{
    QList<QListWidgetItem *> items = sessionListWidget->findItems(name, Qt::MatchExactly);

    if (items.size() > 0) {
        items.at(0)->setCheckState(Qt::Checked);
        sessionListWidget->setCurrentItem(items.at(0));
    }
}

QString sessionDialog::selectedSession()
{
    QString name;

    for (int i = 0; i < sessionListWidget->count(); i++) {
        if (sessionListWidget->item(i)->checkState() == Qt::Checked) {
            name = sessionListWidget->item(i)->text();
            break;
        }
    }

    return name;
}

void sessionDialog::clearChecked()
{
    for (int i = 0; i < sessionListWidget->count(); i++) {
        sessionListWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void sessionDialog::copySession(QString oldName, QString newName, bool deleteOld)
{
    QString cursorPos;
    QString readOnly;
    QByteArray geometry;
    QString fileName;
    int highlightMode;
    bool maximized;

    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("Sessions");

    int max = settings.beginReadArray(oldName);
    settings.endArray();

    for (int i = 0; i < max; ++i) {
        // read
        settings.beginReadArray(oldName);
        settings.setArrayIndex(i);
        fileName = settings.value("OpenedFile").toString();
        cursorPos = settings.value("Cursor", 1).toInt();
        readOnly = settings.value("ReadOnly", false).toBool();
        geometry = settings.value("Geometry", QByteArray()).toByteArray();
        highlightMode = settings.value("HighlightMode", 0).toInt();
        maximized = settings.value("MaximizedMdi", true).toBool();
        settings.endArray();

        // write new
        settings.beginWriteArray(newName);
        settings.setArrayIndex(i);
        settings.setValue("OpenedFile", fileName);
        settings.setValue("Cursor", cursorPos);
        settings.setValue("ReadOnly", readOnly);
        settings.setValue("Geometry", geometry);
        settings.setValue("HighlightMode", highlightMode);
        settings.setValue("MaximizedMdi", maximized);
        settings.endArray();
    }

    if (deleteOld) {
        settings.remove(oldName);
    }

    settings.endGroup();
}

void sessionDialog::deleteSession(QString name)
{
    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("Sessions");
    settings.remove(name);
    settings.endGroup();
}

newSessionDialog::newSessionDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("New session..."));
    setModal(true);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

newSessionDialog::~newSessionDialog()
{
}

QString newSessionDialog::getName()
{
    return lineEdit->text();
}

void newSessionDialog::setName(QString name)
{
    lineEdit->setText(name);
}
