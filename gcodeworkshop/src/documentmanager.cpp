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

#include <QList>            // for QList, QList<>::const_iterator, QList<>::iterator
#include <QMap>             // for QMap, QMap<>::const_iterator
#include <QMdiArea>         // for QMdiArea, QMdiArea::StackingOrder
#include <QObject>          // for emit, SIGNAL, SLOT, QObject
#include <QPointer>         // for QPointer
#include <QString>          // for QString, operator==
#include <Qt>               // for ConnectionType, DirectConnection
#include <QtDebug>          // for QDebug
#include <QtGlobal>         // for qAsConst, qWarning
// As long as qAsConst is used.
// IWYU pragma: no_include "type_traits"
// IWYU pragma: no_include <type_traits>

#include <document.h>                   // for Document
#include <documentinfo.h>               // for DocumentInfo
#include <documentmanager.h>            // IWYU pragma: associated
#include <documentproducer.h>           // for DocumentProducer
#include <documentstyle.h>              // for DocumentStyle
#include <documentwidgetproperties.h>   // for DocumentWidgetProperties


DocumentManager::DocumentManager(QObject* parent) :
	QObject(parent),
	m_producers(),
	m_docList(),
	m_area(nullptr)
{
}

DocumentManager::~DocumentManager()
{
	for (DocumentProducer* prod : qAsConst(m_producers)) {
		delete prod;
	}
}

Document* DocumentManager::activeDocument() const
{
	if (m_area.isNull() || m_area->subWindowList().isEmpty()) {
		return nullptr;
	}

	// WARNING: This is very strange, but opening a single document when the application starts
	// causes the activeSubWindow function to return nullptr. This is probably caused by the fact
	// that the main window is not yet visible when the document is opened.

	QMdiSubWindow* subWindow = m_area->subWindowList(QMdiArea::StackingOrder).last();

	for (const DocumentListItem& item : qAsConst(m_docList)) {
		if (item.subWindow == subWindow) {
			return item.document;
		}
	}

	return nullptr;
}

bool DocumentManager::isActiveDocument(Document* document)
{
	return activeDocument() == document;
}

bool DocumentManager::setActiveDocument(Document* doc)
{
	if (m_area.isNull()) {
		return false;
	}

	for (const DocumentListItem& item : qAsConst(m_docList)) {
		if (item.document == doc) {
			m_area->setActiveSubWindow(item.subWindow);
			return true;
		}
	}

	return false;
}

bool DocumentManager::setActiveDocument(const QString& filePath)
{
	return setActiveDocument(findDocumentByFilePath(filePath));
}

Document* DocumentManager::findDocumentById(const QString& id) const
{
	for (const DocumentListItem& item : qAsConst(m_docList)) {
		if (item.id == id) {
			return item.document;
		}
	}

	return nullptr;
}

Document* DocumentManager::findDocumentByFilePath(const QString& filePath) const
{
	for (const DocumentListItem& item : qAsConst(m_docList)) {
		if (item.document->filePath() == filePath) {
			return item.document;
		}
	}

	return nullptr;
}

QList<Document*> DocumentManager::documentList() const
{
	QList<Document*> list;

	for (auto doc : m_docList) {
		list.append(doc.document);
	}

	return list;
}

DocumentStyle::Ptr DocumentManager::documentStyle(const QString& type) const
{
	DocumentProducer* producer = documentProducer(type);

	if (!producer) {
		qWarning() << "DocumentManager::documentStyle() : The" << type << "type is not registred." ;
		return DocumentStyle::Ptr(new DocumentStyle());
	}

	return producer->documentStyle();
}

void DocumentManager::setDocumentStyle(const DocumentStyle::Ptr& style)
{
	if (!style) {
		qWarning() << "DocumentManager::setDocumentWidgetProperties() : The style is nullptr." ;
		return;
	}

	const QString& type = style->documentType();
	DocumentProducer* producer = documentProducer(type);

	if (!producer) {
		qWarning() << "DocumentManager::setDocumentStyle() : The" << type << "type is not registred." ;
		return;
	}

	producer->setDocumentStyle(style);
}

DocumentWidgetProperties::Ptr DocumentManager::documentWidgetProperties(const QString& type) const
{
	DocumentProducer* producer = documentProducer(type);

	if (!producer) {
		qWarning() << "DocumentManager::documentWidgetProperties() : The" << type << "type is not registred." ;
		return DocumentWidgetProperties::Ptr(new DocumentWidgetProperties());
	}

	return producer->documentWidgetProperties();
}

void DocumentManager::setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr& property)
{
	if (!property) {
		qWarning() << "DocumentManager::setDocumentWidgetProperties() : The property is nullptr." ;
		return;
	}

	const QString& type = property->documentType();
	DocumentProducer* producer = documentProducer(type);

	if (!producer) {
		qWarning() << "DocumentManager::setDocumentWidgetProperties() : The" << type << "type is not registred." ;
		return;
	}

	producer->setDocumentWidgetProperties(property);
}

void DocumentManager::updateDocuments(const QString& type)
{
	DocumentProducer* producer = documentProducer(type);

	if (!producer) {
		qWarning() << "DocumentManager::updateDocuments() : The" << type << "type is not registred." ;
		return;
	}

	for (const DocumentListItem& item : qAsConst(m_docList)) {
		if (item.document->type() == type) {
			producer->updateDocument(item.document);
		}
	}
}

Document* DocumentManager::createDocument(const QString& type, const QString& id)
{
	DocumentProducer* producer = documentProducer(type);

	if (!producer) {
		qWarning() << "DocumentManager::createDocument() : The" << type << "type is not registred." ;
		return nullptr;
	}

	Document* doc = producer->createDocument();

	if (!doc) {
		return nullptr;
	}

	addDocument(doc, id);
	return doc;
}

