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

#include <QApplication>             // for QApplication, qApp
#include <QCheckBox>                // for QCheckBox
#include <QCloseEvent>              // for QCloseEvent
#include <QColor>                   // for QColor
#include <QComboBox>                // for QComboBox
#include <QDateTime>                // for QDateTime
#include <QDir>                     // for QDir, operator|, QDir::NoSymLinks, QDir::Readable, QDir::AllDirs, QDir::F...
#include <QEvent>                   // for QEvent, QEvent::KeyPress
#include <QFile>                    // for QFile
#include <QFileDialog>              // for QFileDialog
#include <QFileInfo>                // for QFileInfo
#include <QFont>                    // for QFont, QFont::Normal
#include <QFrame>                   // for QFrame
#include <QIODevice>                // for QIODevice, QIODevice::ReadOnly
#include <QKeyEvent>                // for QKeyEvent
#include <QPlainTextEdit>           // for QPlainTextEdit
#include <QProgressDialog>          // for QProgressDialog
#include <QPushButton>              // for QPushButton
#include <QRegularExpression>       // for QRegularExpression, QRegularExpression::CaseInsensitiveOption
#include <QRegularExpressionMatch>  // for QRegularExpressionMatch
#include <QSettings>                // for QSettings
#include <QSplitter>                // for QSplitter
#include <QString>                  // for QString, operator+, QCharRef
#include <QStringList>              // for QStringList
#include <QTableWidget>             // for QTableWidget
#include <QTableWidgetItem>         // for QTableWidgetItem
#include <QTextBlock>               // for QTextBlock
#include <QTextCharFormat>          // for QTextCharFormat
#include <QTextCursor>              // for QTextCursor
#include <QTextDocument>            // for QTextDocument, QTextDocument::FindFlags, QTextDocument::FindWholeWords
#include <QTextOption>              // for QTextOption, QTextOption::NoWrap
#include <QTextStream>              // for QTextStream
#include <QToolButton>              // for QToolButton
#include <QVariant>                 // for QVariant
#include <Qt>                       // for operator|, AlignVCenter, KeyboardModifiers, AlignLeft, NoModifier, AlignR...
#include <QtGlobal>                 // for QFlags, qMakeForeachContainer, Q_UNUSED, foreach, qint64

#include <gcoderstyle.h>    // for HighlightColors
#include <utils/medium.h>   // for Medium

#include "findinf.h"
#include "highlighter.h" //  for Highlighter, autoDetectHighligthMode


#define MAXLISTS        20

FindInFiles::FindInFiles(QSplitter* parent): QWidget(parent)
{
	f_parent = parent;
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName("FindInFiles");

	highlighter = nullptr;
	highlight = false;

	connect(browseButton, SIGNAL(clicked()), SLOT(browse()));
	connect(findButton, SIGNAL(clicked()), SLOT(find()));
	connect(hideToolButton, SIGNAL(clicked()), SLOT(hideDlg()));
	//connect(closeToolButton, SIGNAL(clicked()), SLOT(close()));

	createFilesTable();

	textComboBox->installEventFilter(this);

	preview->setReadOnly(true);
	preview->setWordWrapMode(QTextOption::NoWrap);
	preview->setFont(QFont("Courier", 12, QFont::Normal));

	readSettings();
}

void FindInFiles::closeDialog()
{
	setAttribute(Qt::WA_DeleteOnClose);
	close();
}

void FindInFiles::hideDlg()
{
	QList<int> list;
	list = f_parent->sizes();
	int id = f_parent->indexOf(this);

	f_parent->setUpdatesEnabled(false);

	if (hideToolButton->isChecked()) {
		currentHeight = list;
		list[id] = 18;
		list[0] = list[0] + (currentHeight[id] - list[id]);
		splitter->hide();
		frame->hide();
		hideToolButton->setChecked(true);
	} else {
		list = currentHeight;
		splitter->show();
		frame->show();
		hideToolButton->setChecked(false);
	}

	//qApp->processEvents();
	f_parent->setSizes(list);
	f_parent->updateGeometry();
	f_parent->setUpdatesEnabled(true);
}

void FindInFiles::browse()
{
	QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"),
	                    directoryComboBox->currentText());

	if (!directory.isEmpty()) {
		directoryComboBox->addItem(QDir::toNativeSeparators(directory));
		directoryComboBox->setCurrentIndex(directoryComboBox->findText(QDir::toNativeSeparators(
		                                       directory)));
	}
}

