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
#include <QIcon>            // for QIcon

#include <addons-actions.h>
#include <edytornc.h>           // for EdytorNc
#include <utils/medium.h>       // for Medium

#include "addons-context.h" // for Context, Context::SELECTED_BLOCKS

#include "bhc/addons-bhc.h"
#include "blockskip/utils-blockskip.h"


Addons::Actions::Actions(QObject *parent) : QObject(parent),
    m_bhc(new QAction(this)),
    m_blockSkipDecrement(new QAction(this)),
    m_blockSkipIncrement(new QAction(this)),
    m_blockSkipRemove(new QAction(this))
{
    connect(m_bhc, SIGNAL(triggered()), this, SLOT(doBhc()));
    connect(m_blockSkipDecrement, SIGNAL(triggered()), this, SLOT(doBlockSkipDecrement()));
    connect(m_blockSkipIncrement, SIGNAL(triggered()), this, SLOT(doBlockSkipIncrement()));
    connect(m_blockSkipRemove, SIGNAL(triggered()), this, SLOT(doBlockSkipRemove()));

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
}

void Addons::Actions::loadIcons()
{
    m_bhc->setIcon(QIcon(":/images/bhc.png"));
    m_blockSkipDecrement->setIcon(QIcon(":/images/blockskip-.png"));
    m_blockSkipIncrement->setIcon(QIcon(":/images/blockskip+.png"));
    m_blockSkipRemove->setIcon(QIcon(":/images/blockskipr.png"));
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
