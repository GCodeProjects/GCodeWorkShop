/***************************************************************************
 *   Copyright (C) 2006-2010 by Artur Kozioł                               *
 *   artkoz@poczta.onet.pl                                                 *
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



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "commoninc.h"
#include "mdichild.h"
#include "findinf.h"
#include "dialogs.h"
#include "serialtransmission.h"
#include "kdiff3.h"

#include "ui_edytornc.h"


class EdytorNc : public QMainWindow, public Ui::EdytorNc
{
    Q_OBJECT

public:
    EdytorNc();
    ~EdytorNc();
    void openFile(const QString fileName);
    void diffTwoFiles(const QString filename1, const QString filename2);

    enum { MAX_RECENTFILES = 16 };


public slots:
    void messReceived(const QString &text = "");

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
    bool event(QEvent *event);

private slots:
    void newFile();
    void open();
    void openExample();
    void save();
    void saveAs();
    void printFile();
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
    void updateRecentFiles( const QString& filename );
    void fileOpenRecent( QAction *act );
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
    void doBhc();
    void doInsertSpaces();
    void doRemoveSpaces();
    void doRemoveEmptyLines();
    void doInsertDot();
    void doRenumber();
    void doSpeedFeed();
    void doChamfer();
    void doTriangles();
    void doConvert();
    void doConvertProg();
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
    void loadConfig();
    void closeSerialToolbar();
    void sendButtonClicked();
    void receiveButtonClicked();
    void stopButtonClicked();
    void doCmpMacro();
    void lineDelaySlot();
    void attachToDirButtonClicked(bool attach = true);
    void deAttachToDirButtonClicked();
    void setHighLightMode(int mode);
    void createGlobalToolTipsFile();
    void createUserToolTipsFile();
    void attachHighlightToDirActClicked();
    void deAttachHighlightToDirActClicked();
    void sendStartDelayTimeout();
    void doDiffL();
    void doDiffR();
    void doDiff();
    void projectAdd();
    void projectSave();
    void projectSaveAs();
    void projectNew();
    void projectTreeViewDoubleClicked(const QModelIndex & index);
    void fileTreeViewDoubleClicked(const QModelIndex & index);
    void hidePanel();
    void projectTreeRemoveItem();
    void projectLoad(QString projectName);
    void projectOpen();
    void updateOpenFileList();
    void openFileTableWidgetClicked(int x, int y);
    void splitPrograms();



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
    void showError(int error);
    void updateCurrentSerialConfig();
    void attachHighlighterToDirButtonClicked(bool attach);
    int defaultHighlightMode(QString filePath);
    QString projectSelectName();
    bool maybeSaveProject();
    void createFileBrowseTabs();
    void fileTreeViewChangeRootDir();



    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);
    _editor_properites defaultMdiWindowProperites;
    FindInFiles *findFiles;

    bool panelHidden;
    QByteArray panelState;


    QClipboard *clipboard;
    QByteArray fileDialogState;
    QString openFileFilter;

    KDiff3App *diffApp;

    QFileSystemModel *dirModel;
    QStandardItemModel *model;
    QStandardItem *currentProject;
    bool currentProjectModified;
    QString currentProjectName;


    QSignalMapper *windowMapper;
    QStringList m_recentFiles;

    QMenu *fileMenu;
    QMenu *recentFileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *windowToolBar;
    QToolBar *toolsToolBar;   

    QAction *newAct;
    QAction *openAct;
    QAction *openExampleAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *findFilesAct;
    QAction *printAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
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

    QAction *findAct;
    QAction *replaceAct;
    QAction *deleteAct;
    QAction *selAllAct;
    QAction *readOnlyAct;

    QAction *configAct;

    QAction *bhcAct;
    QAction *renumberAct;
    QAction *calcAct;
    QAction *insertDotAct;
    QAction *removeSpcAct;
    QAction *removeEmptyLinesAct;
    QAction *splittAct;
    QAction *insertSpcAct;
    QAction *speedFeedAct;
    QAction *chamferAct;
    QAction *trianglesAct;
    QAction *convertAct;
    QAction *convertProgAct;
    QAction *cmpMacroAct;
    QAction *diffLAct;
    QAction *diffRAct;
    QAction *diffAct;

    QProcess *proc;
    QDir lastDir;

    QLabel *labelStat1;
    QToolButton *readOnlyButton;
    QComboBox *highlightTypeCombo;
    QAction *attachHighlightToDirAct;
    QToolButton *attachHighlightButton;
    QAction *deAttachHighlightToDirAct;
    QToolButton *deAttachHighlightButton;


    QToolBar *findToolBar;
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


    QToolBar *serialToolBar;
    QAction *showSerialToolBarAct;
    QAction *configPortAct;
    QAction *receiveAct;
    QAction *sendAct;
    QAction *attachToDirAct;
    QAction *deAttachToDirAct;
    QAction *diagAct;
    QAction *serialCloseAct;
    QComboBox *configBox;



    bool stop;
    QString portName, sendAtEnd, sendAtBegining;
    int baudRate;
    int dataBits;
    int stopBits;
    int parity;
    int flowControl;
    double lineDelay;
    QextSerialPort *comPort;
    bool readyCont;
    bool xoffReceived;
    bool deleteControlChars;
    int sendStartDelay;
    bool doNotShowProgressInEditor;
    int recieveTimeout;

    PortSettings portSettings;


};

#endif