void FindInFiles::find()
{
	;
	bool notFound;

	QString text = textComboBox->currentText();

	if (text.isEmpty()) {
		return;
	}

	filesTable->setRowCount(0);
	preview->clear();

	QString fileName = fileComboBox->currentText();
	QString path = QDir(directoryComboBox->currentText()).absolutePath();

	findButton->setEnabled(false);
	QApplication::setOverrideCursor(Qt::BusyCursor);
	qApp->processEvents();

	if (fileName.isEmpty()) {
		fileName = "*";
	}

	QProgressDialog* progressDialog = new QProgressDialog(this);
	progressDialog->setCancelButtonText(tr("&Cancel"));
	progressDialog->setRange(0, 100);
	progressDialog->setWindowTitle(tr("Find Files"));
	//progressDialog->setLabelText(tr("Searching in folder: \"%1\"").arg(QDir(path).absolutePath()));
	qApp->processEvents();

	notFound = findFiles(path, path, true, text, fileName, progressDialog);

	progressDialog->close();

	if (notFound) {
		QTableWidgetItem* fileNameItem = new QTableWidgetItem(tr("No"));
		fileNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		QTableWidgetItem* infoNameItem = new QTableWidgetItem(tr("files"));
		infoNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		QTableWidgetItem* sizeItem = new QTableWidgetItem(tr("found."));
		sizeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		filesTable->insertRow(0);
		filesTable->setItem(0, 0, fileNameItem);
		filesTable->setItem(0, 1, infoNameItem);
		filesTable->setItem(0, 2, sizeItem);
	}

	filesTable->resizeRowsToContents();
	filesTable->resizeColumnsToContents();

	findButton->setEnabled(true);
	QApplication::restoreOverrideCursor();
}

