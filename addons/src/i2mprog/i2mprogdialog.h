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

#ifndef I2MPROGDIALOG_H
#define I2MPROGDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for slots, Q_OBJECT
#include <QString>  // for QString
#include <Qt>       // for Qt::WindowFlags, Qt::Dialog

class QWidget;

#include "ui_i2mprogdialog.h"   // for Ui::I2MProgDialog


/**
 * @brief The I2MProgDialog class
 */
class I2MProgDialog : public QDialog, private Ui::I2MProgDialog
{
    Q_OBJECT

public:
    I2MProgDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~I2MProgDialog();

private slots:
    void inputChanged();
};

#endif // I2MPROGDIALOG_H
