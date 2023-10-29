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

#ifndef EDYTORNC_H
#define EDYTORNC_H

#include <QByteArray>
#include <QFileSystemWatcher>
#include <QObject>            // Q_OBJECT
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QToolBar>
#include <QMainWindow>

#include <commoninc.h>

class QAction;
class QCheckBox;
class QClipboard;
class QCloseEvent;
class QComboBox;
class QEvent;
class QFileSystemModel;
class QLabel;
class QLineEdit;
class QMdiSubWindow;
class QMenu;
class QModelIndex;
class QMoveEvent;
class QPoint;
class QPrinter;
class QProcess;
class QResizeEvent;
class QSignalMapper;
class QSize;
class QStandardItem;
class QStandardItemModel;
class QToolButton;
class QWidget;

class CommApp;
class FindInFiles;
class KDiff3App;
class MdiChild;
class Medium;

namespace Addons {
class Actions;
}

namespace Ui {
class EdytorNc;
}


class EdytorNc : public QMainWindow
{
    Q_OBJECT

public:
    static EdytorNc *instance();

    ~EdytorNc();

    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

    Addons::Actions *addonsActions();
    MdiChild *activeMdiChild();

protected:
    Ui::EdytorNc *ui;
    static EdytorNc *SINGLETON;

    EdytorNc(Medium *medium);

    Medium *mMedium;

    struct MWConfig {
        QPoint pos;
        QSize size;
    } mMWConfig;

    Addons::Actions *m_addonsActions;

public:
    void openFile(const QString fileName);
    void diffTwoFiles(const QString filename1, const QString filename2);

    enum { MAX_RECENTFILES = 16 };

public slots:
    void messReceived(const QString &text = "");
    MdiChild *newFile();

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
    bool event(QEvent *event);

private slots:
    MdiChild *newFileFromTemplate();
    void open();
    void openExample();
    void save();
    void saveAll();
    void saveAs();
    void printFile();
    void filePrintPreview();
    void printPreview(QPrinter *printer);
    void cut();
    void undo();
    void redo();
    void copy();
    void paste();
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void setActiveSubWindow(QWidget *window);
    void loadFile(_editor_properites options, bool checkAlreadyLoaded = true);
    void updateRecentFiles(const QString &filename);
    void fileOpenRecent(QAction *act);
    void updateRecentFilesMenu();
    void activeWindowChanged(QMdiSubWindow *window);
    void deleteText();
    void findInFl();
    bool findNext();
    bool findPrevious();
    void replaceNext();
    void replacePrevious();
    void replaceAll();
    void selAll();
    void config();
    void doRemoveByRegExp();
    void doTriangles();
    void readOnly();
    void doCalc();
    void loadFoundedFile(const QString &fileName);
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
    void projectTreeViewDoubleClicked(const QModelIndex &index);
    void fileTreeViewDoubleClicked(const QModelIndex &index);
    void hidePanel();
    void projectTreeRemoveItem();
    void projectLoad(QString projectName);
    void projectOpen();
    void updateOpenFileList();
    void openFileTableWidgetClicked(int x, int y);
    void doSplitPrograms();
    void diffEditorFile();
    void closeTab(int i);
    void closeCurrentWindow();
    void closeAllMdiWindows();
    void goToLine(QString fileName, int line);
    void doSwapAxes();
    void changeSession(QAction *action);
    void sessionMgr();
    void fileChanged(const QString fileName);
    void tileSubWindowsVertycally();
    void clipboardChanged();
    void deleteFromClipboardButtonClicked();
    void clipboardTreeViewContextMenu(const QPoint &point);
    void doShowInLineCalc();
    void watchFile(const QString &fileName, bool add);

signals:
    void needToShow();

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
    int defaultHighlightMode(QString filePath);
    QString projectSelectName();
    bool maybeSaveProject();
    void createFileBrowseTabs();
    void fileTreeViewChangeRootDir();
    void fileTreeViewChangeRootDir(QString path);
    QMdiSubWindow *findMdiChild(const QString &fileName);
    void createDiffApp();
    void updateSessionMenus();
    void loadSession(QString name);
    void saveSession(QString name);
    void savePrinterSettings(QPrinter *printer);
    void loadPrinterSettings(QPrinter *printer);
    void clipboardSave();
    void clipboardLoad();

    QStringList sessionList;
    QString currentSession;

    _editor_properites defaultMdiWindowProperites;
    FindInFiles *findFiles;

    bool panelHidden;
    QByteArray panelState;

    QClipboard *clipboard;
    QByteArray fileDialogState;

    KDiff3App *diffApp;

    QFileSystemModel *dirModel;
    QStandardItemModel *model;
    QStandardItemModel *clipboardModel;
    QStandardItem *currentProject;
    bool currentProjectModified;
    QString currentProjectName;

    QSignalMapper *windowMapper;
    QStringList m_recentFiles;

    QMenu *fileMenu;
    QMenu *recentFileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *blockSkipMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *sessionsMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *windowToolBar;
    QToolBar *toolsToolBar;

    QAction *newAct;
    QAction *openAct;
    QAction *openExampleAct;
    QAction *saveAct;
    QAction *saveAllAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *findFilesAct;
    QAction *printAct;
    QAction *printPreviewAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileHAct;
    QAction *tileVAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *createGlobalToolTipsAct;
    QAction *createUserToolTipsAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *swapAxesAct;
    QAction *inLineCalcAct;

    QAction *sessionMgrAct;

    QAction *findAct;
    QAction *replaceAct;
    QAction *deleteAct;
    QAction *selAllAct;
    QAction *readOnlyAct;

    QAction *configAct;

    QAction *calcAct;
    QAction *splittAct;
    QAction *trianglesAct;
    QAction *diffLAct;
    QAction *diffRAct;
    QAction *diffAct;
    QAction *diffEditorAct;

    QProcess *proc;
    QProcess *sfsProc;

    QLabel *labelStat1;
    QToolButton *readOnlyButton;
    QComboBox *highlightTypeCombo;
    QAction *attachHighlightToDirAct;
    QToolButton *attachHighlightButton;
    QAction *deAttachHighlightToDirAct;
    QToolButton *deAttachHighlightButton;

    QPointer<QToolBar> findToolBar;
    QLineEdit *findEdit;
    QLineEdit *replaceEdit;
    QLabel *findLabel;
    QLabel *replaceLabel;
    QAction *findNextAct;
    QAction *findPreviousAct;
    QAction *replaceNextAct;
    QAction *replacePreviousAct;
    QAction *replaceAllAct;
    QAction *findCloseAct;
    QCheckBox *mCheckFindWholeWords;
    QCheckBox *mCheckIgnoreCase;
    QCheckBox *mCheckIgnoreComments;

    QPointer<QToolBar> serialToolBar;
    QAction *showSerialToolBarAct;
    QAction *configPortAct;
    QAction *receiveAct;
    QAction *sendAct;
    QAction *attachToDirAct;
    QAction *deAttachToDirAct;
    QAction *diagAct;
    QAction *serialCloseAct;
    QAction *commAppAct;
    QComboBox *configBox;

    QPointer<CommApp> commApp;

    QPointer<QFileSystemWatcher> fileChangeMonitor;
};

#endif // EDYTORNC_H
