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

#include <QLatin1String>    // for QLatin1String

#include "documentproducer.h"       // for DocumentProducer
#include "gcoder.h"                 // for DOCUMENT_TYPE
#include "gcoderdocument.h"         // for GCoderDocument
#include "gcoderinfo.h"             // for GCoderInfo
#include "gcoderproducer.h"
#include "gcoderstyle.h"            // for GCoderStyle
#include "gcoderwidgetproperties.h" // for GCoderWidgetProperties


class Document;
class DocumentInfo;
class DocumentStyle;
class DocumentWidgetProperties;


GCoderProducer::GCoderProducer() : DocumentProducer(new GCoderStyle(), new GCoderWidgetProperties)
{
}

QString GCoderProducer::documentType() const
{
	return GCoder::DOCUMENT_TYPE;
}

QString GCoderProducer::brief() const
{
	return "Text edit for g-codes.";
}

Document *GCoderProducer::createDocument() const
{
	GCoderDocument *gdoc = new GCoderDocument();
	gdoc->setDocumentStyle(documentStyle());
	gdoc->setDocumentWidgetProperties(documentWidgetProperties());
	return gdoc;
}

DocumentInfo *GCoderProducer::createDocumentInfo()
{
	return new GCoderInfo();
}

DocumentStyle *GCoderProducer::createDocumentStyle()
{
	return new GCoderStyle();
}

DocumentWidgetProperties *GCoderProducer::createDocumentWidgetProperties()
{
	return new GCoderWidgetProperties();
}
