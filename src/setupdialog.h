/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
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

#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QColorDialog>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QObject>
#include <Qt>      // Qt::WindowFlags
#include <QWidget>

#include "commoninc.h" // _editor_properites

#include "ui_setupdialog.h"

class QAbstractButton;
class QButtonGroup;


/**
 * @brief The SetupDialog class
 */
class SetupDialog : public QDialog, private Ui::SetupDialog
{
    Q_OBJECT

public:
    SetupDialog(QWidget *parent = 0, const _editor_properites *prop = 0,
                Qt::WindowFlags f = Qt::Dialog);
    ~SetupDialog();

public slots:
    _editor_properites getSettings();

private slots:
    void on_btnBrowseDirectory_clicked();
    void on_btnDeleteExtension_clicked();
    void on_btnAddExtension_clicked();
    void changeFont();
    void changeColor(QAbstractButton *button);
    void setDefaultProp();
    void browseButtonClicked();

private:
    _editor_properites editProp;
    QButtonGroup *colorButtons;
};

#endif // SETUPDIALOG_H
