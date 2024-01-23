/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                    *
 *                                                                         *
 *   This file is part of EdytorNC.                                        *
 *                                                                         *
 *   EdytorNC is free software; you can redistribute it and/or modify      *
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

#include <typeinfo> // for bad_cast

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QEvent>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QLabel>
#include <QLineEdit>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QMoveEvent>
#include <QPageLayout>
#include <QPlainTextEdit>
#include <QPoint>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QProcess>
#include <QResizeEvent>
#include <QRegularExpression>
#include <QtSerialPort>
#include <QSignalMapper>
#include <QSize>
#include <QStandardItem>
#include <QStandardItemModel>
#include <Qt>                  // Qt::SplitBehavior
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QtGlobal>            // QT_VERSION QT_VERSION_CHECK
#include <QToolButton>
#include <QToolTip>
#include <QWidget>

// Addons:
// BHC Chamfer CleanUp Feeds Dot I2M I2MProg Renumber SwapAxes Triangle
#include <addons-actions.h>
#include <commapp.h>                  // CommApp
#include <kdiff3/kdiff3.h>            // KDiff3App
#include <kdiff3/common.h>            // getFilters()
#include <serialportconfigdialog.h>   // SerialPortConfigDialog
#include <serialporttestdialog.h>     // SerialPortTestDialog
#include <serialtransmissiondialog.h> // SerialTransmissionDialog
#include <utils/medium.h>             // Medium

#include "setupdialog.h"    // SetupDialog
#include "edytornc.h"       // EdytorNc QObject QMainWindow
#include "gcoderinfo.h"     // GCoderInfo
#include "findinf.h"        // FindInFiles
#include "newfiledialog.h"  // newFileDialog
#include "mdichild.h"       // MdiChild
#include "recentfiles.h"    // RecentFiles
#include "sessiondialog.h"  // sessionDialog
#include "tooltips.h"       // writeTooltipFile()
#include "ui_edytornc.h"


#define EXAMPLES_PATH             "/usr/share/edytornc/EXAMPLES"

EdytorNc *EdytorNc::SINGLETON;

EdytorNc *EdytorNc::instance()
{
    if (SINGLETON == 0) {
        SINGLETON = new EdytorNc(&Medium::instance());
    }

    return SINGLETON;
}

EdytorNc::EdytorNc(Medium *medium)
    : QMainWindow(nullptr)
{
    mMedium = medium;

    setAttribute(Qt::WA_DeleteOnClose);

    ui = new Ui::EdytorNc();
    ui->setupUi(this);

    findToolBar = nullptr;
    serialToolBar = nullptr;
    diffApp = nullptr;
    findFiles = nullptr;
    dirModel = nullptr;
    openExampleAct = nullptr;
    commApp = nullptr;

    m_MdiWidgetsMaximized = true;

    clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));

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
    connect(ui->clipboardTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(clipboardTreeViewContextMenu(const QPoint &)));

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

    createMenus();
    updateMenus();

    setWindowTitle(tr("EdytorNC"));
    setWindowIcon(QIcon(":/images/edytornc.png"));

    readSettings();
    clipboardLoad();

    if (defaultMdiWindowProperites.windowMode & TABBED_MODE) {
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
        QTabBar *tab = ui->mdiArea->findChild<QTabBar *>();

        if (tab) {
            connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
            tab->setTabsClosable(true);
            // The tabs might be very wide
            tab->setExpanding(false);
        }
    } else {
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
    }
}

EdytorNc::~EdytorNc()
{
    proc = findChild<QProcess *>();

    if (proc) {
        proc->close();
        delete (proc);
    }

    if (commApp) {
        commApp->close();
    }

    delete ui;
}

void EdytorNc::resizeEvent(QResizeEvent *event)
{
    if (windowState() == Qt::WindowNoState && event->oldSize().isValid()) {
        mMWConfig.size = event->size();
    }

    QMainWindow::resizeEvent(event);
}

void EdytorNc::moveEvent(QMoveEvent *event)
{
    if (windowState() == Qt::WindowNoState) {
        mMWConfig.pos = geometry().topLeft();
    }

    QMainWindow::moveEvent(event);
}

Addons::Actions *EdytorNc::addonsActions()
{
    return m_addonsActions;
}

void EdytorNc::closeTab(int i)
{
    QTabBar *tab = ui->mdiArea->findChild<QTabBar *>();

    if (tab != nullptr) {
        tab->removeTab(i);
    }
}

void EdytorNc::closeCurrentWindow()
{
    ui->mdiArea->closeActiveSubWindow();
}

void EdytorNc::closeAllMdiWindows()
{
    ui->mdiArea->closeAllSubWindows();
}

void EdytorNc::closeEvent(QCloseEvent *event)
{
    if (commApp) {
        QMessageBox::StandardButton result = QMessageBox::warning(this,
                                             tr("EdytorNC - Serial port file server"),
                                             tr("Serial port file server is running.\nClose anyway?"),
                                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (result == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    setUpdatesEnabled(false);
    writeSettings();
    clipboardSave();

    if (!maybeSaveProject()) {
        event->ignore();
        return;
    }

    foreach (const QMdiSubWindow *window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder)) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());

        mdiChild->blockSignals(true);

        if (mdiChild->isModified()) {
            setUpdatesEnabled(true);
            mdiChild->activateWindow();
            mdiChild->raise();

            if (!mdiChild->parentWidget()->close()) {
                mdiChild->blockSignals(false);
                event->ignore();
                return;
            }

            setUpdatesEnabled(false);
        }
    }

    ui->mdiArea->closeAllSubWindows();

    if (activeMdiChild()) {
        event->ignore();
    } else {
        event->accept();
    }

    setUpdatesEnabled(true);

    if (findFiles != nullptr) {
        findFiles->close();
        findFiles = nullptr;
    }
}

MdiChild *EdytorNc::newFileFromTemplate()
{
    MdiChild *child = 0;

    newFileDialog *newFileDlg = new newFileDialog(this);
    int result = newFileDlg->exec();

    if (result == QDialog::Accepted) {
        const QString &fileName = newFileDlg->getChosenFile();
        child = createMdiChild();

        if (!fileName.isEmpty() && !(fileName == tr("EMPTY FILE"))) {
            child->newFile(fileName);
        } else {
            child->newFile();
        }

        // TODO replace with DocumentProducer::createDocumentInfo
        DocumentInfo::Ptr info = DocumentInfo::Ptr(new GCoderInfo());
        child->setDocumentInfo(info);
    }

    delete (newFileDlg);

    return child;
}

//**************************************************************************************************
//
//**************************************************************************************************

MdiChild *EdytorNc::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    return child;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::open(const QDir &dir)
{
    const QString &filters = getFilters(defaultMdiWindowProperites.extensions);

    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            tr("Select one or more files to open"),
                            dir.canonicalPath(),
                            filters, 0);

    for (const QString &fileName : files) {
        openFile(fileName);
    }
}

void EdytorNc::open()
{
    open(QDir::currentPath());
    statusBar()->showMessage(tr("File loaded"), 5000);
}

void EdytorNc::openExample()
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

void EdytorNc::openFile(const QString &fileName)
{
    GCoderInfo *info = new GCoderInfo();
    info->filePath = fileName;
    info->readOnly = defaultMdiWindowProperites.defaultReadOnly;
    info->highlightMode = defaultHighlightMode(QFileInfo(fileName).absolutePath());
    loadFile(DocumentInfo::Ptr(info), true);
}

