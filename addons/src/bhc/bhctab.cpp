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

#include <QAbstractItemView>    // for QAbstractItemView
#include <QAction>              // for QAction
#include <QActionGroup>         // for QActionGroup
#include <QApplication>         // for QApplication
#include <QChar>                // for operator==, QChar
#include <QCheckBox>            // for QCheckBox
#include <QClipboard>           // for QClipboard
#include <QCursor>              // for QCursor
#include <QDoubleValidator>     // for QDoubleValidator
#include <QEvent>               // for QEvent
#include <QIcon>                // for QIcon
#include <QIntValidator>        // for QIntValidator
#include <QKeyEvent>            // for QKeyEvent
#include <QKeySequence>         // for QKeySequence
#include <QLineEdit>            // for QLineEdit
#include <QLocale>              // for QLocale
#include <QMenu>                // for QMenu
#include <QPalette>             // for QPalette
#include <QSpinBox>             // for QSpinBox
#include <QString>              // for QString, operator+
#include <QStringList>          // for QStringList
#include <QTableWidget>         // for QTableWidget
#include <QTableWidgetItem>     // for QTableWidgetItem
#include <Qt>                   // for Key, KeyboardModifier, ContextMenuPolicy, GlobalColor
#include <QtGlobal>             // for Q_UNUSED

class QValidator;

#include <utils/removezeros.h>  // Utils::removeZeros()

#include "bhctab.h"
#include "bhctaboptions.h"  // for BHCTabOptions


