/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of GCodeWorkShop.                                   *
 *                                                                         *
 *   GCodeWorkShop is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <memory>   // for shared_ptr, shared_ptr<>::element_type

#include <QAbstractItemModel>   // for QTypeInfo<>::isLarge, QTypeInfo<>::isStatic
#include <QAbstractPrintDialog> // for QAbstractPrintDialog, QAbstractPrintDialog::PrintSelection
#include <QAction>              // for QAction
#include <QActionGroup>         // for QActionGroup
#include <QApplication>         // for QApplication, qApp
#include <QByteArray>           // for QByteArray
#include <QCheckBox>            // for QCheckBox
#include <QClipboard>           // for QClipboard
#include <QCloseEvent>          // for QCloseEvent
#include <QColor>               // for QColor
#include <QComboBox>            // for QComboBox, QComboBox::AdjustToContents
#include <QDesktopServices>     // for QDesktopServices
#include <QDialog>              // for QDialog, QDialog::Accepted
#include <QDir>                 // for QDir, operator|, QDir::Files, QDir::Hidden, QDir::Name, QDir::NoSymLinks
#include <QDragEnterEvent>      // for QDragEnterEvent
#include <QDropEvent>           // for QDropEvent
#include <QEvent>               // for QEvent, QEvent::ToolTip
#include <QFile>                // for QFile
#include <QFileDialog>          // for QFileDialog, QFileDialog::DontConfirmOverwrite
#include <QFileIconProvider>    // for QFileIconProvider
#include <QFileInfo>            // for QFileInfo, QFileInfoList
#include <QFileSystemModel>     // for QFileSystemModel
#include <QFileSystemWatcher>   // for QFileSystemWatcher
#include <QFont>                // for QFont
#include <QFrame>               // for QFrame, QFrame::Box, QFrame::Sunken
#include <QHeaderView>          // for QHeaderView, QHeaderView::ResizeToContents, QHeaderView::Stretch, QHea...
#include <QHelpEvent>           // for QHelpEvent
#include <QIODevice>            // for QIODevice, QIODevice::ReadWrite, operator|, QIODevice::ReadOnly, QIODe...
#include <QIcon>                // for QIcon
#include <QItemSelectionModel>  // for QItemSelectionModel
#include <QKeySequence>         // for QKeySequence, QKeySequence::Back, QKeySequence::Copy, QKeySequence::Cut
#include <QLabel>               // for QLabel
#include <QLineEdit>            // for QLineEdit
#include <QList>                // for QList, QList<>::iterator, QList<>::const_iterator, QList<>::Iterator
#include <QMainWindow>          // for QMainWindow
#include <QMdiArea>             // for QMdiArea, QMdiArea::SubWindowView, QMdiArea::TabbedView
#include <QMdiSubWindow>        // for QMdiSubWindow
#include <QMenu>                // for QMenu
#include <QMenuBar>             // for QMenuBar
#include <QMessageBox>          // for QMessageBox, operator|, QMessageBox::Save, QMessageBox::Discard, QMess...
#include <QMimeData>            // for QMimeData
#include <QModelIndex>          // for QModelIndex
#include <QModelIndexList>      // for QModelIndexList
#include <QObject>              // for SIGNAL, SLOT, qobject_cast, emit
#include <QPageLayout>          // for QPageLayout, QPageLayout::Orientation
#include <QPageSize>            // for QPageSize, QPageSize::A4, QPageSize::PageSizeId
#include <QPalette>             // for QPalette, QPalette::Base
#include <QPoint>               // for QPoint
#include <QPointer>             // for QPointer
#include <QPrintDialog>         // for QPrintDialog
#include <QPrintPreviewDialog>  // for QPrintPreviewDialog
#include <QPrinter>             // for QPrinter, QPrinter::HighResolution, QPrinter::NativeFormat, QPrinter::...
#include <QProcess>             // for QProcess
#include <QPushButton>          // for QPushButton
#include <QRect>                // for QRect
#include <QRegularExpression>   // for QRegularExpression
#include <QResizeEvent>         // for QResizeEvent
#include <QSettings>            // for QSettings, QSettings::IniFormat, QSettings::NoError
#include <QSignalMapper>        // for QSignalMapper
#include <QSize>                // for QSize
#include <QSpinBox>             // for QSpinBox
#include <QSplitter>            // for QSplitter
#include <QStandardItemModel>   // for QStandardItem, QStandardItemModel
#include <QStatusBar>           // for QStatusBar
#include <QString>              // for QString, operator+, operator!=, operator==
#include <QStringList>          // for QStringList
#include <QTabBar>              // for QTabBar
#include <QTabWidget>           // for QTabWidget
#include <QTableWidget>         // for QTableWidget, QTableWidgetItem::UserType
#include <QTableWidgetItem>     // for QTableWidgetItem
#include <QTextCursor>          // for QTextCursor
#include <QTextDocument>        // for QTextDocument, QTextDocument::FindFlags, QTextDocument::FindCaseSensit...
#include <QTextStream>          // for QTextStream
#include <QToolBar>             // for QToolBar
#include <QToolButton>          // for QToolButton
#include <QToolTip>             // for QToolTip
#include <QTreeView>            // for QTreeView
#include <QUrl>                 // for QUrl, QUrl::TolerantMode
#include <QVariant>             // for QVariant
#include <QWidget>              // for QWidget
#include <Qt>                   // for operator|, red, WindowStates, BusyCursor, Dialog, MouseFocusReason
#include <QtGlobal>             // for QFlags, QT_VERSION, QT_VERSION_CHECK, qMakeForeachContainer, Q_OS_LINUX

#include <addons-actions.h>             // for Addons::Actions
#include <document.h>                   // for Document
#include <documentinfo.h>               // for DocumentInfo::Ptr, DocumentInfo
#include <documentmanager.h>            // for DocumentManager
#include <documentstyle.h>              // for DocumentStyle::Ptr, DocumentStyle
#include <documentwidgetproperties.h>   // for DocumentWidgetProperties::Ptr, DocumentWidgetProperties
#include <gcodefileserver.h>            // for GCodeFileServer
#include <gcoderdocument.h>             // for GCoderDocument
#include <gcoderstyle.h>                // for GCoderStyle
#include <gcoderwidgetproperties.h>     // for GCoderWidgetProperties
#include <gcodeworkshop.h>              // IWYU pragma: associated
#include <kdiff3/kdiff3.h>              // KDiff3App
#include <kdiff3/common.h>              // getFilters()
#include <serialportconfigdialog.h>     // SerialPortConfigDialog
#include <serialporttestdialog.h>       // SerialPortTestDialog
#include <serialtransmissiondialog.h>   // SerialTransmissionDialog
#include <utils/medium.h>               // Medium
#include <version.h>

#include "capslockeventfilter.h"    // for CapsLockEventFilter
#include "findinf.h"                // for FindInFiles
#include "gcoder.h"                 // for DOCUMENT_TYPE
#include "gcoderinfo.h"             // for GCoderInfo
#include "gcoderproducer.h"         // for GCoderProducer
#include "highlightmode.h"          // for MODE_AUTO, MODE_FANUC, MODE_HEIDENHAIN, MODE_HEIDENHAIN_ISO, MODE_LINU...
#include "newfiledialog.h"          // for newFileDialog
#include "recentfiles.h"            // for RecentFiles
#include "sessiondialog.h"          // for SessionDialog
#include "sessionmanager.h"         // for SessionManager
#include "setupdialog.h"            // for AppConfig, SetupDialog
#include "tooltips.h"               // for writeTooltipFile
#include "ui_gcodeworkshop.h"       // for Ui::GCodeWorkShop


#define EXAMPLES_PATH             "/usr/share/gcodeworkshop/EXAMPLES"

GCodeWorkShop* GCodeWorkShop::SINGLETON;

GCodeWorkShop* GCodeWorkShop::instance()
{
	if (SINGLETON == 0) {
		SINGLETON = new GCodeWorkShop(&Medium::instance());
	}

	return SINGLETON;
}

GCodeWorkShop::GCodeWorkShop(Medium* medium)
	: QMainWindow(nullptr)
{
	mMedium = medium;

	setAttribute(Qt::WA_DeleteOnClose);

	ui = new Ui::GCodeWorkShop();
	ui->setupUi(this);
	setAcceptDrops(true);

	findToolBar = nullptr;
	serialToolBar = nullptr;
	diffApp = nullptr;
	findFiles = nullptr;
	dirModel = nullptr;
	openExampleAct = nullptr;
	m_fileServer = nullptr;

	m_MdiWidgetsMaximized = true;
	m_defaultReadOnly = false;
	m_startEmpty = false;
	m_disableFileChangeMonitor = false;
	m_MdiTabbedMode = false;
	m_findInFilesHighlightEnable = true;

	m_extensions << "*.nc" <<  "*.cnc";
	m_saveExtension = "*.nc";
	m_saveDirectory = QDir::homePath();

	clipboard = QApplication::clipboard();
	connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
	windowMapper = new QSignalMapper(this);
	connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

	model = new QStandardItemModel();
	ui->projectTreeView->setModel(model);
	currentProject = nullptr;

	connect(ui->projectTreeView, SIGNAL(doubleClicked(QModelIndex)), this,
	        SLOT(projectTreeViewDoubleClicked(QModelIndex)));
	connect(ui->projectNewButton, SIGNAL(clicked()), this, SLOT(projectNew()));
	connect(ui->projectAddButton, SIGNAL(clicked()), this, SLOT(projectAdd()));
	connect(ui->projectSaveButton, SIGNAL(clicked()), this, SLOT(projectSave()));
	connect(ui->projectSaveAsButton, SIGNAL(clicked()), this, SLOT(projectSaveAs()));
	connect(ui->projectLoadButton, SIGNAL(clicked()), this, SLOT(projectOpen()));
	connect(ui->projectRemoveButton, SIGNAL(clicked()), this, SLOT(projectTreeRemoveItem()));

	clipboardModel = new QStandardItemModel();
	ui->clipboardTreeView->setModel(clipboardModel);
	ui->clipboardTreeView->setRootIsDecorated(false);
	ui->clipboardTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->deleteFromClipboardButton, SIGNAL(clicked()), this,
	        SLOT(deleteFromClipboardButtonClicked()));
	connect(ui->clipboardTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this,
	        SLOT(clipboardTreeViewContextMenu(const QPoint&)));

	connect(ui->hideButton, SIGNAL(clicked()), this, SLOT(hidePanel()));

	currentProjectModified = false;

	m_recentFiles = new RecentFiles(this);
	connect(m_recentFiles, SIGNAL(fileListChanged(QStringList)), this, SLOT(updateRecentFilesMenu(QStringList)));
	connect(m_recentFiles, SIGNAL(saveRequest()), this, SLOT(recentFilesChanged()));

	m_addonsActions = new Addons::Actions(this);
	createActions();
	createToolBars();
	createStatusBar();
	createFileBrowseTabs();

	m_documentManager = new DocumentManager(this);
	m_documentManager->setMdiArea(ui->mdiArea);
	m_documentManager->registerDocumentProducer(new GCoderProducer());
	connect(m_documentManager, SIGNAL(activeDocumentChanged(Document*)), this, SLOT(updateMenus()));
	connect(m_documentManager, SIGNAL(cursorPositionChanged()), this, SLOT(updateStatusBar()));
	connect(m_documentManager, SIGNAL(modificationChanged(bool)), this, SLOT(updateMenus()));
	connect(m_documentManager, SIGNAL(modificationChanged(bool)), this, SLOT(updateOpenFileList()));
	connect(m_documentManager, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));
	connect(m_documentManager, SIGNAL(briefChanged(Document*)), this, SLOT(updateOpenFileList()));
	connect(m_documentManager, SIGNAL(documentListChanged()), this, SLOT(updateOpenFileList()));
	connect(m_documentManager, SIGNAL(closeRequested(Document*)), this, SLOT(maybeSave(Document*)),
	        Qt::ConnectionType::DirectConnection);
	connect(m_documentManager, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));
	connect(m_documentManager, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));
	connect(m_documentManager, SIGNAL(customContextMenuRequested(Document*, const QPoint&)), this,
	        SLOT(customContextMenuRequest(Document*, const QPoint&)));
	connect(m_documentManager, SIGNAL(fileWatchRequest(const QString&, bool)), this, SLOT(watchFile(const QString&, bool)));

	m_sessionManager = new SessionManager(m_documentManager, this);
	connect(m_sessionManager, SIGNAL(sessionListChanged(QStringList)), this, SLOT(updateSessionMenus(QStringList)));
	connect(m_sessionManager, SIGNAL(beforeCurrentSessionChanged()), this, SLOT(beforeCurrentSessionChanged()));
	connect(m_sessionManager, SIGNAL(currentSessionChanged()), this, SLOT(currentSessionChanged()));
	connect(m_sessionManager, SIGNAL(saveRequest()), this, SLOT(sessionsChanged()));

	createMenus();
	updateMenus();

	setWindowTitle(tr("GCodeWorkShop"));
	setWindowIcon(QIcon(":/images/edytornc.png"));

	readSettings();
	clipboardLoad();
}

GCodeWorkShop::~GCodeWorkShop()
{
	proc = findChild<QProcess*>();

	if (proc) {
		proc->close();
		delete (proc);
	}

	if (m_fileServer) {
		m_fileServer->close();
	}

	delete ui;
}

void GCodeWorkShop::resizeEvent(QResizeEvent* event)
{
	if (windowState() == Qt::WindowNoState && event->oldSize().isValid()) {
		mMWConfig.size = event->size();
	}

	QMainWindow::resizeEvent(event);
}

void GCodeWorkShop::moveEvent(QMoveEvent* event)
{
	if (windowState() == Qt::WindowNoState) {
		mMWConfig.pos = geometry().topLeft();
	}

	QMainWindow::moveEvent(event);
}

void GCodeWorkShop::dragEnterEvent(QDragEnterEvent* event)
{
	if (!event->mimeData()->hasUrls()) {
		QMainWindow::dragEnterEvent(event);
		return;
	}

	event->acceptProposedAction();
}

void GCodeWorkShop::dropEvent(QDropEvent* event)
{
	if (!event->mimeData()->hasUrls()) {
		QMainWindow::dropEvent(event);
		return;
	}

	for (QUrl url : event->mimeData()->urls()) {
		openFile(url.toString(QUrl::PreferLocalFile));
	}

	event->acceptProposedAction();
}

Addons::Actions* GCodeWorkShop::addonsActions()
{
	return m_addonsActions;
}

DocumentManager* GCodeWorkShop::documentManager() const
{
	return m_documentManager;
}

void GCodeWorkShop::setMdiTabbedMode(bool tabbed)
{
	m_MdiTabbedMode = tabbed;

	if (m_MdiTabbedMode) {
		ui->mdiArea->setViewMode(QMdiArea::TabbedView);
		QTabBar* tab = ui->mdiArea->findChild<QTabBar*>();

		if (tab) {
			tab->setTabsClosable(true);
			// The tabs might be very wide
			tab->setExpanding(false);
		}
	} else {
		ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
	}
}

void GCodeWorkShop::closeCurrentWindow()
{
	ui->mdiArea->closeActiveSubWindow();
}

void GCodeWorkShop::closeAllMdiWindows()
{
	ui->mdiArea->closeAllSubWindows();
}

