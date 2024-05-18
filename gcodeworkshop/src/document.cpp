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

#include <QCloseEvent>  // for QCloseEvent
#include <QFile>        // for QFile
#include <QFileInfo>    // for QFileInfo
#include <QIODevice>    // for QIODevice, QIODevice::ReadOnly, QIODevice::WriteOnly
#include <QSize>        // for QSize
#include <QSizePolicy>  // for QSizePolicy, QSizePolicy::Preferred
#include <Qt>           // for ConnectionType, DirectConnection, WheelFocus
#include <QtGlobal>     // for Q_UNUSED
#include <QWidget>      // for QWidget

#include <document.h>       // IWYU pragma: associated
#include <documentinfo.h>   // for DocumentInfo

#include "documentwidgetcloseeventfilter.h" // for DocumentWidgetCloseEventFilter


Document::Document(QObject* parent) :
	QObject(parent),
	m_widget(nullptr),
	m_dir(QDir::current())
{
	m_isUntitled = true;
	m_isModified = false;
	m_redoAvailable = false;
	m_undoAvailable = false;
}

Document::~Document()
{
	fileWatchStop();
}

QDir Document::dir() const
{
	return m_dir;
}

void Document::setDir(const QDir& dir)
{
	m_dir = dir;
}

QString Document::path() const
{
	return m_dir.path();
}

void Document::setPath(const QString& path)
{
	m_dir.setPath(path);
}

QString Document::fileName() const
{
	return  m_fileName;
}

void Document::setFileName(const QString& fileName)
{
	m_fileName = fileName;
}

QString Document::filePath() const
{
	return m_dir.filePath(m_fileName);
}

void Document::setFilePath(const QString& filePath)
{
	QFileInfo fi(filePath);
	m_fileName = fi.fileName();
	m_dir = fi.dir();
}

DocumentInfo::Ptr Document::documentInfo() const
{
	DocumentInfo* info = new DocumentInfo();
	info->filePath = filePath();
	info->geometry = widget()->parentWidget()->saveGeometry();
	info->readOnly = isReadOnly();
	return DocumentInfo::Ptr(info);
}

void Document::setDocumentInfo(const DocumentInfo::Ptr& info)
{
	if (!info->geometry.isEmpty()) {
		widget()->parentWidget()->restoreGeometry(info->geometry);
	}

	if (!info->filePath.isEmpty()) {
		setFilePath(info->filePath);
	}

	setReadOnly(info->readOnly);
}

QString Document::guessFileName() const
{
	return fileName();
}

QString Document::brief() const
{
	return m_brief;
}

void Document::setBrief(const QString& brief)
{
	if (m_brief != brief) {
		m_brief = brief;
		emit briefChanged(this);
	}
}

QWidget* Document::widget() const
{
	return m_widget;
}

void Document::setWidget(QWidget* widget)
{
	m_widget = widget;
	m_widget->setAttribute(Qt::WA_DeleteOnClose);
	m_widget->resize(430, 380);
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(m_widget->sizePolicy().hasHeightForWidth());
	m_widget->setSizePolicy(sizePolicy);
	m_widget->setMinimumSize(QSize(350, 300));
	m_widget->setFocusPolicy(Qt::WheelFocus);
	m_widget->setAutoFillBackground(true);

	DocumentWidgetCloseEventFilter* filter = new DocumentWidgetCloseEventFilter(m_widget);
	connect(filter, SIGNAL(closeRequested(QCloseEvent*)), this, SLOT(closeEvent(QCloseEvent*)),
	        Qt::ConnectionType::DirectConnection);
	m_widget->installEventFilter(filter);
}

void Document::setWidgetTitle(const QString& title)
{
	widget()->setWindowTitle(title + "[*]");
}

bool Document::close()
{
	return widget()->parentWidget()->close();
}

void Document::closeEvent(QCloseEvent* event)
{
	event->setAccepted(closeRequested(this));

	if (event->isAccepted()) {
		emit closed(this);
		deleteLater();
	}
}

QString Document::ioErrorString() const
{
	return m_ioErrorString;
}

bool Document::loadFile(const QString& filePath, bool watch)
{
	QFile file(filePath);
	m_ioErrorString.clear();

	if (!file.open(QIODevice::ReadOnly)) {
		m_ioErrorString = file.errorString();
		return false;
	}

	setRawData(file.readAll());
	file.close();

	if (watch) {
		fileWatchStart(filePath);
	}

	return true;
}

bool Document::saveFile(const QString& filePath)
{
	QFile file(filePath);
	m_ioErrorString.clear();

	// WARNING: If opened in QIODevice::ReadOnly mode, a false positive in QFileSystemWatcher
	// occurs after writing and closing. This is correct for Windows 7.
	if (!file.open(QIODevice::ReadWrite)) {
		m_ioErrorString = file.errorString();
		return false;
	}

	fileWatchStop();
	file.write(rawData());
	file.close();
	fileWatchStart(filePath);
	return true;
}

void Document::customContextMenuRequest(const QPoint& pos)
{
	emit customContextMenuRequested(this, pos);
}

bool Document::isUntitled() const
{
	return m_isUntitled;
}

void Document::setUntitled(bool untitled)
{
	m_isUntitled = untitled;
}

bool Document::isReadOnly() const
{
	return true;
}

void Document::setReadOnly(bool readonly)
{
	Q_UNUSED(readonly);
}

bool Document::isModified() const
{
	return m_isModified;
}

void Document::setModified(bool modified)
{
	bool oldMod = m_isModified;
	m_isModified = modified;

	if (oldMod != m_isModified) {
		widget()->setWindowModified(m_isModified);
		emit modificationChanged(this, m_isModified);
	}
}

void Document::cursorMoved()
{
	emit cursorPositionChanged(this);
}

void Document::selectionUpdated()
{
	emit selectionChanged(this);
}

void Document::redo()
{
}

void Document::undo()
{
}

void Document::clearUndoRedoStacks()
{
}

bool Document::isRedoAvailable() const
{
	return m_redoAvailable;
}

bool Document::isUndoAvailable() const
{
	return m_undoAvailable;
}

void Document::setRedoAvailable(bool available)
{
	bool oldRedo = m_redoAvailable;
	m_redoAvailable = available;

	if (oldRedo != m_redoAvailable) {
		emit redoAvailable(this, m_redoAvailable);
	}
}

void Document::setUndoAvailable(bool available)
{
	bool oldUndo = m_undoAvailable;
	m_undoAvailable = available;

	if (oldUndo != m_undoAvailable) {
		emit undoAvailable(this, m_undoAvailable);
	}
}

QString Document::watchedFile() const
{
	return m_watchedFile;
}

void Document::fileWatchStart(const QString& watchedFile)
{
	if (m_watchedFile != watchedFile) {
		fileWatchStop();
		m_watchedFile = watchedFile;
	}

	if (!m_watchedFile.isEmpty()) {
		emit fileWatchRequested(m_watchedFile, true);
	}
}

void Document::fileWatchStop()
{
	if (!m_watchedFile.isEmpty()) {
		emit fileWatchRequested(m_watchedFile, false);
	}
}