bool FindInFiles::findFiles(const QString startDir, QString mainDir, bool notFound,
                            const QString findText, QString fileFilter, QProgressDialog* progressDialog)
{
	int pos;
	QRegularExpression regex;
	QString comment_tx;
	qint64 size;
	bool textFounded, word;
	QString line;
	QStringList files;
	QStringList dirs;
	bool inComment = false;
	int commentPos;

	if (progressDialog->wasCanceled()) {
		return notFound;
	}

	regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	regex.setPattern("\\([^\\n\\r]*\\)|;[^\\n\\r]*");

	pos = 0;

	dirs.clear();

	QDir directory = QDir(startDir);
	//qDebug() << startDir << directory.absolutePath();

	if (subFoldersCheckBox->isChecked()) {
		dirs.append(directory.entryList(QStringList("*"),
		                                QDir::AllDirs | QDir::NoSymLinks | QDir::Readable | QDir::NoDotAndDotDot));
	}

	foreach (const QString& dirName, dirs) {
		notFound = findFiles(directory.absolutePath() + "/" + dirName, mainDir, notFound, findText,
		                     fileFilter, progressDialog);
	}

	files = directory.entryList(QStringList(fileFilter),
	                            QDir::Files | QDir::NoSymLinks | QDir::Readable);

	progressDialog->setLabelText(tr("Searching in folder: \"%1\"").arg(QDir::toNativeSeparators(
	                                 directory.absolutePath())));

	for (int i = 0; i < files.size(); ++i) {
		progressDialog->setRange(0, files.size());
		progressDialog->setValue(i);
		qApp->processEvents();

		if (progressDialog->wasCanceled()) {
			break;
		}

		QFile file(directory.absoluteFilePath(files[i]));

		if (file.open(QIODevice::ReadOnly)) {
			QTextStream in(&file);

			textFounded = false;
			word = false;
			line = in.readAll();

			if (findText == "*") { //files containing anything
				textFounded = true;
			} else {
				commentPos = -1;
				pos = 0;

				do {
					pos = line.indexOf(findText, pos, Qt::CaseInsensitive);
					textFounded = (pos >= 0);

					if (textFounded && (commentStyle1CheckBox->isChecked() || commentStyle2CheckBox->isChecked())) {
						int lineStartPos = line.lastIndexOf('\n', pos);

						if (lineStartPos < 0) {
							lineStartPos = pos;
						}

						if (commentStyle1CheckBox->isChecked() && commentStyle2CheckBox->isChecked()) {
							commentPos  = line.indexOf('(', lineStartPos);

							if (commentPos > pos) {
								commentPos = -1;
							}

							if (commentPos < 0) {
								commentPos  = line.indexOf(';', lineStartPos);
							}
						} else {
							if (commentStyle2CheckBox->isChecked()) {
								commentPos  = line.indexOf('(', lineStartPos);
							}

							if (commentStyle1CheckBox->isChecked()) {
								commentPos  = line.indexOf(';', lineStartPos);
							}
						}

						if (commentPos < 0) {
							commentPos = pos + 1;
						}

						inComment = (commentPos < pos);

					} else {
						inComment = false;
					}

					pos++;

				} while (inComment);
			}

			pos--;

			if (textFounded && wholeWordsCheckBox->isChecked()) {
				if (pos > 0)
					if (line[pos - 1].isLetterOrNumber()) {
						word = true;
					}

				pos = pos + findText.size();

				if (pos < line.size())
					if (line[pos].isLetterOrNumber()) {
						word = true;
					}
			}

			size = file.size();

			if ((textFounded && (!wholeWordsCheckBox->isChecked())) ||
			        (textFounded && (wholeWordsCheckBox->isChecked() && !word))) {
				notFound = false;
				textFounded = false;
				word = false;

				comment_tx.clear();
				auto match = regex.match(line);

				while (match.hasMatch()) {
					comment_tx = match.captured();

					if (!comment_tx.contains(";$")) {
						comment_tx.remove('(');
						comment_tx.remove(')');
						comment_tx.remove(';');
						break;
					}

					match = regex.match(line, match.capturedEnd());
				}

				QString subDir = startDir;
				subDir.remove(mainDir);

				if (!subDir.isEmpty()) {
					subDir += "/";
				}

				QTableWidgetItem* fileNameItem = new QTableWidgetItem(QDir::toNativeSeparators(
				            subDir) + files[i]);
				fileNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

				QTableWidgetItem* infoNameItem = new QTableWidgetItem(comment_tx);
				infoNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

				QTableWidgetItem* sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
				sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

				//QTableWidgetItem *dateItem = new QTableWidgetItem(QFileInfo(file).lastModified().toString(Qt::SystemLocaleShortDate));
				//dateItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				QTableWidgetItem* dateItem = new QTableWidgetItem();
				dateItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
				dateItem->setData(Qt::DisplayRole, QFileInfo(file).lastModified());


				int row = filesTable->rowCount();
				filesTable->insertRow(row);
				filesTable->setItem(row, 0, fileNameItem);
				filesTable->setItem(row, 1, infoNameItem);
				filesTable->setItem(row, 2, sizeItem);
				filesTable->setItem(row, 3, dateItem);
			}

			file.close();
		}
	}

	return notFound;
}

void FindInFiles::createFilesTable()
{
	QStringList labels;
	labels << tr("File Name") << tr("Info") << tr("Size") << tr("Modified");
	filesTable->setHorizontalHeaderLabels(labels);
	connect(filesTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(filesTableClicked(int,
	        int)));
	connect(filesTable, SIGNAL(cellClicked(int, int)), this, SLOT(filePreview(int, int)));
}

void FindInFiles::closeEvent(QCloseEvent* event)
{
	QStringList list;
	QString item;

	QSettings& settings = *Medium::instance().settings();
	settings.beginGroup("FindFileDialog");

	settings.setValue("WholeWords", wholeWordsCheckBox->isChecked());
	settings.setValue("SubFolders", subFoldersCheckBox->isChecked());
	settings.setValue("CommentStyle1", commentStyle1CheckBox->isChecked());
	settings.setValue("CommentStyle2", commentStyle2CheckBox->isChecked());

	list.clear();
	list.append(directoryComboBox->currentText());

	for (int i = 0; i <= directoryComboBox->count(); i++) {
		item = directoryComboBox->itemText(i);

		if (!item.isEmpty())
			if (!list.contains(item)) {
				list.append(item);
			}
	}

	while (list.size() > MAXLISTS) {
		list.removeLast();
	}

	settings.setValue("Dirs", list);
	settings.setValue("SelectedDir", directoryComboBox->currentText());

	list.clear();
	list.append(fileComboBox->currentText());

	for (int i = 0; i <= fileComboBox->count(); i++) {
		item = fileComboBox->itemText(i);

		if (!item.isEmpty())
			if (!list.contains(item)) {
				list.append(item);
			}
	}

	while (list.size() > MAXLISTS) {
		list.removeLast();
	}

	settings.setValue("Filters", list);
	settings.setValue("SelectedFilter", fileComboBox->currentText());

	list.clear();
	list.append(textComboBox->currentText());

	for (int i = 0; i <= textComboBox->count(); i++) {
		item = textComboBox->itemText(i);

		if (!item.isEmpty())
			if (!list.contains(item, Qt::CaseInsensitive)) {
				list.append(item);
			}
	}

	while (list.size() > MAXLISTS) {
		list.removeLast();
	}

	settings.setValue("Texts", list);
	settings.setValue("SelectedText", textComboBox->currentText());

	settings.endGroup();

	event->accept();
}

