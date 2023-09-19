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

#include <QAction>          // for QAction
#include <QApplication>     // for QApplication
#include <QIcon>            // for QIcon
#include <QMessageBox>      // for QMessageBox
#include <QPlainTextEdit>   // for QPlainTextEdit
#include <Qt>               // for BusyCursor

#include <addons-actions.h>
#include <edytornc.h>           // for EdytorNc
#include <mdichild.h>
#include <utils/medium.h>       // for Medium

#include "addons-context.h" // for Context, Context::SELECTED_BLOCKS

#include "bhc/addons-bhc.h"
#include "blockskip/utils-blockskip.h"
#include "chamfer/addons-chamfer.h"
#include "cleanup/addons-cleanup.h"
#include "comment/utils-comment.h"
#include "compilemacro/utils-compilemacro.h"
#include "dot/addons-dot.h"
#include "emptylines/utils-emptylines.h"
#include "feeds/addons-feeds.h"
#include "i2m/addons-i2m.h"


Addons::Actions::Actions(QObject *parent) : QObject(parent),
    m_bhc(new QAction(this)),
    m_blockSkipDecrement(new QAction(this)),
    m_blockSkipIncrement(new QAction(this)),
    m_blockSkipRemove(new QAction(this)),
    m_chamfer(new QAction(this)),
    m_cleanUp(new QAction(this)),
    m_paraComment(new QAction(this)),
    m_semiComment(new QAction(this)),
    m_compileMacro(new QAction(this)),
    m_dot(new QAction(this)),
    m_insertEmptyLines(new QAction(this)),
    m_removeEmptyLines(new QAction(this)),
    m_feeds(new QAction(this)),
    m_i2m(new QAction(this))
{
    connect(m_bhc, SIGNAL(triggered()), this, SLOT(doBhc()));
    connect(m_blockSkipDecrement, SIGNAL(triggered()), this, SLOT(doBlockSkipDecrement()));
    connect(m_blockSkipIncrement, SIGNAL(triggered()), this, SLOT(doBlockSkipIncrement()));
    connect(m_blockSkipRemove, SIGNAL(triggered()), this, SLOT(doBlockSkipRemove()));
    connect(m_chamfer, SIGNAL(triggered()), this, SLOT(doChamfer()));
    connect(m_cleanUp, SIGNAL(triggered()), this, SLOT(doCleanUp()));
    connect(m_paraComment, SIGNAL(triggered()), this, SLOT(doParaComment()));
    connect(m_semiComment, SIGNAL(triggered()), this, SLOT(doSemiComment()));
    connect(m_compileMacro, SIGNAL(triggered()), this, SLOT(doCompileMacro()));
    connect(m_dot, SIGNAL(triggered()), this, SLOT(doDot()));
    connect(m_insertEmptyLines, SIGNAL(triggered()), this, SLOT(doInsertEmptyLines()));
    connect(m_removeEmptyLines, SIGNAL(triggered()), this, SLOT(doRemoveEmptyLines()));
    connect(m_feeds, SIGNAL(triggered()), this, SLOT(doFeeds()));
    connect(m_i2m, SIGNAL(triggered()), this, SLOT(doI2M()));

    loadIcons();
    loadTranslations();
}

void Addons::Actions::loadTranslations()
{
    m_bhc->setText(tr("&Bolt hole circle"));
    m_bhc->setToolTip(tr("Calculate bolt hole's positions"));
    m_blockSkipDecrement->setText(tr("Block Skip -"));
    m_blockSkipDecrement->setToolTip(tr("Insert/decrease Block Skip /"));
    m_blockSkipIncrement->setText(tr("Block Skip +"));
    m_blockSkipIncrement->setToolTip(tr("Insert/increase Block Skip /"));
    m_blockSkipRemove->setText(tr("Block Skip remove"));
    m_blockSkipRemove->setToolTip(tr("Remove Block Skip /"));
    m_chamfer->setText(tr("Chamfer"));
    m_chamfer->setToolTip(tr("Calculate chamfer"));
    m_cleanUp->setText(tr("Clean &up"));
    m_cleanUp->setToolTip(tr("Remove text using regular expressions"));
    m_paraComment->setText(tr("Comment ()"));
    m_paraComment->setToolTip(tr("Comment/uncomment selected text using parentheses"));
    m_semiComment->setText(tr("Comment ;"));
    m_semiComment->setToolTip(tr("Comment/uncomment selected text using semicolon"));
    m_compileMacro->setText(tr("Compile macro - experimental"));
    m_compileMacro->setToolTip(tr("Translate EdytorNC macro into CNC program"));
    m_dot->setText(tr("Insert dots"));
    m_dot->setToolTip(tr("Inserts decimal dot"));
    m_insertEmptyLines->setText(tr("Insert empty lines"));
    m_insertEmptyLines->setToolTip(tr("Inserts empty lines"));
    m_removeEmptyLines->setText(tr("Remove empty lines"));
    m_removeEmptyLines->setToolTip(tr("Removes empty lines"));
    m_feeds->setText(tr("Feed's speed's"));
    m_feeds->setToolTip(tr("Calculate speed, feed, cutting speed"));
    m_i2m->setText(tr("Convert inch <-> mm"));
    m_i2m->setToolTip(tr("Convert inch <-> mm"));
}