DocumentInfo::Ptr DocumentManager::createDocumentInfo(const QString& type)
{
	DocumentProducer* producer = documentProducer(type);
	DocumentInfo* info = nullptr;

	if (producer) {
		info = producer->createDocumentInfo();
	} else {
		qWarning() << "DocumentManager::createDocumentInfo() : The" << type << "type is not registred." ;
	}

	return DocumentInfo::Ptr(info);
}

DocumentStyle::Ptr DocumentManager::createDocumentStyle(const QString& type)
{
	DocumentProducer* producer = documentProducer(type);
	DocumentStyle* style = nullptr;

	if (producer) {
		style = producer->createDocumentStyle();
	} else {
		qWarning() << "DocumentManager::createDocumentStyle() : The" << type << "type is not registred." ;
	}

	return DocumentStyle::Ptr(style);
}

DocumentWidgetProperties::Ptr DocumentManager::createDocumentWidgetProperties(const QString& type)
{
	DocumentProducer* producer = documentProducer(type);
	DocumentWidgetProperties* properties = nullptr;

	if (producer) {
		properties = producer->createDocumentWidgetProperties();
	} else {
		qWarning() << "DocumentManager::createDocumentWidgetProperties() : The" << type << "type is not registred." ;
	}

	return DocumentWidgetProperties::Ptr(properties);
}

void DocumentManager::addDocument(Document* document, const QString& id)
{
	document->setParent(this);
	connect(document, SIGNAL(closeRequested(Document*)), this, SLOT(documentCloseRequest(Document*)),
	        Qt::ConnectionType::DirectConnection);
	connect(document, SIGNAL(closed(Document*)), this, SLOT(removeDocument(Document*)));
	connect(document, SIGNAL(closed(Document*)), this, SLOT(documentClosed(Document*)));
	connect(document, SIGNAL(modificationChanged(Document*, bool)), this, SLOT(documentModificationChanged(Document*,
	        bool)));
	connect(document, SIGNAL(cursorPositionChanged(Document*)), this, SLOT(documentCursorPositionChanged(Document*)));
	connect(document, SIGNAL(selectionChanged(Document*)), this, SLOT(documentSelectionChanged(Document*)));
	connect(document, SIGNAL(briefChanged(Document*)), this, SLOT(documentBriefChanged(Document*)));
	connect(document, SIGNAL(redoAvailable(Document*, bool)), this, SLOT(documentRedoAvailable(Document*, bool)));
	connect(document, SIGNAL(undoAvailable(Document*, bool)), this, SLOT(documentUndoAvailable(Document*, bool)));
	connect(document, SIGNAL(customContextMenuRequested(Document*, const QPoint&)), this,
	        SLOT(customContextMenuRequest(Document*, const QPoint&)));
	connect(document, SIGNAL(fileWatchRequested(const QString&, bool)), this, SLOT(documentFileWatchRequest(const QString&,
	        bool)));

	DocumentListItem item;
	item.document = document;
	item.id = id;
	item.subWindow = m_area->addSubWindow(document->widget());
	m_docList.append(item);
	emit documentListChanged();
}

void DocumentManager::removeDocument(Document* document)
{
	for (int i = 0; i < m_docList.size(); i++) {
		const DocumentListItem& item = m_docList.at(i);

		if (item.document == document) {
			m_docList.removeAt(i);
			emit documentListChanged();

			if (m_docList.isEmpty()) {
				emit activeDocumentChanged(nullptr);
			}

			return;
		}
	}
}

DocumentProducer* DocumentManager::documentProducer(const QString& type) const
{
	return m_producers.value(type, nullptr);
}

void DocumentManager::registerDocumentProducer(DocumentProducer* producer)
{
	const QString& type = producer->documentType();

	if (m_producers.contains(type)) {
		qWarning() << "DocumentManager::registerProducer() : The" << type << "type always registred." ;
		return;
	}

	m_producers.insert(type, producer);
}

void DocumentManager::setMdiArea(QMdiArea* mdi)
{
	m_area = mdi;
	connect(m_area, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(activeWindowChanged(QMdiSubWindow*)));
}

void DocumentManager::activeWindowChanged(QMdiSubWindow* window)
{
	for (const DocumentListItem& item : qAsConst(m_docList)) {
		if (item.subWindow == window) {
			emit activeDocumentChanged(item.document);
		}
	}
}

void DocumentManager::documentModificationChanged(Document* doc, bool mod)
{
	if (isActiveDocument(doc)) {
		emit modificationChanged(mod);
	}
}

void DocumentManager::documentCursorPositionChanged(Document* doc)
{
	if (isActiveDocument(doc)) {
		emit cursorPositionChanged();
	}
}

void DocumentManager::documentSelectionChanged(Document* doc)
{
	if (isActiveDocument(doc)) {
		emit selectionChanged();
	}
}

void DocumentManager::documentBriefChanged(Document* doc)
{
	emit briefChanged(doc);
}

void DocumentManager::documentRedoAvailable(Document* doc, bool available)
{
	if (isActiveDocument(doc)) {
		emit redoAvailable(available);
	}
}

void DocumentManager::documentUndoAvailable(Document* doc, bool available)
{
	if (isActiveDocument(doc)) {
		emit undoAvailable(available);
	}
}

bool DocumentManager::documentCloseRequest(Document* doc)
{
	return emit closeRequested(doc);
}

void DocumentManager::documentClosed(Document* doc)
{
	emit closed(doc);
}

void DocumentManager::customContextMenuRequest(Document* doc, const QPoint& point)
{
	emit customContextMenuRequested(doc, point);
}

void DocumentManager::documentFileWatchRequest(const QString& filePath, bool watch)
{
	emit fileWatchRequest(filePath, watch);
}