bool EdytorNc::save(MdiChild *child, bool forceSaveAs)
{
    if (child->isUntitled() || forceSaveAs) {
        QString oldFileName;

    #ifdef Q_OS_LINUX
        QString extText = tr("CNC programs files %1 (%1);;");
    #elif defined Q_OS_WIN32
        QString extText = tr("CNC programs files (%1);;");
    #elif defined Q_OS_MACX
        QString extText = tr("CNC programs files %1 (%1);;");
    #endif

        QString filters = extText.arg(defaultMdiWindowProperites.saveExtension);

        for (const QString &ext : defaultMdiWindowProperites.extensions) {
            QString saveExt = extText.arg(ext);

            if (ext != defaultMdiWindowProperites.saveExtension) {
                filters.append(saveExt);
            }
        }

        filters.append(tr("Text files (*.txt);;" "All files (*.* *)"));

        if (child->isUntitled()) {
            oldFileName = child->guessFileName();
        } else {
            oldFileName = child->fileName();
        }

        if (QFileInfo(oldFileName).suffix() == "") {
            // sometimes when file has no extension QFileDialog::getSaveFileName will no apply choosen filter (extension)
            oldFileName.append(".nc");
        }

        QString newFileName = QFileDialog::getSaveFileName(
                           this,
                           tr("Save file as..."),
                           QDir(child->path()).filePath(oldFileName),
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

        child->setFilePath(newFileName);
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool status = child->save();
    QApplication::restoreOverrideCursor();
    return status;
}

bool EdytorNc::save()
{
    MdiChild *child = activeMdiChild();

    if (!child) {
        return true;
    }

    bool saved = save(activeMdiChild(), false);

    if (saved) {
        statusBar()->showMessage(tr("File saved"), 5000);
    } else {
        QMessageBox::warning(this, tr("EdytorNC"), tr("Cannot write file \"%1\".\n %2")
                             .arg(child->filePath()).arg(child->ioErrorString()));
    }

    return saved;
}

bool EdytorNc::saveAll()
{
    bool saved = true;
    int i = 0;

    MdiChild *currentMdiChild = activeMdiChild();

    setUpdatesEnabled(false);

    foreach (const QMdiSubWindow *window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder)) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());

        if (mdiChild->isModified()) {
            if (save(mdiChild, false)) {
                i++;
            } else {
                saved = false;
                QMessageBox::warning(this, tr("EdytorNC"), tr("Cannot write file \"%1\".\n %2")
                                     .arg(mdiChild->filePath()).arg(mdiChild->ioErrorString()));
            }
        }
    }

    if (currentMdiChild != nullptr) {
        currentMdiChild->setFocus();
    }

    setUpdatesEnabled(true);

    statusBar()->showMessage(tr("Saved %1 files").arg(i), 5000);
    return saved;
}

bool EdytorNc::saveAs()
{
    MdiChild *child = activeMdiChild();

    if (!child) {
        return true;
    }

    bool saved = save(activeMdiChild(), true);

    if (saved) {
        statusBar()->showMessage(tr("File saved"), 5000);
    } else {
        QMessageBox::warning(this, tr("EdytorNC"), tr("Cannot write file \"%1\".\n %2")
                             .arg(child->filePath()).arg(child->ioErrorString()));
    }

    return saved;
}

void EdytorNc::printFile()
{
#ifndef QT_NO_PRINTER

    MdiChild *child = activeMdiChild();

    if (child) {
        QPrinter printer(QPrinter::HighResolution);
        loadPrinterSettings(&printer);

        printer.setOutputFormat(QPrinter::NativeFormat);

        QPrintDialog dialog(&printer, this);
        dialog.setWindowTitle(tr("Print Document"));

        if (child->hasSelection()) {
            dialog.setOption(QAbstractPrintDialog::PrintSelection);
            printer.setPrintRange(QPrinter::Selection);
        }

        if (dialog.exec() == QDialog::Accepted) {
            printer.setDocName(child->fileName());
            child->textEdit()->print(&printer);
            statusBar()->showMessage(tr("The document was sent to a printer %1...").arg(
                                         printer.printerName()), 5000);
            savePrinterSettings(&printer);
        }
    }

#endif
}

void EdytorNc::filePrintPreview()
{
#ifndef QT_NO_PRINTER

    MdiChild *child = activeMdiChild();

    if (child) {
        QPrinter printer(QPrinter::HighResolution);
        loadPrinterSettings(&printer);

        printer.setOutputFormat(QPrinter::NativeFormat);

        if (child->hasSelection()) {
            printer.setPrintRange(QPrinter::Selection);
        }

        QPrintPreviewDialog preview(&printer, this);
        preview.setWindowFlags(Qt::Window);
        connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
        preview.exec();
        savePrinterSettings(&printer);
    }

#endif
}

void EdytorNc::printPreview(QPrinter *printer)
{
#ifndef QT_NO_PRINTER

    MdiChild *child = activeMdiChild();

    if (child) {
        printer->setDocName(child->fileName());
        child->textEdit()->print(printer);
        statusBar()->showMessage(tr("The document was sent to a printer %1...").arg(
                                     printer->printerName()), 5000);
    }

#endif
}

void EdytorNc::cut()
{
    if (activeMdiChild()) {
        activeMdiChild()->textEdit()->cut();
    }
}

void EdytorNc::copy()
{
    if (activeMdiChild()) {
        activeMdiChild()->textEdit()->copy();
    }
}

