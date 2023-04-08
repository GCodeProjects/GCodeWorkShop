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

#include <QAbstractItemView>    // for QAbstractItemView, QAbstractItemView::NoSelection
#include <QAction>              // for QAction
#include <QApplication>         // for QApplication
#include <QColor>               // for QColor
#include <QCursor>              // for QCursor
#include <QIcon>                // for QIcon
#include <QKeySequence>         // for QKeySequence, QKeySequence::Delete
#include <QList>                // for QList
#include <QMenu>                // for QMenu
#include <QPlainTextEdit>       // for QPlainTextEdit
#include <QPushButton>          // for QPushButton
#include <QRegularExpression>   // for QRegularExpression
#include <QSettings>            // for QSettings
#include <QString>              // for QString, operator==
#include <QStringList>          // for QStringList
#include <QTableWidget>         // for QTableWidget
#include <QTableWidgetItem>     // for QTableWidgetItem
#include <QTextCharFormat>      // for QTextCharFormat
#include <QTextCursor>          // for QTextCursor, QTextCursor::StartOfBlock
#include <QTextDocument>        // for QTextDocument
#include <QVariant>             // for QVariant
#include <Qt>                   // for operator|, AlignLeft, AlignVCenter, Checked, ItemIsEnabled, ItemIsUserCheckable
#include <QtGlobal>             // for QFlags, Q_UNUSED, foreach

#include <utils/medium.h>   // for Medium

#include "cleanupdialog.h"


#define EXAMPLE_EXP        "('\\()[\\w,.;:/*+\\\\! $%^&-]{0,}(\\))$" << "(\\()[\\w,.;:/*+\\\\! $%^&-]{0,}(\\))" << "[\n]{2,}"

#define EXAMPLE_EXP_COMM   "Lines with: '(comment)" << "Any: (comment)" << "Empty lines:"

CleanUpDialog::CleanUpDialog(QWidget *parent) :
    QDialog(parent),
    Ui::CleanUpDialog()
{
    setupUi(this);

    //    QAction *copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy row"), this);
    //    copyAct->setShortcut(QKeySequence::Copy);
    //    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
    //                              "clipboard"));
    //    connect(copyAct, SIGNAL(triggered()), this, SLOT(copySelection()));
    //    tableWidget->addAction(copyAct);

    //    QAction *pasteAct = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste row"), this);
    //    pasteAct->setShortcut(QKeySequence::Paste);
    //    pasteAct->setStatusTip(tr("Paste the clipboard contents"));
    //    connect(pasteAct, SIGNAL(triggered()), this, SLOT(copySelection()));
    //    tableWidget->addAction(pasteAct);

    //    QAction *cutAct = new QAction(QIcon(":/images/editcut.png"), tr("&Cut row"), this);
    //    cutAct->setShortcut(QKeySequence::Cut);
    //    cutAct->setStatusTip(tr("Cut's the current selection's contents to the "
    //                              "clipboard"));
    //    connect(cutAct, SIGNAL(triggered()), this, SLOT(copySelection()));
    //    tableWidget->addAction(cutAct);

    QAction *deleteRowAct = new QAction(QIcon(":/images/removeemptylines.png"), tr("Delete &row"),
                                        this);
    deleteRowAct->setShortcut(QKeySequence::Delete);
    deleteRowAct->setStatusTip(tr("Delete current row"));
    connect(deleteRowAct, SIGNAL(triggered()), this, SLOT(removeRow()));
    tableWidget->addAction(deleteRowAct);

    contextMenu = new QMenu(this);

    //    contextMenu->addAction(copyAct);
    //    contextMenu->addAction(cutAct);
    //    contextMenu->addAction(pasteAct);
    //    contextMenu->addSeparator();
    contextMenu->addAction(deleteRowAct);

    tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    expressions.clear();
    expressionsComment.clear();
    selectedExpressions.clear();

    QSettings &settings = *Medium::instance().settings();

    settings.beginGroup("CleanUpDialog");

    expressions = settings.value("Expressions", (QStringList() << EXAMPLE_EXP)).toStringList();
    expressionsComment = settings.value("ExpressionsComment",
                                        (QStringList() << EXAMPLE_EXP_COMM)).toStringList();

    settings.endGroup();

    int i = 0;
    QString commTx;

    foreach (const QString expTx, expressions) {
        if (i <= expressionsComment.count()) {
            commTx = expressionsComment[i];
        } else {
            commTx = "";
        }

        QTableWidgetItem *valueItem = new QTableWidgetItem(expTx);
        valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QTableWidgetItem *commentItem = new QTableWidgetItem(commTx);
        commentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QTableWidgetItem *enableItem = new QTableWidgetItem();
        enableItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        enableItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        enableItem->setCheckState(Qt::Unchecked);

        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, commentItem);
        tableWidget->setItem(row, 1, valueItem);
        tableWidget->setItem(row, 2, enableItem);

        i++;
    }

    newRow();

    tableWidget->resizeRowsToContents();
    tableWidget->resizeColumnsToContents();

    connect(tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(highlightText(int, int)));
    connect(tableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(cellChangedSlot(int, int)));
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(tableWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(contextMenuReq(const QPoint &)));
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
    }

    row++;

    QTableWidgetItem *valueItem = new QTableWidgetItem("");
    valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QTableWidgetItem *commentItem = new QTableWidgetItem("");
    commentItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QTableWidgetItem *enableItem = new QTableWidgetItem("");
    enableItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    enableItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    enableItem->setCheckState(Qt::Unchecked);

    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, commentItem);
    tableWidget->setItem(row, 1, valueItem);
    tableWidget->setItem(row, 2, enableItem);
}

