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

#include "bhc/addons-bhc.h"


Addons::Actions::Actions(QObject *parent) : QObject(parent),
    m_bhc(new QAction(this))
{
    connect(m_bhc, SIGNAL(triggered()), this, SLOT(doBhc()));

    loadIcons();
    loadTranslations();
}

void Addons::Actions::loadTranslations()
{
    m_bhc->setText(tr("&Bolt hole circle"));
    m_bhc->setToolTip(tr("Calculate bolt hole's positions"));
}

void Addons::Actions::loadIcons()
{
    m_bhc->setIcon(QIcon(":/images/bhc.png"));
}

void Addons::Actions::doBhc()
{
    Addons::doBhc(EdytorNc::instance(), Medium::instance().settings());
}