void GCodeWorkShop::closeEvent(QCloseEvent* event)
{
	if (m_fileServer) {
		QMessageBox::StandardButton result = QMessageBox::warning(this,
		                                     tr("GCodeWorkShop - Serial port file server"),
		                                     tr("Serial port file server is running.\nClose anyway?"),
		                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if (result == QMessageBox::No) {
			event->ignore();
			return;
		}
	}

	if (!maybeSaveProject() || !maybeSaveAll()) {
		event->ignore();
		return;
	}

	writeSettings();
	clipboardSave();
	closeAllMdiWindows();

	if (findFiles != nullptr) {
		findFiles->close();
		findFiles = nullptr;
	}
}

Document* GCodeWorkShop::newFileFromTemplate()
{
	Document* doc = 0;

	newFileDialog* newFileDlg = new newFileDialog(this);
	int result = newFileDlg->exec();

	if (result == QDialog::Accepted) {
		const QString& fileName = newFileDlg->getChosenFile();
		doc = createDocument(GCoder::DOCUMENT_TYPE);

		if (!doc) {
			return nullptr;
		}

		if (!fileName.isEmpty() && !(fileName == tr("EMPTY FILE"))) {
			doc->loadTemplate(fileName);
		} else {
			doc->loadTemplate();
		}

		// TODO replace with DocumentProducer::createDocumentInfo
		DocumentInfo::Ptr info = DocumentInfo::Ptr(new GCoderInfo());
		doc->setDocumentInfo(info);
	}

	delete (newFileDlg);

	return doc;
}

//**************************************************************************************************
//
//**************************************************************************************************

Document* GCodeWorkShop::newFile()
{
	Document* doc = createDocument(GCoder::DOCUMENT_TYPE);

	if (!doc) {
		return nullptr;
	}

	doc->loadTemplate();
	return doc;
}

//**************************************************************************************************
//
//**************************************************************************************************

void GCodeWorkShop::open(const QDir& dir)
{
	const QString& filters = getFilters(m_extensions);

	QStringList files = QFileDialog::getOpenFileNames(
	                        this,
	                        tr("Select one or more files to open"),
	                        dir.canonicalPath(),
	                        filters, 0);

	for (const QString& fileName : files) {
		openFile(fileName);
	}
}

void GCodeWorkShop::open()
{
	open(currentPath());
	statusBar()->showMessage(tr("File loaded"), 5000);
}

void GCodeWorkShop::openExample()
{
	QString dir;

	// TODO: change a path to the files
	if (QDir(EXAMPLES_PATH).exists()) {
		dir = EXAMPLES_PATH;
	} else {
		dir = QApplication::applicationDirPath() + "/" + "EXAMPLES";
	}

	open(dir);
	statusBar()->showMessage(tr("File loaded"), 5000);
}

void GCodeWorkShop::openFile(const QString& fileName)
{
	GCoderInfo* info = new GCoderInfo();
	info->filePath = fileName;
	info->readOnly = m_defaultReadOnly;
	info->highlightMode = defaultHighlightMode(QFileInfo(fileName).absolutePath());
	loadFile(DocumentInfo::Ptr(info), true);
}

bool GCodeWorkShop::save(Document* doc, bool forceSaveAs)
{
	if (doc->isUntitled() || forceSaveAs) {
		QString oldFileName;

#ifdef Q_OS_LINUX
		QString extText = tr("CNC programs files %1 (%1);;");
#elif defined Q_OS_WIN32
		QString extText = tr("CNC programs files (%1);;");
#elif defined Q_OS_MACX
		QString extText = tr("CNC programs files %1 (%1);;");
#endif

		QString filters = extText.arg(m_saveExtension);

		for (const QString& ext : m_extensions) {
			QString saveExt = extText.arg(ext);

			if (ext != m_saveExtension) {
				filters.append(saveExt);
			}
		}

		filters.append(tr("Text files (*.txt);;" "All files (*.* *)"));

		if (doc->isUntitled()) {
			oldFileName = doc->guessFileName();
		} else {
			oldFileName = doc->fileName();
		}

		if (QFileInfo(oldFileName).suffix() == "") {
			// sometimes when file has no extension QFileDialog::getSaveFileName will no apply choosen filter (extension)
			oldFileName.append(".nc");
		}

		QString newFileName = QFileDialog::getSaveFileName(
		                          this,
		                          tr("Save file as..."),
		                          doc->dir().filePath(oldFileName),
		                          filters, nullptr, QFileDialog::DontConfirmOverwrite);

		if (newFileName.isEmpty() || newFileName.isNull()) {
			return false;
		}

		if (QFile::exists(newFileName)) {
			QMessageBox msgBox;
			msgBox.setParent(this, Qt::Dialog);
			msgBox.setText(tr("<b>File \"%1\" exists.</b>").arg(newFileName));
			msgBox.setInformativeText(tr("Do you want overwrite it ?"));
			msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
			msgBox.setDefaultButton(QMessageBox::Discard);
			msgBox.setIcon(QMessageBox::Warning);

			if (msgBox.exec() != QMessageBox::Save) {
				return false;
			}
		}

		doc->setFilePath(newFileName);
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	bool status = doc->save();
	QApplication::restoreOverrideCursor();
	return status;
}

bool GCodeWorkShop::save()
{
	Document* doc = activeDocument();

	if (!doc) {
		return true;
	}

	bool saved = save(doc, false);

	if (saved) {
		statusBar()->showMessage(tr("File saved"), 5000);
	} else {
		QMessageBox::warning(this, tr("GCodeWorkShop"), tr("Cannot write file \"%1\".\n %2")
		                     .arg(doc->filePath()).arg(doc->ioErrorString()));
	}

	return saved;
}

bool GCodeWorkShop::saveAll()
{
	bool saved = true;
	int i = 0;

	for (Document* doc : m_documentManager->documentList()) {
		if (doc->isModified()) {
			if (save(doc, false)) {
				i++;
			} else {
				saved = false;
				QMessageBox::warning(this, tr("GCodeWorkShop"), tr("Cannot write file \"%1\".\n %2")
				                     .arg(doc->filePath()).arg(doc->ioErrorString()));
			}
		}
	}

	statusBar()->showMessage(tr("Saved %1 files").arg(i), 5000);
	return saved;
}

bool GCodeWorkShop::saveAs()
{
	Document* doc = activeDocument();

	if (!doc) {
		return true;
	}

	bool saved = save(doc, true);

	if (saved) {
		statusBar()->showMessage(tr("File saved"), 5000);
	} else {
		QMessageBox::warning(this, tr("GCodeWorkShop"), tr("Cannot write file \"%1\".\n %2")
		                     .arg(doc->filePath()).arg(doc->ioErrorString()));
	}

	return saved;
}

bool GCodeWorkShop::maybeSaveAll()
{
	bool saved = true;

	for (Document* doc : m_documentManager->documentList()) {
		if (!maybeSave(doc)) {
			saved = false;
		}
	}

	return saved;
}

bool GCodeWorkShop::maybeSave(Document* doc)
{
	if (doc->isModified()) {
		QMessageBox msgBox;
		msgBox.setParent(this, Qt::Dialog);
		msgBox.setText(tr("<b>File: \"%1\"\n has been modified.</b>").arg(doc->filePath()));
		msgBox.setInformativeText(tr("Do you want to save your changes ?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setIcon(QMessageBox::Warning);
		int ret = msgBox.exec();

		switch (ret) {
		case QMessageBox::Save:
			return save(doc, false);
			break;

		case QMessageBox::Discard:
			doc->setModified(false);
			return true;
			break;

		case QMessageBox::Cancel:
			return false;
			break;

		default:
			return true;
			break;
		}
	}

	return true;
}

void GCodeWorkShop::printFile()
{
#ifndef QT_NO_PRINTER

	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		QPrinter printer(QPrinter::HighResolution);
		loadPrinterSettings(&printer);

		printer.setOutputFormat(QPrinter::NativeFormat);

		QPrintDialog dialog(&printer, this);
		dialog.setWindowTitle(tr("Print Document"));

		if (gdoc->hasSelection()) {
			dialog.setOption(QAbstractPrintDialog::PrintSelection);
			printer.setPrintRange(QPrinter::Selection);
		}

		if (dialog.exec() == QDialog::Accepted) {
			printer.setDocName(gdoc->fileName());
			gdoc->print(&printer);
			statusBar()->showMessage(tr("The document was sent to a printer %1...").arg(
			                             printer.printerName()), 5000);
			savePrinterSettings(&printer);
		}
	}

#endif
}

void GCodeWorkShop::filePrintPreview()
{
#ifndef QT_NO_PRINTER

	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		QPrinter printer(QPrinter::HighResolution);
		loadPrinterSettings(&printer);

		printer.setOutputFormat(QPrinter::NativeFormat);

		if (gdoc->hasSelection()) {
			printer.setPrintRange(QPrinter::Selection);
		}

		QPrintPreviewDialog preview(&printer, this);
		preview.setWindowFlags(Qt::Window);
		connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
		preview.exec();
		savePrinterSettings(&printer);
	}

#endif
}

void GCodeWorkShop::printPreview(QPrinter* printer)
{
#ifndef QT_NO_PRINTER

	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		printer->setDocName(gdoc->fileName());
		gdoc->print(printer);
		statusBar()->showMessage(tr("The document was sent to a printer %1...").arg(
		                             printer->printerName()), 5000);
	}

#endif
}

void GCodeWorkShop::cut()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->cut();
	}
}

void GCodeWorkShop::copy()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->copy();
	}
}

void GCodeWorkShop::findInFl()
{
	if (findFiles == nullptr) {
		findFiles = new FindInFiles(ui->splitter);

		if (m_findInFilesHighlightEnable) {
			GCoderStyle* style = dynamic_cast<GCoderStyle*>(m_documentManager->documentStyle(GCoder::DOCUMENT_TYPE).get());

			if (style) {
				findFiles->setHighlightColors(style->hColors);
			}
		}

		if (activeDocument()) {
			findFiles->setDir(activeDocument()->path());
		}

		connect(findFiles, SIGNAL(fileClicked(QString)), this, SLOT(loadFoundedFile(QString)));
	} else if (!findFilesAct->isChecked()) {
		findFiles->close();
		findFiles = nullptr;
	} else {
		findFiles->show();
		findFilesAct->setChecked(true);
	}
}

bool GCodeWorkShop::findNext()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());
	bool hasMdiChild = (gdoc != 0);
	bool found = false;
	QPalette palette;

	findNextAct->setEnabled(false);
	findPreviousAct->setEnabled(false);

	if (!findEdit->text().isEmpty() && hasMdiChild) {
		found = gdoc->findNext(findEdit->text(),
		                       mCheckFindWholeWords->isChecked(),
		                       !mCheckIgnoreCase->isChecked(),
		                       mCheckIgnoreComments->isChecked());

		palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

		if (found) {
			findEdit->setPalette(QPalette());
		} else {
			findEdit->setPalette(palette);
		}
	}

	findNextAct->setEnabled(true);
	findPreviousAct->setEnabled(true);
	return found;
}

bool GCodeWorkShop::findPrevious()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());
	bool hasMdiChild = (gdoc != 0);
	bool found = false;
	QPalette palette;

	findNextAct->setEnabled(false);
	findPreviousAct->setEnabled(false);

	if (!findEdit->text().isEmpty() && hasMdiChild) {
		found = gdoc->findNext(findEdit->text(),
		                       mCheckFindWholeWords->isChecked(),
		                       !mCheckIgnoreCase->isChecked(),
		                       mCheckIgnoreComments->isChecked(),
		                       true);

		palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

		if (found) {
			findEdit->setPalette(QPalette());
		} else {
			findEdit->setPalette(palette);
		}
	}

	findNextAct->setEnabled(true);
	findPreviousAct->setEnabled(true);

	return found;
}

void GCodeWorkShop::replaceNext()
{
	QPalette palette;
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());
	bool hasMdiChildNotReadOnly = ((gdoc != 0) && !activeDocument()->isReadOnly());

	replaceNextAct->setEnabled(false);
	replacePreviousAct->setEnabled(false);
	replaceAllAct->setEnabled(false);

	if (hasMdiChildNotReadOnly) {
		bool found = gdoc->replaceNext(findEdit->text(), replaceEdit->text(),
		                               mCheckFindWholeWords->isChecked(),
		                               !mCheckIgnoreCase->isChecked(),
		                               mCheckIgnoreComments->isChecked());

		palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

		if (found) {
			findEdit->setPalette(QPalette());
		} else {
			findEdit->setPalette(palette);
		}
	}

	replaceNextAct->setEnabled(true);
	replacePreviousAct->setEnabled(true);
	replaceAllAct->setEnabled(true);
}

void GCodeWorkShop::replacePrevious()
{
	QPalette palette;
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());
	bool hasMdiChildNotReadOnly = ((gdoc != 0) && !activeDocument()->isReadOnly());

	replaceNextAct->setEnabled(false);
	replacePreviousAct->setEnabled(false);
	replaceAllAct->setEnabled(false);

	if (hasMdiChildNotReadOnly) {
		bool found = gdoc->replaceNext(findEdit->text(), replaceEdit->text(),
		                               mCheckFindWholeWords->isChecked(),
		                               !mCheckIgnoreCase->isChecked(),
		                               mCheckIgnoreComments->isChecked(),
		                               true);

		palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

		if (found) {
			findEdit->setPalette(QPalette());
		} else {
			findEdit->setPalette(palette);
		}
	}

	replaceNextAct->setEnabled(true);
	replacePreviousAct->setEnabled(true);
	replaceAllAct->setEnabled(true);
}

void GCodeWorkShop::replaceAll()
{
	QPalette palette;
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());
	bool hasMdiChildNotReadOnly = ((gdoc != 0) && !activeDocument()->isReadOnly());

	replaceNextAct->setEnabled(false);
	replacePreviousAct->setEnabled(false);
	replaceAllAct->setEnabled(false);

	if (hasMdiChildNotReadOnly) {
		QApplication::setOverrideCursor(Qt::BusyCursor);
		bool found = gdoc->replaceAll(findEdit->text(), replaceEdit->text(),
		                              mCheckFindWholeWords->isChecked(),
		                              !mCheckIgnoreCase->isChecked(),
		                              mCheckIgnoreComments->isChecked());

		palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

		if (found) {
			findEdit->setPalette(QPalette());
		} else {
			findEdit->setPalette(palette);
		}

		QApplication::restoreOverrideCursor();
	}

	replaceNextAct->setEnabled(true);
	replacePreviousAct->setEnabled(true);
	replaceAllAct->setEnabled(true);
}

void GCodeWorkShop::selAll()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->selectAll();
	}
}

void GCodeWorkShop::config()
{
	AppConfig config;
	config.editorProperties = *m_documentManager->documentWidgetProperties(GCoder::DOCUMENT_TYPE);
	config.codeStyle = *m_documentManager->documentStyle(GCoder::DOCUMENT_TYPE);
	config.calcBinary = m_calcBinary;
	config.extensions = m_extensions;
	config.saveExtension = m_saveExtension;
	config.saveDirectory = m_saveDirectory;
	config.mdiTabbedMode = m_MdiTabbedMode;
	config.defaultReadOnly = m_defaultReadOnly;
	config.disableFileChangeMonitor = m_disableFileChangeMonitor;
	config.startEmpty = m_startEmpty;
	SetupDialog* setUpDialog = new SetupDialog(this, &config);

	if (setUpDialog->exec() == QDialog::Accepted) {
		config = setUpDialog->getSettings();
		QSettings* cfg = Medium::instance().settings();
		m_documentManager->setDocumentWidgetProperties(DocumentWidgetProperties::Ptr(new GCoderWidgetProperties(
		            config.editorProperties)));
		config.editorProperties.save(cfg);
		emit intCapsLockChanged(config.editorProperties.intCapsLock);
		m_documentManager->setDocumentStyle(DocumentStyle::Ptr(new GCoderStyle(config.codeStyle)));
		config.codeStyle.save(cfg);
		m_calcBinary = config.calcBinary;
		m_extensions = config.extensions;
		m_saveExtension = config.saveExtension;
		m_saveDirectory = config.saveDirectory;
		setMdiTabbedMode(config.mdiTabbedMode);
		m_defaultReadOnly = config.defaultReadOnly;
		m_disableFileChangeMonitor = config.disableFileChangeMonitor;
		m_startEmpty = config.startEmpty;

		if (dirModel != nullptr) {
			dirModel->setNameFilters(m_extensions);
		}

		m_documentManager->updateDocuments(GCoder::DOCUMENT_TYPE);

		for (Document* doc : m_documentManager->documentList()) {
			doc->setReadOnly(m_defaultReadOnly);
		}
	}

	delete setUpDialog;
}

