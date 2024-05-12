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

#include <QAbstractButton>
#include <QButtonGroup>
#include <Qt>                          // Qt::WindowFlags
#include <QWidget>

#include <utils/medium.h> // Medium

#include "highlightmode.h"
#include "setupdialog.h" // SetupDialog QDialog QObject


SetupDialog::SetupDialog(QWidget *parent, const AppConfig *prop,
                         Qt::WindowFlags f) : QDialog(parent, f)
{
	QPalette palette;

	setupUi(this);
	setWindowTitle(tr("EdytorNC configuration"));

	editProp = *prop;

	readOnlyModeCheckBox->setChecked(editProp.defaultReadOnly);

	fontLabel->setText(QString(tr("Current font : <b>\"%1\", %2 pt.<\b>")
	                           .arg(editProp.codeStyle.fontName).arg(editProp.codeStyle.fontSize)));
	fontLabel->setFont(QFont(editProp.codeStyle.fontName, editProp.codeStyle.fontSize));

	connect(changeFontButton, SIGNAL(clicked()), SLOT(changeFont()));
	connect(browseButton, SIGNAL(clicked()), SLOT(browseButtonClicked()));

	colorButtons = new QButtonGroup(this);
	connect(colorButtons, SIGNAL(buttonClicked(QAbstractButton *)),
	        SLOT(changeColor(QAbstractButton *)));

	palette.setColor(backgroundColorButton->foregroundRole(), editProp.codeStyle.hColors.backgroundColor);
	backgroundColorButton->setPalette(palette);
	backgroundColorButton->setAutoFillBackground(true);
	colorButtons->addButton(backgroundColorButton);

	QColor color = editProp.codeStyle.hColors.backgroundColor;

	palette.setColor(QPalette::Button, color);
	palette.setColor(QPalette::Light, color);
	palette.setColor(QPalette::Dark, color);
	palette.setColor(QPalette::Shadow, color);
	palette.setColor(QPalette::Midlight, color);
	palette.setColor(QPalette::Base, color);

	palette.setColor(fontColorButton->foregroundRole(), editProp.codeStyle.hColors.defaultColor);
	fontColorButton->setPalette(palette);
	fontColorButton->setAutoFillBackground(true);
	fontColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(fontColorButton);

	palette.setColor(commentColorButton->foregroundRole(), editProp.codeStyle.hColors.commentColor);
	commentColorButton->setPalette(palette);
	commentColorButton->setAutoFillBackground(true);
	commentColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(commentColorButton);

	palette.setColor(gColorButton->foregroundRole(), editProp.codeStyle.hColors.gColor);
	gColorButton->setPalette(palette);
	gColorButton->setAutoFillBackground(true);
	gColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(gColorButton);

	palette.setColor(mColorButton->foregroundRole(), editProp.codeStyle.hColors.mColor);
	mColorButton->setPalette(palette);
	mColorButton->setAutoFillBackground(true);
	mColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(mColorButton);

	palette.setColor(nColorButton->foregroundRole(), editProp.codeStyle.hColors.nColor);
	nColorButton->setPalette(palette);
	nColorButton->setAutoFillBackground(true);
	nColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(nColorButton);

	palette.setColor(lColorButton->foregroundRole(), editProp.codeStyle.hColors.lColor);
	lColorButton->setPalette(palette);
	lColorButton->setAutoFillBackground(true);
	lColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(lColorButton);

	palette.setColor(fsColorButton->foregroundRole(), editProp.codeStyle.hColors.fsColor);
	fsColorButton->setPalette(palette);
	fsColorButton->setAutoFillBackground(true);
	fsColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(fsColorButton);

	palette.setColor(dhtColorButton->foregroundRole(), editProp.codeStyle.hColors.dhtColor);
	dhtColorButton->setPalette(palette);
	dhtColorButton->setAutoFillBackground(true);
	dhtColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(dhtColorButton);

	palette.setColor(rColorButton->foregroundRole(), editProp.codeStyle.hColors.rColor);
	rColorButton->setPalette(palette);
	rColorButton->setAutoFillBackground(true);
	rColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(rColorButton);

	palette.setColor(macroColorButton->foregroundRole(), editProp.codeStyle.hColors.macroColor);
	macroColorButton->setPalette(palette);
	macroColorButton->setAutoFillBackground(true);
	macroColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(macroColorButton);

	palette.setColor(keyWordColorButton->foregroundRole(), editProp.codeStyle.hColors.keyWordColor);
	keyWordColorButton->setPalette(palette);
	keyWordColorButton->setAutoFillBackground(true);
	keyWordColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(keyWordColorButton);

	palette.setColor(progNameColorButton->foregroundRole(), editProp.codeStyle.hColors.progNameColor);
	progNameColorButton->setPalette(palette);
	progNameColorButton->setAutoFillBackground(true);
	progNameColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(progNameColorButton);

	palette.setColor(operatorColorButton->foregroundRole(), editProp.codeStyle.hColors.operatorColor);
	operatorColorButton->setPalette(palette);
	operatorColorButton->setAutoFillBackground(true);
	operatorColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(operatorColorButton);

	palette.setColor(zColorButton->foregroundRole(), editProp.codeStyle.hColors.zColor);
	zColorButton->setPalette(palette);
	zColorButton->setAutoFillBackground(true);
	zColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(zColorButton);

	palette.setColor(aColorButton->foregroundRole(), editProp.codeStyle.hColors.aColor);
	aColorButton->setPalette(palette);
	aColorButton->setAutoFillBackground(true);
	aColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(aColorButton);

	palette.setColor(bColorButton->foregroundRole(), editProp.codeStyle.hColors.bColor);
	bColorButton->setPalette(palette);
	bColorButton->setAutoFillBackground(true);
	bColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(bColorButton);

	palette.setColor(underlineColorButton->foregroundRole(), editProp.codeStyle.underlineColor);
	underlineColorButton->setPalette(palette);
	underlineColorButton->setAutoFillBackground(true);
	underlineColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(underlineColorButton);

	palette.setColor(curLineColorButton->foregroundRole(), editProp.codeStyle.lineColor);
	curLineColorButton->setPalette(palette);
	curLineColorButton->setAutoFillBackground(true);
	curLineColorButton->setBackgroundRole(QPalette::Base);
	colorButtons->addButton(curLineColorButton);

	capsLockCheckBox->setChecked(editProp.editorProperties.intCapsLock);
	syntaxHCheckBox->setChecked(editProp.editorProperties.syntaxH);
	underlineCheckBox->setChecked(editProp.editorProperties.underlineChanges);

	tabbedModecheckBox->setChecked(editProp.mdiTabbedMode);
	fileNameCheckBox->setChecked(editProp.editorProperties.windowMode & SHOW_FILENAME);
	filePathCheckBox->setChecked(editProp.editorProperties.windowMode & SHOW_FILEPATH);
	titleCheckBox->setChecked(editProp.editorProperties.windowMode & SHOW_PROGTITLE);

	calcLineEdit->setText(editProp.calcBinary);
	clearUndocheckBox->setChecked(editProp.editorProperties.clearUndoHistory);
	clearUnderlinecheckBox->setChecked(editProp.editorProperties.clearUnderlineHistory);
	editorToolTipsCheckBox->setChecked(editProp.editorProperties.editorToolTips);
	startEmptyCheckBox->setChecked(editProp.startEmpty);
	disableFileChangeMonitorCheckBox->setChecked(editProp.disableFileChangeMonitor);
	changeDateOnSave->setChecked(editProp.editorProperties.changeDateInComment);

	QStringListIterator extIterator(editProp.extensions);

	while (extIterator.hasNext()) {
		lstExtensions->addItem(extIterator.next().toLocal8Bit().constData());
	}

	edtSaveExtension->setText(editProp.saveExtension);
	edtSaveDirectory->setText(editProp.saveDirectory);

	if (editProp.editorProperties.guessFileNameByProgNum) {
		progNumCheckBox->setChecked(true);
	} else {
		firstCommCheckBox->setChecked(true);
	}

	highlightModeComboBox->addItem(tr("AUTO"), MODE_AUTO);
	highlightModeComboBox->addItem(tr("FANUC"), MODE_FANUC);
	highlightModeComboBox->addItem(tr("HEIDENHAIN DIALOG"), MODE_HEIDENHAIN);
	highlightModeComboBox->addItem(tr("HEIDENHAIN ISO"), MODE_HEIDENHAIN_ISO);
	highlightModeComboBox->addItem(tr("OKUMA OSP"), MODE_OKUMA);
	highlightModeComboBox->addItem(tr("PHILIPS"), MODE_PHILIPS);
	highlightModeComboBox->addItem(tr("SINUMERIK OLD"), MODE_SINUMERIK);
	highlightModeComboBox->addItem(tr("SINUMERIK NEW"), MODE_SINUMERIK_840);
	highlightModeComboBox->addItem(tr("LinuxCNC"), MODE_LINUXCNC);
	highlightModeComboBox->addItem(tr("TOOLTIPS"), MODE_TOOLTIPS);

	int id = highlightModeComboBox->findData(editProp.editorProperties.defaultHighlightMode);
	highlightModeComboBox->setCurrentIndex(id);

	QRegularExpression regex("(\\*\\.)[A-Z0-9]{1,3}");
	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	QValidator *edtExtensionValid = new QRegularExpressionValidator(regex, this);
	edtExtension->setValidator(edtExtensionValid);

	//   QRegularExpression ext("(\\(|;){1,1}[\\s]{0,5}(d|dd|M|MM|YYYY)[-./]{1,1}(M|MM|d|dd)[-./]{1,1}(d|dd|M|MM|YYYY)[\\s]{1,5}(\\)){0,1}");
	//   // (\\(;){1,1}[\\s]{0,5}(d|dd|M|MM|YYYY)[.-/]{1,1}(M|MM|d|dd)[.-/]{1,1}(d|dd|M|MM|YYYY)[\\s]{1,5}(\\)){0,1}
	//   ext.setCaseSensitivity(Qt::CaseSensitive);
	//   QValidator *dateFormatValidator = new QRegExpValidator(ext, this);
	//   dateFormatComboBox->setValidator(dateFormatValidator);

	connect(defaultButton, SIGNAL(clicked()), SLOT(setDefaultProp()));
	connect(okButton, SIGNAL(clicked()), SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
}

SetupDialog::~SetupDialog()
{
}

void SetupDialog::changeFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, QFont(editProp.codeStyle.fontName, editProp.codeStyle.fontSize), this);

	if (ok) {
		editProp.codeStyle.fontName = font.family();
		editProp.codeStyle.fontSize = font.pointSize();
		fontLabel->setText(QString(tr("Current font : <b>\"%1\", %2 pt.<\b>")
		                           .arg(editProp.codeStyle.fontName).arg(editProp.codeStyle.fontSize)));
		fontLabel->setFont(QFont(editProp.codeStyle.fontName, editProp.codeStyle.fontSize));
	}
}

