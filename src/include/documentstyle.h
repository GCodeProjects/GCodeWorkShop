/*
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
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

#ifndef DOCUMENTSTYLE_H
#define DOCUMENTSTYLE_H

#include <memory>

#include <QString>

class QSettings;


class DocumentStyle
{
public:
    typedef std::shared_ptr<DocumentStyle> Ptr;

    DocumentStyle();
    DocumentStyle(const DocumentStyle &that);
    virtual ~DocumentStyle();

    virtual QString documentType() const;

    virtual DocumentStyle *clone() const;

    virtual DocumentStyle &operator=(const DocumentStyle &that);

    void load(QSettings *cfg);
    void save(QSettings *cfg) const;

protected:
    virtual void loadChild(QSettings *cfg);
    virtual void saveChild(QSettings *cfg) const;
};

#endif // DOCUMENTSTYLE_H
