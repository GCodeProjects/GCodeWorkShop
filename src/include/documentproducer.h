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

#ifndef DOCUMENTPRODUCER_H
#define DOCUMENTPRODUCER_H

class QString;

#include <documentstyle.h>
#include <documentwidgetproperties.h>

class Document;
class DocumentInfo;


class DocumentProducer
{
public:
	DocumentProducer(DocumentStyle* style, DocumentWidgetProperties* properties);
	virtual ~DocumentProducer();

	virtual QString documentType() const = 0;
	virtual QString brief() const = 0;

	virtual DocumentStyle::Ptr documentStyle() const;
	virtual bool setDocumentStyle(const DocumentStyle::Ptr& style);

	virtual DocumentWidgetProperties::Ptr documentWidgetProperties() const;
	virtual bool setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr& properties);

	virtual void updateDocument(Document* document);

	virtual Document* createDocument() const = 0;
	virtual DocumentInfo* createDocumentInfo() = 0;
	virtual DocumentStyle* createDocumentStyle() = 0;
	virtual DocumentWidgetProperties* createDocumentWidgetProperties() = 0;

protected:
	DocumentStyle::Ptr m_documentStyle;
	DocumentWidgetProperties::Ptr m_documentWidgetProperties;
};

#endif // DOCUMENTPRODUCER_H
