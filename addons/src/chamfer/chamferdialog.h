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

#ifndef CHAMFERDIALOG_H
#define CHAMFERDIALOG_H

#include <QDialog>  // for QDialog
#include <QObject>  // for slots, Q_OBJECT
#include <QPointer> // for QPointer
#include <QString>  // for QString

class QSettings;
class QWidget;

struct ChamferOptions;

#include "ui_chamferdialog.h"   // for Ui::ChamferDialog


/**
 * @brief The ChamferDialog class
 */
class ChamferDialog : public QDialog, private Ui::ChamferDialog
{
    Q_OBJECT

public:
    ChamferDialog(QWidget *parent, QSettings *settings);
    ~ChamferDialog();

    void setOptions(const ChamferOptions &options);
    ChamferOptions options();

    void loadSettings(const ChamferOptions &defaultOptions);
    void saveSettings(bool saveOptions = true);

protected slots:
    void inputChanged();
    void checkBoxToggled();

private slots:
    void computeButtonClicked();

protected:
    QPointer<QSettings> mSettings;
};

#endif // CHAMFERDIALOG_H
