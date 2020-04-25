/**
 * @file
 * @brief
 *
 * @date 19.04.2020 (2020-04-19) created by Nick Egorrov
 * @author Nick Egorrov
 * @copyright Copyright (C) 2019  Nick Egorrov
 * @copyright
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 * @copyright
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.*
 * @copyright
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "utils/config.h"


Config::Config(QSettings *settings, const QString &group)
{
    m_parentNode = 0;
    m_storage = settings;
    m_groupName = group;
}

Config::Config(Config *parentNode, const QString &group)
{
    m_parentNode = parentNode;
    m_storage = parentNode->m_storage;
    m_groupName = group;
    m_parentNode->add(this);
}


Config::~Config()
{

}

void Config::push()
{
    IGroupControl *item;

    foreach (item, m_items) {
        item->push();
    }
}

void Config::pop()
{
    IGroupControl *item;

    foreach (item, m_items) {
        item->pop();
    }
}

void Config::reset()
{
    IGroupControl *item;

    foreach (item, m_items) {
        item->reset();
    }
}

void Config::begin()
{
    if (m_parentNode != 0) {
        m_parentNode->begin();
    }

    if (!m_groupName.isEmpty()) {
        m_storage->beginGroup(m_groupName);
    }
}

void Config::end()
{
    if (m_parentNode != 0) {
        m_parentNode->end();
    }

    if (!m_groupName.isEmpty()) {
        m_storage->endGroup();
    }
}

void Config::add(IGroupControl *item)
{
    m_items.append(item);
}
