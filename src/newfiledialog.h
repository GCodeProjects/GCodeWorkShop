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





#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QSettings>
#include <QFileDialog>


#define TEMPLATE_PATH             "/usr/share/edytornc/TEMPLATE"


namespace Ui {
class newFileDialog;
}

class newFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newFileDialog(QWidget *parent = 0);
    ~newFileDialog();
    

    int exec();
    QString getChosenFile();



private:
    Ui::newFileDialog *ui;

    QDir path;


private slots:
    void fillFileCombo();
    void saveSettings();
    void browseButtonClicked();


};

#endif // NEWFILEDIALOG_H
