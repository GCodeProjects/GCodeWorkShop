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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QByteArray>   // for QByteArray
#include <QDir>         // for QDir
#include <QObject>      // for QObject, Q_OBJECT, signals, slots
#include <QString>      // for QString

class QCloseEvent;
class QMenu;
class QPoint;
class QWidget;

#include <documentinfo.h>               // for DocumentInfo, DocumentInfo::Ptr
#include <documentstyle.h>              // for DocumentStyle, DocumentStyle::Ptr
#include <documentwidgetproperties.h>   // for DocumentWidgetProperties, DocumentWidgetProperties::Ptr


/**
 * @brief A base class for displaying and editing documents.
 *
 * The derived class must have a unique type returned by type(). The value returned by
 * brief() is used in a widget with a list of open documents. The guessFileName() function
 * is called when an untitled document is saved to guess a possible file name. Whether the
 * document is untitled can be checked using the isUntitled() function.
 *
 * The class has an associated widget, accessible using widget() function. The descendants
 * must call setWidget() in the constructor to associate the widget with the document.
 * The widget participates in the document's closing mechanism, for this purpose the class
 * sets the widget's close event filter. The closing chain of the class looks as follows:
 *
 * @li A call to close() leads to a call to Widget::close()
 * @li Widget::close() fires the QCloseEvent event
 * @li The QCloseEvent event is intercepted by the filter and the closeEvent() function
 *     of this class is called.
 * @li closeEvent() fires the closeRequested() signal.
 * @li If the closeRequest() signal handler returns @c false, the closing of the document
 *     and widget is canceled and close() returns @c false. Otherwise, the document and
 *     widget are closed and close() returns @c true.
 *
 * A document can have styles that determine how the widget displays content. For example,
 * for a text document, these are font type, font size, and highlighting colors. Styles
 * are managed by documentStyle() and setDocumentStyle() functions. Widget behavior is
 * controlled by properties, which are managed by documentWidgetProperties() and
 * setDocumentWidgetProperties(). Both the styles and properties of a particular document
 * type are independent of the content.  Instead, information about the document is
 * individualized and can be retrieved using documentInfo().
 *
 * Document stores the full path to the file from which the document is loaded or saved.
 * The full path can be obtained using filePath(). The full path is also contained in an
 * instance of the DocumentInfo class, which is returned by the documentInfo() function.
 * The full path is also used to monitor file changes on disk. Requests to enable and
 * disable file change monitoring are emitted when a file is loaded and saved. This
 * behavior is implemented in the class and does not require additional actions in the
 * class descendants.
 *
 * To load a document from a file, the application uses the load() function; this function
 * must be implemented in a descendant of the class. In general, the load() function calls
 * loadFile() with the filePath() argument and does some additional actions specific to a
 * particular document type. The loadFile() function is implemented in this class and does
 * not  require  overriding,  it reads  data  from  a file  and  passes  this  data to the
 * setRawData()  function. The  loadTemplate()  function  is  similar to  load(), but when
 * calling loadFile() it should additionally specify that tracking the file is unnecessary.
 *
 * Saving to a file is similar: the save() function calls saveFile() with the filePath()
 * argument, in turn saveFile() uses the rawData() function to get the data written to the
 * file.
 *
 * The class generates @ref fileWatchRequested() "requests to monitor" a file changes
 * on disk:
 *
 * @li Before saving or closing a document, a request to stop watching is emitted.
 * @li After loading or saving a document, a request to run watching is emitted.
 *
 * A descendant of a class may support undo and redo operations, in which case the
 * descendant must override redo(), undo(), and clearUndoRedoStacks(). The descendant must
 * also call setRedoAvailable() and setUndoAvailable() when the availability of the undo
 * and redo operation changes; this ensures that the isRedoAvailable() and
 * isUndoAvailable() functions work correctly.
 */
class Document : public QObject
{
	Q_OBJECT

protected:
	explicit Document(QObject* parent = nullptr);
	~Document();

public:
	/**
	 * @brief Returns the type of the document.
	 */
	virtual QString type() const = 0;

	/**
	 * @brief Returns the path to a file as a QDir object.
	 *
	 * @see setDir()
	 * @see path()
	 */
	QDir dir() const;

	/**
	 * @brief Sets the path to a file as a QDir object.
	 *
	 * @param dir The path to a file.
	 *
	 * @see dir()
	 */
	void setDir(const QDir& dir);

	/**
	 * @brief Returns the path to a file without a filename.
	 *
	 * @see setPath()
	 * @see fileName()
	 * @see filePath()
	 */
	QString path() const;

	/**
	 * @brief Sets the path to a file without a filename.
	 *
	 * @param path The path to a file.
	 *
	 * @see path()
	 */
	void setPath(const QString& path);

