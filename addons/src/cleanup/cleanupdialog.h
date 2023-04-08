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

#ifndef CLEANUPDIALOG_H
#define CLEANUPDIALOG_H

#include <QDialog>      // for QDialog
#include <QList>        // for QList
#include <QObject>      // for Q_OBJECT, slots
#include <QString>      // for QString
#include <QStringList>  // for QStringList
#include <QTextEdit>    // for QTextEdit, QTextEdit::ExtraSelection

class QMenu;
class QPoint;
class QRegularExpression;
class QWidget;

namespace Ui {
class CleanUpDialog;
}


class CleanUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CleanUpDialog(QWidget *parent = 0);
    ~CleanUpDialog();

    void setText(QString text);
    QStringList getSelectedExpressions();
    int exec(QStringList selList, QString text);

private:
    Ui::CleanUpDialog *ui;

    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> findTextExtraSelections;
    QTextEdit::ExtraSelection selection;

    QStringList expressions;
    QStringList expressionsComment;
    QStringList selectedExpressions;

    QMenu *contextMenu;

    void highlightFindText(QRegularExpression regex);
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