void EdytorNc::findInFl()
{
    if (findFiles == nullptr) {
        findFiles = new FindInFiles(ui->splitter);

        if (defaultMdiWindowProperites.syntaxH) {
            findFiles->setHighlightColors(defaultMdiWindowProperites.hColors);
        }

        if (activeMdiChild()) {
            findFiles->setDir(activeMdiChild()->path());
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

bool EdytorNc::findNext()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    bool found = false;
    QPalette palette;

    findNextAct->setEnabled(false);
    findPreviousAct->setEnabled(false);

    if (!findEdit->text().isEmpty() && hasMdiChild) {
        found = activeMdiChild()->findNext(findEdit->text(),
                                           ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                             QTextDocument::FindFlags()) |
                                            (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                             QTextDocument::FindFlags())),
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

bool EdytorNc::findPrevious()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    bool found = false;
    QPalette palette;

    findNextAct->setEnabled(false);
    findPreviousAct->setEnabled(false);

    if (!findEdit->text().isEmpty() && hasMdiChild) {
        found = activeMdiChild()->findNext(findEdit->text(), QTextDocument::FindBackward |
                                           ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                             QTextDocument::FindFlags()) |
                                            (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                             QTextDocument::FindFlags())),
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

void EdytorNc::replaceNext()
{
    QPalette palette;
    bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->isReadOnly());

    replaceNextAct->setEnabled(false);
    replacePreviousAct->setEnabled(false);
    replaceAllAct->setEnabled(false);

    if (hasMdiChildNotReadOnly) {
        bool found = activeMdiChild()->replaceNext(findEdit->text(), replaceEdit->text(),
                                              ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                                QTextDocument::FindFlags()) |
                                               (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                                QTextDocument::FindFlags())),
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

void EdytorNc::replacePrevious()
{
    QPalette palette;
    bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->isReadOnly());

    replaceNextAct->setEnabled(false);
    replacePreviousAct->setEnabled(false);
    replaceAllAct->setEnabled(false);

    if (hasMdiChildNotReadOnly) {
        bool found = activeMdiChild()->replaceNext(findEdit->text(), replaceEdit->text(),
                                              QTextDocument::FindBackward |
                                              ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                                QTextDocument::FindFlags()) |
                                               (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                                QTextDocument::FindFlags())),
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

void EdytorNc::replaceAll()
{
    QPalette palette;
    bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->isReadOnly());

    replaceNextAct->setEnabled(false);
    replacePreviousAct->setEnabled(false);
    replaceAllAct->setEnabled(false);

    if (hasMdiChildNotReadOnly) {
        QApplication::setOverrideCursor(Qt::BusyCursor);
        bool found = activeMdiChild()->replaceAll(findEdit->text(), replaceEdit->text(),
                                             ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                               QTextDocument::FindFlags()) |
                                              (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                               QTextDocument::FindFlags())),
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

void EdytorNc::selAll()
{
    if (activeMdiChild()) {
        activeMdiChild()->textEdit()->selectAll();
    }
}

void EdytorNc::config()
{
    SetupDialog *setUpDialog = new SetupDialog(this, &defaultMdiWindowProperites);

    if (setUpDialog->exec() == QDialog::Accepted) {
        defaultMdiWindowProperites = setUpDialog->getSettings();

        if (defaultMdiWindowProperites.windowMode & TABBED_MODE) {
            ui->mdiArea->setViewMode(QMdiArea::TabbedView);
            QTabBar *tab = ui->mdiArea->findChild<QTabBar *>();

            if (tab) {
                connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
                tab->setTabsClosable(true);
                // The tabs might be very wide
                tab->setExpanding(false);
            }
        } else {
            ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
        }

        foreach (const QMdiSubWindow *window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder)) {
            MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());

            if (dirModel != nullptr) {
                dirModel->setNameFilters(defaultMdiWindowProperites.extensions);
            }

            mdiChild->setReadOnly(defaultMdiWindowProperites.defaultReadOnly);
            mdiChild->setMdiWindowProperites(defaultMdiWindowProperites);
        }
    }

    delete setUpDialog;
}

void EdytorNc::readOnly()
{
    if (activeMdiChild()) {
        activeMdiChild()->textEdit()->setReadOnly(readOnlyAct->isChecked());
    }

    updateMenus();
}

void EdytorNc::goToLine(const QString &fileName, int line)
{
    if (activeMdiChild()) {
        QString childFileName = activeMdiChild()->filePath();
        childFileName =  QDir().toNativeSeparators(childFileName);

        if (QDir().toNativeSeparators(fileName) != childFileName) {
            return;
        }

        QTextBlock block = activeMdiChild()->textEdit()->document()->findBlockByNumber(line);
        QTextCursor cursor = QTextCursor(block);
        activeMdiChild()->textEdit()->setTextCursor(cursor);
        activeMdiChild()->textEdit()->centerCursor();
        activeMdiChild()->setFocus();
    }
}

void EdytorNc::createDiffApp()
{
    if (diffApp == nullptr) {
        diffApp = new KDiff3App(ui->splitter, "DiffApp", defaultMdiWindowProperites.extensions);

        connect(diffApp, SIGNAL(lineClicked(QString, int)), this, SLOT(goToLine(QString, int)));
    }
}

void EdytorNc::doDiffL()
{
    QString fileName;

    createDiffApp();

    if (diffApp != nullptr) {
        diffLAct->setEnabled(false);
        diffRAct->setEnabled(false);
        diffAct->setEnabled(false);

        diffAct->setChecked(true);

        if (activeMdiChild()) {
            fileName = activeMdiChild()->filePath();
        }

        if (fileName.isEmpty()) {
            fileName = QDir::currentPath();
        }

        //        diffApp->close();
        diffApp->completeInit(fileName, "");

        diffLAct->setEnabled(true);
        diffRAct->setEnabled(true);
        diffAct->setEnabled(true);
    }
}

void EdytorNc::doDiffR()
{
    QString fileName;

    createDiffApp();

    if (diffApp != nullptr) {
        diffLAct->setEnabled(false);
        diffRAct->setEnabled(false);
        diffAct->setEnabled(false);

        diffAct->setChecked(true);

        if (activeMdiChild()) {
            fileName = activeMdiChild()->filePath();
        }

        if (fileName.isEmpty()) {
            fileName = QDir::currentPath();
        }

        //        diffApp->close();
        diffApp->completeInit("", fileName);

        diffLAct->setEnabled(true);
        diffRAct->setEnabled(true);
        diffAct->setEnabled(true);
    }
}

void EdytorNc::diffTwoFiles(const QString &filename1, const QString &filename2)
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

void EdytorNc::diffEditorFile()
{
    MdiChild *child = activeMdiChild();

    if (!child) {
        return;
    }

    createDiffApp();

    if (diffApp != nullptr) {
        QString fileName = child->filePath();

        if (fileName.isEmpty()) {
            return;
        }

        QString fileName1 = QDir::tempPath() + QDir::separator() + QFileInfo(
                                fileName).baseName() + ".tmp";
        //qDebug() << fileName << fileName1;

        QFile file(fileName1);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, tr("EdytorNC"),
                                 tr("Cannot write tmp file \"%1\".\n %2")
                                 .arg(QDir::toNativeSeparators(fileName1))
                                 .arg(file.errorString()));
            return;
        }

        file.write(child->rawData());
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

void EdytorNc::doDiff()
{
    QString fileName;

    if (diffApp == nullptr) {
        createDiffApp();

        if (activeMdiChild()) {
            fileName = activeMdiChild()->filePath();
        }

        if (fileName.isEmpty()) {
            fileName = QDir::currentPath();
        }

        //        diffApp->close();
        diffApp->completeInit(QFileInfo(fileName).canonicalPath(), QFileInfo(fileName).canonicalPath());

    } else if (!diffAct->isChecked()) {
        diffAct->setChecked(false);
        diffApp->close();
        diffApp = nullptr;
    }
}

void EdytorNc::doCalc()
{
    if (!QFile::exists(defaultMdiWindowProperites.calcBinary)) {
        QMessageBox::information(this, tr("Information"),
                                 tr("Set correct calculator program name in configuration dialog."));
        return;
    }

    proc = findChild<QProcess *>("Calc569");

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
        proc->start(defaultMdiWindowProperites.calcBinary, QStringList());
    }
}

void EdytorNc::deleteText()
{
    if (activeMdiChild()) {
        activeMdiChild()->textEdit()->textCursor().removeSelectedText();
    }
}

void EdytorNc::paste()
{
    if (activeMdiChild()) {
        if (defaultMdiWindowProperites.underlineChanges) {
            QTextCharFormat format = activeMdiChild()->textEdit()->currentCharFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(defaultMdiWindowProperites.underlineColor));
            activeMdiChild()->textEdit()->setCurrentCharFormat(format);
        }

        activeMdiChild()->textEdit()->paste();
    }
}

void EdytorNc::undo()
{
    if (activeMdiChild()) {
        activeMdiChild()->doUndo();
    }
}

void EdytorNc::redo()
{
    if (activeMdiChild()) {
        activeMdiChild()->doRedo();
    }
}

void EdytorNc::activeWindowChanged(QMdiSubWindow *window)
{
    Q_UNUSED(window);
    MdiChild *mdiChild;

    if (ui->mdiArea->subWindowList().count() <= 1) {
        m_MdiWidgetsMaximized = true;
    }

    mdiChild = activeMdiChild();

    if (mdiChild) {
        m_MdiWidgetsMaximized = mdiChild->parentWidget()->isMaximized();
        statusBar()->showMessage(mdiChild->filePath(), 9000);
    }

    updateCurrentSerialConfig();
    updateOpenFileList();
    fileTreeViewChangeRootDir();
}

void EdytorNc::about()
{
    QMessageBox::about(this, tr("About EdytorNC"),
                       tr("The <b>EdytorNC</b> is text editor for CNC programmers."
                          "<P>Version: " "2018-07-26 BETA"
                          "<P>Copyright (C) 1998 - 2018 by <a href=\"mailto:artkoz78@gmail.com\">Artur Kozioł</a>"
                          "<P>Spanish translation, fixes and other help thanks to Otto Huamani"
                          "<br />Catalan translation and deb package thanks to Jordi Sayol i Salomó"
                          "<br />German translation and other fixes thanks to Michael Numberger"
                          "<br />Czech translation thanks to Pavel Fric"
                          "<br />Finnish translation thanks to Janne Mäntyharju"
                          "<br />Dutch translation thanks to Henk van Andel"
                          "<br />OS X patch and other updates thanks to Janne Mäntyharju"
                          "<P>New EdytorNC icon thanks to Jakub Gajewski"
                          "<P><a href=\"http://sourceforge.net/projects/edytornc/\">http://sourceforge.net/projects/edytornc</a>"
                          "<P><a href=\"https://plus.google.com/communities/117267107585407507443/\">EdytorNC Google Plus Community</a>"
                          "<P>"
                          // "<P>Cross platform installer made by <a href=\"http://installbuilder.bitrock.com/\">BitRock InstallBuilder for Qt</a>"
                          // "<P>"
                          "<P>EdytorNC contains pieces of code from other Open Source projects."
                          "<P>"
                          "<P><i>EdytorNC is free software; you can redistribute it and/or modify"
                          "it under the terms of the GNU General Public License  as published by"
                          "the Free Software Foundation; either version 2 of the License, or"
                          "(at your option) any later version.</i>"
                          "<P><i>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                          "INCLUDING THE WARRANTY OF DESIGN,"
                          "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</i>"));
}