void CleanUpDialog::highlightText(int row, int col)
{
    Q_UNUSED(col);
    QTableWidgetItem *item = tableWidget->item(row, 1);

    if (item != nullptr) {
        if (!item->text().isEmpty()) {
            if (item->text() == "$") { //causes endless loop
                return;
            }

            highlightFindText(QRegularExpression(item->text()));
        }
    }

    newRow();
}

void CleanUpDialog::closeButtonClicked()
{
    reject();
}

int CleanUpDialog::exec(QStringList selList, QString text)
{
    textEdit->setPlainText(text);

    foreach (QString exp, selList) {
        for (int i = 0; i < tableWidget->rowCount(); i++) {
            if (exp.isEmpty()) {
                continue;
            }

            if (tableWidget->item(i, 1)->text() == exp) {
                tableWidget->item(i, 2)->setCheckState(Qt::Checked);
            }
        }
    }

    return QDialog::exec();
}

void CleanUpDialog::okButtonClicked()
{
    expressions.clear();
    expressionsComment.clear();

    for (int i = 0; i < (tableWidget->rowCount()); i++) {
        if (!tableWidget->item(i, 0)->text().isEmpty()
                && !tableWidget->item(i, 1)->text().isEmpty()) {
            expressions.append(tableWidget->item(i, 1)->text());
            expressionsComment.append(tableWidget->item(i, 0)->text());
        }
    }

    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("CleanUpDialog");

    settings.setValue("Expressions", expressions);
    settings.setValue("ExpressionsComment", expressionsComment);

    settings.endGroup();

    accept();
}

QStringList CleanUpDialog::getSelectedExpressions()
{
    selectedExpressions.clear();

    for (int i = 0; i < tableWidget->rowCount(); i++) {
        if (tableWidget->item(i, 2)->checkState() == Qt::Checked) {
            selectedExpressions.append(tableWidget->item(i, 1)->text());
        }
    }

    return selectedExpressions;
}

void CleanUpDialog::highlightFindText(QRegularExpression regex)
{
    QTextCursor firstFoundCursor;

    findTextExtraSelections.clear();
    QColor lineColor = QColor(Qt::red).lighter(155);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = textEdit->document();
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

void CleanUpDialog::contextMenuReq(const QPoint &pos)
{
    Q_UNUSED(pos);
    contextMenu->popup(QCursor::pos());
}

void CleanUpDialog::removeRow()
{
    int row = tableWidget->currentRow();
    tableWidget->removeRow(row);
}
