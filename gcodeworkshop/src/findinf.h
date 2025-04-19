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

#ifndef FINDINFILES_H
#define FINDINFILES_H

#include <QList>            // for QList
#include <QObject>          // for slots, Q_OBJECT, signals
#include <QString>          // for QString
#include <QTextDocument>    // for QTextDocument, QTextDocument::FindFlags
#include <QTextEdit>        // for QTextEdit::ExtraSelection, QTextEdit
#include <QWidget>          // for QWidget
#include <QtGlobal>         // for QT_VERSION, QT_VERSION_CHECK

class QCloseEvent;
class QDir;
class QEvent;
class QProgressDialog;
class QSplitter;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	class QStringList;
#else
	using QStringList = QList<QString>;
#endif

#include <gcoderstyle.h>    // for HighlightColors

class Highlighter;

#include "ui_findinfilesdialog.h"   // for Ui::FindInFiles


class FindInFiles : public QWidget, private Ui::FindInFiles
{
	Q_OBJECT

public:
	FindInFiles(QSplitter* parent = 0);

public slots:
	void setHighlightColors(const HighlightColors colors);
	void setDir(const QString dir);

private slots:
	void browse();
	void find();
	void closeDialog();
	void filesTableClicked(int x, int y);
	void filePreview(int x, int y);
	void hideDlg();

protected:
	void closeEvent(QCloseEvent* event);
	bool eventFilter(QObject* obj, QEvent* ev);

private:
	bool findFiles(const QString startDir, QString mainDir, bool notFound,
	               const QString findText, QString fileFilter, QProgressDialog* progressDialog);
	void showFiles(const QDir& directory, const QStringList& files);
	void createFilesTable();
	void readSettings();
	void highlightFindText(QString searchString,
	                       QTextDocument::FindFlags options = QTextDocument::FindFlags());
	bool findText(const QString& exp, QTextDocument::FindFlags options, bool ignoreComments = true);

	Highlighter* highlighter;
	QList<QTextEdit::ExtraSelection> findTextExtraSelections;
	QTextEdit::ExtraSelection selection;
	bool intCapsLock;
	HighlightColors highlighterColors;
	bool highlight;
	QList<int> currentHeight;
	QSplitter* f_parent;

signals:
	void fileClicked(const QString&);
};

#endif // FINDINFILES_H
