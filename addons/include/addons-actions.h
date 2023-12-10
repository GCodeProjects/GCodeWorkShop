/*
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

#ifndef ADDONS_ACTIONS_H
#define ADDONS_ACTIONS_H

#include <QObject>  // for QObject, Q_OBJECT, slots
#include <QString>  // for QString

class QAction;


namespace Addons {
class Actions : public QObject
{
    Q_OBJECT

public:
    explicit Actions(QObject *parent = nullptr);

public slots:
    void loadTranslations();
    void loadIcons();

public:
    // *INDENT-OFF*
    QAction *bhc() {return m_bhc;}
    QAction *blockSkipDecrement() {return m_blockSkipDecrement;}
    QAction *blockSkipIncrement() {return m_blockSkipIncrement;}
    QAction *blockSkipRemove() {return m_blockSkipRemove;}
    QAction *chamfer() {return m_chamfer;}
    QAction *cleanUp() {return m_cleanUp;}
    QAction *paraComment() {return m_paraComment;}
    QAction *semiComment() {return m_semiComment;}
    QAction *compileMacro() {return m_compileMacro;}
    QAction *dot() {return m_dot;}
    QAction *insertEmptyLines() {return m_insertEmptyLines;}
    QAction *removeEmptyLines() {return m_removeEmptyLines;}
    QAction *feeds() {return m_feeds;}
    QAction *i2m() {return m_i2m;}
    QAction *i2mProg() {return m_i2mProg;}
    QAction *renumber() {return m_renumber;}
    QAction *insertSpaces() {return m_insertSpaces;}
    QAction *removeSpaces() {return m_removeSpaces;}
    QAction *splitProgramms() {return m_splitProgramms;}

protected:
    QAction *m_bhc;
    QAction *m_blockSkipDecrement;
    QAction *m_blockSkipIncrement;
    QAction *m_blockSkipRemove;
    QAction *m_chamfer;
    QAction *m_cleanUp;
    QAction *m_paraComment;
    QAction *m_semiComment;
    QAction *m_compileMacro;
    QAction *m_dot;
    QAction *m_insertEmptyLines;
    QAction *m_removeEmptyLines;
    QAction *m_feeds;
    QAction *m_i2m;
    QAction *m_i2mProg;
    QAction *m_renumber;
    QAction *m_insertSpaces;
    QAction *m_removeSpaces;
    QAction *m_splitProgramms;

protected slots:
    void doBhc();
    void doBlockSkipDecrement();
    void doBlockSkipIncrement();
    void doBlockSkipRemove();
    void doChamfer();
    void doCleanUp();
    void doParaComment();
    void doSemiComment();
    void doCompileMacro();
    void doDot();
    void doInsertEmptyLines();
    void doRemoveEmptyLines();
    void doFeeds();
    void doI2M();
    void doI2MProg();
    void doRenumber();
    void doInsertSpaces();
    void doRemoveSpaces();
    void doSplitProgramms();

protected:
    void doBlockSkip(bool remove, bool inc);
};
} // namespace Addons

#endif // ADDONS_ACTIONS_H