	/**
	 * @brief Returns the file name.
	 *
	 * @see setFileName()
	 * @see filePath()
	 * @see path()
	 * @see guessFileName()
	 */
	QString fileName() const;

	/**
	 * @brief sets the file name.
	 *
	 * @param fileName The file name.
	 *
	 * @see fileName()
	 */
	void setFileName(const QString& fileName);

	/**
	 * @brief Returns the full file name, including the path.
	 *
	 * @see setFilePath()
	 * @see path()
	 * @see fileName()
	 */
	QString filePath() const;

	/**
	 * @brief Sets the full file name, including the path.
	 *
	 * @param filePath The full file name.
	 *
	 * @see filePath()
	 */
	void setFilePath(const QString& filePath);

	/**
	 * @brief Returns the document information.
	 *
	 * The document information contains the state of the document that can be saved to
	 * disk. The base implementation contains the full file name, widget position and
	 * size, and whether the the document is readonly.
	 *
	 * @see setDocumentInfo()
	 */
	virtual DocumentInfo::Ptr documentInfo() const;

	/**
	 * @brief Sets the document information.
	 *
	 * The base implementation sets:
	 *
	 * @li The full file name on condition that the info->filePath field is not empty.
	 * @li The position and size of the widget.
	 * @li Whether the document is readonly.
	 *
	 * @param info The document information.
	 *
	 * @see documentInfo()
	 */
	virtual void setDocumentInfo(const DocumentInfo::Ptr& info);

	/**
	 * @brief Returns the document style.
	 */
	virtual DocumentStyle::Ptr documentStyle() const = 0;

	/**
	 * @brief Sets the document style.
	 *
	 * @param style The document style.
	 */
	virtual void setDocumentStyle(const DocumentStyle::Ptr& style) = 0;

	/**
	 * @brief Returns the document widget properties.
	 */
	virtual DocumentWidgetProperties::Ptr documentWidgetProperties() const = 0;

	/**
	 * @brief Sets the document widget properties.
	 *
	 * @param properties Document widget properties.
	 */
	virtual void setDocumentWidgetProperties(const DocumentWidgetProperties::Ptr& properties) = 0;

	/**
	 * @brief Guesses the file name from the contents of the document.
	 *
	 * @return The default implementation of the function returns fileName().
	 */
	virtual QString guessFileName() const;

	/**
	 * @brief Returns a brief description of the document.
	 *
	 * The short description is based on the content of the document. For example, for
	 * G-code it could be the program number or the first comment.
	 */
	QString brief() const;

protected:
	/**
	 * @brief Sets a short description of the document.
	 *
	 * If the description changes, the briefChanged() signal is emitted.
	 *
	 * @param brief Brief description of the document.
	 *
	 * @see brief()
	 */
	void setBrief(const QString& brief);

signals:
	/**
	 * @brief Emitted when the brief description is changed.
	 *
	 * @param doc This document.
	 *
	 * @see setBrief()
	 * @see brief()
	 */
	void briefChanged(Document* doc);

public:
	/**
	 * @brief Returns a widget of the document.
	 *
	 * Each document has its own widget for displaying and editing the document.
	 *
	 * @return A widget of the document.
	 *
	 * @see setWidget()
	 */
	QWidget* widget() const;

protected:
	/**
	 * @brief Sets the widget to display and edit the document.
	 *
	 * Class descendants must use this function in the constructor. The function specifies
	 * some widget properties and sets an event filter to handle the widget close event.
	 *
	 * @param widget The widget to display and edit the document.
	 *
	 * @see closeEvent()
	 */
	void setWidget(QWidget* widget);

	/**
	 * @brief Sets the widget title.
	 *
	 * @param title The widget title.
	 */
	void setWidgetTitle(const QString& title);

public:
	/**
	 * @brief Closes the document.
	 *
	 * An attempt to close the document results in an attempt to close the widget, which
	 * in turn emits the closeRequested() signal. If the signal handler returns @c false,
	 * the closing of the document and the widget is canceled.
	 *
	 * @return Returns @c true if the document is closed.
	 */
	bool close();

protected slots:
	/**
	 * @brief Widget close event handler.
	 *
	 * The setWidget() function sets the event filter for the widget close event.
	 * The closing event is redirected to this slot.
	 *
	 * The slot emits the closeRequested() signal. If the signal returns @c true, the widget
	 * and document are closed.
	 *
	 * @param event Widget closing event.
	 *
	 * @see close()
	 */
	void closeEvent(QCloseEvent* event);

signals:
	/**
	 * @brief Emitted when requesting to close a document.
	 *
	 * The signal connection to the slot must be with the
	 * Qt::ConnectionType::DirectConnection flag.
	 *
	 * @param doc This document.
	 *
	 * @return Returns @c true if document closing is allowed.
	 *
	 * @see close()
	 * @see closeEvent()
	 */
	bool closeRequested(Document* doc);

