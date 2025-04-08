/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
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

#include <QAbstractItemView>    // for QAbstractItemView
#include <QAction>              // for QAction
#include <QApplication>         // for QApplication
#include <QBrush>               // for QBrush
#include <QColor>               // for QColor
#include <QCursor>              // for QCursor
#include <QIcon>                // for QIcon
#include <QKeySequence>         // for QKeySequence
#include <QList>                // for QList
#include <QMenu>                // for QMenu
#include <QPlainTextEdit>       // for QPlainTextEdit
#include <QPoint>               // for QPoint
#include <QPushButton>          // for QPushButton
#include <QRect>                // for QRect
#include <QRegularExpression>   // for QRegularExpression
#include <QSettings>            // for QSettings
#include <QSize>                // for QSize
#include <QString>              // for QString, operator==
#include <QStringList>          // for QStringList
#include <QTableWidget>         // for QTableWidget
#include <QTableWidgetItem>     // for QTableWidgetItem
#include <QTextCharFormat>      // for QTextCharFormat
#include <QTextCursor>          // for QTextCursor
#include <QTextDocument>        // for QTextDocument
#include <QTextEdit>            // for QTextEdit
#include <QVariant>             // for QVariant
#include <Qt>                   // for operator|, AlignmentFlag, CheckState, ItemFlag, ContextMenuPolicy, GlobalColor
#include <QtGlobal>             // for QFlags, QTypeInfo<>::isLarge, QTypeInfo<>::isStatic, QTypeInfo<>::isComplex

#include "cleanupdialog.h"
#include "cleanupoptions.h" // for CleanUpOptions


#define CFG_SECTION  "CleanUpDialog"
#define CFG_KEY_POS  "Position"
#define CFG_KEY_SIZE "Size"


