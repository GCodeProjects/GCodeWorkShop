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

#ifndef SERIALPORTCFGHELPDIALOG_H
#define SERIALPORTCFGHELPDIALOG_H

#include <QDialog>


namespace Ui {
class SerialPortCfgHelpDialog;
}

class SerialPortCfgHelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortCfgHelpDialog(QWidget *parent = 0);
    ~SerialPortCfgHelpDialog();

private:
    Ui::SerialPortCfgHelpDialog *ui;
};

#endif // SERIALPORTCFGHELPDIALOG_H