void GCodeWorkShop::readOnly()
{
	if (activeDocument()) {
		activeDocument()->setReadOnly(readOnlyAct->isChecked());
	}

	updateMenus();
}

void GCodeWorkShop::goToLine(const QString& fileName, int line)
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		QString childFileName = gdoc->filePath();
		childFileName = QDir().toNativeSeparators(childFileName);

		if (QDir().toNativeSeparators(fileName) != childFileName) {
			return;
		}

		gdoc->goToLine(line);
	}
}

void GCodeWorkShop::createDiffApp()
{
	if (diffApp == nullptr) {
		diffApp = new KDiff3App(ui->splitter, "DiffApp", m_extensions);

		connect(diffApp, SIGNAL(lineClicked(QString, int)), this, SLOT(goToLine(QString, int)));
	}
}

void GCodeWorkShop::doDiffL()
{
	QString fileName;

	createDiffApp();

	if (diffApp != nullptr) {
		diffLAct->setEnabled(false);
		diffRAct->setEnabled(false);
		diffAct->setEnabled(false);

		diffAct->setChecked(true);

		if (activeDocument()) {
			fileName = activeDocument()->filePath();
		}

		if (fileName.isEmpty()) {
			fileName = lastOpenedPath();
		}

		//        diffApp->close();
		diffApp->completeInit(fileName, "");

		diffLAct->setEnabled(true);
		diffRAct->setEnabled(true);
		diffAct->setEnabled(true);
	}
}

void GCodeWorkShop::doDiffR()
{
	QString fileName;

	createDiffApp();

	if (diffApp != nullptr) {
		diffLAct->setEnabled(false);
		diffRAct->setEnabled(false);
		diffAct->setEnabled(false);

		diffAct->setChecked(true);

		if (activeDocument()) {
			fileName = activeDocument()->filePath();
		}

		if (fileName.isEmpty()) {
			fileName = lastOpenedPath();
		}

		//        diffApp->close();
		diffApp->completeInit("", fileName);

		diffLAct->setEnabled(true);
		diffRAct->setEnabled(true);
		diffAct->setEnabled(true);
	}
}

void GCodeWorkShop::diffTwoFiles(const QString& filename1, const QString& filename2)
{
	createDiffApp();

	if (diffApp != nullptr) {
		diffAct->setChecked(true);
		//        diffApp->close();
		diffApp->completeInit(filename1, filename2);

		QList<int> sizes;
		sizes.clear();
		sizes.append(0);
		sizes.append(ui->splitter->height());
		ui->splitter->setSizes(sizes);
	}
}

void GCodeWorkShop::diffEditorFile()
{
	Document* doc = activeDocument();

	if (!doc) {
		return;
	}

	createDiffApp();

	if (diffApp != nullptr) {
		QString fileName = doc->filePath();

		if (fileName.isEmpty()) {
			return;
		}

		QString fileName1 = QDir::tempPath() + QDir::separator() + QFileInfo(
		                        fileName).baseName() + ".tmp";
		//qDebug() << fileName << fileName1;

		QFile file(fileName1);

		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::warning(this, tr("GCodeWorkShop"),
			                     tr("Cannot write tmp file \"%1\".\n %2")
			                     .arg(QDir::toNativeSeparators(fileName1))
			                     .arg(file.errorString()));
			return;
		}

		file.write(doc->rawData());
		file.close();

		diffAct->setChecked(true);
		//        diffApp->close();
		diffApp->completeInit(fileName, fileName1);

		if (file.exists()) {
			file.remove();
		}

		//      QList<int> sizes;
		//      sizes.clear();
		//      sizes.append(0);
		//      sizes.append(splitter->height());
		//      splitter->setSizes(sizes);
	}
}

void GCodeWorkShop::doDiff()
{
	QString fileName;

	if (diffApp == nullptr) {
		createDiffApp();

		if (activeDocument()) {
			fileName = activeDocument()->filePath();
		}

		if (fileName.isEmpty()) {
			fileName = lastOpenedPath();
		}

		//        diffApp->close();
		diffApp->completeInit(QFileInfo(fileName).canonicalPath(), QFileInfo(fileName).canonicalPath());

	} else if (!diffAct->isChecked()) {
		diffAct->setChecked(false);
		diffApp->close();
		diffApp = nullptr;
	}
}

void GCodeWorkShop::doCalc()
{
	if (!QFile::exists(m_calcBinary)) {
		QMessageBox::information(this, tr("Information"),
		                         tr("Set correct calculator program name in configuration dialog."));
		return;
	}

	proc = findChild<QProcess*>("Calc569");

	if (!proc) {
		proc = new QProcess(this);
		proc->setObjectName("Calc569");
	}

	bool isNotRun;
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
	isNotRun = proc->pid() == 0;
#else
	isNotRun = proc->processId() == 0;
#endif

	if (isNotRun) {
		proc->start(m_calcBinary, QStringList());
	}
}

void GCodeWorkShop::deleteText()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->removeSelectedText();
	}
}

void GCodeWorkShop::paste()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->paste();
	}
}

void GCodeWorkShop::undo()
{
	if (activeDocument()) {
		activeDocument()->undo();
	}
}

void GCodeWorkShop::redo()
{
	if (activeDocument()) {
		activeDocument()->redo();
	}
}

void GCodeWorkShop::activeWindowChanged(QMdiSubWindow* window)
{
	Q_UNUSED(window);
	Document* doc;

	if (ui->mdiArea->subWindowList().count() <= 1) {
		m_MdiWidgetsMaximized = true;
	}

	doc = activeDocument();

	if (doc) {
		m_MdiWidgetsMaximized = doc->widget()->parentWidget()->isMaximized();
		statusBar()->showMessage(doc->filePath(), 9000);
	}

	updateCurrentSerialConfig();
	updateOpenFileList();
	fileTreeViewChangeRootDir();
}

void GCodeWorkShop::about()
{
	QMessageBox::about(this, tr("About GCodeWorkShop"),
	                   tr("The <b>GCodeWorkShop</b> is text editor for CNC programmers."
	                      "<br>This is a fork of <a href='https://github.com/artur3/EdytorNC'>EdytorNC</a>."
	                      "<P>Version: %1"
	                      "<P>Copyright (C) 1998 - 2018 by <a href=\"mailto:artkoz78@gmail.com\">Artur Kozioł</a>"
	                      "<br>Copyright (C) 2020 - 2024 by <a href='mailto:nicegorov@yandex.ru'>Nick Egorrov</a>"
	                      "<P>Spanish translation, fixes and other help thanks to Otto Huamani"
	                      "<br />Catalan translation and deb package thanks to Jordi Sayol i Salomó"
	                      "<br />German translation and other fixes thanks to Michael Numberger"
	                      "<br />Czech translation thanks to Pavel Fric"
	                      "<br />Finnish translation thanks to Janne Mäntyharju"
	                      "<br />Dutch translation thanks to Henk van Andel"
	                      "<br />OS X patch and other updates thanks to Janne Mäntyharju"
	                      "<P>New EdytorNC icon thanks to Jakub Gajewski"
	                      "<P><a href='https://github.com/GCodeProjects/GCodeWorkShop'>GCodeWorkShop on GitHub</a>"
	                      "<P>GCodeWorkShop contains pieces of code from other Open Source projects."
	                      "<P><i>GCodeWorkShop is free software; you can redistribute it and/or modify"
	                      "it under the terms of the GNU General Public License  as published by"
	                      "the Free Software Foundation; either version 2 of the License, or"
	                      "(at your option) any later version.</i>"
	                      "<P><i>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
	                      "INCLUDING THE WARRANTY OF DESIGN,"
	                      "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</i>").arg(GCODEWORKSHOP_VERSION));
}

void GCodeWorkShop::updateMenus()
{
	Document* doc = activeDocument();
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(doc);
	bool hasMdiChildNotReadOnly;
	bool hasSelection;
	bool hasModifiedMdiChild;
	bool hasMdiChild = doc != nullptr;

	if (hasMdiChild) {
		hasMdiChildNotReadOnly = !doc->isReadOnly();
		hasSelection = gdoc ? gdoc->hasSelection() : false;
		hasModifiedMdiChild = doc->isModified();

		redoAct->setEnabled(hasMdiChild && doc->isRedoAvailable());
		undoAct->setEnabled(hasMdiChild && doc->isUndoAvailable());
	} else {
		hasMdiChildNotReadOnly = false;
		hasSelection = false;
		hasModifiedMdiChild = false;

		redoAct->setEnabled(false);
		undoAct->setEnabled(false);
	}

	saveAct->setEnabled(hasModifiedMdiChild);
	saveAllAct->setEnabled(hasMdiChild);
	saveAsAct->setEnabled(hasMdiChild);
	printAct->setEnabled(hasMdiChild);
	printPreviewAct->setEnabled(hasMdiChild);
	pasteAct->setEnabled(hasMdiChild);
	closeAct->setEnabled(hasMdiChild);
	closeAllAct->setEnabled(hasMdiChild);
	tileHAct->setEnabled(hasMdiChild);
	tileVAct->setEnabled(hasMdiChild);
	cascadeAct->setEnabled(hasMdiChild);
	nextAct->setEnabled(hasMdiChild);
	previousAct->setEnabled(hasMdiChild);
	separatorAct->setVisible(hasMdiChild);
	selAllAct->setEnabled(hasMdiChildNotReadOnly);
	findAct->setEnabled(hasMdiChild);

	diffLAct->setEnabled(hasMdiChild);
	diffRAct->setEnabled(hasMdiChild);
	diffEditorAct->setEnabled(hasMdiChildNotReadOnly);

	replaceAct->setEnabled(hasMdiChildNotReadOnly);
	readOnlyAct->setEnabled(hasMdiChild);
	m_addonsActions->renumber()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->dot()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->removeEmptyLines()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->insertEmptyLines()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->insertSpaces()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->removeSpaces()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->splitProgramms()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->i2mProg()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->compileMacro()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->cleanUp()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->swapAxes()->setEnabled(hasMdiChildNotReadOnly);
	m_addonsActions->paraComment()->setEnabled(hasMdiChildNotReadOnly && hasSelection);
	m_addonsActions->semiComment()->setEnabled(hasMdiChildNotReadOnly && hasSelection);
	m_addonsActions->blockSkipDecrement()->setEnabled(hasMdiChildNotReadOnly && hasSelection);
	m_addonsActions->blockSkipIncrement()->setEnabled(hasMdiChildNotReadOnly && hasSelection);
	m_addonsActions->blockSkipRemove()->setEnabled(hasMdiChildNotReadOnly && hasSelection);
	inLineCalcAct->setEnabled(hasMdiChild);

	if (!hasMdiChildNotReadOnly) {
		readOnlyAct->setChecked(true);
		readOnlyAct->setIcon(QIcon(":/images/lock.png"));
	} else {
		readOnlyAct->setChecked(false);
		readOnlyAct->setIcon(QIcon(":/images/unlock.png"));
	}

	cutAct->setEnabled(hasSelection && hasMdiChildNotReadOnly);
	deleteAct->setEnabled(hasSelection && hasMdiChildNotReadOnly);
	copyAct->setEnabled(hasSelection);

	pasteAct->setEnabled((!clipboard->text().isEmpty()) && hasMdiChildNotReadOnly);

	if (gdoc) {
		if (findToolBar)
			gdoc->highlightFindText(findEdit->text(),
			                        mCheckFindWholeWords->isChecked(),
			                        !mCheckIgnoreCase->isChecked(),
			                        mCheckIgnoreComments->isChecked());
		else {
			gdoc->highlightFindText("");
		}
	}

	if (doc) {
		saveAct->setText(tr("&Save \"%1\"").arg(doc->fileName()));
		saveAsAct->setText(tr("Save \"%1\" &As...").arg(doc->fileName()));
		closeAct->setText(tr("Cl&ose \"%1\"").arg(doc->fileName()));
	}

	updateStatusBar();
}

void GCodeWorkShop::updateCurrentSerialConfig()
{
	bool hasMdiChild = (activeDocument() != nullptr);

	if (hasMdiChild && (serialToolBar != nullptr)) {
		QDir dir = activeDocument()->dir();
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Name);
		dir.setNameFilters(QStringList("*.ini"));

		QFileInfoList list = dir.entryInfoList();

		if (!list.isEmpty()) {
			QFileInfo name = list.at(0);
			int id = configBox->findText(name.baseName());

			if (id >= 0) {
				configBox->setCurrentIndex(id);
			}
		}
	}
}

void GCodeWorkShop::updateStatusBar()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		int id = highlightTypeCombo->findData(gdoc->highlightMode());
		highlightTypeCombo->blockSignals(true);
		highlightTypeCombo->setCurrentIndex(id);
		highlightTypeCombo->blockSignals(false);

		int line = gdoc->currentLine();
		int column = gdoc->currentColumn();

		labelStat1->setText(tr(" Col: ") + QString::number(column + 1) +
		                    tr("  Line: ") + QString::number(line) +
		                    (gdoc->isModified() ? tr("  <b>Modified</b>  ") : " ") +
		                    (gdoc->isReadOnly() ? tr(" Read only  ") : " ") +
		                    (gdoc->overwriteMode() ? tr(" Overwrite  ") : tr(" Insert ")));

	}
}

void GCodeWorkShop::updateWindowMenu()
{
	QString text;

	windowMenu->clear();
	windowMenu->addAction(closeAct);
	windowMenu->addAction(closeAllAct);
	windowMenu->addSeparator();
	windowMenu->addAction(tileHAct);
	windowMenu->addAction(tileVAct);
	windowMenu->addAction(cascadeAct);
	windowMenu->addSeparator();
	windowMenu->addAction(nextAct);
	windowMenu->addAction(previousAct);
	windowMenu->addAction(separatorAct);

	windowMenu->setAttribute(Qt::WA_AlwaysShowToolTips, true);

	QList<Document*> docList = m_documentManager->documentList();
	separatorAct->setVisible(!docList.isEmpty());

	for (int i = 0; i < docList.size(); ++i) {
		Document* doc = docList.at(i);

		if (i < 9) {
			text = tr("&%1 %2").arg(i + 1).arg(doc->filePath());
		} else {
			text = tr("%1 %2").arg(i + 1).arg(doc->filePath());
		}

		QAction* action = windowMenu->addAction(text);
		action->setCheckable(true);
		action->setChecked(doc == activeDocument());
		action->setToolTip(doc->brief());
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, docList.at(i));
	}
}

Document* GCodeWorkShop::createDocument(const QString& type)
{
	Document* doc = m_documentManager->createDocument(type, "");

	if (!doc) {
		return nullptr;
	}

	if (m_saveDirectory.isEmpty()) {
		doc->setPath(lastOpenedPath());
	} else {
		doc->setPath(m_saveDirectory);
	}

	if (m_MdiWidgetsMaximized) {
		doc->widget()->parentWidget()->showMaximized();
	} else {
		doc->widget()->parentWidget()->showNormal();
	}

	return doc;
}

