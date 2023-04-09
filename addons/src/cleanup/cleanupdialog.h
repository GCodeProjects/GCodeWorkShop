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

#include <QDialog>  // for QDialog
#include <QObject>  // for Q_OBJECT, slots
#include <QPointer> // for QPointer
#include <QString>  // for QString

class QMenu;
class QPoint;
class QRegularExpression;
class QSettings;
class QWidget;

struct CleanUpOptions;

#include "ui_cleanupdialog.h"   // for Ui::CleanUpDialog


class CleanUpDialog : public QDialog, private Ui::CleanUpDialog
{
    Q_OBJECT

public:
    explicit CleanUpDialog(QWidget *parent, QSettings *settings);
    ~CleanUpDialog();

    void setText(QString text);

    void setOptions(const CleanUpOptions &options);
    CleanUpOptions options();

    void loadSettings(const CleanUpOptions &defaultOptions);
    void saveSettings(bool saveOptions = true);

private:
    QMenu *contextMenu;

    void highlightFindText(QRegularExpression regex);
    void newRow();

private slots:
    void onCellCliced(int row, int col);
    void highlightText(int row, int col);
    void cellChangedSlot(int row, int col);
    void contextMenuReq(const QPoint &pos);
    void removeRow();
    void onFinished(int result);

protected:
    QPointer<QSettings> mSettings;
};

#endif // CLEANUPDIALOG_H