void SetupDialog::browseButtonClicked()
{
#ifdef Q_OS_WIN32
	QString filter = tr("Executables (*.exe)");
#else
	QString filter = tr("All files (*)");
#endif

	QString fileName = QFileDialog::getOpenFileName(
	                       this,
	                       tr("Select calculator executable"),
	                       calcLineEdit->text(),
	                       filter);

	QFileInfo file(fileName);

	if ((file.exists()) && (file.isReadable())) {
		calcLineEdit->setText(QDir::toNativeSeparators(file.canonicalFilePath()));
	}
}

AppConfig SetupDialog::getSettings()
{
	QPalette palette;
	int r, g, b;
	bool ok;

	r = 0;

	if (fileNameCheckBox->isChecked()) {
		r |= SHOW_FILENAME;
	}

	if (filePathCheckBox->isChecked()) {
		r |= SHOW_FILEPATH;
	}

	if (titleCheckBox->isChecked()) {
		r |= SHOW_PROGTITLE;
	}

	editProp.mdiTabbedMode = tabbedModecheckBox->isChecked();
	editProp.editorProperties.windowMode = r;
	editProp.editorProperties.intCapsLock = capsLockCheckBox->isChecked();
	editProp.editorProperties.syntaxH = syntaxHCheckBox->isChecked();
	editProp.editorProperties.underlineChanges = underlineCheckBox->isChecked();
	editProp.calcBinary = calcLineEdit->text();
	editProp.editorProperties.clearUndoHistory = clearUndocheckBox->isChecked();
	editProp.editorProperties.clearUnderlineHistory = clearUnderlinecheckBox->isChecked();
	editProp.editorProperties.editorToolTips = editorToolTipsCheckBox->isChecked();
	editProp.defaultReadOnly = readOnlyModeCheckBox->isChecked();
	editProp.startEmpty = startEmptyCheckBox->isChecked();
	editProp.disableFileChangeMonitor = disableFileChangeMonitorCheckBox->isChecked();
	editProp.editorProperties.changeDateInComment = changeDateOnSave->isChecked();

	int id = highlightModeComboBox->currentIndex();

	if (id >= 0) {
		editProp.editorProperties.defaultHighlightMode = highlightModeComboBox->itemData(id).toInt(&ok);
	}

	palette = backgroundColorButton->palette();
	palette.color(backgroundColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.backgroundColor = (r << 16) + (g << 8) + b;

	palette = fontColorButton->palette();
	palette.color(fontColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.defaultColor = (r << 16) + (g << 8) + b;

	palette = commentColorButton->palette();
	palette.color(commentColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.commentColor = (r << 16) + (g << 8) + b;

	palette = gColorButton->palette();
	palette.color(gColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.gColor = (r << 16) + (g << 8) + b;

	palette = mColorButton->palette();
	palette.color(mColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.mColor = (r << 16) + (g << 8) + b;

	palette = nColorButton->palette();
	palette.color(nColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.nColor = (r << 16) + (g << 8) + b;

	palette = lColorButton->palette();
	palette.color(lColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.lColor = (r << 16) + (g << 8) + b;

	palette = fsColorButton->palette();
	palette.color(fsColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.fsColor = (r << 16) + (g << 8) + b;

	palette = dhtColorButton->palette();
	palette.color(dhtColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.dhtColor = (r << 16) + (g << 8) + b;

	palette = rColorButton->palette();
	palette.color(rColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.rColor = (r << 16) + (g << 8) + b;

	palette = macroColorButton->palette();
	palette.color(macroColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.macroColor = (r << 16) + (g << 8) + b;

	palette = keyWordColorButton->palette();
	palette.color(keyWordColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.keyWordColor = (r << 16) + (g << 8) + b;

	palette = progNameColorButton->palette();
	palette.color(progNameColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.progNameColor = (r << 16) + (g << 8) + b;

	palette = operatorColorButton->palette();
	palette.color(operatorColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.operatorColor = (r << 16) + (g << 8) + b;

	palette = zColorButton->palette();
	palette.color(zColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.zColor = (r << 16) + (g << 8) + b;

	palette = aColorButton->palette();
	palette.color(aColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.aColor = (r << 16) + (g << 8) + b;

	palette = bColorButton->palette();
	palette.color(bColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.hColors.bColor = (r << 16) + (g << 8) + b;

	palette = underlineColorButton->palette();
	palette.color(underlineColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.underlineColor = (r << 16) + (g << 8) + b;

	palette = curLineColorButton->palette();
	palette.color(curLineColorButton->foregroundRole()).getRgb(&r, &g, &b);
	editProp.codeStyle.lineColor = (r << 16) + (g << 8) + b;

	editProp.extensions.clear();

	for (int row = 0; row < lstExtensions->count(); row++) {
		QListWidgetItem *item = lstExtensions->item(row);
		editProp.extensions.append(item->text());
	}

	editProp.saveExtension = edtSaveExtension->text();
	editProp.saveDirectory = edtSaveDirectory->text();

	editProp.editorProperties.guessFileNameByProgNum = progNumCheckBox->isChecked();

	return (editProp);
}

void SetupDialog::changeColor(QAbstractButton *button)
{
	QPalette palette;

	palette = button->palette();

	QColor color = QColorDialog::getColor(palette.color(button->foregroundRole()), this);

	if (color.isValid()) {
		palette.setColor(button->foregroundRole(), color);
		button->setPalette(palette);
	}

	palette = backgroundColorButton->palette();
	color = palette.color(backgroundColorButton->foregroundRole());

	palette = fontColorButton->palette();
	palette.setColor(fontColorButton->backgroundRole(), color);
	fontColorButton->setPalette(palette);

	palette = commentColorButton->palette();
	palette.setColor(commentColorButton->backgroundRole(), color);
	commentColorButton->setPalette(palette);

	palette = commentColorButton->palette();
	palette.setColor(commentColorButton->backgroundRole(), color);
	commentColorButton->setPalette(palette);

	palette = gColorButton->palette();
	palette.setColor(gColorButton->backgroundRole(), color);
	gColorButton->setPalette(palette);

	palette = mColorButton->palette();
	palette.setColor(mColorButton->backgroundRole(), color);
	mColorButton->setPalette(palette);

	palette = nColorButton->palette();
	palette.setColor(nColorButton->backgroundRole(), color);
	nColorButton->setPalette(palette);

	palette = lColorButton->palette();
	palette.setColor(lColorButton->backgroundRole(), color);
	lColorButton->setPalette(palette);

	palette = fsColorButton->palette();
	palette.setColor(fsColorButton->backgroundRole(), color);
	fsColorButton->setPalette(palette);

	palette = dhtColorButton->palette();
	palette.setColor(dhtColorButton->backgroundRole(), color);
	dhtColorButton->setPalette(palette);

	palette = rColorButton->palette();
	palette.setColor(rColorButton->backgroundRole(), color);
	rColorButton->setPalette(palette);

	palette = macroColorButton->palette();
	palette.setColor(macroColorButton->backgroundRole(), color);
	macroColorButton->setPalette(palette);

	palette = keyWordColorButton->palette();
	palette.setColor(keyWordColorButton->backgroundRole(), color);
	keyWordColorButton->setPalette(palette);

	palette = progNameColorButton->palette();
	palette.setColor(progNameColorButton->backgroundRole(), color);
	progNameColorButton->setPalette(palette);

	palette = operatorColorButton->palette();
	palette.setColor(operatorColorButton->backgroundRole(), color);
	operatorColorButton->setPalette(palette);

	palette = zColorButton->palette();
	palette.setColor(zColorButton->backgroundRole(), color);
	zColorButton->setPalette(palette);

	palette = aColorButton->palette();
	palette.setColor(aColorButton->backgroundRole(), color);
	aColorButton->setPalette(palette);

	palette = bColorButton->palette();
	palette.setColor(bColorButton->backgroundRole(), color);
	bColorButton->setPalette(palette);

	palette = underlineColorButton->palette();
	palette.setColor(underlineColorButton->backgroundRole(), color);
	underlineColorButton->setPalette(palette);

	palette = curLineColorButton->palette();
	palette.setColor(curLineColorButton->backgroundRole(), color);
	curLineColorButton->setPalette(palette);
}

void SetupDialog::setDefaultProp()
{
	QPalette palette;

	palette.setColor(commentColorButton->foregroundRole(), 0xde0020);
	commentColorButton->setPalette(palette);

	palette.setColor(gColorButton->foregroundRole(), 0x1600ee);
	gColorButton->setPalette(palette);

	palette.setColor(mColorButton->foregroundRole(), 0x80007d);
	mColorButton->setPalette(palette);

	palette.setColor(nColorButton->foregroundRole(), Qt::darkGray);
	nColorButton->setPalette(palette);

	palette.setColor(lColorButton->foregroundRole(), 0x535b5f);
	lColorButton->setPalette(palette);

	palette.setColor(fsColorButton->foregroundRole(), 0x516600);
	fsColorButton->setPalette(palette);

	palette.setColor(dhtColorButton->foregroundRole(), 0x660033);
	dhtColorButton->setPalette(palette);

	palette.setColor(rColorButton->foregroundRole(), 0x24576f);
	rColorButton->setPalette(palette);

	palette.setColor(macroColorButton->foregroundRole(), 0x000080);
	macroColorButton->setPalette(palette);

	palette.setColor(keyWordColorButton->foregroundRole(), 0x1d8000);
	keyWordColorButton->setPalette(palette);

	palette.setColor(progNameColorButton->foregroundRole(), Qt::black);
	progNameColorButton->setPalette(palette);

	palette.setColor(operatorColorButton->foregroundRole(), 0x9a2200);
	operatorColorButton->setPalette(palette);

	palette.setColor(zColorButton->foregroundRole(), 0x000080);
	zColorButton->setPalette(palette);

	palette.setColor(aColorButton->foregroundRole(), Qt::black);
	aColorButton->setPalette(palette);

	palette.setColor(bColorButton->foregroundRole(), Qt::black);
	bColorButton->setPalette(palette);

	palette.setColor(underlineColorButton->foregroundRole(), Qt::green);
	underlineColorButton->setPalette(palette);

	palette.setColor(curLineColorButton->foregroundRole(), 0xFEFFB6);
	curLineColorButton->setPalette(palette);

	palette.setColor(fontColorButton->foregroundRole(), Qt::black);
	fontColorButton->setPalette(palette);

	palette.setColor(backgroundColorButton->foregroundRole(), 0xFFFFFF);
	backgroundColorButton->setPalette(palette);

	syntaxHCheckBox->setChecked(true);
	capsLockCheckBox->setChecked(true);
	underlineCheckBox->setChecked(true);
	tabbedModecheckBox->setChecked(false);
	fileNameCheckBox->setChecked(true);
	filePathCheckBox->setChecked(false);
	titleCheckBox->setChecked(false);
	editorToolTipsCheckBox->setChecked(true);
	readOnlyModeCheckBox->setChecked(false);
	disableFileChangeMonitorCheckBox->setChecked(false);
	editProp.codeStyle.fontName = "Courier";
	editProp.codeStyle.fontSize = 12;

	editProp.editorProperties.defaultHighlightMode = MODE_AUTO;
	int id = highlightModeComboBox->findData(editProp.editorProperties.defaultHighlightMode);
	highlightModeComboBox->setCurrentIndex(id);

	clearUndocheckBox->setChecked(false);
	clearUnderlinecheckBox->setChecked(true);
	startEmptyCheckBox->setChecked(false);

#ifdef Q_OS_LINUX
	editProp.calcBinary = "kcalc";
#endif

#ifdef Q_OS_WIN32
	editProp.calcBinary = "calc.exe";
#endif

	calcLineEdit->setText(editProp.calcBinary);

	fontLabel->setText(QString(tr("Current font : <b>\"%1\", %2 pt.<\b>")
	                           .arg(editProp.codeStyle.fontName).arg(editProp.codeStyle.fontSize)));
	fontLabel->setFont(QFont(editProp.codeStyle.fontName, editProp.codeStyle.fontSize));

	lstExtensions->clear();
	lstExtensions->addItem("*.nc");
	lstExtensions->addItem("*.cnc");
	//lstExtensions->addItem("*.anc");

	progNumCheckBox->setChecked(true);

	edtSaveExtension->setText("*.nc");
}

void SetupDialog::on_btnAddExtension_clicked()
{
	QString ext = edtExtension->text();
	ext.remove(" ");
	ext.simplified();

	if ((ext == "") || (ext == "*.")) {
		return;
	}

	lstExtensions->addItem(ext);
	edtExtension->setText("*.");
}

void SetupDialog::on_btnDeleteExtension_clicked()
{
	qDeleteAll(lstExtensions->selectedItems());
}

void SetupDialog::on_btnBrowseDirectory_clicked()
{
	QString dir = edtSaveDirectory->text();

	if (dir.isEmpty()) {
		dir = QDir::homePath();
	}

	dir = QFileDialog::getExistingDirectory(
	          this,
	          tr("Select default save directory"),
	          dir);

	if (!dir.isEmpty()) {
		edtSaveDirectory->setText(dir);
	}
}
