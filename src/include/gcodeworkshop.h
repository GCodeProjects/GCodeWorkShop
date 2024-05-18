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

#ifndef GCODEWORKSHOP_H
#define GCODEWORKSHOP_H

#include <QByteArray>   // for QByteArray
#include <QMainWindow>  // for QMainWindow
#include <QObject>      // for slots, Q_OBJECT, signals
#include <QPoint>       // for QPoint
#include <QPointer>     // for QPointer
#include <QSize>        // for QSize
#include <QString>      // for QString
#include <QStringList>  // for QStringList

class QAction;
class QCheckBox;
class QClipboard;
class QCloseEvent;
class QComboBox;
class QDir;
class QEvent;
class QFileSystemModel;
class QFileSystemWatcher;
class QLabel;
class QLineEdit;
class QMdiSubWindow;
class QMenu;
class QModelIndex;
class QMoveEvent;
class QPrinter;
class QProcess;
class QResizeEvent;
class QSignalMapper;
class QStandardItem;
class QStandardItemModel;
class QToolBar;
class QToolButton;
class QWidget;

#include <documentinfo.h>   // for DocumentInfo, DocumentInfo::Ptr

class CommApp;
class Document;
class DocumentManager;
class FindInFiles;
class GCoderDocument;
class KDiff3App;
class Medium;
class RecentFiles;
class SessionManager;

namespace Addons {
class Actions;
}

namespace Ui {
class GCodeWorkShop;
}


class GCodeWorkShop : public QMainWindow
{
	Q_OBJECT

public:
	static GCodeWorkShop* instance();

	~GCodeWorkShop();

	void resizeEvent(QResizeEvent* event);
	void moveEvent(QMoveEvent* event);

	Addons::Actions* addonsActions();
	DocumentManager* documentManager() const;
	Document* activeDocument() const;
	QString currentPath() const;
	QString lastOpenedPath() const;

protected:
	Ui::GCodeWorkShop* ui;
	static GCodeWorkShop* SINGLETON;

	GCodeWorkShop(Medium* medium);

	Medium* mMedium;

	struct MWConfig {
		QPoint pos;
		QSize size;
	} mMWConfig;

	Addons::Actions* m_addonsActions;

public:
	void openFile(const QString& fileName);
	void diffTwoFiles(const QString& filename1, const QString& filename2);

public slots:
	void messReceived(const QString& text = "");
	Document* newFile();
	bool maybeSave(Document* doc);
	bool maybeSaveAll();

protected:
	void closeEvent(QCloseEvent* event);
	bool event(QEvent* event);
	void setLastOpenedPath(const QString& path);

	bool save(Document* doc, bool forceSaveAs);

	void setMdiTabbedMode(bool tabbed);

private slots:
	Document* newFileFromTemplate();
	void open(const QDir& dir);
	void open();
	void openExample();
	bool save();
	bool saveAll();
	bool saveAs();
	void printFile();
	void filePrintPreview();
	void printPreview(QPrinter* printer);
	void cut();
	void undo();
	void redo();
	void copy();
	void paste();
	void about();
	void updateMenus();
	void updateWindowMenu();
	Document* createDocument(const QString& type);
	void setActiveSubWindow(QWidget* window);
	void loadFile(const DocumentInfo::Ptr& options, bool checkAlreadyLoaded = true);
	void recentFilesChanged();
	void fileOpenRecent(QAction* act);
	void updateRecentFilesMenu(const QStringList& fileList);
	void activeWindowChanged(QMdiSubWindow* window);
	void deleteText();
	void findInFl();
	bool findNext();
	bool findPrevious();
	void replaceNext();
	void replacePrevious();
	void replaceAll();
	void selAll();
	void config();
	void readOnly();
	void doCalc();
	void loadFoundedFile(const QString& fileName);
	void updateStatusBar();
	void createFindToolBar();
	void closeFindToolBar();
	void findTextChanged();
	void createSerialToolBar();
	void serialConfig();
	void serialConfigTest();
	void closeSerialToolbar();
	void sendButtonClicked();
	void receiveButtonClicked();
	void startSerialPortServer();
	void attachToDirButtonClicked(bool attach = true);
	void deAttachToDirButtonClicked();
	void setHighLightMode(int mode);
	void createGlobalToolTipsFile();
	void createUserToolTipsFile();
	void attachHighlightToDirActClicked();
	void deAttachHighlightToDirActClicked();
	void doDiffL();
	void doDiffR();
	void doDiff();
	void projectAdd();
	void projectSave();
	void projectSaveAs();
	void projectNew();
	void projectTreeViewDoubleClicked(const QModelIndex& index);
	void fileTreeViewDoubleClicked(const QModelIndex& index);
	void hidePanel();
	void projectTreeRemoveItem();
	void projectLoad(const QString& projectName);
	void projectOpen();
	void updateOpenFileList();
	void openFileTableWidgetClicked(int x, int y);
	void diffEditorFile();
	void closeCurrentWindow();
	void closeAllMdiWindows();
	void goToLine(const QString& fileName, int line);
	void updateSessionMenus(const QStringList& sessionList);
	void sessionsChanged();
	void changeSession(QAction* action);
	void beforeCurrentSessionChanged();
	void currentSessionChanged();
	void showSessionDialog();
	void fileChanged(const QString& fileName);
	void tileSubWindowsVertycally();
	void clipboardChanged();
	void deleteFromClipboardButtonClicked();
	void clipboardTreeViewContextMenu(const QPoint& point);
	void customContextMenuRequest(Document* doc, const QPoint& pos);
	QMenu* doContextMenuGCoder(GCoderDocument* doc, const QPoint& pos);
	void doShowInLineCalc();
	void watchFile(const QString& fileName, bool add);

signals:
	void needToShow();
	void intCapsLockChanged(bool enable);

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	void loadSerialConfignames();
	void updateCurrentSerialConfig();
	void attachHighlighterToDirButtonClicked(bool attach);
	int defaultHighlightMode(const QString& filePath);
	QString projectSelectName();
	bool maybeSaveProject();
	void createFileBrowseTabs();
	void fileTreeViewChangeRootDir();
	void fileTreeViewChangeRootDir(const QString& path);
	Document* findDocument(const QString& fileName);
	void createDiffApp();
	void openFilesFromSession();
	void storeFileInfoInSession();
	void savePrinterSettings(QPrinter* printer);
	void loadPrinterSettings(QPrinter* printer);
	void clipboardSave();
	void clipboardLoad();

