/***************************************************************************
 *   Copyright (C) 2008 by Artur Kozioł   *
 *   artkoz@poczta.onet.pl   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include <QMainWindow>
#include <QProcess>

#include "commoninc.h"
#include "customfiledialog.h"
#include "findinf.h"
#include "dialogs.h"



class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QTextEdit;
QT_END_NAMESPACE

class edytornc : public QMainWindow
{
    Q_OBJECT

public:
    edytornc();
    ~edytornc();
    enum { MAX_RECENTFILES = 9 };

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    void openWithPreview();
    void save();
    void saveAs();
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
    void loadFile(_editor_properites options);
    void updateRecentFiles( const QString& filename );
    void fileOpenRecent( QAction *act );
    void updateRecentFilesMenu();
    void activeWindowChanged(QMdiSubWindow * window);
    void deleteText();
    void findInFl();
    void find();
    void findNext();
    void replace();
    void selAll();
    void zoomIn();
    void zoomOut();
    void config();
    void doBhc();
    void doInsertSpaces();
    void doRemoveSpaces();
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
    void findSlot(SSearchOptions &s, bool findFirst);
    void replaceSlot(bool &found);
    void replaceFindSlot(bool &found);
    void replaceAllSlot(bool &found);
    

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    MdiChild *currentMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);
    _editor_properites defaultMdiWindowProperites;
    FindInFiles *findFiles;
    FindDialog *findDialog;
    SSearchOptions searchOptions;

    QClipboard *clipboard;
    QByteArray fileDialogState;
    QString openFileFilter;

    QMdiArea *mdiArea;
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
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *findFilesAct;
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
    QAction *undoAct;
    QAction *redoAct;

    QAction *findAct;
    QAction *findNextAct;
    QAction *replaceAct;
    QAction *deleteAct;
    QAction *selAllAct;
    QAction *readOnlyAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;

    QAction *configAct;

    QAction *bhcAct;
    QAction *renumberAct;
    QAction *calcAct;
    QAction *insertDotAct;
    QAction *removeSpcAct;
    QAction *insertSpcAct;
    QAction *speedFeedAct;
    QAction *chamferAct;
    QAction *trianglesAct;
    QAction *convertAct;
    QAction *convertProgAct;

    QProcess *proc;
    QDir lastDir;


};

#endif