void GCodeWorkShop::createActions()
{
	newAct = new QAction(QIcon(":/images/filenew.png"), tr("&New"), this);
	newAct->setShortcut(QKeySequence::New);
	newAct->setToolTip(tr("Create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFileFromTemplate()));

	openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open..."), this);
	openAct->setShortcut(QKeySequence::Open);
	openAct->setToolTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	if (QDir(EXAMPLES_PATH).exists()
	        || QDir(QApplication::applicationDirPath() + "/" + "EXAMPLES").exists()) {
		openExampleAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open example..."), this);
		openExampleAct->setToolTip(tr("Open an example file"));
		connect(openExampleAct, SIGNAL(triggered()), this, SLOT(openExample()));
	}

	saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save \"%1\"").arg(""), this);
	saveAct->setShortcut(QKeySequence::Save);
	saveAct->setToolTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAllAct = new QAction(QIcon(":/images/filesaveall.png"), tr("Save A&ll"), this);
	saveAllAct->setShortcut(tr("Ctrl+Shift+S"));
	saveAllAct->setToolTip(tr("Save all modified documents to disk"));
	connect(saveAllAct, SIGNAL(triggered()), this, SLOT(saveAll()));

	saveAsAct = new QAction(QIcon(":/images/filesaveas.png"), tr("Save \"%1\" &As...").arg(""),
	                        this);
	saveAsAct->setShortcut(QKeySequence::SaveAs);
	saveAsAct->setToolTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
	//exitAct->setShortcut(QKeySequence::Quit);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setToolTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

	findFilesAct = new QAction(QIcon(":/images/filefind.png"), tr("&Find files..."), this);
	//openAct->setShortcut(tr("Ctrl+"));
	findFilesAct->setCheckable(true);
	findFilesAct->setToolTip(tr("Find files"));
	connect(findFilesAct, SIGNAL(triggered()), this, SLOT(findInFl()));

	printAct = new QAction(QIcon(":/images/document-print.png"), tr("&Print"), this);
	printAct->setShortcut(QKeySequence::Print);
	printAct->setToolTip(tr("Print file"));
	connect(printAct, SIGNAL(triggered()), this, SLOT(printFile()));

	printPreviewAct = new QAction(QIcon(":/images/document-print-preview.png"),
	                              tr("Pr&int preview"), this);
	//printPreviewAct->setShortcut(QKeySequence::Print);
	printPreviewAct->setToolTip(tr("Preview printing"));
	connect(printPreviewAct, SIGNAL(triggered()), this, SLOT(filePrintPreview()));

	sessionMgrAct = new QAction(tr("Session manager..."), this);
	sessionMgrAct->setToolTip(tr("Sessions manager"));
	connect(sessionMgrAct, SIGNAL(triggered()), this, SLOT(showSessionDialog()));


	undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
	undoAct->setShortcut(QKeySequence::Undo);
	undoAct->setToolTip(tr("Undo last operation"));
	connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
	undoAct->setEnabled(false);

	redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
	redoAct->setShortcut(QKeySequence::Redo);
	redoAct->setToolTip(tr("Redo last operation"));
	connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));
	redoAct->setEnabled(false);

	cutAct = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"), this);
	cutAct->setShortcut(QKeySequence::Cut);
	cutAct->setToolTip(tr("Cut the current selection's contents to the "
	                      "clipboard"));
	connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

	copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
	copyAct->setShortcut(QKeySequence::Copy);
	copyAct->setToolTip(tr("Copy the current selection's contents to the "
	                       "clipboard"));
	connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

	pasteAct = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste"), this);
	pasteAct->setShortcut(QKeySequence::Paste);
	pasteAct->setToolTip(tr("Paste the clipboard's contents into the current "
	                        "selection"));
	connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

	findAct = new QAction(QIcon(":/images/find.png"), tr("&Find"), this);
	findAct->setShortcut(QKeySequence::Find);
	findAct->setToolTip(tr("Find text"));
	connect(findAct, SIGNAL(triggered()), this, SLOT(createFindToolBar()));

	replaceAct = new QAction(QIcon(":/images/replace.png"), tr("&Replace"), this);
	replaceAct->setShortcut(tr("Ctrl+R"));
	replaceAct->setToolTip(tr("Find and replace text"));
	connect(replaceAct, SIGNAL(triggered()), this, SLOT(createFindToolBar()));


	deleteAct = new QAction(QIcon(":/images/editdelete.png"), tr("&Delete"), this);
	deleteAct->setShortcut(QKeySequence::Delete);
	deleteAct->setToolTip(tr("Removes selected text"));
	connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteText()));

	selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"), this);
	selAllAct->setShortcut(QKeySequence::SelectAll);
	selAllAct->setToolTip(tr("Select all text"));
	connect(selAllAct, SIGNAL(triggered()), this, SLOT(selAll()));

	readOnlyAct = new QAction(QIcon(":/images/unlock.png"), tr("Read &only"), this);
	readOnlyAct->setShortcut(tr("F12"));
	readOnlyAct->setCheckable(true);
	readOnlyAct->setToolTip(tr("Makes text read only"));
	connect(readOnlyAct, SIGNAL(triggered()), this, SLOT(readOnly()));

	configAct = new QAction(QIcon(":/images/configure.png"), tr("Configuration"), this);
	configAct->setShortcut(QKeySequence::Preferences);
	configAct->setToolTip(tr("Open configuration dialog"));
	connect(configAct, SIGNAL(triggered()), this, SLOT(config()));

	inLineCalcAct = new QAction(QIcon(":/images/inlinecalc.png"), tr("Inline calculator"), this);
	inLineCalcAct->setShortcut(tr("Ctrl+0"));
	connect(inLineCalcAct, SIGNAL(triggered()), this, SLOT(doShowInLineCalc()));

	m_addonsActions->bhc()->setShortcut(tr("F8"));
	m_addonsActions->blockSkipRemove()->setShortcut(tr("Ctrl+1"));
	m_addonsActions->blockSkipIncrement()->setShortcut(tr("Ctrl+2"));
	m_addonsActions->blockSkipDecrement()->setShortcut(tr("Ctrl+3"));
	//m_addonsActions->chamfer()->setShortcut(tr("F9"));
	//m_addonsActions->cleanUp()->setShortcut(QKeySequence::Print);
	m_addonsActions->paraComment()->setShortcut(tr("Ctrl+9"));
	m_addonsActions->semiComment()->setShortcut(tr("Ctrl+;"));
	//m_addonsActions->compileMacro()->setShortcut(tr("F9"));
	m_addonsActions->dot()->setShortcut(tr("F6"));
	//m_addonsActions->insertEmptyLines()->setShortcut(tr("F5"));
	//m_addonsActions->removeEmptyLines()->setShortcut(tr("F5"));
	m_addonsActions->feeds()->setShortcut(tr("F9"));
	//m_addonsActions->i2m()->setShortcut(tr("F9"));
	//m_addonsActions->i2mProg()->setShortcut(tr("F9"));
	m_addonsActions->renumber()->setShortcut(tr("F7"));
	m_addonsActions->insertSpaces()->setShortcut(tr("F4"));
	m_addonsActions->removeSpaces()->setShortcut(tr("F5"));
	//m_addonsActions->swapAxes()->setShortcut(QKeySequence::Save);
	//m_addonsActions->triangle()->setShortcut(tr("F9"));

	calcAct = new QAction(QIcon(":/images/calc.png"), tr("Calculator"), this);
	//calcAct->setShortcut(tr("F9"));
	calcAct->setToolTip(tr("Run calculator"));
	connect(calcAct, SIGNAL(triggered()), this, SLOT(doCalc()));

	showSerialToolBarAct = new QAction(QIcon(":/images/serial.png"), tr("Serial port send/receive"),
	                                   this);
	//showSerialToolBarAct->setShortcut(tr("F9"));
	showSerialToolBarAct->setCheckable(true);
	showSerialToolBarAct->setToolTip(tr("Serial port send/receive"));
	connect(showSerialToolBarAct, SIGNAL(triggered()), this, SLOT(createSerialToolBar()));

	diffRAct = new QAction(QIcon(":/images/diffr.png"),
	                       tr("Show diff - open current file in right diff window"), this);
	diffRAct->setToolTip(tr("Show diff - open current file in right diff window"));
	connect(diffRAct, SIGNAL(triggered()), this, SLOT(doDiffR()));

	diffLAct = new QAction(QIcon(":/images/diffl.png"),
	                       tr("Show diff - open current file in left diff window"), this);
	diffLAct->setToolTip(tr("Show diff - open current file in left diff window"));
	connect(diffLAct, SIGNAL(triggered()), this, SLOT(doDiffL()));

	diffAct = new QAction(QIcon(":/images/diff.png"), tr("On/off diff window"), this);
	diffAct->setCheckable(true);
	diffAct->setToolTip(tr("Show diff window"));
	connect(diffAct, SIGNAL(triggered()), this, SLOT(doDiff()));

	diffEditorAct = new QAction(QIcon(":/images/diff_editor.png"), tr("Show unsaved changes"),
	                            this);
	diffEditorAct->setToolTip(tr("Show diff of currently edited file and file on disk"));
	connect(diffEditorAct, SIGNAL(triggered()), this, SLOT(diffEditorFile()));

	closeAct = new QAction(QIcon(":/images/fileclose.png"), tr("Cl&ose \"%1\"").arg(""), this);
	//closeAct->setShortcut(QKeySequence::Close);
	closeAct->setToolTip(tr("Close the active window"));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(closeCurrentWindow()));

	closeAllAct = new QAction(QIcon(":/images/window-close.png"), tr("Close &All"), this);
	closeAllAct->setToolTip(tr("Close all the windows"));
	connect(closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllMdiWindows()));

	tileHAct = new QAction(QIcon(":/images/tile_h.png"), tr("Tile &horyzontally"), this);
	tileHAct->setToolTip(tr("Tile the windows horyzontally"));
	connect(tileHAct, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));

	tileVAct = new QAction(QIcon(":/images/tile_v.png"), tr("Tile &vertycally"), this);
	tileVAct->setToolTip(tr("Tile the windows vertycally"));
	connect(tileVAct, SIGNAL(triggered()), this, SLOT(tileSubWindowsVertycally()));

	cascadeAct = new QAction(QIcon(":/images/cascade.png"), tr("&Cascade"), this);
	cascadeAct->setToolTip(tr("Cascade the windows"));
	connect(cascadeAct, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));

	nextAct = new QAction(QIcon(":/images/go-next.png"), tr("Ne&xt"), this);
	nextAct->setShortcut(QKeySequence::Forward);
	nextAct->setToolTip(tr("Move the focus to the next window"));
	connect(nextAct, SIGNAL(triggered()), ui->mdiArea, SLOT(activateNextSubWindow()));

	previousAct = new QAction(QIcon(":/images/go-previous.png"), tr("Pre&vious"), this);
	previousAct->setShortcut(QKeySequence::Back);
	previousAct->setToolTip(tr("Move the focus to the previous window"));
	connect(previousAct, SIGNAL(triggered()), ui->mdiArea, SLOT(activatePreviousSubWindow()));

	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this,
	        SLOT(activeWindowChanged(QMdiSubWindow*)));

	separatorAct = new QAction(this);
	separatorAct->setSeparator(true);

	createGlobalToolTipsAct = new QAction(tr("&Create global cnc tooltips"), this);
	createGlobalToolTipsAct->setToolTip(tr("Create default global cnc tooltips file"));
	connect(createGlobalToolTipsAct, SIGNAL(triggered()), this, SLOT(createGlobalToolTipsFile()));

	createUserToolTipsAct = new QAction(tr("&Create user cnc tooltips"), this);
	createUserToolTipsAct->setToolTip(tr("Create/edit user cnc tooltips file"));
	connect(createUserToolTipsAct, SIGNAL(triggered()), this, SLOT(createUserToolTipsFile()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setToolTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setToolTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void GCodeWorkShop::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);

	if (openExampleAct != nullptr) {
		fileMenu->addAction(openExampleAct);
	}

	fileMenu->addSeparator();
	recentFileMenu = fileMenu->addMenu(tr("&Recent files"));
	recentFileMenu->setIcon(QIcon(":/images/document-open-recent.png"));
	connect(recentFileMenu, SIGNAL(triggered(QAction*)), this, SLOT(fileOpenRecent(QAction*)));
	fileMenu->addSeparator();
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addAction(saveAllAct);
	fileMenu->addSeparator();
	fileMenu->addAction(findFilesAct);
	fileMenu->addSeparator();

	sessionsMenu = fileMenu->addMenu(tr("Sessions"));
	connect(sessionsMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeSession(QAction*)));
	fileMenu->addAction(sessionMgrAct);

	fileMenu->addSeparator();
	fileMenu->addAction(printAct);
	fileMenu->addAction(printPreviewAct);
	fileMenu->addSeparator();
	fileMenu->addAction(closeAct);
	fileMenu->addAction(closeAllAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(undoAct);
	editMenu->addAction(redoAct);
	editMenu->addSeparator();
	editMenu->addAction(cutAct);
	editMenu->addAction(copyAct);
	editMenu->addAction(pasteAct);
	editMenu->addAction(deleteAct);
	editMenu->addSeparator();
	editMenu->addAction(selAllAct);
	editMenu->addSeparator();
	editMenu->addAction(findAct);
	editMenu->addAction(replaceAct);

	editMenu->addSeparator();
	editMenu->addAction(m_addonsActions->semiComment());
	editMenu->addAction(m_addonsActions->paraComment());
	blockSkipMenu = editMenu->addMenu(tr("&Block Skip"));
	blockSkipMenu->setIcon(QIcon(":/images/blockskip.png"));
	blockSkipMenu->addAction(m_addonsActions->blockSkipIncrement());
	blockSkipMenu->addAction(m_addonsActions->blockSkipDecrement());
	blockSkipMenu->addAction(m_addonsActions->blockSkipRemove());
	editMenu->addSeparator();
	editMenu->addAction(readOnlyAct);
	editMenu->addSeparator();
	editMenu->addAction(configAct);

	toolsMenu = menuBar()->addMenu(tr("&Tools"));
	toolsMenu->addAction(showSerialToolBarAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_addonsActions->insertSpaces());
	toolsMenu->addAction(m_addonsActions->removeSpaces());
	toolsMenu->addAction(m_addonsActions->dot());
	toolsMenu->addAction(m_addonsActions->insertEmptyLines());
	toolsMenu->addAction(m_addonsActions->removeEmptyLines());
	toolsMenu->addAction(m_addonsActions->cleanUp());
	toolsMenu->addAction(m_addonsActions->swapAxes());
	toolsMenu->addAction(m_addonsActions->splitProgramms());
	toolsMenu->addAction(m_addonsActions->renumber());
	toolsMenu->addSeparator();
	toolsMenu->addAction(diffAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(diffLAct);
	toolsMenu->addAction(diffRAct);
	toolsMenu->addAction(diffEditorAct);
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_addonsActions->bhc());
	toolsMenu->addAction(m_addonsActions->feeds());
	toolsMenu->addAction(m_addonsActions->triangle());
	toolsMenu->addAction(m_addonsActions->chamfer());
	toolsMenu->addAction(m_addonsActions->i2m());
	toolsMenu->addAction(m_addonsActions->i2mProg());
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_addonsActions->compileMacro());
	toolsMenu->addSeparator();
	toolsMenu->addAction(inLineCalcAct);
	toolsMenu->addAction(calcAct);

	windowMenu = menuBar()->addMenu(tr("&Window"));
	updateWindowMenu();
	connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(createGlobalToolTipsAct);
	helpMenu->addAction(createUserToolTipsAct);
	helpMenu->addSeparator();
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void GCodeWorkShop::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->setObjectName("File");
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);
	fileToolBar->addAction(saveAllAct);
	fileToolBar->addAction(saveAsAct);
	fileToolBar->addSeparator();
	fileToolBar->addAction(findFilesAct);
	fileToolBar->addSeparator();
	fileToolBar->addAction(printAct);

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->setObjectName("Edit");
	editToolBar->addAction(undoAct);
	editToolBar->addAction(redoAct);
	editToolBar->addSeparator();
	editToolBar->addAction(cutAct);
	editToolBar->addAction(copyAct);
	editToolBar->addAction(pasteAct);
	editToolBar->addSeparator();
	editToolBar->addAction(deleteAct);
	editToolBar->addSeparator();
	editToolBar->addAction(selAllAct);
	editToolBar->addSeparator();
	editToolBar->addAction(findAct);
	editToolBar->addAction(replaceAct);
	editToolBar->addSeparator();
	editToolBar->addAction(diffAct);
	editToolBar->addSeparator();
	editToolBar->addAction(diffLAct);
	editToolBar->addAction(diffRAct);
	editToolBar->addAction(diffEditorAct);

	toolsToolBar = new QToolBar(tr("Tools"));
	addToolBar(Qt::LeftToolBarArea, toolsToolBar);
	toolsToolBar->setObjectName("Tools");
	toolsToolBar->addAction(showSerialToolBarAct);
	toolsToolBar->addSeparator();
	toolsToolBar->addAction(m_addonsActions->insertSpaces());
	toolsToolBar->addAction(m_addonsActions->removeSpaces());
	toolsToolBar->addAction(m_addonsActions->cleanUp());
	toolsToolBar->addAction(m_addonsActions->dot());
	toolsToolBar->addAction(m_addonsActions->swapAxes());
	toolsToolBar->addAction(m_addonsActions->renumber());
	toolsToolBar->addAction(m_addonsActions->splitProgramms());
	toolsToolBar->addSeparator();
	toolsToolBar->addAction(m_addonsActions->bhc());
	toolsToolBar->addAction(m_addonsActions->feeds());
	toolsToolBar->addAction(m_addonsActions->triangle());
	toolsToolBar->addAction(m_addonsActions->chamfer());
	toolsToolBar->addAction(m_addonsActions->i2m());
	toolsToolBar->addAction(m_addonsActions->i2mProg());
	toolsToolBar->addSeparator();
	toolsToolBar->addAction(m_addonsActions->compileMacro());
	toolsToolBar->addSeparator();
	toolsToolBar->addAction(calcAct);
	toolsToolBar->addSeparator();

	windowToolBar = addToolBar(tr("Window"));
	windowToolBar->setObjectName("Window");
	windowToolBar->addAction(previousAct);
	windowToolBar->addAction(nextAct);
}