	SessionManager* m_sessionManager;

	bool m_MdiWidgetsMaximized;
	DocumentManager* m_documentManager;
	QString m_calcBinary;
	QStringList m_extensions;
	QString m_saveExtension;
	QString m_saveDirectory;
	QString m_lastOpenedPath;
	bool m_MdiTabbedMode;
	bool m_defaultReadOnly;
	bool m_startEmpty;
	bool m_disableFileChangeMonitor;
	bool m_findInFilesHighlightEnable;
	FindInFiles* findFiles;

	bool panelHidden;
	QByteArray panelState;

	QClipboard* clipboard;
	QByteArray fileDialogState;

	KDiff3App* diffApp;

	QFileSystemModel* dirModel;
	QStandardItemModel* model;
	QStandardItemModel* clipboardModel;
	QStandardItem* currentProject;
	bool currentProjectModified;
	QString currentProjectName;

	QSignalMapper* windowMapper;
	RecentFiles* m_recentFiles;

	QMenu* fileMenu;
	QMenu* recentFileMenu;
	QMenu* editMenu;
	QMenu* toolsMenu;
	QMenu* blockSkipMenu;
	QMenu* windowMenu;
	QMenu* helpMenu;
	QMenu* sessionsMenu;

	QToolBar* fileToolBar;
	QToolBar* editToolBar;
	QToolBar* windowToolBar;
	QToolBar* toolsToolBar;

	QAction* newAct;
	QAction* openAct;
	QAction* openExampleAct;
	QAction* saveAct;
	QAction* saveAllAct;
	QAction* saveAsAct;
	QAction* exitAct;
	QAction* findFilesAct;
	QAction* printAct;
	QAction* printPreviewAct;
	QAction* cutAct;
	QAction* copyAct;
	QAction* pasteAct;
	QAction* closeAct;
	QAction* closeAllAct;
	QAction* tileHAct;
	QAction* tileVAct;
	QAction* cascadeAct;
	QAction* nextAct;
	QAction* previousAct;
	QAction* separatorAct;
	QAction* aboutAct;
	QAction* aboutQtAct;
	QAction* createGlobalToolTipsAct;
	QAction* createUserToolTipsAct;
	QAction* undoAct;
	QAction* redoAct;
	QAction* inLineCalcAct;

	QAction* sessionMgrAct;

	QAction* findAct;
	QAction* replaceAct;
	QAction* deleteAct;
	QAction* selAllAct;
	QAction* readOnlyAct;

	QAction* configAct;

	QAction* calcAct;
	QAction* diffLAct;
	QAction* diffRAct;
	QAction* diffAct;
	QAction* diffEditorAct;

	QProcess* proc;
	QProcess* sfsProc;

	QLabel* labelStat1;
	QToolButton* readOnlyButton;
	QComboBox* highlightTypeCombo;
	QAction* attachHighlightToDirAct;
	QToolButton* attachHighlightButton;
	QAction* deAttachHighlightToDirAct;
	QToolButton* deAttachHighlightButton;

	QPointer<QToolBar> findToolBar;
	QLineEdit* findEdit;
	QLineEdit* replaceEdit;
	QLabel* findLabel;
	QLabel* replaceLabel;
	QAction* findNextAct;
	QAction* findPreviousAct;
	QAction* replaceNextAct;
	QAction* replacePreviousAct;
	QAction* replaceAllAct;
	QAction* findCloseAct;
	QCheckBox* mCheckFindWholeWords;
	QCheckBox* mCheckIgnoreCase;
	QCheckBox* mCheckIgnoreComments;

	QPointer<QToolBar> serialToolBar;
	QAction* showSerialToolBarAct;
	QAction* configPortAct;
	QAction* receiveAct;
	QAction* sendAct;
	QAction* attachToDirAct;
	QAction* deAttachToDirAct;
	QAction* diagAct;
	QAction* serialCloseAct;
	QAction* commAppAct;
	QComboBox* configBox;

	QPointer<CommApp> commApp;

	QPointer<QFileSystemWatcher> fileChangeMonitor;
};

#endif // GCODEWORKSHOP_H
