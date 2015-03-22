/***************************************************************************
 *   Copyright (C) 2006-2015 by Artur Kozioł                               *
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


#include <QtWidgets>



#ifndef CLEANUPDIALOG_H
#define CLEANUPDIALOG_H

#include <QDialog>



namespace Ui {
class cleanUpDialog;
}

class cleanUpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit cleanUpDialog(QWidget *parent = 0);
    ~cleanUpDialog();
    
    void setText(QString text);
    QStringList getSelectedExpressions();
    int exec(QStringList selList, QString text);


private:
    Ui::cleanUpDialog *ui;

    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> findTextExtraSelections;
    QTextEdit::ExtraSelection selection;

    QStringList expressions;
    QStringList expressionsComment;
    QStringList selectedExpressions;

    QMenu *contextMenu;

    void highlightFindText(QRegExp exp);
    void newRow();





private slots:
    void highlightText(int row, int col);
    void cellChangedSlot(int row, int col);
    void closeButtonClicked();
    void okButtonClicked();
    void contextMenuReq(const QPoint &pos);
    void removeRow();





};

#endif // CLEANUPDIALOG_H