void EdytorNc::updateMenus()
{
    bool hasMdiChildNotReadOnly;
    bool hasSelection;
    bool hasModifiedMdiChild;
    bool hasMdiChild = (activeMdiChild() != nullptr);

    if (hasMdiChild) {
        hasMdiChildNotReadOnly = (hasMdiChild && !activeMdiChild()->isReadOnly());
        hasSelection = (hasMdiChild && activeMdiChild()->hasSelection());
        hasModifiedMdiChild = hasMdiChild && activeMdiChild()->isModified();

        redoAct->setEnabled(hasMdiChild && activeMdiChild()->isRedoAvailable());
        undoAct->setEnabled(hasMdiChild && activeMdiChild()->isUndoAvailable());
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

    if (hasMdiChild) {
        if (findToolBar)
            activeMdiChild()->highlightFindText(findEdit->text(),
                                                ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                                  QTextDocument::FindFlags()) |
                                                 (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                                  QTextDocument::FindFlags())), mCheckIgnoreComments->isChecked());
        else {
            activeMdiChild()->highlightFindText("");
        }

        saveAct->setText(tr("&Save \"%1\"").arg(activeMdiChild()->fileName()));
        saveAsAct->setText(tr("Save \"%1\" &As...").arg(activeMdiChild()->fileName()));
        closeAct->setText(tr("Cl&ose \"%1\"").arg(activeMdiChild()->fileName()));
    }

    updateStatusBar();
}

void EdytorNc::updateCurrentSerialConfig()
{
    bool hasMdiChild = (activeMdiChild() != nullptr);

    if (hasMdiChild && (serialToolBar != nullptr)) {
        QDir dir;
        dir.setPath(activeMdiChild()->path());
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

void EdytorNc::updateStatusBar()
{
    QTextBlock b;
    bool hasMdiChild = (activeMdiChild() != nullptr);
    bool hasMdiChildNotReadOnly = (hasMdiChild && !activeMdiChild()->isReadOnly());

    if (hasMdiChild) {
        int id = highlightTypeCombo->findData(activeMdiChild()->highligthMode());
        highlightTypeCombo->blockSignals(true);
        highlightTypeCombo->setCurrentIndex(id);
        highlightTypeCombo->blockSignals(false);

        b = activeMdiChild()->textCursor().block();
        int line = b.firstLineNumber() + 1;
        int column = activeMdiChild()->textCursor().position() - b.position();

        labelStat1->setText(tr(" Col: ") + QString::number(column + 1) +
                            tr("  Line: ") + QString::number(line) +
                            (activeMdiChild()->isModified() ? tr("  <b>Modified</b>  ") : " ") +
                            (!hasMdiChildNotReadOnly ? tr(" Read only  ") : " ") +
                            (activeMdiChild()->overwriteMode() ? tr(" Overwrite  ") : tr(" Insert ")));

    }
}

void EdytorNc::updateWindowMenu()
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

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                   .arg(child->filePath());
        } else {
            text = tr("%1 %2").arg(i + 1)
                   .arg(child->filePath());
        }

        QAction *action = windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
        action->setToolTip(child->brief());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MdiChild *EdytorNc::createMdiChild()
{
    MdiChild *child = new MdiChild(this);
    ui->mdiArea->addSubWindow(child);

    connect(child->textEdit(), SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));
    connect(child->textEdit(), SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));
    connect(child->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(updateMenus()));
    connect(child->textEdit(), SIGNAL(modificationChanged(bool)), this, SLOT(updateMenus()));
    connect(child->textEdit(), SIGNAL(modificationChanged(bool)), this, SLOT(updateOpenFileList()));
    connect(child, SIGNAL(message(const QString &, int)), statusBar(),
            SLOT(showMessage(const QString &, int)));
    connect(child, SIGNAL(addRemoveFileWatch(const QString &, bool)), this,
            SLOT(watchFile(const QString &, bool)));

    if (defaultMdiWindowProperites.saveDirectory.isEmpty()) {
        child->setPath(QDir::currentPath());
    } else {
        child->setPath(defaultMdiWindowProperites.saveDirectory);
    }

    child->setMdiWindowProperites(defaultMdiWindowProperites);
    child->setHighligthMode(defaultMdiWindowProperites.defaultHighlightMode);

    if (m_MdiWidgetsMaximized) {
        child->showMaximized();
    } else {
        child->showNormal();
    }

    return child;
}

void EdytorNc::createActions()
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
    connect(sessionMgrAct, SIGNAL(triggered()), this, SLOT(sessionMgr()));


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

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this,
            SLOT(activeWindowChanged(QMdiSubWindow *)));

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

void EdytorNc::createMenus()
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
    connect(recentFileMenu, SIGNAL(triggered(QAction *)), this, SLOT(fileOpenRecent(QAction *)));
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveAllAct);
    fileMenu->addSeparator();
    fileMenu->addAction(findFilesAct);
    fileMenu->addSeparator();

    sessionsMenu = fileMenu->addMenu(tr("Sessions"));
    connect(sessionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(changeSession(QAction *)));
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

void EdytorNc::createToolBars()
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

void EdytorNc::createStatusBar()
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

void EdytorNc::setHighLightMode(int mode)
{
    bool ok;
    bool hasMdiChild = (activeMdiChild() != 0);

    int id = highlightTypeCombo->itemData(mode).toInt(&ok);

    if (hasMdiChild) {
        activeMdiChild()->setHighligthMode(id);
        activeMdiChild()->setFocus(Qt::MouseFocusReason);
    }
}

