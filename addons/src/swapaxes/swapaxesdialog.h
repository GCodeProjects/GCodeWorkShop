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

#ifndef SWAPAXESDIALOG_H
#define SWAPAXESDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for Q_OBJECT, slots
#include <QString>  // for QString

class QWidget;

#include "ui_swapaxesdialog.h"  // for Ui::SwapAxesDialog


class SwapAxesDialog : public QDialog, private Ui::SwapAxesDialog
{
    Q_OBJECT

public:
    explicit SwapAxesDialog(QWidget *parent = 0);
    ~SwapAxesDialog();

private slots:
    void betweenCheckBoxClicked(bool checked);
    void modifyCheckBoxClicked(bool checked);
    void precisionSpinBoxChanded(int val);
};

#endif // SWAPAXESDIALOG_H