	/**
	 * @brief Emitted before the document is closed.
	 *
	 * @param doc This document.
	 *
	 * @see close()
	 */
	void closed(Document* doc);

public:
	/**
	 * @brief Loads a document from a template.
	 *
	 * Unlike load(), loading a template does not reset the untitled flag.
	 *
	 * @param fileName The full path and filename of the document template file.
	 *
	 * @see isUntitled()
	 */
	virtual void loadTemplate(const QString& fileName = QString()) = 0;

	/**
	 * @brief Loads a document from a file.
	 *
	 * The file name and path must be specified using setFilePath() or setDocumentInfo()
	 * functions.
	 *
	 * If the file is successfully loaded, the untitled flag is reset. If it fails, the
	 * reason can be found out using the ioErrorString() function.
	 *
	 * @return Returns true on success.
	 *
	 * @see isUntitled()
	 */
	virtual bool load() = 0;

	/**
	 * @brief Saves the document to a file.
	 *
	 * The file name and path must be specified using setFilePath() or setDocumentInfo()
	 * functions.
	 *
	 * If the saving is successful, the untitled flag is reset.  If it fails, the reason
	 * can be found out using the ioErrorString() function.
	 *
	 * @return Returns true on success.
	 *
	 * @see isUntitled()
	 */
	virtual bool save() = 0;

	/**
	 * @brief Returns the reason for an I/O error.
	 *
	 * If the load() or save() functions return false, this function returns a description
	 * of the error that occurred.
	 *
	 * @return Text describing the I/O error.
	 */
	QString ioErrorString() const;

protected:
	/**
	 * @brief Loads the specified file into the document.
	 *
	 * The function reads binary data from the file and passes it to setRawData(). No data
	 * transformations are performed.
	 *
	 * If the file is successfully loaded, the untitled flag is reset. If it fails,
	 * the reason can be found out using the ioErrorString() function.
	 *
	 * @param filePath Full file name, including path.
	 * @param watch Whether to request watch for the specified file.
	 *
	 * @return Returns true on success.
	 */
	bool loadFile(const QString& filePath, bool watch = true);

	/**
	 * @brief Saves the document to the specified file.
	 *
	 * The function obtains binary data from rawData() and writes it to a file. No data
	 * transformations are performed.
	 *
	 * If the file is successfully saved, the untitled flag is reset. If it fails,
	 * the reason can be found out using the ioErrorString() function.
	 *
	 * @param filePath Full file name, including path.
	 *
	 * @return Returns true on success.
	 */
	bool saveFile(const QString& filePath);

public:
	/**
	 * @brief Returns the raw data.
	 *
	 * Raw data is suitable for direct writing to a file.
	 */
	virtual QByteArray rawData() const = 0;

	/**
	 * @brief Sets the raw data.
	 *
	 * Raw data is usually obtained when reading from a file.
	 *
	 * @param data The raw data.
	 */
	virtual void setRawData(const QByteArray& data) = 0;

	/**
	 * @brief Creates the standard context menu.
	 *
	 * The function should create a standard context menu which is shown when the right
	 * mouse button is clicked.
	 */
	virtual QMenu* createStandardContextMenu(const QPoint& pos) = 0;

protected slots:
	/**
	 * @brief This slot catches the QWidget::customContextMenuRequested() from
	 * the document widget and emits the customContextMenuRequested() signal.
	 *
	 * @param pos The position of the mouse pointer relative to the document widget.
	 *
	 * @see  QWidget::mapToGlobal()
	 */
	void customContextMenuRequest(const QPoint& pos);

signals:
	/**
	 * @brief A custom context menu was requested.
	 *
	 * @param doc This document.
	 * @param pos The position of the mouse pointer relative to the document widget.
	 */
	void customContextMenuRequested(Document* doc, const QPoint& pos);

public:
	/**
	 * @brief Whether the document is untitled.
	 *
	 * This function returns @c true if the document was created empty or from
	 * a template (using loadTemplate()) and has not yet been saved to a file.
	 * After the first successful call to load() or save(), this function will
	 * always return @c false.
	 *
	 * @return Returns @c true if the document is untitled.
	 *
	 * @see setUntitled()
	 */
	bool isUntitled() const;

protected:
	/**
	 * @brief Sets the untitled flag.
	 *
	 * @param untitled The new value of the untitled flag.
	 *
	 * @see isUntitled()
	 */
	void setUntitled(bool untitled);

public:
	/**
	 * @brief Whether the document is readonly.
	 *
	 * @see setReadOnly()
	 */
	virtual bool isReadOnly() const;

