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

#ifndef GCODERPRODUCER_H
#define GCODERPRODUCER_H

#include <QString>  // for QString

#include <documentproducer.h>

class Document;
class DocumentInfo;
class DocumentStyle;
class DocumentWidgetProperties;


class GCoderProducer : public DocumentProducer
{
public:
	GCoderProducer();

	QString documentType() const;
	QString brief() const;

	Document* createDocument() const;
	DocumentInfo* createDocumentInfo();
	DocumentStyle* createDocumentStyle();
	DocumentWidgetProperties* createDocumentWidgetProperties();
};

#endif // GCODERPRODUCER_H