void EdytorNc::readSettings()
{
    QSettings &settings = *Medium::instance().settings();

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

    defaultMdiWindowProperites.disableFileChangeMonitor = settings.value("DisableFileChangeMonitor",
            false).toBool();

    if (defaultMdiWindowProperites.disableFileChangeMonitor) {
        fileChangeMonitor.clear();
    } else {
        fileChangeMonitor = new QFileSystemWatcher(this);
        connect(fileChangeMonitor, SIGNAL(fileChanged(const QString)), this,
                SLOT(fileChanged(const QString)));
    }

    QDir::setCurrent(settings.value("LastDir",  QDir::homePath()).toString());

    defaultMdiWindowProperites.extensions = settings.value("Extensions",
                                            (QStringList() << "*.nc" <<  "*.cnc")).toStringList();
    defaultMdiWindowProperites.saveExtension = settings.value("DefaultSaveExtension",
            "*.nc").toString();
    defaultMdiWindowProperites.saveDirectory = settings.value("DefaultSaveDirectory",
            QDir::homePath()).toString();

    defaultMdiWindowProperites.fontName = settings.value("FontName", "Courier").toString();
    defaultMdiWindowProperites.fontSize = settings.value("FontSize", 12).toInt();
    defaultMdiWindowProperites.intCapsLock = settings.value("IntCapsLock", true).toBool();
    defaultMdiWindowProperites.underlineChanges = settings.value("UnderlineChanges", true).toBool();
    defaultMdiWindowProperites.windowMode = settings.value("WindowMode", 0x0E).toInt();
    defaultMdiWindowProperites.clearUndoHistory = settings.value("ClearUndoRedo", false).toBool();
    defaultMdiWindowProperites.clearUnderlineHistory = settings.value("ClearUnderline",
            false).toBool();
    defaultMdiWindowProperites.editorToolTips = settings.value("EditorToolTips", true).toBool();
    defaultMdiWindowProperites.startEmpty = settings.value("StartEmpty", false).toBool();

    defaultMdiWindowProperites.lineColor = settings.value("LineColor", 0xFEFFB6).toInt();
    defaultMdiWindowProperites.underlineColor = settings.value("UnderlineColor", 0x00FF00).toInt();

    defaultMdiWindowProperites.defaultReadOnly = settings.value("ViewerMode", false).toBool();
    defaultMdiWindowProperites.defaultHighlightMode = settings.value("DefaultHighlightMode",
            MODE_AUTO).toInt();

    defaultMdiWindowProperites.guessFileNameByProgNum = settings.value("GuessFileNameByProgNum",
            true).toBool();
    defaultMdiWindowProperites.changeDateInComment = settings.value("ChangeDateInComment",
            false).toBool();

    fileDialogState = settings.value("FileDialogState", QByteArray()).toByteArray();

#ifdef Q_OS_LINUX
    defaultMdiWindowProperites.calcBinary = "kcalc";
#endif

#ifdef Q_OS_WIN32
    defaultMdiWindowProperites.calcBinary = "calc.exe";
#endif

    defaultMdiWindowProperites.calcBinary = settings.value("CalcBinary",
                                            defaultMdiWindowProperites.calcBinary).toString();

    m_recentFiles->load(&settings);

    //m_MdiWidgetsMaximized = settings.value("MaximizedMdi", true).toBool();

    settings.beginGroup("Highlight");
    defaultMdiWindowProperites.syntaxH = settings.value("HighlightOn", true).toBool();

    defaultMdiWindowProperites.hColors.commentColor = settings.value("CommentColor",
            0xde0020).toInt();
    defaultMdiWindowProperites.hColors.gColor = settings.value("GColor", 0x1600ee).toInt();
    defaultMdiWindowProperites.hColors.mColor = settings.value("MColor", 0x80007d).toInt();
    defaultMdiWindowProperites.hColors.nColor = settings.value("NColor", 0x808080).toInt();
    defaultMdiWindowProperites.hColors.lColor = settings.value("LColor", 0x535b5f).toInt();
    defaultMdiWindowProperites.hColors.fsColor = settings.value("FsColor", 0x516600).toInt();
    defaultMdiWindowProperites.hColors.dhtColor = settings.value("DhtColor", 0x660033).toInt();
    defaultMdiWindowProperites.hColors.rColor = settings.value("RColor", 0x24576f).toInt();
    defaultMdiWindowProperites.hColors.macroColor = settings.value("MacroColor", 0x000080).toInt();
    defaultMdiWindowProperites.hColors.keyWordColor = settings.value("KeyWordColor",
            0x1d8000).toInt();
    defaultMdiWindowProperites.hColors.progNameColor = settings.value("ProgNameColor",
            0x000000).toInt();
    defaultMdiWindowProperites.hColors.operatorColor = settings.value("OperatorColor",
            0x9a2200).toInt();
    defaultMdiWindowProperites.hColors.zColor = settings.value("ZColor", 0x000080).toInt();
    defaultMdiWindowProperites.hColors.aColor = settings.value("AColor", 0x000000).toInt();
    defaultMdiWindowProperites.hColors.bColor = settings.value("BColor", 0x000000).toInt();
    defaultMdiWindowProperites.hColors.defaultColor = settings.value("DefaultColor",
            0x000000).toInt();
    defaultMdiWindowProperites.hColors.backgroundColor = settings.value("BackgroundColor",
            0xFFFFFF).toInt();
    settings.endGroup();

    settings.beginGroup("Sessions");
    sessionList = settings.value("SessionList", (QStringList(tr("default")))).toStringList();

    if (settings.value("RestoreLastSession", false).toBool()) {
        currentSession = settings.value("CurrentSession", tr("default")).toString();
    } else {
        currentSession = tr("default");
    }

    settings.endGroup();

    if (!defaultMdiWindowProperites.startEmpty) {
        loadSession(currentSession);
    }

    updateSessionMenus();

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

void EdytorNc::writeSettings()
{
    QSettings &settings = *Medium::instance().settings();

    settings.beginGroup("mainwindow");
    settings.setValue("pos", mMWConfig.pos);
    settings.setValue("size", mMWConfig.size);
    settings.setValue("maximized", isMaximized());
    settings.setValue("state", saveState());
    settings.endGroup();

    settings.setValue("LastDir", QDir::currentPath());

    settings.setValue("Extensions", defaultMdiWindowProperites.extensions);
    settings.setValue("DefaultSaveExtension", defaultMdiWindowProperites.saveExtension);
    settings.setValue("DefaultSaveDirectory", defaultMdiWindowProperites.saveDirectory);

    settings.setValue("FontName", defaultMdiWindowProperites.fontName);
    settings.setValue("FontSize", defaultMdiWindowProperites.fontSize);
    settings.setValue("IntCapsLock", defaultMdiWindowProperites.intCapsLock);
    settings.setValue("UnderlineChanges", defaultMdiWindowProperites.underlineChanges);
    settings.setValue("WindowMode", defaultMdiWindowProperites.windowMode);
    settings.setValue("LineColor", defaultMdiWindowProperites.lineColor);
    settings.setValue("UnderlineColor", defaultMdiWindowProperites.underlineColor);
    settings.setValue("CalcBinary", defaultMdiWindowProperites.calcBinary);
    settings.setValue("ClearUndoRedo", defaultMdiWindowProperites.clearUndoHistory);
    settings.setValue("ClearUnderline", defaultMdiWindowProperites.clearUnderlineHistory);
    settings.setValue("EditorToolTips", defaultMdiWindowProperites.editorToolTips);
    settings.setValue("ViewerMode", defaultMdiWindowProperites.defaultReadOnly);
    settings.setValue("DefaultHighlightMode", defaultMdiWindowProperites.defaultHighlightMode);
    settings.setValue("StartEmpty", defaultMdiWindowProperites.startEmpty);
    settings.setValue("DisableFileChangeMonitor",
                      defaultMdiWindowProperites.disableFileChangeMonitor);

    settings.setValue("GuessFileNameByProgNum", defaultMdiWindowProperites.guessFileNameByProgNum);
    settings.setValue("ChangeDateInComment", defaultMdiWindowProperites.changeDateInComment);

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

    settings.beginGroup("Highlight");
    settings.setValue("HighlightOn", defaultMdiWindowProperites.syntaxH);

    settings.setValue("CommentColor", defaultMdiWindowProperites.hColors.commentColor);
    settings.setValue("GColor", defaultMdiWindowProperites.hColors.gColor);
    settings.setValue("MColor", defaultMdiWindowProperites.hColors.mColor);
    settings.setValue("NColor", defaultMdiWindowProperites.hColors.nColor);
    settings.setValue("LColor", defaultMdiWindowProperites.hColors.lColor);
    settings.setValue("FsColor", defaultMdiWindowProperites.hColors.fsColor);
    settings.setValue("DhtColor", defaultMdiWindowProperites.hColors.dhtColor);
    settings.setValue("RColor", defaultMdiWindowProperites.hColors.rColor);
    settings.setValue("MacroColor", defaultMdiWindowProperites.hColors.macroColor);
    settings.setValue("KeyWordColor", defaultMdiWindowProperites.hColors.keyWordColor);
    settings.setValue("ProgNameColor", defaultMdiWindowProperites.hColors.progNameColor);
    settings.setValue("OperatorColor", defaultMdiWindowProperites.hColors.operatorColor);
    settings.setValue("BColor", defaultMdiWindowProperites.hColors.bColor);
    settings.setValue("AColor", defaultMdiWindowProperites.hColors.aColor);
    settings.setValue("ZColor", defaultMdiWindowProperites.hColors.zColor);
    settings.setValue("DefaultColor", defaultMdiWindowProperites.hColors.defaultColor);
    settings.setValue("BackgroundColor", defaultMdiWindowProperites.hColors.backgroundColor);

    settings.endGroup();

    //cleanup old settings
    settings.remove("LastDoc");

    settings.beginGroup("Sessions");
    settings.setValue("SessionList", sessionList);
    settings.setValue("CurrentSession", currentSession);
    settings.endGroup();

    if (!defaultMdiWindowProperites.startEmpty) {
        saveSession(currentSession);
    }
}

MdiChild *EdytorNc::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow()) {
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    }

    return 0;
}

QMdiSubWindow *EdytorNc::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    if (canonicalFilePath.isEmpty()) {
        canonicalFilePath = fileName;
    }

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());

        if (mdiChild->filePath() == QDir::toNativeSeparators(canonicalFilePath)) {
            return window;
        }
    }

    return 0;
}