void GCodeWorkShop::createStatusBar()
{
	labelStat1 = new QLabel("    ");

	labelStat1->setFrameShadow(QFrame::Sunken);
	labelStat1->setFrameShape(QFrame::Box);

	highlightTypeCombo = new QComboBox();
	highlightTypeCombo->setToolTip(tr("Highlight style and tooltip mode"));
	highlightTypeCombo->setEditable(false);
	highlightTypeCombo->addItem(tr("AUTO"), MODE_AUTO);
	highlightTypeCombo->addItem(tr("FANUC"), MODE_FANUC);
	highlightTypeCombo->addItem(tr("HEIDENHAIN DIALOG"), MODE_HEIDENHAIN);
	highlightTypeCombo->addItem(tr("HEIDENHAIN ISO"), MODE_HEIDENHAIN_ISO);
	highlightTypeCombo->addItem(tr("OKUMA OSP"), MODE_OKUMA);
	highlightTypeCombo->addItem(tr("PHILIPS"), MODE_PHILIPS);
	highlightTypeCombo->addItem(tr("SINUMERIK OLD"), MODE_SINUMERIK);
	highlightTypeCombo->addItem(tr("SINUMERIK NEW"), MODE_SINUMERIK_840);
	highlightTypeCombo->addItem(tr("LinuxCNC"), MODE_LINUXCNC);
	highlightTypeCombo->addItem(tr("TOOLTIPS"), MODE_TOOLTIPS);

	connect(highlightTypeCombo, SIGNAL(currentIndexChanged(int)), this,
	        SLOT(setHighLightMode(int)));

	attachHighlightToDirAct = new QAction(QIcon(":/images/attach.png"),
	                                      tr("Attach current highlight setting to current directory of programs"), this);
	attachHighlightToDirAct->setToolTip(
	    tr("Attach current highlight setting to current directory of programs"));
	connect(attachHighlightToDirAct, SIGNAL(triggered()), this,
	        SLOT(attachHighlightToDirActClicked()));

	attachHighlightButton = new QToolButton();
	attachHighlightButton->setDefaultAction(attachHighlightToDirAct);

	deAttachHighlightToDirAct = new QAction(QIcon(":/images/deattach.png"),
	                                        tr("Remove highlight settings from the directory"), this);
	deAttachHighlightToDirAct->setToolTip(tr("Remove highlight settings from the directory"));
	connect(deAttachHighlightToDirAct, SIGNAL(triggered()), this,
	        SLOT(deAttachHighlightToDirActClicked()));

	deAttachHighlightButton = new QToolButton();
	deAttachHighlightButton->setDefaultAction(deAttachHighlightToDirAct);

	readOnlyButton = new QToolButton();
	readOnlyButton->setDefaultAction(readOnlyAct);

	//statusBar()->addPermanentWidget(highlightLabel);
	statusBar()->addPermanentWidget(labelStat1);
	statusBar()->addPermanentWidget(highlightTypeCombo);
	statusBar()->addPermanentWidget(attachHighlightButton);
	statusBar()->addPermanentWidget(deAttachHighlightButton);
	statusBar()->addPermanentWidget(readOnlyButton);
	statusBar()->setSizeGripEnabled(true);

	statusBar()->showMessage(tr("Ready"));
}

void GCodeWorkShop::setHighLightMode(int mode)
{
	bool ok;
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());
	int id = highlightTypeCombo->itemData(mode).toInt(&ok);

	if (gdoc && ok) {
		gdoc->setHighlightMode(id);
		gdoc->widget()->setFocus(Qt::MouseFocusReason);
	}
}

void GCodeWorkShop::readSettings()
{
	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("mainwindow");
	mMWConfig.pos = settings.value("pos", QPoint(0, 0)).toPoint();
	mMWConfig.size = settings.value("size", QSize(400, 240)).toSize();
	bool maximized = settings.value("maximized", false).toBool();
	QByteArray state = settings.value("state", QByteArray()).toByteArray();
	settings.endGroup();

	setGeometry(QRect(mMWConfig.pos, mMWConfig.size));
	setWindowState(maximized ? Qt::WindowMaximized : Qt::WindowNoState);

	if (settings.value("SerialToolbarShown", false).toBool()) {
		createSerialToolBar();
		showSerialToolBarAct->setChecked(true);
	}

	if (settings.value("FindToolBarShown", false).toBool()) {
		createFindToolBar();
	}

	restoreState(state);

	m_disableFileChangeMonitor = settings.value("DisableFileChangeMonitor",
	                             false).toBool();

	if (m_disableFileChangeMonitor) {
		fileChangeMonitor.clear();
	} else {
		fileChangeMonitor = new QFileSystemWatcher(this);
		connect(fileChangeMonitor, SIGNAL(fileChanged(const QString)), this,
		        SLOT(fileChanged(const QString)));
	}

	m_lastOpenedPath = settings.value("LastDir",  m_lastOpenedPath).toString();
	m_extensions = settings.value("Extensions", m_extensions).toStringList();
	m_saveExtension = settings.value("DefaultSaveExtension", m_saveExtension).toString();
	m_saveDirectory = settings.value("DefaultSaveDirectory", m_saveDirectory).toString();
	m_startEmpty = settings.value("StartEmpty", false).toBool();
	m_defaultReadOnly = settings.value("ViewerMode", false).toBool();
	setMdiTabbedMode(settings.value("TabbetMode", false).toBool());

	fileDialogState = settings.value("FileDialogState", QByteArray()).toByteArray();

#ifdef Q_OS_LINUX
	m_calcBinary = "kcalc";
#endif

#ifdef Q_OS_WIN32
	m_calcBinary = "calc.exe";
#endif

	m_calcBinary = settings.value("CalcBinary", m_calcBinary).toString();

	m_recentFiles->load(&settings);
	GCoderWidgetProperties::Ptr prop = m_documentManager->documentWidgetProperties(GCoder::DOCUMENT_TYPE);

	if (prop) {
		prop->load(&settings);
		m_documentManager->setDocumentWidgetProperties(prop);
	}

	GCoderStyle::Ptr style = m_documentManager->documentStyle(GCoder::DOCUMENT_TYPE);

	if (style) {
		style->load(&settings);
		m_documentManager->setDocumentStyle(style);
	}

	m_sessionManager->load(&settings);

	if (!m_startEmpty) {
		openFilesFromSession();
	}

	ui->fileTreeView->header()->restoreState(settings.value("FileTreeViewState",
	        QByteArray()).toByteArray());

	ui->vSplitter->restoreState(settings.value("VSplitterState", QByteArray()).toByteArray());

	currentProjectName = settings.value("CurrentProjectName", "").toString();
	projectLoad(currentProjectName);

	panelState = settings.value("ProjectPanelState", QByteArray()).toByteArray();
	ui->hSplitter->restoreState(panelState);
	panelHidden = settings.value("PanelHidden", false).toBool();

	if (panelHidden) {
		ui->vSplitter->hide();
		ui->frame->setMaximumWidth(ui->hideButton->width());
		ui->hideButton->setText(">>");
	}

	ui->tabWidget->setCurrentIndex(settings.value("TabCurrentIndex", 0).toInt());
	ui->currentPathCheckBox->setChecked(settings.value("FileBrowserShowCurrentFileDir",
	                                    false).toBool());
	ui->filePreviewSpinBox->setValue(settings.value("FilePreviewNo", 10).toInt());
}

void GCodeWorkShop::writeSettings()
{
	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("mainwindow");
	settings.setValue("pos", mMWConfig.pos);
	settings.setValue("size", mMWConfig.size);
	settings.setValue("maximized", isMaximized());
	settings.setValue("state", saveState());
	settings.endGroup();

	settings.setValue("LastDir", m_lastOpenedPath);
	settings.setValue("Extensions", m_extensions);
	settings.setValue("DefaultSaveExtension", m_saveExtension);
	settings.setValue("DefaultSaveDirectory", m_saveDirectory);
	settings.setValue("CalcBinary", m_calcBinary);
	settings.setValue("TabbetMode", m_MdiTabbedMode);
	settings.setValue("ViewerMode", m_defaultReadOnly);
	settings.setValue("StartEmpty", m_startEmpty);
	settings.setValue("DisableFileChangeMonitor", m_disableFileChangeMonitor);
	settings.setValue("FileDialogState", fileDialogState);

	settings.setValue("SerialToolbarShown", (!serialToolBar.isNull()));

	if (!serialToolBar.isNull()) {
		settings.beginGroup("SerialPortConfigs");
		settings.setValue("CurrentSerialPortSettings", configBox->currentText());
		settings.endGroup();
	}

	settings.setValue("CurrentProjectName", currentProjectName);

	settings.setValue("FileTreeViewState", ui->fileTreeView->header()->saveState());
	settings.setValue("VSplitterState", ui->vSplitter->saveState());
	settings.setValue("TabCurrentIndex", ui->tabWidget->currentIndex());
	settings.setValue("FilePreviewNo", ui->filePreviewSpinBox->value());

	if (panelHidden) {
		settings.setValue("ProjectPanelState", panelState);
	} else {
		settings.setValue("ProjectPanelState", ui->hSplitter->saveState());
	}

	settings.setValue("PanelHidden", panelHidden);
	settings.setValue("FileBrowserShowCurrentFileDir", ui->currentPathCheckBox->isChecked());

	settings.setValue("FindToolBarShown", !findToolBar.isNull());

	//cleanup old settings
	settings.remove("LastDoc");

	if (!m_startEmpty) {
		storeFileInfoInSession();
	}
}

Document* GCodeWorkShop::activeDocument() const
{
	return m_documentManager->activeDocument();
}

Document* GCodeWorkShop::findDocument(const QString& fileName)
{
	QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

	if (canonicalFilePath.isEmpty()) {
		canonicalFilePath = fileName;
	}

	return m_documentManager->findDocumentByFilePath(canonicalFilePath);
}

void GCodeWorkShop::setActiveSubWindow(QWidget* window)
{
	if (!window) {
		return;
	}

	ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}

QString GCodeWorkShop::currentPath() const
{
	Document* child = activeDocument();

	if (child) {
		return child->filePath();
	}

	return QDir::homePath();
}

QString GCodeWorkShop::lastOpenedPath() const
{
	return m_lastOpenedPath;
}

void GCodeWorkShop::setLastOpenedPath(const QString& path)
{
	m_lastOpenedPath = path;
}

void GCodeWorkShop::loadFile(const DocumentInfo::Ptr& info, bool checkAlreadyLoaded)
{
	QFileInfo file;

	if (checkAlreadyLoaded && m_documentManager->setActiveDocument(info->filePath)) {
		return;
	}

	file.setFile(info->filePath);

	if ((file.exists()) && (file.isReadable()) && file.isFile()) {
		Document* doc = createDocument(info->documentType());

		if (!doc) {
			return;
		}

		doc->setDocumentInfo(info);
		QApplication::setOverrideCursor(Qt::WaitCursor);
		bool status = doc->load();
		QApplication::restoreOverrideCursor();

		if (status) {
			setLastOpenedPath(info->filePath);
			updateStatusBar();
			m_recentFiles->add(info->filePath);
		} else {
			QMessageBox::warning(this, tr("GCodeWorkShop"), tr("Cannot read file \"%1\".\n %2")
			                     .arg(doc->filePath()).arg(doc->ioErrorString()));
			doc->close();
		}
	}
}

void GCodeWorkShop::recentFilesChanged()
{
	m_recentFiles->save(Medium::instance().settings());
}

void GCodeWorkShop::fileOpenRecent(QAction* act)
{
	openFile(act->data().toString());
}

void GCodeWorkShop::updateRecentFilesMenu(const QStringList& fileList)
{
	recentFileMenu->clear();

	for (const QString& file : fileList) {
		QAction* newAc = recentFileMenu->addAction(QIcon(":/images/document-open-recent.png"), file);
		newAc->setData(file);
	}
}

void GCodeWorkShop::loadFoundedFile(const QString& fileName)
{
	openFile(fileName);
}

void GCodeWorkShop::messReceived(const QString& text)
{
	QString str = text;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	auto behavior = QString::SkipEmptyParts;
#else
	auto behavior = Qt::SkipEmptyParts;
#endif
	QStringList list1 = str.split(";", behavior);

	for (int i = 0; i < list1.size(); ++i) {
		openFile(list1.at(i));
	}

	emit needToShow();
}