void FindInFiles::readSettings()
{
	QStringList list;
	QString item;
	int i;

	textComboBox->clear();
	directoryComboBox->clear();
	fileComboBox->clear();

	QSettings& settings = *Medium::instance().settings();

	intCapsLock = settings.value("IntCapsLock", true).toBool();

	list = settings.value("Extensions", "").toStringList();

	settings.beginGroup("FindFileDialog");

	list.append(settings.value("Filters", "*.nc").toStringList());
	list.removeDuplicates();
	list.sort();
	fileComboBox->addItems(list);
	item = settings.value("SelectedFilter", QString("*.nc")).toString();
	i = fileComboBox->findText(item);
	fileComboBox->setCurrentIndex(i);

	wholeWordsCheckBox->setChecked(settings.value("WholeWords", false).toBool());
	subFoldersCheckBox->setChecked(settings.value("SubFolders", false).toBool());
	commentStyle1CheckBox->setChecked(settings.value("CommentStyle1", false).toBool());
	commentStyle2CheckBox->setChecked(settings.value("CommentStyle2", false).toBool());

	list = settings.value("Dirs", QStringList(QDir::homePath())).toStringList();
	list.removeDuplicates();
	list.sort();
	directoryComboBox->addItems(list);
	item = settings.value("SelectedDir", QDir::toNativeSeparators(QDir::homePath())).toString();
	i = directoryComboBox->findText(item);
	directoryComboBox->setCurrentIndex(i);

	list = settings.value("Texts", QStringList()).toStringList();
	list.removeDuplicates();
	list.sort();
	textComboBox->addItems(list);
	item = settings.value("SelectedText", QString("*")).toString();
	i = textComboBox->findText(item, Qt::MatchExactly);
	textComboBox->setCurrentIndex(i);

	settings.endGroup();
}

void FindInFiles::filesTableClicked(int x, int y)
{
	Q_UNUSED(y);

	QTableWidgetItem* item = filesTable->item(x, 0);

	QString dir = directoryComboBox->currentText();

	if (!dir.endsWith("/")) {
		dir = dir + "/";
	}

	emit fileClicked(dir + item->text());
}

void FindInFiles::filePreview(int x, int y)
{
	Q_UNUSED(y);

	QTableWidgetItem* item = filesTable->item(x, 0);

	QApplication::setOverrideCursor(Qt::BusyCursor);

	QTextCursor cursor = preview->textCursor();

	if (!cursor.isNull()) {
		QTextCharFormat format = cursor.charFormat();
		format.setFontPointSize(12);
		cursor.mergeCharFormat(format);
		preview->setTextCursor(cursor);
	}

	QString dir = directoryComboBox->currentText();

	if (!dir.endsWith("/")) {
		dir = dir + "/";
	}

	QFile file(dir + item->text());

	if (file.open(QIODevice::ReadOnly)) {
		QTextStream in(&file);
		preview->setPlainText(in.readAll());
		file.close();

		if (highlight) {
			if (highlighter == nullptr) {
				highlighter = new Highlighter(preview->document());
			}

			if (highlighter != nullptr) {
				int mode = autoDetectHighligthMode(preview->toPlainText().toUpper());
				highlighter->setHighlightMode(mode);
				highlighter->setHColors(highlighterColors, preview->font());
				highlighter->rehighlight();
			}
		}

		qApp->processEvents();

		if ((!textComboBox->currentText().isEmpty()) && !(textComboBox->currentText() == "*")) {
			highlightFindText(textComboBox->currentText(),
			                  (wholeWordsCheckBox->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags()));

			if (findText(textComboBox->currentText(),
			             (wholeWordsCheckBox->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags())),
			        (commentStyle1CheckBox->isChecked() || commentStyle2CheckBox->isChecked())) {
				preview->centerCursor();
			}
		}
	}

	QApplication::restoreOverrideCursor();
}

