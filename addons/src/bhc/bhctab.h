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

#ifndef BHCDTAB_H
#define BHCDTAB_H

#include <QObject>  // for slots, Q_OBJECT, signals
#include <QString>  // for QString
#include <QWidget>  // for QWidget

class QAbstractItemModel;
class QAction;
class QActionGroup;
class QEvent;
class QMenu;
class QPoint;

#include "ui_bhctab.h"  // for Ui::BHCTab


/**
 * @brief The BHCTab class
 */
class BHCTab : public QWidget, public Ui::BHCTab
{
    Q_OBJECT

public:
    BHCTab(QWidget *parent = 0);
    ~BHCTab();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void contextMenuReq(const QPoint &pos);
    void inputChk();

signals:
    void commonChk();

private slots:
    void copySelection();
    void sellAll();
    void inputChanged();

private:
    QMenu *contextMenu;
    QAbstractItemModel *model;
    QAction *addCommentsId;
    QAction *addCommentsParaId;
    QActionGroup *commentActGroup;
};

#endif // BHCDTAB_H