void GCodeWorkShop::createFindToolBar()
{
	QString selText;
	QTextCursor cursor;

	if (!findToolBar) {
		findToolBar = new QToolBar(tr("Find"));
		addToolBar(Qt::BottomToolBarArea, findToolBar);
		findToolBar->setObjectName("Find");

		findToolBar->setAttribute(Qt::WA_DeleteOnClose);

		findNextAct = new QAction(QIcon(":/images/arrow-right.png"), tr("Find next"), this);
		findNextAct->setShortcut(QKeySequence::FindNext);
		findNextAct->setToolTip(tr("Find next"));
		connect(findNextAct, SIGNAL(triggered()), this, SLOT(findNext()));

		findPreviousAct = new QAction(QIcon(":/images/arrow-left.png"), tr("Find previous"), this);
		findPreviousAct->setShortcut(QKeySequence::FindPrevious);
		findPreviousAct->setToolTip(tr("Find previous"));
		connect(findPreviousAct, SIGNAL(triggered()), this, SLOT(findPrevious()));

		replaceNextAct = new QAction(QIcon(":/images/arrow-right.png"), tr("Replace && find next"),
		                             this);
		//replaceNextAct->setShortcut(tr("F3"));
		replaceNextAct->setToolTip(tr("Replace && find next"));
		connect(replaceNextAct, SIGNAL(triggered()), this, SLOT(replaceNext()));

		replacePreviousAct = new QAction(QIcon(":/images/arrow-left.png"),
		                                 tr("Replace && find previous"), this);
		//replacePreviousAct->setShortcut(tr("F3"));
		replacePreviousAct->setToolTip(tr("Replace && find previous"));
		connect(replacePreviousAct, SIGNAL(triggered()), this, SLOT(replacePrevious()));

		replaceAllAct = new QAction(QIcon(":/images/arrow-right-double.png"), tr("Replace all"), this);
		//replaceAllAct->setShortcut(tr("F3"));
		replaceAllAct->setToolTip(tr("Replace all"));
		connect(replaceAllAct, SIGNAL(triggered()), this, SLOT(replaceAll()));

		findCloseAct = new QAction(QIcon(":/images/close_small.png"), tr("Close find toolbar"), this);
		findCloseAct->setToolTip(tr("Close find toolbar"));
		connect(findCloseAct, SIGNAL(triggered()), this, SLOT(closeFindToolBar()));

		findLabel = new QLabel(tr("Find:"));
		findToolBar->addWidget(findLabel);
		findEdit = new QLineEdit();
		findEdit->setClearButtonEnabled(true);
		findEdit->setToolTip(
		    tr("<b>Letter$$</b> - matches any number.<p><b>Letter$max$min</b> - matches number &lt;=max &gt;=min.</p>"
		       \
		       "<p><b>$min</b> can be ommited, then equal 0</p>" \
		       "<p><b>X$100$-10</b> - matches all X with value -10 to 100</p>"));
		CapsLockEventFilter* findEditEventFilter = new CapsLockEventFilter(findEdit);
		connect(this, SIGNAL(intCapsLockChanged(bool)), findEditEventFilter, SLOT(setCapsLockEnable(bool)));
		bool intCapsLock = false;
		GCoderWidgetProperties* prop = dynamic_cast<GCoderWidgetProperties*>(m_documentManager->documentWidgetProperties(
		                                   GCoder::DOCUMENT_TYPE).get());

		if (prop) {
			intCapsLock = prop->intCapsLock;
		}

		findEditEventFilter->setCapsLockEnable(intCapsLock);
		findEdit->installEventFilter(findEditEventFilter);
		connect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));
		findToolBar->addWidget(findEdit);
		findToolBar->addAction(findPreviousAct);
		findToolBar->addAction(findNextAct);
		findToolBar->addSeparator();

		replaceLabel = new QLabel(tr("Replace with:"));
		findToolBar->addWidget(replaceLabel);
		replaceEdit = new QLineEdit();
		replaceEdit->setClearButtonEnabled(true);
		replaceEdit->setToolTip(
		    tr("<b>$$OperatorNumber</b> - do some math on replaced numbers. Operator +-*/" \
		       "<p>$$+1 - will add 1 to replaced numbers</p>"));
		CapsLockEventFilter* replaceEditEventFilter = new CapsLockEventFilter(replaceEdit);
		connect(this, SIGNAL(intCapsLockChanged(bool)), replaceEditEventFilter, SLOT(setCapsLockEnable(bool)));
		replaceEditEventFilter->setCapsLockEnable(intCapsLock);
		replaceEdit->installEventFilter(replaceEditEventFilter);
		findToolBar->addWidget(replaceEdit);
		findToolBar->addAction(replacePreviousAct);
		findToolBar->addAction(replaceNextAct);
		findToolBar->addAction(replaceAllAct);
		findToolBar->addSeparator();

		mCheckIgnoreCase = new QCheckBox(tr("Ignore c&ase"));
		connect(mCheckIgnoreCase, SIGNAL(clicked()), this, SLOT(findTextChanged()));
		findToolBar->addWidget(mCheckIgnoreCase);
		mCheckFindWholeWords = new QCheckBox(tr("&Whole words only"));
		connect(mCheckFindWholeWords, SIGNAL(clicked()), this, SLOT(findTextChanged()));
		findToolBar->addWidget(mCheckFindWholeWords);
		//findToolBar->addSeparator();
		mCheckIgnoreComments = new QCheckBox(tr("Ignore c&omments"));
		connect(mCheckIgnoreComments, SIGNAL(clicked()), this, SLOT(findTextChanged()));
		findToolBar->addWidget(mCheckIgnoreComments);
		findToolBar->addSeparator();
		findToolBar->addAction(findCloseAct);

		QSettings& settings = *Medium::instance().settings();
		mCheckIgnoreComments->setChecked(settings.value("FindIgnoreComments", true).toBool());
		mCheckFindWholeWords->setChecked(settings.value("FindWholeWords", false).toBool());
		mCheckIgnoreCase->setChecked(settings.value("FindIgnoreCase", true).toBool());
	} else {
		findToolBar->show();
	}

	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		disconnect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));

		if (gdoc->hasSelection()) {
			selText = gdoc->selectedText();
		} else {
			selText = gdoc->wordUnderCursor();
		}

		if (selText.size() < 32) {
			findEdit->setText(selText);
		}

		findEdit->setPalette(QPalette());
		connect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));
		findEdit->setFocus(Qt::MouseFocusReason);

		gdoc->highlightFindText(findEdit->text(),
		                        ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
		                          QTextDocument::FindFlags()) |
		                         (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
		                          QTextDocument::FindFlags())), mCheckIgnoreComments->isChecked());
	}

	findEdit->selectAll();
}

void GCodeWorkShop::closeFindToolBar()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->widget()->setFocus(Qt::MouseFocusReason);
		gdoc->highlightFindText("");
		gdoc->centerCursor();
	}

	QSettings& settings = *Medium::instance().settings();
	settings.setValue("FindIgnoreComments", mCheckIgnoreComments->isChecked());
	settings.setValue("FindWholeWords", mCheckFindWholeWords->isChecked());
	settings.setValue("FindIgnoreCase", mCheckIgnoreCase->isChecked());

	findToolBar->close();
	//findToolBar = nullptr;
}

void GCodeWorkShop::findTextChanged()
{
	if (findEdit->text().contains(QRegularExpression("\\$\\$"))
	        || findEdit->text().contains(
	            QRegularExpression("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"))) {
		replaceAllAct->setEnabled(false);
	} else {
		replaceAllAct->setEnabled(true);
	}

	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->clearSelection(true);

		if (!findEdit->text().isEmpty()) {
			findNext();
		} else {
			findEdit->setPalette(QPalette());
		}
	}
}

void GCodeWorkShop::createSerialToolBar()
{
	if (serialToolBar == nullptr) {
		serialToolBar = new QToolBar(tr("Serial port toolbar"));
		addToolBar(Qt::TopToolBarArea, serialToolBar);
		serialToolBar->setObjectName("SerialToolBar");

		configPortAct = new QAction(QIcon(":/images/serialconfig.png"), tr("Serial port configuration"),
		                            this);
		//configPortAct->setShortcut(tr("F3"));
		configPortAct->setToolTip(tr("Serial port configuration"));
		connect(configPortAct, SIGNAL(triggered()), this, SLOT(serialConfig()));

		receiveAct = new QAction(QIcon(":/images/receive.png"), tr("Receive new file"), this);
		//receiveAct->setShortcut(tr("Shift+F3"));
		receiveAct->setToolTip(tr("Receive new file"));
		connect(receiveAct, SIGNAL(triggered()), this, SLOT(receiveButtonClicked()));

		sendAct = new QAction(QIcon(":/images/send.png"), tr("Send current file"), this);
		//sendAct->setShortcut(tr("F3"));
		sendAct->setToolTip(tr("Send current file"));
		connect(sendAct, SIGNAL(triggered()), this, SLOT(sendButtonClicked()));

		attachToDirAct = new QAction(QIcon(":/images/attach.png"),
		                             tr("Attach current port settings to current directory of programs"), this);
		//attachToDirAct->setShortcut(tr("F3"));
		attachToDirAct->setToolTip(tr("Attach current port settings to current directory of programs"));
		connect(attachToDirAct, SIGNAL(triggered()), this, SLOT(attachToDirButtonClicked()));

		deAttachToDirAct = new QAction(QIcon(":/images/deattach.png"),
		                               tr("Remove settings from the current directory"), this);
		//deAttachToDirAct->setShortcut(tr("F3"));
		deAttachToDirAct->setToolTip(tr("Remove settings from the directory"));
		connect(deAttachToDirAct, SIGNAL(triggered()), this, SLOT(deAttachToDirButtonClicked()));

		diagAct = new QAction(QIcon(":/images/serialtest.png"), tr("Check serial port settings"), this);
		//diagAct->setShortcut(tr("F3"));
		diagAct->setToolTip(tr("Check serial port settings"));
		connect(diagAct, SIGNAL(triggered()), this, SLOT(serialConfigTest()));

		serialCloseAct = new QAction(QIcon(":/images/close_small.png"),
		                             tr("Close send/receive toolbar"), this);
		serialCloseAct->setToolTip(tr("Close send/receive toolbar"));
		connect(serialCloseAct, SIGNAL(triggered()), this, SLOT(closeSerialToolbar()));

		commAppAct = new QAction(QIcon(":/images/spserver.png"),
		                         tr("Start application \"Serial port file server\""), this);
		//diagAct->setShortcut(tr("F3"));
		commAppAct->setToolTip(tr("Start application \"Serial port file server\""));
		connect(commAppAct, SIGNAL(triggered()), this, SLOT(startSerialPortServer()));

		configBox = new QComboBox();
		configBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		configBox->setDuplicatesEnabled(false);

		//serialToolBar->addSeparator();
		serialToolBar->addAction(commAppAct);
		serialToolBar->addAction(attachToDirAct);
		serialToolBar->addAction(deAttachToDirAct);
		serialToolBar->addSeparator();
		serialToolBar->addAction(diagAct);
		serialToolBar->addWidget(configBox);
		serialToolBar->addAction(configPortAct);
		serialToolBar->addSeparator();
		serialToolBar->addAction(receiveAct);
		serialToolBar->addSeparator();
		serialToolBar->addAction(sendAct);

		serialToolBar->addSeparator();
		serialToolBar->addAction(serialCloseAct);
	} else if (!showSerialToolBarAct->isChecked()) {
		closeSerialToolbar();
		return;
	} else {
		serialToolBar->show();
		showSerialToolBarAct->setChecked(true);
	}

	loadSerialConfignames();
	configBox->adjustSize();
	updateCurrentSerialConfig();
}

void GCodeWorkShop::closeSerialToolbar()
{
	serialToolBar->close();
	delete (serialToolBar);
	serialToolBar = nullptr;
	showSerialToolBarAct->setChecked(false);
}

void GCodeWorkShop::attachToDirButtonClicked(bool attach)
{
	QFile file;

	bool hasMdiChild = (activeDocument() != 0);

	if (hasMdiChild && (serialToolBar != nullptr)) {
		QDir dir = activeDocument()->dir();
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Name);
		dir.setNameFilters(QStringList("*.ini"));

		QFileInfoList list = dir.entryInfoList();

		for (const QFileInfo& fileInfo : list) {
			file.setFileName(fileInfo.absoluteFilePath());
			file.remove();
		}

		if (attach) {
			file.setFileName(activeDocument()->path() + "/" + configBox->currentText() + ".ini");
			file.open(QIODevice::ReadWrite);
			file.close();;
		}
	}
}

void GCodeWorkShop::deAttachToDirButtonClicked()
{
	attachToDirButtonClicked(false);
}

void GCodeWorkShop::createUserToolTipsFile()
{
	QString fileName;

	if (activeDocument()) {
		fileName = activeDocument()->path();
	} else {
		return;
	}

	fileName += "/cnc_tips.txt";

	if (QFileInfo(fileName).exists()) {
		openFile(fileName);
	} else {
		QFile file(fileName);

		if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
			return;
		}

		// qDebug() << fileName;

		QTextStream out(&file);
		out << "# " << fileName << "\n" << "\n";
		out << "#+++++++++++++++++++++++++++++++++\n";
		out << tr("# ++++++ EXAMPLE ++++++") << "\n";
		out << "# [OKUMA]" << "\n";
		out << tr("# M00=\"<b>M00</b> - program stop, unconditional\"") << "\n";
		out << "#+++++++++++++++++++++++++++++++++\n\n";

		out << "[OKUMA]" << "\n" << "\n" << "\n";
		out << "[FANUC]" << "\n" << "\n" << "\n";
		out << "[SINUMERIK]" << "\n" << "\n" << "\n";
		out << "[SINUMERIK_840]" << "\n" << "\n" << "\n";
		out << "[PHILIPS]" << "\n" << "\n" << "\n";
		out << "[HEIDENHAIN]" << "\n" << "\n" << "\n";
		out << "[HEIDENHAIN_ISO]" << "\n" << "\n" << "\n";
		file.close();
		openFile(fileName);
	}

	m_documentManager->setActiveDocument(fileName);
}

void GCodeWorkShop::createGlobalToolTipsFile()
{
	QString fileName = writeTooltipFile();


	if (QFileInfo(fileName).exists()) {
		openFile(fileName);
	}

	m_documentManager->setActiveDocument(fileName);
}

void GCodeWorkShop::attachHighlighterToDirButtonClicked(bool attach)
{
	QFile file;

	bool hasMdiChild = (activeDocument() != 0);

	if (hasMdiChild) {
		QDir dir = activeDocument()->dir();
		dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		dir.setSorting(QDir::Name);
		dir.setNameFilters(QStringList("*.cfg"));

		QFileInfoList list = dir.entryInfoList();

		for (const QFileInfo& fileInfo : list) {
			file.setFileName(fileInfo.absoluteFilePath());
			file.remove();
		}

		if (attach) {
			file.setFileName(activeDocument()->path() + "/" + highlightTypeCombo->currentText() +
			                 ".cfg");
			file.open(QIODevice::ReadWrite);
			file.close();;
		}
	}
}

void GCodeWorkShop::attachHighlightToDirActClicked()
{
	attachHighlighterToDirButtonClicked(true);
}

void GCodeWorkShop::deAttachHighlightToDirActClicked()
{
	attachHighlighterToDirButtonClicked(false);
}

int GCodeWorkShop::defaultHighlightMode(const QString& filePath)
{
	QDir dir;
	bool ok;

	dir.setPath(filePath);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Name);
	dir.setNameFilters(QStringList("*.cfg"));

	QFileInfoList list = dir.entryInfoList();

	if (!list.isEmpty()) {
		QFileInfo name = list.at(0);
		int id = highlightTypeCombo->findText(name.baseName());

		if (id >= 0) {
			//highlightTypeCombo->setCurrentIndex(id);
			return (highlightTypeCombo->itemData(id).toInt(&ok));
		}
	}

	return MODE_AUTO;
}