void EdytorNc::setActiveSubWindow(QWidget *window)
{
    if (!window) {
        return;
    }

    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void EdytorNc::loadFile(const DocumentInfo::Ptr &info, bool checkAlreadyLoaded)
{
    QFileInfo file;

    if (checkAlreadyLoaded) {
        QMdiSubWindow *existing = findMdiChild(info->filePath);

        if (existing) {
            ui->mdiArea->setActiveSubWindow(existing);
            return;
        }
    }

    file.setFile(info->filePath);

    if ((file.exists()) && (file.isReadable())) {
        MdiChild *child = createMdiChild();

        child->setFilePath(info->filePath);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool status = child->load();
        QApplication::restoreOverrideCursor();

        if (status) {
            child->setDocumentInfo(info);
            updateStatusBar();
            m_recentFiles->add(info->filePath);
        } else {
            QMessageBox::warning(this, tr("EdytorNC"), tr("Cannot read file \"%1\".\n %2")
                                 .arg(child->filePath()).arg(child->ioErrorString()));
            child->parentWidget()->close();
        }
    }
}

void EdytorNc::recentFilesChanged()
{
    m_recentFiles->save(Medium::instance().settings());
}

void EdytorNc::fileOpenRecent(QAction *act)
{
    openFile(act->data().toString());
}

void EdytorNc::updateRecentFilesMenu(const QStringList &fileList)
{
    recentFileMenu->clear();

    for (const QString &file : fileList) {
        QAction *newAc = recentFileMenu->addAction(QIcon(":/images/document-open-recent.png"), file);
        newAc->setData(file);
    }
}

void EdytorNc::loadFoundedFile(const QString &fileName)
{
    openFile(fileName);
}

void EdytorNc::messReceived(const QString &text)
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

void EdytorNc::createFindToolBar()
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
        findEdit->installEventFilter(this);
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
        replaceEdit->installEventFilter(this);
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

        QSettings &settings = *Medium::instance().settings();
        mCheckIgnoreComments->setChecked(settings.value("FindIgnoreComments", true).toBool());
        mCheckFindWholeWords->setChecked(settings.value("FindWholeWords", false).toBool());
        mCheckIgnoreCase->setChecked(settings.value("FindIgnoreCase", true).toBool());
    } else {
        findToolBar->show();
    }

    if (activeMdiChild()) {
        disconnect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));

        if (!activeMdiChild()->hasSelection()) {
            cursor = activeMdiChild()->textCursor();
            cursor.select(QTextCursor::WordUnderCursor);
            selText = cursor.selectedText();

            if ((selText.size() > 32) || (selText.size() < 2)) {
                cursor.clearSelection();
            }

            activeMdiChild()->textEdit()->setTextCursor(cursor);
        }

        cursor = activeMdiChild()->textCursor();

        if (cursor.hasSelection()) {
            selText = cursor.selectedText();

            if ((selText.size() < 32)) {
                findEdit->setText(selText);
            } else {
                cursor.clearSelection();
                activeMdiChild()->textEdit()->setTextCursor(cursor);
            }
        }

        findEdit->setPalette(QPalette());
        connect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));
        findEdit->setFocus(Qt::MouseFocusReason);

        activeMdiChild()->highlightFindText(findEdit->text(),
                                            ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords :
                                              QTextDocument::FindFlags()) |
                                             (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively :
                                              QTextDocument::FindFlags())), mCheckIgnoreComments->isChecked());
    }

    findEdit->selectAll();
}

void EdytorNc::closeFindToolBar()
{
    if (activeMdiChild()) {
        activeMdiChild()->setFocus(Qt::MouseFocusReason);
        activeMdiChild()->highlightFindText("");
        activeMdiChild()->textEdit()->centerCursor();
    }

    QSettings &settings = *Medium::instance().settings();
    settings.setValue("FindIgnoreComments", mCheckIgnoreComments->isChecked());
    settings.setValue("FindWholeWords", mCheckFindWholeWords->isChecked());
    settings.setValue("FindIgnoreCase", mCheckIgnoreCase->isChecked());

    findToolBar->close();
    //findToolBar = nullptr;
}

void EdytorNc::findTextChanged()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    QTextCursor cursor;

    if (findEdit->text().contains(QRegularExpression("\\$\\$"))
            || findEdit->text().contains(
                QRegularExpression("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"))) {
        replaceAllAct->setEnabled(false);
    } else {
        replaceAllAct->setEnabled(true);
    }


    if (hasMdiChild) {
        cursor = activeMdiChild()->textCursor();

        if (!findEdit->text().isEmpty()) {
            int pos = cursor.position() - findEdit->text().size();

            if (pos < 0) {
                pos = 0;
            }

            do {
                cursor.movePosition(QTextCursor::Left);  //cursor.movePosition(QTextCursor::StartOfWord)
            } while ((pos <= cursor.position()) && (cursor.position() > 0));

            activeMdiChild()->textEdit()->setTextCursor(cursor);

            findNext();
        } else {
            findEdit->setPalette(QPalette());
            cursor.clearSelection();
            activeMdiChild()->textEdit()->setTextCursor(cursor);
        }
    }
}

bool EdytorNc::eventFilter(QObject *obj, QEvent *ev)
{
    if ((obj == findEdit) || (obj == replaceEdit)) {
        if (ev->type() == QEvent::KeyPress) {
            QKeyEvent *k = (QKeyEvent *) ev;

            if (k->key() == Qt::Key_Comma) { //Keypad comma should always prints period
                if ((k->modifiers() == Qt::KeypadModifier)
                        || (k->nativeScanCode() == 0x53)) { // !!! Qt::KeypadModifier - Not working for keypad comma !!!
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier,
                                            ".", false, 1));
                    return true;
                }
            }

            if (defaultMdiWindowProperites.intCapsLock) {
                if (k->text()[0].isLower() && (k->modifiers() == Qt::NoModifier)) {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::NoModifier,
                                            k->text().toUpper(), false, 1));
                    return true;
                }

                if (k->text()[0].isUpper() && (k->modifiers() == Qt::ShiftModifier)) {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::ShiftModifier,
                                            k->text().toLower(), false, 1));
                    return true;
                }
            }
        }

        return false;
    } else {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, ev);
    }
}

void EdytorNc::createSerialToolBar()
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

void EdytorNc::closeSerialToolbar()
{
    serialToolBar->close();
    delete (serialToolBar);
    serialToolBar = nullptr;
    showSerialToolBarAct->setChecked(false);
}

void EdytorNc::attachToDirButtonClicked(bool attach)
{
    QFile file;

    bool hasMdiChild = (activeMdiChild() != 0);

    if (hasMdiChild && (serialToolBar != nullptr)) {
        QDir dir;
        dir.setPath(activeMdiChild()->path());
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QStringList("*.ini"));

        QFileInfoList list = dir.entryInfoList();

        for (const QFileInfo &fileInfo : list) {
            file.setFileName(fileInfo.absoluteFilePath());
            file.remove();
        }

        if (attach) {
            file.setFileName(activeMdiChild()->path() + "/" + configBox->currentText() + ".ini");
            file.open(QIODevice::ReadWrite);
            file.close();;
        }
    }
}

void EdytorNc::deAttachToDirButtonClicked()
{
    attachToDirButtonClicked(false);
}

