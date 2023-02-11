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

#ifndef FILECHECKER_H
#define FILECHECKER_H

#include <QByteArray>
#include <QDialog>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QWidget>

class QToolButton;

class KDiff3App;

namespace Ui {
class FileChecker;
}


class FileChecker : public QDialog
{
    Q_OBJECT

public:
    explicit FileChecker(QWidget *parent = 0);
    ~FileChecker();

    void setData(const QString startDir, QStringList readPaths, QStringList fileFilter);
    void findFiles();

private slots:
    void filesTableClicked(int row, int col);
    void acceptFile();
    void deleteFile();
    void readPathComboBoxChanged(QString text);
    void filesTableCurrentCellChanged(int row, int col, int pRow, int pCol);

private:
    Ui::FileChecker *ui;
    QPointer<KDiff3App> diffApp;
    QStringList extensions;
    QPointer<QToolButton> okBtn;
    QPointer<QToolButton> noBtn;
    int prevRow;
    QStringList readPathList;
    QString savePath;
    QByteArray splitterState;

    void createDiff();
    void findFiles(const QString startDir, QStringList fileFilter);

    void preliminaryDiff(QString file1, QString file2);
};

#endif // FILECHECKER_H