CleanUpDialog::CleanUpDialog(QWidget* parent, QSettings* settings) :
	QDialog(parent),
	Ui::CleanUpDialog()
{
	setupUi(this);

	mSettings = settings;

	QAction* deleteRowAct = new QAction(QIcon(":/images/removeemptylines.png"), tr("Delete &row"),
	                                    this);
	deleteRowAct->setShortcut(QKeySequence::Delete);
	deleteRowAct->setStatusTip(tr("Delete current row"));
	connect(deleteRowAct, SIGNAL(triggered()), this, SLOT(removeRow()));
	tableWidget->addAction(deleteRowAct);

	contextMenu = new QMenu(this);
	contextMenu->addAction(deleteRowAct);

	tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	newRow();

	connect(tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(onCellCliced(int, int)));
	connect(tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(cellChangedSlot(int, int)));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(this, SIGNAL(finished(int)), SLOT(onFinished(int)));
	connect(tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this,
	        SLOT(contextMenuReq(const QPoint&)));
	tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

CleanUpDialog::~CleanUpDialog()
{
}

void CleanUpDialog::setText(QString text)
{
	textEdit->setPlainText(text);
}

void CleanUpDialog::cellChangedSlot(int row, int col)
{
	if (col == 1) {
		highlightText(row, col);
	}
}

void CleanUpDialog::newRow()
{
	int row = tableWidget->rowCount();

	if (row > 0) {
		row--;

		if (tableWidget->item(row, 0) != nullptr)
			if (tableWidget->item(row, 0)->text().isNull()
			        || tableWidget->item(row, 0)->text().isEmpty()) {
				return;
			}

		if (tableWidget->item(row, 1) != nullptr)
			if (tableWidget->item(row, 1)->text().isNull()
			        || tableWidget->item(row, 1)->text().isEmpty()) {
				return;
			}

		row++;
	}

	QTableWidgetItem* valueItem = new QTableWidgetItem("");
	valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	QTableWidgetItem* commentItem = new QTableWidgetItem("");
	commentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	QTableWidgetItem* enableItem = new QTableWidgetItem("");
	enableItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	enableItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	enableItem->setCheckState(Qt::Unchecked);

	tableWidget->insertRow(row);
	tableWidget->setItem(row, 0, commentItem);
	tableWidget->setItem(row, 1, valueItem);
	tableWidget->setItem(row, 2, enableItem);
}

void CleanUpDialog::onCellCliced(int row, int col)
{
	highlightText(row, col);
	newRow();
}

void CleanUpDialog::highlightText(int row, int col)
{
	Q_UNUSED(col);
	QTableWidgetItem* item = tableWidget->item(row, 1);

	if (item != nullptr) {
		if (!item->text().isEmpty()) {
			if (item->text() == "$") { //causes endless loop
				return;
			}

			highlightFindText(QRegularExpression(item->text()));
		}
	}
}

void CleanUpDialog::highlightFindText(QRegularExpression regex)
{
	QList<QTextEdit::ExtraSelection> findTextExtraSelections;
	QTextEdit::ExtraSelection selection;
	QTextCursor firstFoundCursor;

	QColor lineColor = QColor(Qt::red).lighter(155);
	selection.format.setBackground(lineColor);

	QTextDocument* doc = textEdit->document();
	QTextCursor cursor = textEdit->textCursor();

	cursor.setPosition(0);

	firstFoundCursor = cursor;

	do {
		cursor = doc->find(regex, cursor);

		if (!cursor.isNull()) {
			if (cursor.selectedText().length() == 0) { //cursor not null but nothing found
				break;
			}

			if (firstFoundCursor.position() == 0) {
				firstFoundCursor = cursor;
			}

			selection.cursor = cursor;
			findTextExtraSelections.append(selection);
		}

		QApplication::processEvents();

	} while (!cursor.isNull() && !cursor.atEnd());

	firstFoundCursor.movePosition(QTextCursor::StartOfBlock);
	textEdit->setExtraSelections(findTextExtraSelections);
	textEdit->setTextCursor(firstFoundCursor);
	textEdit->centerCursor();
}

void CleanUpDialog::contextMenuReq(const QPoint& pos)
{
	Q_UNUSED(pos);
	contextMenu->popup(QCursor::pos());
}

void CleanUpDialog::removeRow()
{
	int row = tableWidget->currentRow();
	tableWidget->removeRow(row);
}

void CleanUpDialog::setOptions(const CleanUpOptions& options)
{
	while (tableWidget->rowCount() > 0) {
		tableWidget->removeRow(0);
	}

	for (int i = 0; i < options.expressions.count(); i++) {
		QTableWidgetItem* valueItem = new QTableWidgetItem(options.expressions[i]);
		valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		QTableWidgetItem* commentItem = new QTableWidgetItem(options.comments.value(i));
		commentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		QTableWidgetItem* enableItem = new QTableWidgetItem();
		enableItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		enableItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		bool checked = options.selected.contains(options.expressions[i]);
		enableItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

		tableWidget->insertRow(i);
		tableWidget->setItem(i, 0, commentItem);
		tableWidget->setItem(i, 1, valueItem);
		tableWidget->setItem(i, 2, enableItem);
	}

	newRow();

	tableWidget->resizeRowsToContents();
	tableWidget->resizeColumnsToContents();
}

CleanUpOptions CleanUpDialog::options()
{
	CleanUpOptions options;

	for (int i = 0; i < tableWidget->rowCount(); i++) {
		if (tableWidget->item(i, 1)->text().isEmpty()) {
			continue;
		}

		options.expressions.append(tableWidget->item(i, 1)->text());
		options.comments.append(tableWidget->item(i, 0)->text());

		if (tableWidget->item(i, 2)->checkState() == Qt::Checked) {
			options.selected.append(tableWidget->item(i, 1)->text());
		}
	}

	return options;
}

void CleanUpDialog::loadSettings(const CleanUpOptions& defaultOptions)
{
	if (mSettings.isNull()) {
		return;
	}

	mSettings->beginGroup(CFG_SECTION);

	if (mSettings->contains(CFG_KEY_POS)) {
		QPoint pos = mSettings->value(CFG_KEY_POS, geometry().topLeft()).toPoint();
		QSize size = mSettings->value(CFG_KEY_SIZE, geometry().size()).toSize();
		setGeometry(QRect(pos, size));
	}

	CleanUpOptions opt;
	opt.load(mSettings, defaultOptions);

	mSettings->endGroup();

	setOptions(opt);
}

void CleanUpDialog::saveSettings(bool saveOptions)
{
	if (mSettings.isNull()) {
		return;
	}

	mSettings->beginGroup(CFG_SECTION);

	mSettings->setValue(CFG_KEY_POS, geometry().topLeft());
	mSettings->setValue(CFG_KEY_SIZE, geometry().size());

	if (saveOptions) {
		options().save(mSettings);
	}

	mSettings->endGroup();
}

void CleanUpDialog::onFinished(int result)
{
	saveSettings(result == QDialog::Accepted);
}