	/**
	 * @brief Sets the readonly flag.
	 *
	 * @param readonly The new value of the readonly flag.
	 *
	 * @see isReadOnly()
	 */
	virtual void setReadOnly(bool readonly);

	/**
	 * @brief Whether the document has been modified.
	 *
	 * @return Returns true if the document has been modified.
	 *
	 * @see setModified()
	 * @see modificationChanged()
	 */
	bool isModified() const;

public slots:
	/**
	 * @brief Sets the document modification flag.
	 *
	 * If the flag state has changed, the modificationChanged() signal is emitted
	 * and the widget's modification flag is updated.
	 *
	 * @param modified The new value of the document modification flag.
	 *
	 * @see isModified()
	 */
	void setModified(bool modified);

signals:
	/**
	 * @brief The document modification status has changed.
	 *
	 * @param doc This document.
	 * @param mod The current status of the document modification.
	 *
	 * @see setModified()
	 * @see isModified()
	 */
	void modificationChanged(Document* doc, bool mod);

public:
	/**
	 * @brief Redoes the last operation.
	 *
	 * @see isRedoAvailable()
	 */
	virtual void redo();

	/**
	 * @brief Undoes the last operation.
	 *
	 * @see isUndoAvailable()
	 */
	virtual void undo();

	/**
	 * @brief Clears the undo/redo stacks.
	 *
	 * @see isRedoAvailable()
	 * @see isUndoAvailable()
	 */
	virtual void clearUndoRedoStacks();

	/**
	 * @brief Returns @c true if @ref redo() "redo" is available; otherwise returns false.
	 */
	bool isRedoAvailable() const;

	/**
	 * @brief Returns @c true if @ref undo() "undo" is available; otherwise returns false.
	 */
	bool isUndoAvailable() const;

protected slots:
	/**
	 * @brief Sets the availability of redo.
	 *
	 * If the availability has changed, the redoAvailable() signal is emitted.
	 *
	 * @param available Redo availability.
	 *
	 * @see isRedoAvailable()
	 */
	void setRedoAvailable(bool available);

	/**
	 * @brief Sets the availability of undo.
	 *
	 * If the availability has changed, the undoAvailable() signal is emitted.
	 *
	 * @param available Undo availability.
	 *
	 * @see isUndoAvailable()
	 */
	void setUndoAvailable(bool available);

signals:
	/**
	 * @brief The redo availability has changed.
	 *
	 * @param doc This document.
	 * @param available Redo availability.
	 *
	 * @see setRedoAvailable()
	 */
	void redoAvailable(Document* doc, bool available);

	/**
	 * @brief The undo availability has changed.
	 *
	 * @param doc This document.
	 * @param available Undo availability.
	 *
	 * @see setUndoAvailable()
	 */
	void undoAvailable(Document* doc, bool available);

protected slots:
	/**
	 * @brief Emits the cursorPositionChanged() signal.
	 */
	void cursorMoved();

signals:
	/**
	 * @brief Emitted when the @ref cursorMoved() "cursor position is changed".
	 *
	 * @param doc This document.
	 */
	void cursorPositionChanged(Document* doc);

protected slots:
	/**
	 * @brief Emits the selectionChanged() signal.
	 */
	void selectionUpdated();

signals:
	/**
	 * @brief Emitted when the @ref selectionUpdated() "selection is changed".
	 *
	 * @param doc This document.
	 */
	void selectionChanged(Document* doc);

public:
	/**
	 * @brief Returns the name of the watched file.
	 *
	 * This is the name of the file with which the last successful call to loadFile() or
	 * saveFile() was made. If these two functions have not been called yet, an empty
	 * string will be returned.
	 */
	QString watchedFile() const;

protected:
	/**
	 * @brief Starts watching the file.
	 *
	 * @param watchedFile The name of the watched file.
	 *
	 * @see fileWatchRequested()
	 */
	void fileWatchStart(const QString& watchedFile);

	/**
	 * @brief Stops watching the file.
	 *
	 * @see fileWatchRequested()
	 */
	void fileWatchStop();

signals:
	/**
	 * @brief Emitted when file watching is requested.
	 *
	 * @param filePath The name of the watched file.
	 * @param watch @c true if file monitoring is requested to start; @c false if file
	 *        monitoring is requested to stop.
	 *
	 * @see fileWatchStart()
	 * @see fileWatchStop()
	 */
	void fileWatchRequested(const QString& filePath, bool watch);

private:
	QString m_brief;
	QWidget* m_widget;
	QDir m_dir;
	QString m_fileName;
	QString m_ioErrorString;
	QString m_watchedFile;
	bool m_isUntitled;
	bool m_isModified;
	bool m_redoAvailable;
	bool m_undoAvailable;
};

#endif // DOCUMENT_H
