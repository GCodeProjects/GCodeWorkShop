/*
 *  Copyright (C) 2024 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of GCodeWorkShop.
 *
 *  GCodeWorkShop is free software: you can redistribute it and/or modify
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

#include <QString>  // for operator!=, QString
#include <QtDebug>  // for QDebug
#include <QtGlobal> // for qWarning

#include <document.h>           // for Document
#include <documentproducer.h>   // IWYU pragma: associated

#include "documentstyle.h"              // for DocumentStyle, DocumentStyle::Ptr
#include "documentwidgetproperties.h"   // for DocumentWidgetProperties, DocumentWidgetProperties::Ptr


DocumentProducer::DocumentProducer(DocumentStyle* style, DocumentWidgetProperties* properties) :
	m_documentStyle(style),
	m_documentWidgetProperties(properties)
{
}

DocumentProducer::~DocumentProducer()
{
}

DocumentStyle::Ptr DocumentProducer::documentStyle() const
{
	return m_documentStyle;
}

bool DocumentProducer::setDocumentStyle(const DocumentStyle::Ptr& style)
{
	if (!style) {
		qWarning() << "DocumentProducer::setDocumentStyle() : The \"style\" object stores a null pointer.";
		return false;
	}

	if (style->documentType() != documentType()) {
		qWarning() << "DocumentProducer::setDocumentStyle() : The object \"style\" has the inappropriate type" <<
		           style->documentType();
		return false;
	}

	m_documentStyle = style;
	return true;
}

DocumentWidgetProperties::Ptr DocumentProducer::documentWidgetProperties() const
{
	return m_documentWidgetProperties;
}

bool DocumentProducer::setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr& properties)
{
	if (!properties) {
		qWarning() << "DocumentProducer::setDocumentWidgetProperties() : The \"properties\" object stores a null pointer.";
		return false;
	}

	if (properties->documentType() != documentType()) {
		qWarning() << "DocumentProducer::setDocumentWidgetProperties() : The object \"properties\" has the inappropriate type"
		           << properties->documentType();
		return false;
	}

	m_documentWidgetProperties = properties;
	return true;
}

void DocumentProducer::updateDocument(Document* document)
{
	document->setDocumentWidgetProperties(documentWidgetProperties());
	document->setDocumentStyle(documentStyle());
}
