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

#include <QApplication>
#include <QFileDialog>
#include <QSettings>

#include <utils/medium.h> // Medium

#include "newfiledialog.h" // newFileDialog QObject QDialog

#include "ui_newfiledialog.h"

// TODO: change a path to the file
#define TEMPLATE_PATH             "/usr/share/edytornc/TEMPLATE"

newFileDialog::newFileDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::newFileDialog)
{
	ui->setupUi(this);

	path.setPath(TEMPLATE_PATH);

	if (!path.exists()) {
		path.setPath(QApplication::applicationDirPath() + "/" + "TEMPLATE");
	}

	if (!path.exists()) {
		path.setPath(QDir::homePath());
	}

	QSettings &settings = *Medium::instance().settings();

	QDir savedPath(settings.value("TemplatePath", path.canonicalPath()).toString());

	if (savedPath.exists()) {
		path = savedPath;
	}

	ui->pathLineEdit->setText(path.canonicalPath());

	fillFileCombo();

	connect(ui->browsePushButton, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));

	connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

newFileDialog::~newFileDialog()
{
	delete ui;
}

void newFileDialog::browseButtonClicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, tr("Choose template path"),
	                    path.canonicalPath());

	if (!directory.isEmpty()) {
		path.setPath(directory);
		ui->pathLineEdit->setText(path.canonicalPath());
		fillFileCombo();
	}
}

void newFileDialog::fillFileCombo()
{
	QStringList files = path.entryList((QStringList() << "*.nc"),
	                                   QDir::Files | QDir::NoDotAndDotDot | QDir::Readable,
	                                   QDir::Name | QDir::IgnoreCase);
	ui->fileComboBox->clear();
	ui->fileComboBox->addItem(tr("EMPTY FILE"));
	ui->fileComboBox->addItems(files);
}

void newFileDialog::saveSettings()
{
	QSettings &settings = *Medium::instance().settings();

	settings.setValue("TemplatePath", path.canonicalPath());
}

QString newFileDialog::getChosenFile()
{
	QFile file(path.canonicalPath() + "/" + ui->fileComboBox->currentText());

	if (file.exists()) {
		return file.fileName();
	} else {
		return tr("EMPTY FILE");
	}
}

int newFileDialog::exec()
{
	return QDialog::exec();
}
