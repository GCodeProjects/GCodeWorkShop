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

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QList>    // for QList
#include <QMap>     // for QMap
#include <QObject>  // for Q_OBJECT, slots
#include <QPointer> // for QPointer
#include <QString>  // for QString
#include <QtGlobal> // for QTypeInfo<>::isLarge, QTypeInfo<>::isStatic

class QMdiArea;
class QMdiSubWindow;
class QPoint;

#include <documentinfo.h>

#include "documentstyle.h"             // for DocumentStyle, DocumentStyle::Ptr
#include "documentwidgetproperties.h"  // for DocumentWidgetProperties, DocumentWidgetProperties::Ptr

class Document;
class DocumentProducer;


class DocumentManager : public QObject
{
	Q_OBJECT

public:
	explicit DocumentManager(QObject *parent = nullptr);
	~DocumentManager();

	/**
	 * @brief Returns the current active document.
	 *
	 * @return Pointer to the document or \c nullptr if there is no active document.
	 *
	 * @see activeDocumentChanged()
	 * @see setActiveDocument(Document *)
	 * @see setActiveDocument(const QString &)
	 */
	Document *activeDocument() const;

	bool isActiveDocument(Document *document);

	bool setActiveDocument(Document *doc);

	bool setActiveDocument(const QString &filePath);

signals:
	void activeDocumentChanged(Document *document);

public:
	/**
	 * @brief Returns the document with the specified identifier.
	 *
	 * @param id Document identifier.
	 *
	 * @return Pointer to the document or \c nullptr if there is no document with the specified identifier.
	 */
	Document *findDocumentById(const QString &id) const;

	Document *findDocumentByFilePath(const QString &filePath) const;

	QList<Document *> documentList() const;

	/**
	 * @brief Returns the DocumentStyle for the specified document type.
	 *
	 * @param type Document type.
	 *
	 * @return Returns an instance of a descendant of the DocumentStyle class for the specified document type,
	 * or an instance of the DocumentStyle class if an unknown type is specified.
	 */
	DocumentStyle::Ptr documentStyle(const QString &type) const;

	/**
	 * @brief Sets the DocumentStyle for the specified document type.
	 *
	 * @param style A descendant of the DocumentStyle class for the specified document type.
	 */
	void setDocumentStyle(const DocumentStyle::Ptr &style);

	/**
	 * @brief  Returns the DocumentWidgetProperties for the specified document type.
	 *
	 * @param type Document type.
	 *
	 * @return Returns an instance of a descendant of the DocumentWidgetProperties class for the specified
	 * document type, or an instance of the DocumentWidgetProperties class if an unknown type is specified.
	 */
	DocumentWidgetProperties::Ptr documentWidgetProperties(const QString &type) const;

	/**
	 * @brief Sets the DocumentWidgetProperties for the specified document type.
	 *
	 * @param property A descendant of the DocumentWidgetProperties class for the specified document type.
	 */
	void setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr &property);

	/**
	 * @brief Applies the new \c documentStyle and \c DocumentWidgetProperties to documents of the specified type.
	 *
	 * The method passes to already existing documents the values previously set using setDocumentStyle() and
	 * setDocumentWidgetProperties() methods.
	 *
	 * @param type Document type.
	 */
	void updateDocuments(const QString &type);

	/**
	 * @brief Creates an instance of a document of the specified type.
	 *
	 * The owner of the created document is the DocumentManager. If you need a document without a parent object,
	 * use DocumentProducer, which can be obtained using the documentProducer() method.
	 *
	 * @param type Document type.
	 * @param id Document identifier.
	 *
	 * @return Document or nullptr if the specified type is not registered.
	 */
	Document *createDocument(const QString &type, const QString &id);

	DocumentInfo::Ptr createDocumentInfo(const QString &type);
	DocumentStyle::Ptr createDocumentStyle(const QString &type);
	DocumentWidgetProperties::Ptr createDocumentWidgetProperties(const QString &type);

	/**
	 * @brief Returns the DocumentProducer for the specified document type.
	 *
	 * Since the owner of this DocumentProducer instance is the DocumentManager, you should not free it.
	 *
	 * @param type Document type.
	 *
	 * @return DocumentProducer or nullptr if the specified type is not registered.
	 */
	DocumentProducer *documentProducer(const QString &type) const;

	/**
	 * @brief Registers a DocumentProducer for the some document type.
	 *
	 * The DocumentManager becomes the owner of this DocumentProducer and responsible for freeing it.
	 *
	 * @param producer Registered DocumentProducer.
	 */
	void registerDocumentProducer(DocumentProducer *producer);

	void setMdiArea(QMdiArea *mdi);

signals:
	void documentListChanged();
	void modificationChanged(bool mod);
	void cursorPositionChanged();
	void selectionChanged();
	void briefChanged(Document *doc);
	void redoAvailable(bool available);
	void undoAvailable(bool available);
	bool closeRequested(Document *doc);
	void closed(Document *doc);
	void customContextMenuRequested(Document *doc, const QPoint &point);
	void fileWatchRequest(const QString &filePath, bool watch);

protected slots:
	void activeWindowChanged(QMdiSubWindow *window);
	void removeDocument(Document *document);
	void documentModificationChanged(Document *doc, bool mod);
	void documentCursorPositionChanged(Document *doc);
	void documentSelectionChanged(Document *doc);
	void documentBriefChanged(Document *doc);
	void documentRedoAvailable(Document *doc, bool available);
	void documentUndoAvailable(Document *doc, bool available);
	bool documentCloseRequest(Document *doc);
	void documentClosed(Document *doc);
	void customContextMenuRequest(Document *doc, const QPoint &point);
	void documentFileWatchRequest(const QString &filePath, bool watch);

protected:
	void addDocument(Document *document, const QString &id);

	class  DocumentListItem
	{
	public:
		QString id;
		Document *document;
		QMdiSubWindow *subWindow;
	};

	QMap<QString, DocumentProducer *> m_producers;
	QList<DocumentListItem> m_docList;
	QPointer<QMdiArea> m_area;
};

#endif // DOCUMENTMANAGER_H