bool FindInFiles::findText(const QString& exp, QTextDocument::FindFlags options,
                           bool ignoreComments)
{
	bool found = false;
	QTextCursor cursor;
	bool inComment = false;
	QString cur_line;
	int cur_line_column;
	int commentPos;

	preview->setUpdatesEnabled(false);

	commentPos = -1;

	do {
		found = preview->find(exp, options);

		if (found && ignoreComments) {
			cursor = preview->textCursor();
			cur_line = cursor.block().text();

			cur_line_column = cursor.columnNumber();

			if (commentStyle1CheckBox->isChecked() && commentStyle2CheckBox->isChecked()) {
				commentPos  = cur_line.indexOf(';', 0);

				if (commentPos < 0) {
					commentPos  = cur_line.indexOf('(', 0);
				}
			} else {
				if (commentStyle2CheckBox->isChecked()) {
					commentPos  = cur_line.indexOf('(', 0);
				}

				if (commentStyle1CheckBox->isChecked()) {
					commentPos  = cur_line.indexOf(';', 0);
				}
			}

			if (commentPos < 0) {
				commentPos = cur_line_column + 1;
			}

			inComment = (commentPos < cur_line_column);
		} else {
			inComment = false;
		}

	} while (inComment);

	if (!found) {
		cursor.clearSelection();
		preview->setTextCursor(cursor);
	}

	preview->setUpdatesEnabled(true);
	return found;
}

void FindInFiles::setHighlightColors(const HighlightColors colors)
{
	highlight = true;
	highlighterColors = colors;
}

void FindInFiles::setDir(const QString dir)
{
	directoryComboBox->addItem(QDir::toNativeSeparators(dir));
	directoryComboBox->setCurrentIndex(directoryComboBox->findText(QDir::toNativeSeparators(dir)));
}

void FindInFiles::highlightFindText(QString searchString, QTextDocument::FindFlags options)
{
	findTextExtraSelections.clear();
	QColor lineColor = QColor(Qt::yellow).lighter(155);
	selection.format.setBackground(lineColor);

	QTextDocument* doc = preview->document();
	QTextCursor cursor = preview->textCursor();
	cursor.setPosition(0);

	do {
		cursor = doc->find(searchString, cursor, options);

		if (!cursor.isNull()) {
			selection.cursor = cursor;

			QTextCharFormat format = cursor.charFormat();
			format.setFontPointSize(16);
			qApp->processEvents();
			cursor.mergeCharFormat(format);

			findTextExtraSelections.append(selection);

		}
	} while (!cursor.isNull());

	preview->setExtraSelections(findTextExtraSelections);
}

bool FindInFiles::eventFilter(QObject* obj, QEvent* ev)
{
	if (obj == textComboBox) {
		if (ev->type() == QEvent::KeyPress) {
			QKeyEvent* k = (QKeyEvent*) ev;

			if ((k->key() == Qt::Key_Return) || (k->key() == Qt::Key_Enter)) {
				find();
				return false;
			}

			if (k->key() == Qt::Key_Comma) { //Keypad comma should always prints period
				if ((k->modifiers() == Qt::KeypadModifier)
				        || (k->nativeScanCode() == 0x53)) { // !!! Qt::KeypadModifier - Not working for keypad comma !!!
					QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier,
					                        ".", false, 1));
					return true;
				}
			}

			if (intCapsLock) {
				if (k->text()[0].isLower() && (k->modifiers() == Qt::NoModifier)) {
					QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::NoModifier,
					                        k->text().toUpper(), false, 1));
					return true;
				}

				if (k->text()[0].isUpper() && (k->modifiers() == Qt::ShiftModifier)) {
					QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::ShiftModifier,
					                        k->text().toLower(), false, 1));
					return true;
				}
			}
		}

		return false;
	} else {
		// pass the event on to the parent class
		return eventFilter(obj, ev);
	}
}