BHCTab::BHCTab(QWidget* parent) : QWidget(parent)
{
	setupUi(this);

	QValidator* xCenterInputValid = new QDoubleValidator(-9999, 9999, 3, this);
	xCenterInput->setValidator(xCenterInputValid);
	xCenterInput->installEventFilter(this);
	QValidator* yCenterInputValid = new QDoubleValidator(-9999, 9999, 3, this);
	yCenterInput->setValidator(yCenterInputValid);
	yCenterInput->installEventFilter(this);
	QValidator* diaInputValid = new QDoubleValidator(1, 9999, 3, this);
	diaInput->setValidator(diaInputValid);
	diaInput->installEventFilter(this);
	QValidator* holesInputValid = new QIntValidator(1, 360, this);
	holesInput->setValidator(holesInputValid);
	QValidator* angleStartInputValid = new QDoubleValidator(0, 360, 3, this);
	angleStartInput->setValidator(angleStartInputValid);
	angleStartInput->installEventFilter(this);
	QValidator* angleBeetwenInputValid = new QDoubleValidator(0, 360, 3, this);
	angleBeetwenInput->setValidator(angleBeetwenInputValid);
	angleBeetwenInput->installEventFilter(this);

	connect(roateInput, SIGNAL(valueChanged(int)), SLOT(inputChk()));
	connect(mirrorX, SIGNAL(toggled(bool)), SLOT(inputChk()));
	connect(mirrorY, SIGNAL(toggled(bool)), SLOT(inputChk()));

	contextMenu = new QMenu(this);
	commentActGroup = new QActionGroup(this);

	QAction* copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
	copyAct->setShortcut(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
	                         "clipboard"));
	connect(copyAct, SIGNAL(triggered()), this, SLOT(copySelection()));
	resultTable->addAction(copyAct);

	QAction* selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"),
	                                 this);
	selAllAct->setShortcut(QKeySequence::SelectAll);
	selAllAct->setStatusTip(tr("Select all"));
	connect(selAllAct, SIGNAL(triggered()), this, SLOT(sellAll()));
	resultTable->addAction(selAllAct);

	contextMenu->addAction(copyAct);
	contextMenu->addSeparator();
	contextMenu->addAction(selAllAct);
	contextMenu->addSeparator();
	addCommentsId = contextMenu->addAction(tr("Add ; comments"));
	addCommentsId->setCheckable(true);
	addCommentsId->setActionGroup(commentActGroup);
	addCommentsParaId = contextMenu->addAction(tr("Add () comments"));
	addCommentsParaId->setCheckable(true);
	addCommentsParaId->setActionGroup(commentActGroup);

	resultTable->setContextMenuPolicy(Qt::CustomContextMenu);

	resultTable->setHorizontalHeaderLabels(QStringList() << "X" << "Y");

	connect(resultTable, SIGNAL(customContextMenuRequested(const QPoint&)), this,
	        SLOT(contextMenuReq(const QPoint&)));
	resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	connect(xCenterInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
	connect(yCenterInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
	connect(diaInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
	connect(holesInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
	connect(angleStartInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
	connect(angleBeetwenInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
}

BHCTab::~BHCTab()
{
}

bool BHCTab::eventFilter(QObject* obj, QEvent* ev)
{
	if (!qobject_cast<QLineEdit*>(obj)) {
		// pass the event on to the parent class
		return QWidget::eventFilter(obj, ev);
	}

	if (ev->type() == QEvent::KeyPress) {
		return false;
	}

	QKeyEvent* k = (QKeyEvent*) ev;

	if (QLocale().decimalPoint() == '.' && k->key() == Qt::Key_Comma) {
		QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier,
		                        ".", false, 1));
		return true;
	}

	if (QLocale().decimalPoint() == ',' && k->key() == Qt::Key_Period) {
		QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier, ",",
		                        false, 1));
		return true;
	}

	return false;
}

void BHCTab::inputChanged()
{
	QPalette palette;

	palette.setColor(xCenterInput->foregroundRole(), Qt::red);

	if (xCenterInput->hasAcceptableInput()) {
		xCenterInput->setPalette(QPalette());
	} else {
		xCenterInput->setPalette(palette);
	}

	if (yCenterInput->hasAcceptableInput()) {
		yCenterInput->setPalette(QPalette());
	} else {
		yCenterInput->setPalette(palette);
	}

	if (diaInput->hasAcceptableInput()) {
		diaInput->setPalette(QPalette());
	} else {
		diaInput->setPalette(palette);
	}

	if (holesInput->hasAcceptableInput()) {
		holesInput->setPalette(QPalette());
	} else {
		holesInput->setPalette(palette);
	}

	if (angleStartInput->hasAcceptableInput()) {
		angleStartInput->setPalette(QPalette());
	} else {
		angleStartInput->setPalette(palette);
	}

	if (angleBeetwenInput->hasAcceptableInput()) {
		angleBeetwenInput->setPalette(QPalette());
	} else {
		angleBeetwenInput->setPalette(palette);
	}
}

void BHCTab::contextMenuReq(const QPoint& pos)
{
	Q_UNUSED(pos);
	contextMenu->popup(QCursor::pos());
}

void BHCTab::sellAll()
{
	resultTable->selectAll();
}

void BHCTab::copySelection()
{
	int i;
	QString selText, tmp;
	QTableWidgetItem* it;

	selText = "";

	for (i = 0; i < resultTable->rowCount(); i++) {
		if (resultTable->item(i, 0)->isSelected() || resultTable->item(i, 1)->isSelected()) {
			it = resultTable->item(i, 0);
			selText += "X" + it->text();
			it = resultTable->item(i, 1);
			selText += " Y" + it->text();

			if (addCommentsParaId->isChecked()) {
				it = resultTable->verticalHeaderItem(i);
				tmp = it->text();
				tmp.remove(")");
				tmp.replace("(", "- ");
				tmp = tmp.simplified();
				selText += " (" + tmp + ")\n";
			} else if (addCommentsId->isChecked()) {
				it = resultTable->verticalHeaderItem(i);
				tmp = it->text();
				tmp.remove(")");
				tmp.replace("(", "- ");
				tmp = tmp.simplified();
				selText += " ;" + tmp + "\n";
			} else {
				selText += "\n";
			}
		}
	}

	selText.remove(selText.length() - 1, 1);
	selText = Utils::removeZeros(selText);

	if (addCommentsParaId->isChecked())
		selText.prepend(QString(tr("(DIAMETER: %1, NO. OF HOLES: %2, START ANGLE: %3)\n"))
		                .arg(diaInput->text()).arg(holesInput->text()).arg(angleStartInput->text()));

	if (addCommentsId->isChecked())
		selText.prepend(QString(tr(";DIAMETER: %1, NO. OF HOLES: %2, START ANGLE: %3\n"))
		                .arg(diaInput->text()).arg(holesInput->text()).arg(angleStartInput->text()));

	QClipboard* clipBoard = QApplication::clipboard();
	clipBoard->setText(selText, QClipboard::Clipboard);

	if (clipBoard->supportsSelection()) {
		clipBoard->setText(selText, QClipboard::Selection);
	}
}

void BHCTab::inputChk()
{
	emit commonChk();
}

void BHCTab::setOptions(const BHCTabOptions& options)
{
	xCenterInput->setText(QString::number(options.x));
	yCenterInput->setText(QString::number(options.y));
	diaInput->setText(QString::number(options.diam));
	angleStartInput->setText(QString::number(options.start));
	angleBeetwenInput->setText(QString::number(options.step));
	holesInput->setText(QString::number(options.count));
	roateInput->setValue(options.rotate);
	mirrorX->setChecked(options.mirrorX);
	mirrorY->setChecked(options.mirrorY);
	all->setChecked(options.common);

	inputChanged();
}

BHCTabOptions BHCTab::options()
{
	BHCTabOptions options;

	options.x = xCenterInput->text().toDouble();
	options.y = yCenterInput->text().toDouble();
	options.diam = diaInput->text().toDouble();
	options.start = angleStartInput->text().toDouble();
	options.step = angleBeetwenInput->text().toDouble();
	options.count = holesInput->text().toInt();
	options.rotate = roateInput->value();
	options.mirrorX = mirrorX->isChecked();
	options.mirrorY = mirrorY->isChecked();
	options.common = all->isChecked();

	return options;
}
