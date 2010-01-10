/***************************************************************************
 *   Copyright (C) 2006-2010 by Artur Kozioł                               *
 *   artkoz@poczta.onet.pl                                                 *
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

#ifndef FINDINFILES_H
#define FINDINFILES_H

#include <QDialog>
#include <QTableWidget>
#include <QTextEdit>


#include "commoninc.h"
#include "highlighter.h"

#include "ui_findinfilesdialog.h"


QT_BEGIN_NAMESPACE
class QComboBox;
class QDir;
class QLabel;
class QPushButton;
class QTableWidget;
QT_END_NAMESPACE


class FindInFiles : public QDialog, private Ui::FindInFiles
{
    Q_OBJECT

public:
    FindInFiles(QWidget *parent = 0);

public slots:
    void setHighlightColors(const _h_colors colors);
    void setDir(const QString dir);

private slots:
    void browse();
    void find();
    void closeDialog();
    void filesTableClicked(int x, int y);
    void filePreview(int x, int y);

protected:
    void closeEvent(QCloseEvent *event);

private:
    QStringList findFiles(const QDir &directory, const QStringList &files,
                          const QString &text);
    void showFiles(const QDir &directory, const QStringList &files);
    void createFilesTable();
    void readSettings();
    void highlightFindText(QString searchString, QTextDocument::FindFlags options = 0);

    Highlighter *highlighter;
    QList<QTextEdit::ExtraSelection> findTextExtraSelections;
    QTextEdit::ExtraSelection selection;


signals:
    void fileClicket(const QString&);
   
};


#endif