void EdytorNc::createUserToolTipsFile()
{
    QString fileName;

    if (activeMdiChild()) {
        fileName = activeMdiChild()->path();
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

    QMdiSubWindow *existing = findMdiChild(fileName);

    if (existing) {
        ui->mdiArea->setActiveSubWindow(existing);
    }
}

void EdytorNc::createGlobalToolTipsFile()
{
    QString fileName = writeTooltipFile();


    if (QFileInfo(fileName).exists()) {
        openFile(fileName);
    }

    QMdiSubWindow *existing = findMdiChild(fileName);

    if (existing) {
        ui->mdiArea->setActiveSubWindow(existing);
    }
}

void EdytorNc::attachHighlighterToDirButtonClicked(bool attach)
{
    QFile file;

    bool hasMdiChild = (activeMdiChild() != 0);

    if (hasMdiChild) {
        QDir dir;
        dir.setPath(activeMdiChild()->path());
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(QStringList("*.cfg"));

        QFileInfoList list = dir.entryInfoList();

        for (const QFileInfo &fileInfo : list) {
            file.setFileName(fileInfo.absoluteFilePath());
            file.remove();
        }

        if (attach) {
            file.setFileName(activeMdiChild()->path() + "/" + highlightTypeCombo->currentText() +
                             ".cfg");
            file.open(QIODevice::ReadWrite);
            file.close();;
        }
    }
}

void EdytorNc::attachHighlightToDirActClicked()
{
    attachHighlighterToDirButtonClicked(true);
}

void EdytorNc::deAttachHighlightToDirActClicked()
{
    attachHighlighterToDirButtonClicked(false);
}

int EdytorNc::defaultHighlightMode(const QString &filePath)
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

void EdytorNc::projectAdd()
{
    QFileInfo file;
    QStandardItem *item;
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
                            QDir::currentPath(),
                            filters, 0);

    QStringList list = files;

    if (list.isEmpty()) {
        return;
    }

    QStringList::Iterator it = list.begin();

    QStandardItem *parentItem = currentProject;

    if (it != list.end()) {
        file.setFile(*it);

        if ((file.absoluteDir().exists()) && (file.absoluteDir().isReadable())) {

            QList<QStandardItem *> items = model->findItems(QDir::toNativeSeparators(
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

    QFileSystemModel *fModel = new QFileSystemModel;

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

void EdytorNc::projectSave()
{
    QString path, fileName;
    int fileCount;
    QStandardItem *item;


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

void EdytorNc::projectSaveAs()
{
    QString fileName = projectSelectName();

    if (fileName.isEmpty()) {
        return;
    }

    currentProjectName = fileName;
    QStandardItem *parentItem = model->invisibleRootItem();
    parentItem->child(0, 0)->setText(QFileInfo(currentProjectName).fileName());
    parentItem->child(0, 0)->setToolTip(QDir::toNativeSeparators(QFileInfo(
                                            currentProjectName).absoluteFilePath()));
    projectSave();
}

void EdytorNc::projectNew()
{
    if (!maybeSaveProject()) {
        return;
    }

    QString fileName = projectSelectName();

    if (fileName.isEmpty()) {
        return;
    }

    currentProjectName = fileName;

    QStandardItem *parentItem = model->invisibleRootItem();
    QStandardItem *item = new QStandardItem(QIcon(":/images/edytornc.png"),
                                            QFileInfo(currentProjectName).fileName());

    parentItem->appendRow(item);

    currentProject = item;
    currentProjectModified = true;
}

void EdytorNc::projectTreeViewDoubleClicked(const QModelIndex &index)
{
    QFileInfo file;

    if ((!index.isValid())) {
        return;
    }

    QStandardItem *item = model->itemFromIndex(index);

    if (item == nullptr || item->parent() == nullptr) {
        return;
    }

    if (item->hasChildren()) {
        return;
    }

    file.setFile(item->parent()->text(), item->text());

    if ((file.exists()) && (file.isReadable())) {
        if (defaultMdiWindowProperites.extensions.contains("*." + file.suffix())) {
            openFile(file.canonicalFilePath());
        } else {
            QDesktopServices::openUrl(QUrl("file:///" + file.absoluteFilePath(), QUrl::TolerantMode));
        }
    }
}

void EdytorNc::fileTreeViewDoubleClicked(const QModelIndex &index)
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
        } else if (defaultMdiWindowProperites.extensions.contains("*." + file.suffix())) {
            openFile(file.canonicalFilePath());
        } else {
            QDesktopServices::openUrl(QUrl("file:///" + file.absoluteFilePath(), QUrl::TolerantMode));
        }
    }
}

QString EdytorNc::projectSelectName()
{
    QString filters = tr("EdytorNC project file (*.ncp)");
    QString file = QFileDialog::getSaveFileName(
                       this,
                       tr("Select the project name and location..."),
                       currentProjectName,
                       filters);

    return file;
}

void EdytorNc::projectOpen()
{
    if (!maybeSaveProject()) {
        return;
    }

    QString filters = tr("EdytorNC project file (*.ncp)");
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

void EdytorNc::hidePanel()
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

void EdytorNc::projectTreeRemoveItem()
{
    QModelIndexList list = ui->projectTreeView->selectionModel()->selectedIndexes();

    for (QModelIndex it : list) {
        QStandardItem *item = model->itemFromIndex(it);

        if (item == nullptr) {
            return;
        }

        if (!item->hasChildren()) {
            currentProjectModified = model->removeRow(item->row(), model->indexFromItem(item->parent()));
        }
    }
}

void EdytorNc::projectLoad(const QString &projectName)
{
    QFileInfo file;
    QIcon icon;


    if (projectName.isEmpty()) {
        return;
    }

    currentProjectName = projectName;

    model->clear();

    QSettings settings(currentProjectName, QSettings::IniFormat);

    QStandardItem *item = new QStandardItem(QIcon(":/images/edytornc.png"),
                                            QFileInfo(currentProjectName).fileName());
    item->setToolTip(QDir::toNativeSeparators(currentProjectName));

    model->invisibleRootItem()->appendRow(item);

    currentProject = item;

    QFileSystemModel *fModel = new QFileSystemModel;

    int max = settings.beginReadArray("ProjectFiles");

    for (int i = 0; i < max; ++i) {
        settings.setArrayIndex(i);
        file.setFile(settings.value("File", "").toString());

        if ((file.absoluteDir().exists()) && (file.absoluteDir().isReadable())) {
            QList<QStandardItem *> items = model->findItems(file.absoluteDir().canonicalPath(),
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

                QStandardItem *childItem = new QStandardItem(icon, file.fileName());
                childItem->setToolTip(file.fileName());
                item->appendRow(childItem);
            }
        }
    }

    settings.endArray();

    ui->projectTreeView->expandAll();

    currentProjectModified = false;
}

bool EdytorNc::maybeSaveProject()
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

void EdytorNc::createFileBrowseTabs()
{
    dirModel = new QFileSystemModel();
    dirModel->setResolveSymlinks(true);

    //dirModel->setRootPath(lastDir.absolutePath());
    //fileTreeViewChangeRootDir();

    dirModel->setNameFilters(defaultMdiWindowProperites.extensions); //QStringList("*.nc")
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

void EdytorNc::updateOpenFileList()
{
    QFileInfo file;
    QStringList labels;

    ui->openFileTableWidget->setUpdatesEnabled(false);

    ui->openFileTableWidget->clear();
    labels << tr("Info") << tr("File Name") << "";
    ui->openFileTableWidget->setHorizontalHeaderLabels(labels);
    ui->openFileTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();

    ui->openFileTableWidget->setSortingEnabled(false);
    ui->openFileTableWidget->setRowCount(windows.size());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        file.setFile(child->filePath());

        QTableWidgetItem *newItem = new QTableWidgetItem(file.fileName() + (child->isModified() ? "*" : ""));

        if (file.canonicalFilePath().isEmpty()) {
            newItem->setToolTip(child->filePath());
        } else {
            newItem->setToolTip(QDir::toNativeSeparators(file.canonicalFilePath()));
        }

        ui->openFileTableWidget->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(child->brief());
        newItem->setToolTip(child->brief() + " --> " + QDir::toNativeSeparators(
                                file.canonicalFilePath()));
        ui->openFileTableWidget->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QIcon(":/images/fileclose_small.png"), "",
                                       QTableWidgetItem::UserType);
        newItem->setToolTip(tr("Close"));
        ui->openFileTableWidget->setItem(i, 2, newItem);

        if (child == activeMdiChild()) {
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

void EdytorNc::openFileTableWidgetClicked(int x, int y)
{
    QTableWidgetItem *item = ui->openFileTableWidget->item(x, 1);

    QMdiSubWindow *existing = findMdiChild(item->toolTip());

    if (existing) {
        if (y == 2) {
            existing->close();
            updateOpenFileList();
        } else {
            ui->mdiArea->setActiveSubWindow(existing);
        }
    }
}

void EdytorNc::fileTreeViewChangeRootDir()
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

    if (ui->currentPathCheckBox->isChecked() && (activeMdiChild() != nullptr)) {
        path = activeMdiChild()->filePath();

        if (QFileInfo(path).exists()) {
            path = QFileInfo(path).canonicalPath();
        } else {
            path = path.remove(QFileInfo(path).fileName());
        }
    } else {
        path = QDir::currentPath();
    }

    if (path.isEmpty()) {
        return;
    }

    if (dirModel->rootPath() == path) {
        return;
    }

    fileTreeViewChangeRootDir(path);
}

void EdytorNc::fileTreeViewChangeRootDir(const QString &path)
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

bool EdytorNc::event(QEvent *event)
{
    QString key, text;
    QModelIndex index;
    QFile file;
    QString fileName;

    if ((event->type() == QEvent::ToolTip)) {
        if (panelHidden) {
            return true;
        }

        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

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

void EdytorNc::updateSessionMenus()
{
    sessionsMenu->clear();

    QActionGroup *actionGroup = new QActionGroup(sessionsMenu);
    actionGroup->setExclusive(true);

    QStringList::const_iterator constIterator;

    for (constIterator = sessionList.constBegin(); constIterator != sessionList.constEnd();
            ++constIterator) {
        QString name = (*constIterator).toLocal8Bit().constData();
        QAction *action = actionGroup->addAction(name);
        action->setCheckable(true);
        action->setChecked(name == currentSession);
    }

    sessionsMenu->addActions(actionGroup->actions());
}

void EdytorNc::changeSession(QAction *action)
{
    QString name = action->text();

    if (currentSession == name) {
        return;
    }

    saveSession(currentSession);
    ui->mdiArea->closeAllSubWindows();

    action->setChecked(true);
    loadSession(name);
}

void EdytorNc::loadSession(const QString &name)
{
    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("Sessions");

    int max = settings.beginReadArray(name);

    for (int i = 0; i < max; ++i) {
        settings.setArrayIndex(i);
        defaultMdiWindowProperites.lastDir = QDir::currentPath();

        GCoderInfo *info = new GCoderInfo();
        info->filePath = settings.value("OpenedFile").toString();

        if (!info->filePath.isEmpty()) {
            info->cursorPos = settings.value("Cursor", 1).toInt();
            info->readOnly = settings.value("ReadOnly", false).toBool();
            info->geometry = settings.value("Geometry", QByteArray()).toByteArray();
            info->highlightMode = settings.value("HighlightMode", MODE_AUTO).toInt();
            m_MdiWidgetsMaximized = settings.value("MaximizedMdi", true).toBool();
            loadFile(DocumentInfo::Ptr(info), false);
        }
    }

    settings.endArray();
    settings.endGroup();

    currentSession = name;
    statusBar()->showMessage(tr("Session %1 loaded").arg(name), 5000);
}

void EdytorNc::saveSession(const QString &name)
{
    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("Sessions");

    settings.remove(name);

    settings.beginWriteArray(name);
    int i = 0;

    foreach (const QMdiSubWindow *window, ui->mdiArea->subWindowList(QMdiArea::StackingOrder)) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());

        try {
            const GCoderInfo &info = dynamic_cast<const GCoderInfo &>(*mdiChild->documentInfo());
            settings.setArrayIndex(i);
            settings.setValue("OpenedFile", info.filePath);
            settings.setValue("Cursor", info.cursorPos);
            settings.setValue("ReadOnly", info.readOnly);
            settings.setValue("Geometry", info.geometry);
            settings.setValue("HighlightMode", info.highlightMode);
            settings.setValue("MaximizedMdi", m_MdiWidgetsMaximized);

            i++;
        }  catch (std::bad_cast &e) {
        }
    }

    settings.endArray();
    settings.endGroup();
}

void EdytorNc::sessionMgr()
{
    sessionDialog *sesDialog = new sessionDialog(this);
    sesDialog->setSessionList(sessionList);
    sesDialog->setSelectedSession(currentSession);

    sesDialog->exec();

    sessionList = sesDialog->sessionList();
    QString name = sesDialog->selectedSession();

    if (name != currentSession) {
        saveSession(currentSession);
        ui->mdiArea->closeAllSubWindows();
        loadSession(name);
    }

    updateSessionMenus();


    delete (sesDialog);
}

void EdytorNc::savePrinterSettings(QPrinter *printer)
{
#ifndef QT_NO_PRINTER

    QSettings &settings = *Medium::instance().settings();

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

void EdytorNc::loadPrinterSettings(QPrinter *printer)
{
#ifndef QT_NO_PRINTER

    QSettings &settings = *Medium::instance().settings();

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

void EdytorNc::serialConfig()
{
    SerialPortConfigDialog *serialConfigDialog = new SerialPortConfigDialog(this,
            configBox->currentText());

    if (serialConfigDialog->exec() == QDialog::Accepted) {
        loadSerialConfignames();
    }
}

void EdytorNc::loadSerialConfignames()
{
    int id;
    QStringList list;
    QString item;

    QSettings &settings = *Medium::instance().settings();
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

void EdytorNc::serialConfigTest()
{
    SerialPortTestDialog *trDialog = new SerialPortTestDialog(this);

    trDialog->show();
}

void EdytorNc::sendButtonClicked()
{
    QString tx;
    MdiChild *activeWindow;

    activeWindow = activeMdiChild();

    if (activeWindow == nullptr) {
        return;
    }

    receiveAct->setEnabled(false);
    sendAct->setEnabled(false);
    commAppAct->setEnabled(false);
    QApplication::setOverrideCursor(Qt::BusyCursor);

    tx.append(activeWindow->text());

    SerialTransmissionDialog transmissionDialog(this);
    transmissionDialog.sendData(tx, configBox->currentText());

    receiveAct->setEnabled(true);
    sendAct->setEnabled(true);
    commAppAct->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void EdytorNc::receiveButtonClicked()
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
                MdiChild *activeWindow = newFile();

                if (activeWindow == nullptr) {
                    return;
                }

                if (activeWindow) {
                    activeWindow->textEdit()->clear();
                    activeWindow->textEdit()->insertPlainText(*it);

                    activeWindow->setHighligthMode(MODE_AUTO);

                    if (defaultMdiWindowProperites.defaultReadOnly) {
                        activeWindow->textEdit()->isReadOnly();
                    }

                    activeWindow->textEdit()->document()->clearUndoRedoStacks(QTextDocument::UndoAndRedoStacks);
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

void EdytorNc::fileChanged(const QString &fileName)
{
    QMdiSubWindow *existing;
    MdiChild *mdiChild = nullptr;
    bool modified = false;

    existing = findMdiChild(fileName);

    if (existing) {
        mdiChild = qobject_cast<MdiChild *>(existing->widget());
        modified = mdiChild->isModified();
        ui->mdiArea->setActiveSubWindow(existing);
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
        mdiChild->load();
        QApplication::restoreOverrideCursor();
        break;

    case QMessageBox::No:
        break;

    default:
        break;
    }
}

void EdytorNc::startSerialPortServer()
{
    QString path = QDir::toNativeSeparators(QApplication::applicationDirPath() + "/");
    QString fileName;

#ifdef Q_OS_WIN32
    fileName = "sfs.exe";
#else
    fileName = "sfs";
#endif

    QProcess::startDetached(path + fileName, QStringList(), path);
}

void EdytorNc::tileSubWindowsVertycally()
{
    if (ui->mdiArea->subWindowList().isEmpty()) {
        return;
    }

    QPoint position(0, 0);

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        QRect rect(0, 0, ui->mdiArea->width(),
                   ui->mdiArea->height() / ui->mdiArea->subWindowList().count());
        window->setGeometry(rect);
        window->move(position);
        position.setY(position.y() + window->height());
    }
}

void EdytorNc::clipboardChanged()
{
    QStandardItem *item;
    QFont font;
    bool notFound = true;

    updateMenus();

    QString text = clipboard->text();

    if (text.isEmpty()) {
        return;
    }

    QStandardItem *parentItem = clipboardModel->invisibleRootItem();

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

void EdytorNc::clipboardTreeViewContextMenu(const QPoint &point)
{
    Q_UNUSED(point);

    QStandardItem *item;
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

void EdytorNc::deleteFromClipboardButtonClicked()
{
    QModelIndexList list = ui->clipboardTreeView->selectionModel()->selectedIndexes();

    for (QModelIndex it : list) {
        QStandardItem *item = clipboardModel->itemFromIndex(it);

        if (item == nullptr) {
            return;
        }

        ui->clipboardTreeView->setSortingEnabled(false);
        clipboardModel->removeRow(item->row(), clipboardModel->invisibleRootItem()->index());
        ui->clipboardTreeView->setSortingEnabled(true);
    }
}

void EdytorNc::clipboardSave()
{
    QSettings settings(Medium::instance().settingsDir() + "/clipboard", QSettings::IniFormat);

    settings.remove("ClipboardItems");
    settings.beginWriteArray("ClipboardItems");

    QStandardItem *parentItem = clipboardModel->invisibleRootItem();

    for (int i = 0; i < parentItem->rowCount(); i++) {
        QStandardItem *item = parentItem->child(i, 0);

        settings.setArrayIndex(i);
        settings.setValue("Title", item->text());
        settings.setValue("ItemText", item->child(0, 0)->text());
    }

    settings.endArray();
}

void EdytorNc::clipboardLoad()
{
    QString itemText;
    QStandardItem *item;
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

        QStandardItem *parentItem = clipboardModel->invisibleRootItem();
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

void EdytorNc::doShowInLineCalc()
{
    if (activeMdiChild()) {
        activeMdiChild()->showInLineCalc();
    }
}

void EdytorNc::watchFile(const QString &fileName, bool add)
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