void GCodeWorkShop::projectAdd()
{
	QFileInfo file;
	QStandardItem* item;
	QIcon icon;

	if (currentProject == nullptr) {
		return;
	}


	QString filters = tr("All files (*.* *);;"
	                     "CNC programs files (*.nc);;"
	                     "CNC programs files (*.nc *.ngc *.min *.anc *.cnc);;"
	                     "Documents (*.odf *.odt *.pdf *.doc *.docx  *.xls *.xlsx);;"
	                     "Drawings (*.dwg *.dxf);;"
	                     "Pictures (*.jpg *.bmp *.svg);;"
	                     "Text files (*.txt)");
	QStringList files = QFileDialog::getOpenFileNames(
	                        this,
	                        tr("Add files to project"),
	                        lastOpenedPath(),
	                        filters, 0);

	QStringList list = files;

	if (list.isEmpty()) {
		return;
	}

	QStringList::Iterator it = list.begin();

	QStandardItem* parentItem = currentProject;

	if (it != list.end()) {
		file.setFile(*it);

		if ((file.absoluteDir().exists()) && (file.absoluteDir().isReadable())) {

			QList<QStandardItem*> items = model->findItems(QDir::toNativeSeparators(
			                                  file.absoluteDir().canonicalPath()),
			                              Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive, 0);

			if (!items.isEmpty()) {
				item = items.at(0);

				if (item->text() != file.absoluteDir().canonicalPath()) {
					item = new QStandardItem(QIcon(":/images/folder.png"),
					                         QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
					parentItem->appendRow(item);
				}
			} else {
				item = new QStandardItem(QIcon(":/images/folder.png"),
				                         QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
				parentItem->appendRow(item);
			}

			parentItem = item;
		} else {
			return;
		}
	}

	QFileSystemModel* fModel = new QFileSystemModel;

	while (it != list.end()) {
		file.setFile(*it);

		if ((file.exists()) && (file.isReadable())) {
			icon = fModel->iconProvider()->icon(file);

			if (icon.isNull()) {
				icon = QIcon(":/images/ncfile.png");
			}

			item = new QStandardItem(icon, file.fileName());
			parentItem->appendRow(item);
		}

		++it;
	}

	ui->projectTreeView->expandAll(); //model->indexFromItem(currentProject));

	currentProjectModified = true;
	statusBar()->showMessage(tr("Project opened"), 5000);
}

void GCodeWorkShop::projectSave()
{
	QString path, fileName;
	int fileCount;
	QStandardItem* item;


	if (currentProjectName.isEmpty() || currentProject == nullptr) {
		return;
	}

	QSettings settings(currentProjectName, QSettings::IniFormat);

	settings.remove("ProjectFiles");
	settings.beginWriteArray("ProjectFiles");

	fileCount = 0;

	for (int i = 0; i < currentProject->rowCount(); i++) {
		item = currentProject->child(i, 0);
		path = item->text();

		for (int j = 0; j < item->rowCount(); j++) {
			fileName = item->child(j, 0)->text();

			//qDebug() << path;
			//qDebug() << fileName;
			settings.setArrayIndex(fileCount);
			settings.setValue("File", QFileInfo(path, fileName).absoluteFilePath());
			fileCount++;
		}
	}

	settings.endArray();

	if (settings.status() == QSettings::NoError) {
		currentProjectModified = false;
		statusBar()->showMessage(tr("Project saved"), 5000);
	}
}

void GCodeWorkShop::projectSaveAs()
{
	QString fileName = projectSelectName();

	if (fileName.isEmpty()) {
		return;
	}

	currentProjectName = fileName;
	QStandardItem* parentItem = model->invisibleRootItem();
	parentItem->child(0, 0)->setText(QFileInfo(currentProjectName).fileName());
	parentItem->child(0, 0)->setToolTip(QDir::toNativeSeparators(QFileInfo(
	                                        currentProjectName).absoluteFilePath()));
	projectSave();
}

void GCodeWorkShop::projectNew()
{
	if (!maybeSaveProject()) {
		return;
	}

	QString fileName = projectSelectName();

	if (fileName.isEmpty()) {
		return;
	}

	currentProjectName = fileName;

	QStandardItem* parentItem = model->invisibleRootItem();
	QStandardItem* item = new QStandardItem(QIcon(":/images/edytornc.png"),
	                                        QFileInfo(currentProjectName).fileName());

	parentItem->appendRow(item);

	currentProject = item;
	currentProjectModified = true;
}

void GCodeWorkShop::projectTreeViewDoubleClicked(const QModelIndex& index)
{
	QFileInfo file;

	if ((!index.isValid())) {
		return;
	}

	QStandardItem* item = model->itemFromIndex(index);

	if (item == nullptr || item->parent() == nullptr) {
		return;
	}

	if (item->hasChildren()) {
		return;
	}

	file.setFile(item->parent()->text(), item->text());

	if ((file.exists()) && (file.isReadable())) {
		if (m_extensions.contains("*." + file.suffix())) {
			openFile(file.canonicalFilePath());
		} else {
			QDesktopServices::openUrl(QUrl("file:///" + file.absoluteFilePath(), QUrl::TolerantMode));
		}
	}
}

void GCodeWorkShop::fileTreeViewDoubleClicked(const QModelIndex& index)
{
	QFileInfo file;

	if (!index.isValid()) {
		return;
	}

	file.setFile(dirModel->filePath(index));

	if ((file.exists()) && (file.isReadable())) {
		if (file.isDir()) {
			QString path = dirModel->filePath(index);

			if (path.endsWith("..")) {
				int idx = path.lastIndexOf('/');

				if (idx > 0) {
					idx = path.lastIndexOf('/', idx - 1);

					if (idx > 0) {
						path.remove(idx, (path.length() - idx));
					}
				}
			}

			fileTreeViewChangeRootDir(path);
		} else if (m_extensions.contains("*." + file.suffix())) {
			openFile(file.canonicalFilePath());
		} else {
			QDesktopServices::openUrl(QUrl("file:///" + file.absoluteFilePath(), QUrl::TolerantMode));
		}
	}
}

QString GCodeWorkShop::projectSelectName()
{
	QString filters = tr("GCodeWorkShop project file (*.ncp)");
	QString file = QFileDialog::getSaveFileName(
	                   this,
	                   tr("Select the project name and location..."),
	                   currentProjectName,
	                   filters);

	return file;
}

void GCodeWorkShop::projectOpen()
{
	if (!maybeSaveProject()) {
		return;
	}

	QString filters = tr("GCodeWorkShop project file (*.ncp)");
	QString fileName = QFileDialog::getOpenFileName(
	                       this,
	                       tr("Open the project file..."),
	                       currentProjectName,
	                       filters);

	if (fileName.isEmpty()) {
		return;
	}

	projectLoad(fileName);
}

void GCodeWorkShop::hidePanel()
{
	ui->hSplitter->setUpdatesEnabled(false);

	if (!panelHidden) {
		panelState = ui->hSplitter->saveState();
		ui->frame->setMaximumWidth(ui->hideButton->width());
		ui->vSplitter->hide();
		//openFileTableWidget->hide();
		ui->hideButton->setText(">>");
		ui->hideButton->setToolTip(tr("Show"));
		panelHidden = true;
	} else {
		panelHidden = false;
		fileTreeViewChangeRootDir();
		ui->frame->setMaximumWidth(16777215);
		ui->vSplitter->show();
		//openFileTableWidget->show();
		ui->hideButton->setText("<<");
		ui->hideButton->setToolTip(tr("Hide"));
		ui->hSplitter->restoreState(panelState);
	}

	ui->hSplitter->updateGeometry();
	ui->hSplitter->setUpdatesEnabled(true);
}

void GCodeWorkShop::projectTreeRemoveItem()
{
	QModelIndexList list = ui->projectTreeView->selectionModel()->selectedIndexes();

	for (QModelIndex it : list) {
		QStandardItem* item = model->itemFromIndex(it);

		if (item == nullptr) {
			return;
		}

		if (!item->hasChildren()) {
			currentProjectModified = model->removeRow(item->row(), model->indexFromItem(item->parent()));
		}
	}
}

void GCodeWorkShop::projectLoad(const QString& projectName)
{
	QFileInfo file;
	QIcon icon;


	if (projectName.isEmpty()) {
		return;
	}

	currentProjectName = projectName;

	model->clear();

	QSettings settings(currentProjectName, QSettings::IniFormat);

	QStandardItem* item = new QStandardItem(QIcon(":/images/edytornc.png"),
	                                        QFileInfo(currentProjectName).fileName());
	item->setToolTip(QDir::toNativeSeparators(currentProjectName));

	model->invisibleRootItem()->appendRow(item);

	currentProject = item;

	QFileSystemModel* fModel = new QFileSystemModel;

	int max = settings.beginReadArray("ProjectFiles");

	for (int i = 0; i < max; ++i) {
		settings.setArrayIndex(i);
		file.setFile(settings.value("File", "").toString());

		if ((file.absoluteDir().exists()) && (file.absoluteDir().isReadable())) {
			QList<QStandardItem*> items = model->findItems(file.absoluteDir().canonicalPath(),
			                              Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive, 0);

			if (!items.isEmpty()) {
				item = items.at(0);

				if (item->text() != file.absoluteDir().canonicalPath()) {
					item = new QStandardItem(QIcon(":/images/folder.png"), file.absoluteDir().canonicalPath());
					item->setToolTip(QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
					currentProject->appendRow(item);
				}
			} else {
				item = new QStandardItem(QIcon(":/images/folder.png"), file.absoluteDir().canonicalPath());
				item->setToolTip(QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
				currentProject->appendRow(item);
			}

			if ((file.exists()) && (file.isReadable())) {
				icon = fModel->iconProvider()->icon(file);

				if (icon.isNull()) {
					icon = QIcon(":/images/ncfile.png");
				}

				QStandardItem* childItem = new QStandardItem(icon, file.fileName());
				childItem->setToolTip(file.fileName());
				item->appendRow(childItem);
			}
		}
	}

	settings.endArray();

	ui->projectTreeView->expandAll();

	currentProjectModified = false;
}

bool GCodeWorkShop::maybeSaveProject()
{
	if (currentProjectModified) {
		QMessageBox msgBox;
		msgBox.setParent(this, Qt::Dialog);
		msgBox.setText(tr("<b>Project: \"%1\"\n has been modified.</b>").arg(currentProjectName));
		msgBox.setInformativeText(tr("Do you want to save your changes ?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setIcon(QMessageBox::Warning);
		int ret = msgBox.exec();

		switch (ret) {
		case QMessageBox::Save:
			projectSave();
			return true;
			break;

		case QMessageBox::Discard:
			currentProjectModified = false;
			return true;
			break;

		case QMessageBox::Cancel:
			return false;
			break;

		default:
			return true;
			break;
		}
	}

	return true;
}

void GCodeWorkShop::createFileBrowseTabs()
{
	dirModel = new QFileSystemModel();
	dirModel->setResolveSymlinks(true);

	//dirModel->setRootPath(lastDir.absolutePath());
	//fileTreeViewChangeRootDir();

	dirModel->setNameFilters(m_extensions); //QStringList("*.nc")
	dirModel->setNameFilterDisables(false);
	dirModel->setFilter(QDir::Files | QDir::AllDirs | QDir::Drives | QDir::NoDot);

	ui->fileTreeView->setModel(dirModel);
	fileTreeViewChangeRootDir();

	connect(ui->fileTreeView, SIGNAL(doubleClicked(QModelIndex)), this,
	        SLOT(fileTreeViewDoubleClicked(QModelIndex)));
	connect(ui->openFileTableWidget, SIGNAL(cellClicked(int, int)), this,
	        SLOT(openFileTableWidgetClicked(int, int)));
	ui->openFileTableWidget->setToolTip(tr("Open files"));
}

void GCodeWorkShop::updateOpenFileList()
{
	QFileInfo file;
	QStringList labels;

	ui->openFileTableWidget->setUpdatesEnabled(false);

	ui->openFileTableWidget->clear();
	labels << tr("Info") << tr("File Name") << "";
	ui->openFileTableWidget->setHorizontalHeaderLabels(labels);
	ui->openFileTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	QList<Document*> docList = m_documentManager->documentList();

	ui->openFileTableWidget->setSortingEnabled(false);
	ui->openFileTableWidget->setRowCount(docList.size());

	for (int i = 0; i < docList.size(); ++i) {
		Document* doc = docList.at(i);

		file.setFile(doc->filePath());

		QTableWidgetItem* newItem = new QTableWidgetItem(file.fileName() + (doc->isModified() ? "*" : ""));

		if (file.canonicalFilePath().isEmpty()) {
			newItem->setToolTip(doc->filePath());
		} else {
			newItem->setToolTip(QDir::toNativeSeparators(file.canonicalFilePath()));
		}

		ui->openFileTableWidget->setItem(i, 1, newItem);

		newItem = new QTableWidgetItem(doc->brief());
		newItem->setToolTip(doc->brief() + " --> " + QDir::toNativeSeparators(
		                        file.canonicalFilePath()));
		ui->openFileTableWidget->setItem(i, 0, newItem);

		newItem = new QTableWidgetItem(QIcon(":/images/fileclose_small.png"), "",
		                               QTableWidgetItem::UserType);
		newItem->setToolTip(tr("Close"));
		ui->openFileTableWidget->setItem(i, 2, newItem);

		if (doc == activeDocument()) {
			ui->openFileTableWidget->selectRow(i);
		}
	}

	ui->openFileTableWidget->setVisible(false);
	ui->openFileTableWidget->resizeRowsToContents();

	ui->openFileTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
	ui->openFileTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->openFileTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	//ui->openFileTableWidget->resizeColumnsToContents();
	ui->openFileTableWidget->setSortingEnabled(true);
	ui->openFileTableWidget->setVisible(true);

	ui->openFileTableWidget->setUpdatesEnabled(true);
}

void GCodeWorkShop::openFileTableWidgetClicked(int x, int y)
{
	QTableWidgetItem* item = ui->openFileTableWidget->item(x, 1);

	Document* existing = findDocument(item->toolTip());

	if (existing) {
		if (y == 2) {
			existing->close();
			updateOpenFileList();
		} else {
			m_documentManager->setActiveDocument(existing);
		}
	}
}

void GCodeWorkShop::fileTreeViewChangeRootDir()
{
	QString path;

	if (panelHidden) { //if((!isVisible()) || panelHidden)
		return;
	}

	if (ui->tabWidget->currentIndex() != 1) {
		return;
	}

	if ((ui->fileTreeView == nullptr) || (dirModel == nullptr)) {
		return;
	}

	if (ui->currentPathCheckBox->isChecked() && (activeDocument() != nullptr)) {
		path = activeDocument()->filePath();

		if (QFileInfo(path).exists()) {
			path = QFileInfo(path).canonicalPath();
		} else {
			path = path.remove(QFileInfo(path).fileName());
		}
	} else {
		path = lastOpenedPath();
	}

	if (path.isEmpty()) {
		return;
	}

	if (dirModel->rootPath() == path) {
		return;
	}

	fileTreeViewChangeRootDir(path);
}

void GCodeWorkShop::fileTreeViewChangeRootDir(const QString& path)
{
	ui->fileTreeView->setRootIndex(dirModel->index(path));
	dirModel->setRootPath(path);
	//ui->fileTreeView->setToolTip(path);
	ui->fileTreeView->setSortingEnabled(true);
	ui->fileTreeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->fileTreeView->resizeColumnToContents(0);
	ui->fileTreeView->resizeColumnToContents(1);
	ui->fileTreeView->setColumnHidden(2, true);
	ui->fileTreeView->resizeColumnToContents(3);
}

bool GCodeWorkShop::event(QEvent* event)
{
	QString key, text;
	QModelIndex index;
	QFile file;
	QString fileName;

	if ((event->type() == QEvent::ToolTip)) {
		if (panelHidden) {
			return true;
		}

		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);

		QPoint pos = ui->fileTreeView->viewport()->mapFromGlobal(helpEvent->globalPos());

		if ((pos.y() >= ui->fileTreeView->viewport()->height()) ||
		        (pos.x() >= ui->fileTreeView->viewport()->width()) || (ui->tabWidget->currentIndex() != 1)) {
			return true;
		}

		index = ui->fileTreeView->indexAt(pos);

		if (!index.isValid()) {
			return true;
		}

		fileName = dirModel->filePath(index);
		file.setFileName(fileName);
		text = "<b>" + QDir::toNativeSeparators(fileName) + "</b>";

		if (ui->filePreviewSpinBox->value() > 0) {
			text.append("<br />");

			if (file.open(QIODevice::ReadOnly)) {
				for (int i = 0; i < ui->filePreviewSpinBox->value(); i++) {
					char buf[1024];
					qint64 lineLength = file.readLine(buf, sizeof(buf));

					if (lineLength != -1) {
						text.append(buf);
					}
				}

				file.close();

				if (text.endsWith('\n')) {
					text.remove(text.size() - 1, 1);
				}
			}
		}

		if (!text.isEmpty()) {
			if (text.length() < fileName.size()) {
				key = "<p style='white-space:normal'>";
			} else {
				key = "<p style='white-space:pre'>";
			}

			QToolTip::showText(helpEvent->globalPos(), key + text, this, QRect());
		} else {
			QToolTip::hideText();
			event->ignore();
		}

		return true;
	}

	return QWidget::event(event);
}

void GCodeWorkShop::updateSessionMenus(const QStringList& sessionList)
{
	sessionsMenu->clear();

	// TODO memory leack?
	QActionGroup* actionGroup = new QActionGroup(sessionsMenu);
	actionGroup->setExclusive(true);

	bool checked = true;

	for (const QString& name : sessionList) {
		QAction* action = actionGroup->addAction(name);
		action->setCheckable(true);
		action->setChecked(checked);
		checked = false;
	}

	sessionsMenu->addActions(actionGroup->actions());
}

void GCodeWorkShop::sessionsChanged()
{
	m_sessionManager->save(Medium::instance().settings());
}

void GCodeWorkShop::changeSession(QAction* action)
{
	m_sessionManager->setCurrentSession(action->text());
}

void GCodeWorkShop::beforeCurrentSessionChanged()
{
	storeFileInfoInSession();
}

void GCodeWorkShop::currentSessionChanged()
{
	closeAllMdiWindows();
	openFilesFromSession();
	statusBar()->showMessage(tr("Session %1 loaded").arg(m_sessionManager->currentSession()), 5000);
}

void GCodeWorkShop::openFilesFromSession()
{
	for (const DocumentInfo::Ptr& info : m_sessionManager->documentInfoList()) {
		m_MdiWidgetsMaximized = false;
		loadFile(info, false);
	}
}

void GCodeWorkShop::storeFileInfoInSession()
{
	QList<DocumentInfo::Ptr> infoList;

	for (const Document* doc : m_documentManager->documentList()) {
		DocumentInfo::Ptr info = doc->documentInfo();
		infoList.append(info);
	}

	m_sessionManager->setDocumentInfoList(infoList);
}

void GCodeWorkShop::showSessionDialog()
{
	SessionDialog sesDialog(this, m_sessionManager);
	sesDialog.exec();
}

void GCodeWorkShop::savePrinterSettings(QPrinter* printer)
{
#ifndef QT_NO_PRINTER

	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("PrinterSettings");

	settings.setValue("PrinterName", printer->printerName());
	settings.setValue("CollateCopies", printer->collateCopies());
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
	settings.setValue("Orientation", printer->orientation());
#else
	settings.setValue("Orientation", printer->pageLayout().orientation());
#endif
	settings.setValue("ColorMode", printer->colorMode());
	QPageLayout layout = printer->pageLayout();
	settings.setValue("PageSize", layout.pageSize().id());
	settings.setValue("Duplex", printer->duplex());
	//settings.setValue("Resolution", printer->resolution());

	settings.endGroup();

#endif
}

void GCodeWorkShop::loadPrinterSettings(QPrinter* printer)
{
#ifndef QT_NO_PRINTER

	QSettings& settings = *Medium::instance().settings();

	settings.beginGroup("PrinterSettings");

	printer->setPrinterName(settings.value("PrinterName").toString());
	printer->setCollateCopies(settings.value("CollateCopies").toBool());
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
	printer->setOrientation((QPrinter::Orientation)settings.value("Orientation").toInt());
#else
	printer->pageLayout().setOrientation((QPageLayout::Orientation)
	                                     settings.value("Orientation").toInt());
#endif
	printer->setColorMode((QPrinter::ColorMode)settings.value("ColorMode").toInt());

	QPageLayout layout = printer->pageLayout();
	layout.setPageSize(QPageSize((QPageSize::PageSizeId)settings.value("PageSize",
	                             (int)QPageSize::A4).toInt()));
	printer->setPageLayout(layout);

	//printer->setPaperSize((QPrinter::PaperSize)settings.value("PageSize", (int)QPrinter::A4).toInt());
	printer->setDuplex((QPrinter::DuplexMode)settings.value("Duplex").toInt());
	//printer->setResolution(settings.value("Resolution").toInt());

	settings.endGroup();

#endif
}

void GCodeWorkShop::serialConfig()
{
	SerialPortConfigDialog* serialConfigDialog = new SerialPortConfigDialog(this,
	    configBox->currentText());

	if (serialConfigDialog->exec() == QDialog::Accepted) {
		loadSerialConfignames();
	}
}

void GCodeWorkShop::loadSerialConfignames()
{
	int id;
	QStringList list;
	QString item;

	QSettings& settings = *Medium::instance().settings();
	settings.beginGroup("SerialPortConfigs");

	configBox->clear();
	list = settings.value("SettingsList", QStringList(tr("Default"))).toStringList();
	list.sort();
	configBox->addItems(list);
	item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
	id = configBox->findText(item);

	if (id >= 0) {
		configBox->setCurrentIndex(id);
	}

	settings.endGroup();
}

void GCodeWorkShop::serialConfigTest()
{
	SerialPortTestDialog* trDialog = new SerialPortTestDialog(this);

	trDialog->show();
}

void GCodeWorkShop::sendButtonClicked()
{
	QString tx;
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (!gdoc) {
		return;
	}

	receiveAct->setEnabled(false);
	sendAct->setEnabled(false);
	commAppAct->setEnabled(false);
	QApplication::setOverrideCursor(Qt::BusyCursor);

	tx.append(gdoc->text());

	SerialTransmissionDialog transmissionDialog(this);
	transmissionDialog.sendData(tx, configBox->currentText());

	receiveAct->setEnabled(true);
	sendAct->setEnabled(true);
	commAppAct->setEnabled(true);
	QApplication::restoreOverrideCursor();
}

void GCodeWorkShop::receiveButtonClicked()
{
	receiveAct->setEnabled(false);
	sendAct->setEnabled(false);
	commAppAct->setEnabled(false);
	QApplication::setOverrideCursor(Qt::BusyCursor);

	SerialTransmissionDialog transmissionDialog(this);
	QStringList progList = transmissionDialog.receiveData(configBox->currentText());

	if (!progList.isEmpty()) {
		int id = configBox->currentIndex();

		QStringList::const_iterator it = progList.constBegin();

		if ((*it) == "#FILE_LIST#") {
			it++;

			while (it != progList.constEnd()) {
				openFile(*it);
				it++;
			}
		} else {
			if (!(*it).isEmpty() && !(*it).isNull()) {
				GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(newFile());

				if (gdoc) {
					gdoc->clear();
					gdoc->insertText(*it);
					gdoc->setHighlightMode(MODE_AUTO);
					//activeWindow->setReadOnly(defaultMdiWindowProperites.defaultReadOnly);
					gdoc->clearUndoRedoStacks();
				}
			}
		}

		configBox->setCurrentIndex(id);
	}

	receiveAct->setEnabled(true);
	sendAct->setEnabled(true);
	commAppAct->setEnabled(true);
	QApplication::restoreOverrideCursor();
}

void GCodeWorkShop::fileChanged(const QString& fileName)
{
	Document* doc = findDocument(fileName);
	bool modified = false;

	if (doc) {
		modified = doc->isModified();
		m_documentManager->setActiveDocument(doc);
	} else {
		fileChangeMonitor->removePath(fileName);
		return;
	}

	fileChangeMonitor->addPath(fileName);

	QMessageBox msgBox;
	msgBox.setText(tr("File \"%1\" <b>was modified on disk.</b><p>Do you want to reload it?</p>%2")
	               .arg(fileName)
	               .arg((modified ? tr("<p><b>Warning:</b> File in editor contains unsaved changes.</p>") : "")));
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	msgBox.setIcon(QMessageBox::Warning);

	int ret = msgBox.exec();

	switch (ret) {
	case QMessageBox::Yes:
		QApplication::setOverrideCursor(Qt::WaitCursor);
		doc->load();
		QApplication::restoreOverrideCursor();
		break;

	case QMessageBox::No:
		break;

	default:
		break;
	}
}

void GCodeWorkShop::startSerialPortServer()
{
	QString path = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/");
	QString fileName;

#ifdef Q_OS_WIN32
	fileName = "gcodefileserver.exe";
#else
	fileName = "gcodefileserver";
#endif

	QProcess::startDetached(path + fileName, QStringList(), path);
}

void GCodeWorkShop::tileSubWindowsVertycally()
{
	if (ui->mdiArea->subWindowList().isEmpty()) {
		return;
	}

	QPoint position(0, 0);

	foreach (QMdiSubWindow* window, ui->mdiArea->subWindowList()) {
		QRect rect(0, 0, ui->mdiArea->width(),
		           ui->mdiArea->height() / ui->mdiArea->subWindowList().count());
		window->setGeometry(rect);
		window->move(position);
		position.setY(position.y() + window->height());
	}
}

void GCodeWorkShop::clipboardChanged()
{
	QStandardItem* item;
	QFont font;
	bool notFound = true;

	if (!isActiveWindow()) {
		return;
	}

	updateMenus();

	QString text = clipboard->text();

	if (text.isEmpty()) {
		return;
	}

	QStandardItem* parentItem = clipboardModel->invisibleRootItem();

	for (int i = 0; i < parentItem->rowCount(); i++) { // check that text is already in clipboard
		item = parentItem->child(i, 0);

		if (text == item->child(0, 0)->text()) {
			notFound = false;
			break;
		}

	}

	if (notFound) {
		ui->clipboardTreeView->setSortingEnabled(false);

		if (parentItem->rowCount() >= 5) {
			item = parentItem->child(5, 0);

			if (item) {
				if (item->text() == "") {
					parentItem->removeRow(5);
				}
			}
		}

		item = new QStandardItem(QIcon(":/images/editpaste.png"), "");
		item->setEditable(true);
		font = item->font();
		font.setBold(true);
		font.setPointSize(font.pointSize() + 1);
		item->setFont(font);
		parentItem->insertRow(0, item);
		parentItem = item;

		item = new QStandardItem(text);
		item->setEditable(false);
		font = item->font();
		font.setFixedPitch(true);
		//font.setFamily("Courier");
		item->setFont(font);
		parentItem->appendRow(item);

		ui->clipboardTreeView->setSortingEnabled(true);
	}

	ui->clipboardTreeView->expandAll();
}

void GCodeWorkShop::clipboardTreeViewContextMenu(const QPoint& point)
{
	Q_UNUSED(point);

	QStandardItem* item;
	QString text;

	QModelIndexList list = ui->clipboardTreeView->selectionModel()->selectedIndexes();
	QModelIndexList::Iterator it = list.begin();

	while (it != list.end()) {
		item = clipboardModel->itemFromIndex(*it);

		if (item == nullptr) {
			return;
		}

		if (item->hasChildren()) {
			item = item->child(0, 0);

			if (item) {
				text = item->text();
			}
		} else {
			text = item->text();
		}

		if (!text.isEmpty()) {
			disconnect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
			clipboard->setText(text);
			connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
		}

		++it;
	}
}

void GCodeWorkShop::customContextMenuRequest(Document* doc, const QPoint& pos)
{
	QMenu* menu = nullptr;

	if (!menu) {
		GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(doc);

		if (gdoc) {
			menu = doContextMenuGCoder(gdoc, pos);
		}
	}

	if (menu) {
		menu->exec(doc->widget()->mapToGlobal(pos));
		menu->deleteLater();
	}
}

QMenu* GCodeWorkShop::doContextMenuGCoder(GCoderDocument* doc, const QPoint& pos)
{
	QMenu* menu = doc->createStandardContextMenu(pos);
	menu->addSeparator();

	menu->addAction(addonsActions()->semiComment());
	menu->addAction(addonsActions()->paraComment());
	menu->addSeparator();
	menu->addAction(addonsActions()->blockSkipIncrement());
	menu->addAction(addonsActions()->blockSkipDecrement());
	menu->addAction(addonsActions()->blockSkipRemove());
	menu->addSeparator();

	QAction* inLineCalcAct = new QAction(QIcon(":/images/inlinecalc.png"), tr("Inline calculator"), menu);
	inLineCalcAct->setShortcut(tr("Ctrl+0"));
	connect(inLineCalcAct, SIGNAL(triggered()), doc, SLOT(showInLineCalc()));
	menu->addAction(inLineCalcAct);

	return menu;
}

void GCodeWorkShop::deleteFromClipboardButtonClicked()
{
	QModelIndexList list = ui->clipboardTreeView->selectionModel()->selectedIndexes();

	for (QModelIndex it : list) {
		QStandardItem* item = clipboardModel->itemFromIndex(it);

		if (item == nullptr) {
			return;
		}

		ui->clipboardTreeView->setSortingEnabled(false);
		clipboardModel->removeRow(item->row(), clipboardModel->invisibleRootItem()->index());
		ui->clipboardTreeView->setSortingEnabled(true);
	}
}

void GCodeWorkShop::clipboardSave()
{
	QSettings settings(Medium::instance().settingsDir() + "/clipboard", QSettings::IniFormat);

	settings.remove("ClipboardItems");
	settings.beginWriteArray("ClipboardItems");

	QStandardItem* parentItem = clipboardModel->invisibleRootItem();

	for (int i = 0; i < parentItem->rowCount(); i++) {
		QStandardItem* item = parentItem->child(i, 0);

		settings.setArrayIndex(i);
		settings.setValue("Title", item->text());
		settings.setValue("ItemText", item->child(0, 0)->text());
	}

	settings.endArray();
}

void GCodeWorkShop::clipboardLoad()
{
	QString itemText;
	QStandardItem* item;
	QFont font;

	ui->clipboardTreeView->setSortingEnabled(false);
	clipboardModel->clear();
	clipboardModel->setColumnCount(1);
	clipboardModel->setHorizontalHeaderLabels(QStringList() << "Clipboard");

	QSettings settings(Medium::instance().settingsDir() + "/clipboard", QSettings::IniFormat);

	int max = settings.beginReadArray("ClipboardItems");

	for (int i = 0; i < max; ++i) {
		settings.setArrayIndex(i);
		itemText = settings.value("Title", "").toString();

		QStandardItem* parentItem = clipboardModel->invisibleRootItem();
		item = new QStandardItem(QIcon(":/images/editpaste.png"), itemText);
		item->setEditable(true);
		font = item->font();
		font.setBold(true);
		font.setPointSize(font.pointSize() + 1);
		item->setFont(font);
		parentItem->insertRow(0, item);
		parentItem = item;

		itemText = settings.value("ItemText", "").toString();
		item = new QStandardItem(itemText);
		item->setEditable(false);
		font = item->font();
		font.setFixedPitch(true);
		//font.setFamily("Courier");
		item->setFont(font);
		parentItem->appendRow(item);

	}

	settings.endArray();

	ui->clipboardTreeView->setSortingEnabled(true);
	clipboardModel->sort(Qt::AscendingOrder);
	ui->clipboardTreeView->expandAll();
}

void GCodeWorkShop::doShowInLineCalc()
{
	GCoderDocument* gdoc = dynamic_cast<GCoderDocument*>(activeDocument());

	if (gdoc) {
		gdoc->showInLineCalc();
	}
}

void GCodeWorkShop::watchFile(const QString& fileName, bool add)
{
	if (fileChangeMonitor) {
		bool exists = fileChangeMonitor->files().contains(fileName);

		if (add) {
			if (!exists) {
				fileChangeMonitor->addPath(fileName);
			}
		} else {
			if (exists) {
				fileChangeMonitor->removePath(fileName);
			}
		}
	}
}