void Addons::Actions::loadIcons()
{
    m_bhc->setIcon(QIcon(":/images/bhc.png"));
    m_blockSkipDecrement->setIcon(QIcon(":/images/blockskip-.png"));
    m_blockSkipIncrement->setIcon(QIcon(":/images/blockskip+.png"));
    m_blockSkipRemove->setIcon(QIcon(":/images/blockskipr.png"));
    m_chamfer->setIcon(QIcon(":/images/chamfer.png"));
    m_cleanUp->setIcon(QIcon(":/images/cleanup.png"));
    m_paraComment->setIcon(QIcon(":/images/paracomment.pn"));
    m_semiComment->setIcon(QIcon(":/images/semicomment.png"));
    m_compileMacro->setIcon(QIcon(":/images/compfile.png"));
    m_dot->setIcon(QIcon(":/images/dots.png"));
    m_insertEmptyLines->setIcon(QIcon(":/images/insertemptylines.png"));
    m_removeEmptyLines->setIcon(QIcon(":/images/removeemptylines.png"));
    m_feeds->setIcon(QIcon(":/images/vcf.png"));
    m_i2m->setIcon(QIcon(":/images/i2m.png"));
}

void Addons::Actions::doBhc()
{
    Addons::doBhc(EdytorNc::instance(), Medium::instance().settings());
}

void Addons::Actions::doBlockSkip(bool remove, bool inc)
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::SELECTED_BLOCKS)) {
        return;
    }

    Utils::blockSkip(ctx.text(), remove, inc);
    ctx.setSelectionStart(ctx.fragmentStart());
    ctx.setSelectionEnd(ctx.fragmentStart() + ctx.text().length());
    ctx.push();
}

void Addons::Actions::doBlockSkipDecrement()
{
    doBlockSkip(false, false);
}

void Addons::Actions::doBlockSkipIncrement()
{
    doBlockSkip(false, true);
}

void Addons::Actions::doBlockSkipRemove()
{
    doBlockSkip(true, true);
}

void Addons::Actions::doChamfer()
{
    Addons::doChamfer(EdytorNc::instance(), Medium::instance().settings());
}

void Addons::Actions::doCleanUp()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::ALL)) {
        return;
    }

    if (Addons::doCleanUp(EdytorNc::instance(), Medium::instance().settings(), ctx.text())) {
        ctx.push();
    }
}

void Addons::Actions::doParaComment()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::SELECTED)) {
        return;
    }

    Utils::paraComment(ctx.text());
    ctx.push();
}

void Addons::Actions::doSemiComment()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::SELECTED)) {
        return;
    }

    Utils::semiComment(ctx.text());
    ctx.push();
}

void Addons::Actions::doCompileMacro()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::ALL)) {
        return;
    }

    Utils::CompileMacro compiler;
    EdytorNc *enc = EdytorNc::instance();

    if (compiler.compile(ctx.text()) == -1) {
        QMessageBox::warning(enc, tr("EdytorNc - compile macro"), compiler.status());
        //return;
    }

    MdiChild *child = enc->newFile();
    child->textEdit()->insertPlainText(compiler.result());
}

void Addons::Actions::doDot()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::ALL)) {
        return;
    }

    if (Addons::doDot(EdytorNc::instance(), Medium::instance().settings(), ctx.text())) {
        ctx.push();
    }
}

void Addons::Actions::doInsertEmptyLines()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::ALL)) {
        return;
    }

    QApplication::setOverrideCursor(Qt::BusyCursor);
    Utils::insertEmptyLines(ctx.text());
    ctx.push();
    QApplication::restoreOverrideCursor();
}

void Addons::Actions::doRemoveEmptyLines()
{
    Addons::Context ctx;

    if (!ctx.pull(Addons::Context::ALL)) {
        return;
    }

    QApplication::setOverrideCursor(Qt::BusyCursor);
    Utils::removeEmptyLines(ctx.text());
    ctx.push();
    QApplication::restoreOverrideCursor();
}

void Addons::Actions::doFeeds()
{
    Addons::doFeeds(EdytorNc::instance(), Medium::instance().settings());
}

void Addons::Actions::doI2M()
{
    Addons::doI2M(EdytorNc::instance(), Medium::instance().settings());
}
