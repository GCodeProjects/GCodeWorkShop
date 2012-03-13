/***************************************************************************
 *   Copyright (C) 2006-2012 by Artur Kozioł                               *
 *   artkoz78@gmail.com                                                 *
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

#include "edytornc.h"
#include "mdichild.h"


#define EXAMPLES_PATH             "/usr/share/edytornc/EXAMPLES"


//**************************************************************************************************
//
//**************************************************************************************************

EdytorNc::EdytorNc()
{
    setAttribute(Qt::WA_DeleteOnClose);

    setupUi(this);

    findToolBar = NULL;
    serialToolBar = NULL;
    diffApp = NULL;
    findFiles = NULL;
    dirModel = NULL;
    openExampleAct = NULL;


    clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(updateMenus()));


    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));
    
    model = new QStandardItemModel();
    projectTreeView->setModel(model);
    currentProject = NULL;

    connect(projectTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(projectTreeViewDoubleClicked(QModelIndex)));
    connect(projectNewButton, SIGNAL(clicked()), this, SLOT(projectNew()));
    connect(projectAddButton, SIGNAL(clicked()), this, SLOT(projectAdd()));
    connect(projectSaveButton, SIGNAL(clicked()), this, SLOT(projectSave()));
    connect(projectSaveAsButton, SIGNAL(clicked()), this, SLOT(projectSaveAs()));
    connect(projectLoadButton, SIGNAL(clicked()), this, SLOT(projectOpen()));
    connect(projectRemoveButton, SIGNAL(clicked()), this, SLOT(projectTreeRemoveItem()));

    connect(hideButton, SIGNAL(clicked()), this, SLOT(hidePanel()));


    currentProjectModified = false;



    createActions();
    createMenus();
    createToolBars();

    updateMenus();

    readSettings();

    createFileBrowseTabs();


    setWindowTitle(tr("EdytorNC"));
    setWindowIcon(QIcon(":/images/edytornc.png"));
    createStatusBar();
    
    if(defaultMdiWindowProperites.windowMode & TABBED_MODE)
    {
       mdiArea->setViewMode(QMdiArea::TabbedView);
       QTabBar* tab = mdiArea->findChild<QTabBar*>();
       if(tab)
       {
          connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
          tab->setTabsClosable(true);
          // The tabs might be very wide
          tab->setExpanding(false);
       };
    }
    else
       mdiArea->setViewMode(QMdiArea::SubWindowView);

}

//**************************************************************************************************
//
//**************************************************************************************************

EdytorNc::~EdytorNc()
{
   proc = findChild<QProcess *>();
   if(proc)
   {
      proc->close();
      delete(proc);
   };
}    

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::closeTab(int i)
{
   QMdiSubWindow *sub = mdiArea->subWindowList()[i];
   QWidget *win = sub->widget();
   win->close();
   mdiArea->setActiveSubWindow(sub);
   mdiArea->closeActiveSubWindow();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::closeCurrentWindow()
{
   mdiArea->closeActiveSubWindow();
}

//**************************************************************************************************
//
//**************************************************************************************************


void EdytorNc::closeEvent(QCloseEvent *event)
{
    setUpdatesEnabled(FALSE);
    writeSettings();

    if(!maybeSaveProject())
    {
       event->ignore();
       return;
    };

    foreach(const QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder))
    {
       MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
       if(mdiChild->textEdit->document()->isModified())
       {
          setUpdatesEnabled(TRUE);
          mdiChild->activateWindow();
          mdiChild->raise();
          if(!mdiChild->parentWidget()->close())
          {
              event->ignore();
              return;
          };
          setUpdatesEnabled(FALSE);
       };
    };

    mdiArea->closeAllSubWindows();

    if(activeMdiChild())
    {
       event->ignore();
    } 
    else 
    {  
       event->accept();
    }
    setUpdatesEnabled(TRUE);

    if(findFiles != NULL)
    {
      findFiles->close();
      findFiles = NULL;
    };

}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::newFile()
{
    MdiChild *child = createMdiChild();

    child->newFile();
    defaultMdiWindowProperites.cursorPos = 0;
    defaultMdiWindowProperites.readOnly = FALSE;
    //defaultMdiWindowProperites.maximized = FALSE;
    defaultMdiWindowProperites.geometry = QByteArray();
    defaultMdiWindowProperites.editorToolTips = true;
    defaultMdiWindowProperites.hColors.highlightMode = MODE_AUTO;
    child->setMdiWindowProperites(defaultMdiWindowProperites);

    if(defaultMdiWindowProperites.maximized)
      child->showMaximized();
    else
      child->showNormal();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::open()
{
    QFileInfo file;
    QMdiSubWindow *existing;

    existing = 0;
    QString *filters = getFilters(defaultMdiWindowProperites.extensions);

    QStringList files = QFileDialog::getOpenFileNames(
                         this,
                         tr("Select one or more files to open"),
                         lastDir.absolutePath(),
                         *filters, 0);

    delete filters;


    QStringList list = files;
    QStringList::Iterator it = list.begin();
    while(it != list.end())
    {
       file.setFile(*it);
       existing = findMdiChild(*it);

       if((file.exists()) && (file.isReadable()) && !existing)
       {
          lastDir = file.absoluteDir();
          fileTreeViewChangeRootDir();
          MdiChild *child = createMdiChild();
          if(child->loadFile(*it))
          {
             defaultMdiWindowProperites.cursorPos = 0;
             defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
             defaultMdiWindowProperites.geometry = QByteArray();
             defaultMdiWindowProperites.hColors.highlightMode = defaultHighlightMode(QFileInfo(*it).absolutePath());
             defaultMdiWindowProperites.editorToolTips = true;
             child->setMdiWindowProperites(defaultMdiWindowProperites);
             if(defaultMdiWindowProperites.maximized)
               child->showMaximized();
             else
               child->showNormal();
             updateRecentFiles(*it);
          }
          else
          {
             child->parentWidget()->close();
          };
       };
       ++it;
    };
    if(existing)
      mdiArea->setActiveSubWindow(existing);

    statusBar()->showMessage(tr("File loaded"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::openExample()
{
   QFileInfo file;
   QMdiSubWindow *existing;
   QString dir;

   existing = 0;


   if(QDir(EXAMPLES_PATH).exists())
      dir = EXAMPLES_PATH;
   else
      dir = QApplication::applicationDirPath() + "/" + "EXAMPLES";



   QString *filters = getFilters(defaultMdiWindowProperites.extensions);

   QStringList files = QFileDialog::getOpenFileNames(
                        this,
                        tr("Select one or more files to open"),
                        dir,
                        *filters, 0);

   delete filters;


   QStringList list = files;
   QStringList::Iterator it = list.begin();
   while(it != list.end())
   {
      file.setFile(*it);
      existing = findMdiChild(*it);

      if((file.exists()) && (file.isReadable()) && !existing)
      {
         lastDir = file.absoluteDir();
         fileTreeViewChangeRootDir();
         MdiChild *child = createMdiChild();
         if(child->loadFile(*it))
         {
            defaultMdiWindowProperites.cursorPos = 0;
            defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
            defaultMdiWindowProperites.geometry = QByteArray();
            defaultMdiWindowProperites.hColors.highlightMode = defaultHighlightMode(QFileInfo(*it).absolutePath());
            defaultMdiWindowProperites.editorToolTips = true;
            child->setMdiWindowProperites(defaultMdiWindowProperites);
            if(defaultMdiWindowProperites.maximized)
              child->showMaximized();
            else
              child->showNormal();
            updateRecentFiles(*it);
         }
         else
         {
            child->parentWidget()->close();
         };
      };
      ++it;
   };
   if(existing)
     mdiArea->setActiveSubWindow(existing);

   statusBar()->showMessage(tr("File loaded"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::openFile(const QString fileName)
{
   QFileInfo file;

   file.setFile(fileName);

   QMdiSubWindow *existing = findMdiChild(fileName);

   if((file.exists()) && (file.isReadable()) && !existing)
   {
      lastDir = file.absoluteDir();
      fileTreeViewChangeRootDir();
      MdiChild *child = createMdiChild();
      if(child->loadFile(fileName))
      {
         defaultMdiWindowProperites.cursorPos = 0;
         defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
         //defaultMdiWindowProperites.maximized = FALSE;
         defaultMdiWindowProperites.geometry = QByteArray();
         defaultMdiWindowProperites.hColors.highlightMode = defaultHighlightMode(child->filePath());
         defaultMdiWindowProperites.editorToolTips = true;
         child->setMdiWindowProperites(defaultMdiWindowProperites);
         if(defaultMdiWindowProperites.maximized)
           child->showMaximized();
         else
           child->showNormal();
      }
      else
      {
         child->parentWidget()->close();
      };
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::save()
{
    if(activeMdiChild() && activeMdiChild()->save())
      statusBar()->showMessage(tr("File saved"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::saveAs()
{
    if(activeMdiChild() && activeMdiChild()->saveAs())
      statusBar()->showMessage(tr("File saved"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::printFile()
{
   if(activeMdiChild())
   {
      QTextDocument *document = activeMdiChild()->textEdit->document();
      QPrinter printer;

      QPrintDialog *dlg = new QPrintDialog(&printer, this);
      if(dlg->exec() != QDialog::Accepted)
        return;

      document->print(&printer);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::cut()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->cut();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::copy()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->copy();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::findInFl()
{
   if(findFiles == NULL)
   {
      findFiles = new FindInFiles(splitter);

      if(defaultMdiWindowProperites.syntaxH)
         findFiles->setHighlightColors(defaultMdiWindowProperites.hColors);

      if(activeMdiChild())
         findFiles->setDir(QFileInfo(activeMdiChild()->currentFile()).canonicalPath());

      connect(findFiles, SIGNAL(fileClicked(QString)), this, SLOT(loadFoundedFile(QString)));
   }
   else
     if(!findFilesAct->isChecked())
     {
        findFiles->close();
        findFiles = NULL;
     }
     else
     {
        findFiles->show();
        findFilesAct->setChecked(true);
     };
}

//**************************************************************************************************
//
//**************************************************************************************************

bool EdytorNc::findNext()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   bool found = false;
   QTextCursor cursor, cursorOld;
   QPalette palette;

   findNextAct->setEnabled(false);
   findPreviousAct->setEnabled(false);

   if(!findEdit->text().isEmpty() && hasMdiChild)
   {
      found = activeMdiChild()->findText(findEdit->text(),
                                         ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                          (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))),
                                         mCheckIgnoreComments->isChecked());

      if(!found)
      {
         cursor = activeMdiChild()->textEdit->textCursor();
         cursorOld = cursor;
         cursor.movePosition(QTextCursor::Start);
         activeMdiChild()->textEdit->setTextCursor(cursor);


         found = activeMdiChild()->findText(findEdit->text(),
                                            ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                             (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))),
                                            mCheckIgnoreComments->isChecked());

         if(!found)
         {
            cursorOld.clearSelection();
            activeMdiChild()->textEdit->setTextCursor(cursorOld);
         };
      };

      palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

      if(found)
         findEdit->setPalette(QPalette());
      else
         findEdit->setPalette(palette);
   };

   findNextAct->setEnabled(true);
   findPreviousAct->setEnabled(true);
   return found;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool EdytorNc::findPrevious()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   bool found = false;
   QTextCursor cursor, cursorOld;
   QPalette palette;

   findNextAct->setEnabled(false);
   findPreviousAct->setEnabled(false);

   if(!findEdit->text().isEmpty() && hasMdiChild)
   {
      found = activeMdiChild()->findText(findEdit->text(), QTextDocument::FindBackward |
                                         ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                          (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))),
                                         mCheckIgnoreComments->isChecked());

      if(!found)
      {
         cursor = activeMdiChild()->textEdit->textCursor();
         cursorOld = cursor;
         cursor.movePosition(QTextCursor::End);
         activeMdiChild()->textEdit->setTextCursor(cursor);

         found = activeMdiChild()->findText(findEdit->text(), QTextDocument::FindBackward |
                                            ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                             (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))),
                                            mCheckIgnoreComments->isChecked());

         if(!found)
         {
            cursorOld.clearSelection();
            activeMdiChild()->textEdit->setTextCursor(cursorOld);
         };
      };

      palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

      if(found)
         findEdit->setPalette(QPalette());
      else
         findEdit->setPalette(palette);
   };

   findNextAct->setEnabled(true);
   findPreviousAct->setEnabled(true);

   return found;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::replaceNext()
{
   QString replacedText, foundText;
   double val, val1;
   bool ok;
   QRegExp regExp;
   QChar op;

   bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->textEdit->isReadOnly());
   bool found = false;

   replaceNextAct->setEnabled(false);
   replacePreviousAct->setEnabled(false);
   replaceAllAct->setEnabled(false);

   if(hasMdiChildNotReadOnly) //!replaceEdit->text().isEmpty() &&
   {
      if(activeMdiChild()->findTextMatched(findEdit->text(), activeMdiChild()->textEdit->textCursor().selectedText()))
         found = true;
      else
         found = findNext();

      if(found)
      {
         QTextCursor cr = activeMdiChild()->textEdit->textCursor();
         cr.beginEditBlock();
         if(defaultMdiWindowProperites.underlineChanges)
         {
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(defaultMdiWindowProperites.underlineColor));
            cr.setCharFormat(format);
         };

         replacedText = replaceEdit->text();

         regExp.setPattern(QString("\\$\\$[\\/*+\\-]{1,1}[0-9.]{1,}"));
         if(replacedText.contains(regExp))
         {
            replacedText.remove("$$");
            op = replacedText.at(0);
            replacedText.remove(0, 1);
            val = replacedText.toDouble(&ok);

            foundText = cr.selectedText();
            foundText.remove(QRegExp("[A-Za-z]{1,}"));
            val1 = foundText.toDouble(&ok);
            replacedText = cr.selectedText();
            replacedText.remove(foundText);

            if(op == '+')
               val = val1 + val;
            if(op == '-')
               val = val1 - val;
            if(op == '*')
               val = val1 * val;
            if(op == '/')
               val = val1 / val;

            replacedText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', 3));
         };

         cr.insertText(replacedText);
         cr.endEditBlock();
         activeMdiChild()->textEdit->setTextCursor(cr);
         findNext();
      };
   };

   replaceNextAct->setEnabled(true);
   replacePreviousAct->setEnabled(true);
   replaceAllAct->setEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::replacePrevious()
{
   QString replacedText, foundText;
   double val, val1;
   bool ok;
   QRegExp regExp;
   QChar op;

   bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->textEdit->isReadOnly());
   bool found = false;

   replaceNextAct->setEnabled(false);
   replacePreviousAct->setEnabled(false);
   replaceAllAct->setEnabled(false);

   if(hasMdiChildNotReadOnly) //!replaceEdit->text().isEmpty() &&
   {
      if(activeMdiChild()->findTextMatched(findEdit->text(), activeMdiChild()->textEdit->textCursor().selectedText()))
         found = true;
      else
         found = findPrevious();

      if(found)
      {
         QTextCursor cr = activeMdiChild()->textEdit->textCursor();
         cr.beginEditBlock();
         if(defaultMdiWindowProperites.underlineChanges)
         {
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(defaultMdiWindowProperites.underlineColor));
            cr.setCharFormat(format);
         };

         replacedText = replaceEdit->text();

         regExp.setPattern(QString("\\$\\$[\\/*+\\-]{1,1}[0-9.]{1,}"));
         if(replacedText.contains(regExp))
         {
            replacedText.remove("$$");
            op = replacedText.at(0);
            replacedText.remove(0, 1);
            val = replacedText.toDouble(&ok);

            foundText = cr.selectedText();
            foundText.remove(QRegExp("[A-Za-z]{1,}"));
            val1 = foundText.toDouble(&ok);
            replacedText = cr.selectedText();
            replacedText.remove(foundText);

            if(op == '+')
               val = val1 + val;
            if(op == '-')
               val = val1 - val;
            if(op == '*')
               val = val1 * val;
            if(op == '/')
               val = val1 / val;

            replacedText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', 3));
         };

         cr.insertText(replacedText);
         cr.endEditBlock();
         activeMdiChild()->textEdit->setTextCursor(cr);
         findPrevious();
      };
   };

   replaceNextAct->setEnabled(true);
   replacePreviousAct->setEnabled(true);
   replaceAllAct->setEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::replaceAll()
{
   QTextCursor startCursor, cr;
   QTextCharFormat format;

   bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->textEdit->isReadOnly());
   bool found = false;

   replaceNextAct->setEnabled(false);
   replacePreviousAct->setEnabled(false);
   replaceAllAct->setEnabled(false);

   if(hasMdiChildNotReadOnly) //!replaceEdit->text().isEmpty() &&
   {
      QApplication::setOverrideCursor(Qt::BusyCursor);
      if(activeMdiChild()->textEdit->textCursor().selectedText() == findEdit->text())
        found = true;
      else
        found = findNext();

      startCursor = activeMdiChild()->textEdit->textCursor();

      while(found)
      {
         cr = activeMdiChild()->textEdit->textCursor();
         cr.beginEditBlock();
         if(defaultMdiWindowProperites.underlineChanges)
         {
            format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(defaultMdiWindowProperites.underlineColor));
            cr.setCharFormat(format);
         };
         cr.insertText(replaceEdit->text());
         cr.endEditBlock();
         activeMdiChild()->textEdit->setTextCursor(cr);

         found = findNext();
         if(startCursor.blockNumber() == activeMdiChild()->textEdit->textCursor().blockNumber())
           break;
         qApp->processEvents();
      };
      QApplication::restoreOverrideCursor();
   };

   replaceNextAct->setEnabled(true);
   replacePreviousAct->setEnabled(true);
   replaceAllAct->setEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::selAll()
{
   if(activeMdiChild())
      activeMdiChild()->textEdit->selectAll();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::config()
{
   _editor_properites opt;
   MdiChild *mdiChild;

   SetupDialog *setUpDialog = new SetupDialog(this, &defaultMdiWindowProperites);

   if(setUpDialog->exec() == QDialog::Accepted)
   {
      defaultMdiWindowProperites = setUpDialog->getSettings();

      if(defaultMdiWindowProperites.windowMode & TABBED_MODE)
      {
         mdiArea->setViewMode(QMdiArea::TabbedView);
         QTabBar* tab = mdiArea->findChild<QTabBar*>();
         if(tab)
         {
            connect(tab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
            tab->setTabsClosable(true);
            // The tabs might be very wide
            tab->setExpanding(false);
         };
      }
      else
        mdiArea->setViewMode(QMdiArea::SubWindowView);

      foreach(const QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder))
      {
         mdiChild = qobject_cast<MdiChild *>(window->widget());
         opt = mdiChild->getMdiWindowProperites();

         defaultMdiWindowProperites.hColors.highlightMode = opt.hColors.highlightMode;

         opt.fontName = defaultMdiWindowProperites.fontName;
         opt.fontSize = defaultMdiWindowProperites.fontSize;
         opt.syntaxH = defaultMdiWindowProperites.syntaxH;
         opt.hColors = defaultMdiWindowProperites.hColors;
         opt.intCapsLock = defaultMdiWindowProperites.intCapsLock;
         opt.lineColor = defaultMdiWindowProperites.lineColor;
         opt.underlineColor = defaultMdiWindowProperites.underlineColor;
         opt.underlineChanges = defaultMdiWindowProperites.underlineChanges;
         opt.clearUnderlineHistory = defaultMdiWindowProperites.clearUnderlineHistory;
         opt.clearUndoHistory = defaultMdiWindowProperites.clearUndoHistory;
         opt.editorToolTips = defaultMdiWindowProperites.editorToolTips;
         opt.windowMode = defaultMdiWindowProperites.windowMode;
         opt.readOnly = defaultMdiWindowProperites.defaultReadOnly;

         mdiChild->setMdiWindowProperites(opt);
      };
   };
   delete setUpDialog;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::readOnly()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->setReadOnly(readOnlyAct->isChecked());
    updateMenus();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doBhc()
{
   BHCDialog *bhcDialog;
   bhcDialog = findChild<BHCDialog *>();
   if(!bhcDialog)
   {
      BHCDialog *bhcDialog = new BHCDialog(this);
      bhcDialog->show();
      bhcDialog->move((geometry().x() + width() - 10) - bhcDialog->width(), geometry().y()+35);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doInsertSpaces()
{
   if(activeMdiChild())
      activeMdiChild()->doInsertSpace();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doRemoveSpaces()
{
   if(activeMdiChild())
      activeMdiChild()->doRemoveSpace();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::goToLine(QString fileName, int line)
{
   if(activeMdiChild())
   {
      QString childFileName = activeMdiChild()->filePath() + "/" + activeMdiChild()->fileName();
      childFileName =  QDir().toNativeSeparators(childFileName);

      if(QDir().toNativeSeparators(fileName) != childFileName)
         return;

      QTextBlock block = activeMdiChild()->textEdit->document()->findBlockByNumber(line);
      QTextCursor cursor = QTextCursor(block);
      activeMdiChild()->textEdit->setTextCursor(cursor);
      activeMdiChild()->textEdit->centerCursor();
      activeMdiChild()->setFocus();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createDiffApp()
{
   if(diffApp == NULL)
   {
      diffApp = new KDiff3App(splitter, "DiffApp", defaultMdiWindowProperites.extensions);

      connect(diffApp, SIGNAL(lineClicked(QString, int)), this, SLOT(goToLine(QString, int)));

   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doDiffL()
{
   QString fileName;

   createDiffApp();

   if(diffApp != NULL)
   {
      diffLAct->setEnabled(false);
      diffRAct->setEnabled(false);
      diffAct->setEnabled(false);

      diffAct->setChecked(true);

      if(activeMdiChild())
         fileName = activeMdiChild()->currentFile();
      if(fileName.isEmpty())
         fileName = lastDir.canonicalPath();
      diffApp->completeInit("", fileName);

      diffLAct->setEnabled(true);
      diffRAct->setEnabled(true);
      diffAct->setEnabled(true);
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doDiffR()
{
   QString fileName;

   createDiffApp();

   if(diffApp != NULL)
   {
      diffLAct->setEnabled(false);
      diffRAct->setEnabled(false);
      diffAct->setEnabled(false);

      diffAct->setChecked(true);

      if(activeMdiChild())
         fileName = activeMdiChild()->currentFile();
      if(fileName.isEmpty())
         fileName = lastDir.canonicalPath();
      diffApp->completeInit(fileName, "");

      diffLAct->setEnabled(true);
      diffRAct->setEnabled(true);
      diffAct->setEnabled(true);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::diffTwoFiles(const QString filename1, const QString filename2)
{
   createDiffApp();

   if(diffApp != NULL)
   {
      diffAct->setChecked(true);
      diffApp->completeInit(filename1, filename2);

      QList<int> sizes;
      sizes.clear();
      sizes.append(0);
      sizes.append(splitter->height());
      splitter->setSizes(sizes);
   };
}

//**************************************************************************************************
// Shows diff of currently edited file and file on disk
//**************************************************************************************************

void EdytorNc::diffEditorFile()
{
   MdiChild *child = activeMdiChild();
   if(!child)
      return;

   createDiffApp();

   if(diffApp != NULL)
   {
      QString fileName = child->currentFile();
      if(fileName.isEmpty())
         return;

      QString fileName1 = QDir::tempPath() + QDir::separator() + QFileInfo(fileName).baseName() + ".tmp";
      //qDebug() << fileName << fileName1;

      QFile file(fileName1);
      if(!file.open(QIODevice::WriteOnly))
      {
         QMessageBox::warning(this, tr("EdytorNC"),
                              tr("Cannot write tmp file \"%1\".\n %2")
                              .arg(QDir::toNativeSeparators(fileName1))
                              .arg(file.errorString()));
         return;
      };

      QTextStream out(&file);

      QString tex = child->textEdit->toPlainText();
      if(!tex.contains(QLatin1String("\r\n")))
        tex.replace(QLatin1String("\n"), QLatin1String("\r\n"));
      out << tex;
      file.close();

      diffAct->setChecked(true);
      diffApp->completeInit(fileName, fileName1);

      if(file.exists())
         file.remove();

//      QList<int> sizes;
//      sizes.clear();
//      sizes.append(0);
//      sizes.append(splitter->height());
//      splitter->setSizes(sizes);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doDiff()
{
   QString fileName;

   if(diffApp == NULL)
   {
      createDiffApp();

      if(activeMdiChild())
         fileName = activeMdiChild()->currentFile();
      if(fileName.isEmpty())
         fileName = lastDir.canonicalPath();
      diffApp->completeInit(QFileInfo(fileName).canonicalPath(), QFileInfo(fileName).canonicalPath());

   }
   else
      if(!diffAct->isChecked())
      {
         diffAct->setChecked(false);
         diffApp->close();
         diffApp = NULL;
      };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doRemoveEmptyLines()
{
   if(activeMdiChild())
      activeMdiChild()->doRemoveEmptyLines();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doInsertEmptyLines()
{
   if(activeMdiChild())
      activeMdiChild()->doInsertEmptyLines();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doInsertDot()
{
   MdiChild *child;

   DotDialog *dotDialog = new DotDialog(this);
   dotDialog->setState(defaultMdiWindowProperites.dotAdr, defaultMdiWindowProperites.atEnd, defaultMdiWindowProperites.dotAfter, defaultMdiWindowProperites.dotAftrerCount);

   child = activeMdiChild();

   if(dotDialog->exec() == QDialog::Accepted)
   {
      if(child)
      {
         defaultMdiWindowProperites = child->getMdiWindowProperites();
         dotDialog->getState(defaultMdiWindowProperites.dotAdr, defaultMdiWindowProperites.atEnd, defaultMdiWindowProperites.dotAfter, defaultMdiWindowProperites.dotAftrerCount);
         child->setMdiWindowProperites(defaultMdiWindowProperites);
         child->doInsertDot();
      };
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doRenumber()
{

   int startAt, inc, to, from, prec, mode;
   bool renumEmpty, renumComm, renumMarked;
   MdiChild *child;
   
   child = activeMdiChild();
   
   RenumberDialog *renumberDialog = new RenumberDialog(this);
   //renumberDialog->setState(editorOpt.dotAdr, editorOpt.atEnd, editorOpt.dotAfter, editorOpt.dotAftrerCount);

   if(renumberDialog->exec() == QDialog::Accepted)
   {
      renumberDialog->getState(mode, startAt, from, prec, inc, to, renumEmpty, renumComm, renumMarked);
      if(child)
      {
         inc = child->doRenumber(mode, startAt, from, prec, inc, to, renumEmpty, renumComm, renumMarked);
         if(mode == 3)
           statusBar()->showMessage(QString(tr("Removed : %1 line numbers.")).arg(inc), 9000);
         else
           statusBar()->showMessage(QString(tr("Renumbered : %1 lines.")).arg(inc), 9000);
      };

   };

   delete(renumberDialog);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doSpeedFeed()
{
   FeedsDialog *feedsDialog;
   feedsDialog = findChild<FeedsDialog *>();
   if(!feedsDialog)
   {
      FeedsDialog *feedsDialog = new FeedsDialog(this);
      feedsDialog->move((geometry().x() + width() - 10) - feedsDialog->width(), geometry().y()+35);
      feedsDialog->show();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doChamfer()
{
   ChamferDialog *chamferDialog = new ChamferDialog(this);
   chamferDialog->move((geometry().x() + width()) - chamferDialog->width(), geometry().y()+35);
   chamferDialog->show();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doTriangles()
{
   TriangleDialog *triangleDialog;
   triangleDialog = findChild<TriangleDialog *>();
   if(!triangleDialog)
   {
      TriangleDialog *triangleDialog = new TriangleDialog(this);
      triangleDialog->move((geometry().x() + width() - 10) - triangleDialog->width(), geometry().y()+35);
      triangleDialog->show();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doConvert()
{
   I2MDialog *i2MDialog;
   i2MDialog = findChild<I2MDialog *>();
   if(!i2MDialog)
   {
      I2MDialog *i2MDialog = new I2MDialog(this);
      i2MDialog->move((geometry().x() + width() - 10) - i2MDialog->width(), geometry().y()+35);
      i2MDialog->show();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doConvertProg()
{
   MdiChild *child;

   I2MProgDialog *i2mProgDialog = new I2MProgDialog(this);
   i2mProgDialog->setState(defaultMdiWindowProperites.i2mAdr, defaultMdiWindowProperites.i2mprec, defaultMdiWindowProperites.inch);

   child = activeMdiChild();

   if(i2mProgDialog->exec() == QDialog::Accepted)
   {
      if(child)
      {
         defaultMdiWindowProperites = child->getMdiWindowProperites();
         i2mProgDialog->getState(defaultMdiWindowProperites.i2mAdr, defaultMdiWindowProperites.i2mprec, defaultMdiWindowProperites.inch);
         child->setMdiWindowProperites(defaultMdiWindowProperites);
         child->doI2M();
      };
   };

   delete(i2mProgDialog);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doCalc()
{
   if(defaultMdiWindowProperites.calcBinary.isEmpty())
   {
      QMessageBox::information(this, tr("Information"),
                               tr("Set correct calculator program name in configuration dialog."));
      return;
   };

   proc = findChild<QProcess *>();

   if(!proc)
   {
      proc = new QProcess(this);
      proc->start(defaultMdiWindowProperites.calcBinary);
   }
   else
      if(proc->pid() == 0)
         proc->start(defaultMdiWindowProperites.calcBinary);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::deleteText()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->textCursor().removeSelectedText();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::paste()
{
   if(activeMdiChild())
   {
      if(defaultMdiWindowProperites.underlineChanges)
      {
         QTextCharFormat format = activeMdiChild()->textEdit->currentCharFormat();
         format.setUnderlineStyle(QTextCharFormat::DotLine);
         format.setUnderlineColor(QColor(defaultMdiWindowProperites.underlineColor));
         activeMdiChild()->textEdit->setCurrentCharFormat(format);
      };

      activeMdiChild()->textEdit->paste();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::undo()
{
   if(activeMdiChild())
   {
      activeMdiChild()->doUndo();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::redo()
{
   if(activeMdiChild())
   {
      activeMdiChild()->doRedo();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::activeWindowChanged(QMdiSubWindow *window)
{
   Q_UNUSED(window);
   MdiChild *mdiChild;

   if(mdiArea->subWindowList().count() <= 1)
     defaultMdiWindowProperites.maximized = TRUE;

   mdiChild = activeMdiChild();
   if(mdiChild)
   {
      defaultMdiWindowProperites.maximized = mdiChild->parentWidget()->isMaximized();
      statusBar()->showMessage(mdiChild->currentFile(), 9000);
   };
   updateCurrentSerialConfig();
   updateOpenFileList();
   fileTreeViewChangeRootDir();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::about()
{
   QMessageBox::about(this, trUtf8("About EdytorNC"),
                            trUtf8("The <b>EdytorNC</b> is text editor for CNC programmers.") +
                            trUtf8("<P>Version: ") +
                                   "2012.03.13 BETA" +
                            trUtf8("<P>Copyright (C) 1998 - 2012 by <a href=\"mailto:artkoz78@gmail.com\">Artur Koziol</a>") +
                            trUtf8("<P>Catalan translation and deb package thanks to Jordi Sayol i Salomó") +
                            trUtf8("<br />German translation thanks to Michael Numberger") +
                            trUtf8("<br />Czech translation thanks to Pavel Fric") +
                            trUtf8("<br />Finnish translation thanks to Janne Mäntyharju") +
                            trUtf8("<br />OS X patch and other updates thanks to Janne Mäntyharju") +
                            trUtf8("<P>New EdytorNC icon thanks to Jakub Gajewski") +
                            trUtf8("<P><a href=\"http://sourceforge.net/projects/edytornc/\">http://sourceforge.net/projects/edytornc</a>") +
                            trUtf8("<P>") +
                            trUtf8("<P>Cross platform installer made by <a href=\"http://installbuilder.bitrock.com/\">BitRock InstallBuilder for Qt</a>") +
                            trUtf8("<P>") +
                            trUtf8("<P>EdytorNC contains pieces of code from other Open Source projects.") +
                            trUtf8("<P>") +
                            trUtf8("<P><i>EdytorNC is free software; you can redistribute it and/or modify"
                                   "it under the terms of the GNU General Public License  as published by"
                                   "the Free Software Foundation; either version 2 of the License, or"
                                   "(at your option) any later version.</i>") +
                            trUtf8("<P><i>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                                   "INCLUDING THE WARRANTY OF DESIGN,"
                                   "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</i>"));
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateMenus()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   bool hasMdiChildNotReadOnly = (hasMdiChild && !activeMdiChild()->textEdit->isReadOnly());
   bool hasSelection = (hasMdiChild && activeMdiChild()->textEdit->textCursor().hasSelection());

   saveAct->setEnabled(hasMdiChild);
   saveAsAct->setEnabled(hasMdiChild);
   pasteAct->setEnabled(hasMdiChild);
   closeAct->setEnabled(hasMdiChild);
   closeAllAct->setEnabled(hasMdiChild);
   tileAct->setEnabled(hasMdiChild);
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
   renumberAct->setEnabled(hasMdiChildNotReadOnly);
   insertDotAct->setEnabled(hasMdiChildNotReadOnly);
   insertSpcAct->setEnabled(hasMdiChildNotReadOnly);
   removeSpcAct->setEnabled(hasMdiChildNotReadOnly);
   removeEmptyLinesAct->setEnabled(hasMdiChildNotReadOnly);
   insertEmptyLinesAct->setEnabled(hasMdiChildNotReadOnly);
   splittAct->setEnabled(hasMdiChildNotReadOnly);
   convertProgAct->setEnabled(hasMdiChildNotReadOnly);
   cmpMacroAct->setEnabled(hasMdiChildNotReadOnly);

   redoAct->setEnabled(hasMdiChild && activeMdiChild()->textEdit->document()->isRedoAvailable());
   undoAct->setEnabled(hasMdiChild && activeMdiChild()->textEdit->document()->isUndoAvailable());

   if(!hasMdiChildNotReadOnly)
   {
      readOnlyAct->setChecked(TRUE);
      readOnlyAct->setIcon(QIcon(":/images/lock.png"));
   }
   else
   {
      readOnlyAct->setChecked(FALSE);
      readOnlyAct->setIcon(QIcon(":/images/unlock.png"));
   };

   cutAct->setEnabled(hasSelection && hasMdiChildNotReadOnly);
   deleteAct->setEnabled(hasSelection && hasMdiChildNotReadOnly);
   copyAct->setEnabled(hasSelection);

   pasteAct->setEnabled((!clipboard->text().isEmpty()) && hasMdiChildNotReadOnly);

   if(hasMdiChild)
   {
      if(findToolBar != NULL)
        activeMdiChild()->highlightFindText(findEdit->text(),
                                           ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                           (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))), mCheckIgnoreComments->isChecked());
      else
        activeMdiChild()->highlightFindText("");
   };

   updateStatusBar();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateCurrentSerialConfig()
{
   int id;
   QDir dir;

   bool hasMdiChild = (activeMdiChild() != 0);
   if(hasMdiChild && (serialToolBar > NULL))
   {
      dir.setPath(activeMdiChild()->filePath());
      dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
      dir.setSorting(QDir::Name);
      dir.setNameFilters(QStringList("*.ini"));

      QFileInfoList list = dir.entryInfoList();

      if(!list.isEmpty())
      {
         QFileInfo name = list.at(0);
         id = configBox->findText(name.baseName());
         if(id >= 0)
            configBox->setCurrentIndex(id);
      };
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateStatusBar()
{
   QTextBlock b;
   int column = 1;
   int line = 1;
   int id;

   bool hasMdiChild = (activeMdiChild() != 0);
   bool hasMdiChildNotReadOnly = (hasMdiChild && !activeMdiChild()->textEdit->isReadOnly());

   if(hasMdiChild)
   {
      id = highlightTypeCombo->findData(activeMdiChild()->getHighligthMode());
      highlightTypeCombo->blockSignals(true);
      highlightTypeCombo->setCurrentIndex(id);
      highlightTypeCombo->blockSignals(false);

      b = activeMdiChild()->textEdit->textCursor().block();
      line = b.firstLineNumber() + 1;
      column = activeMdiChild()->textEdit->textCursor().position() - b.position();

      labelStat1->setText(tr(" Col: ") + QString::number(column + 1) +
                          tr("  Line: ") + QString::number(line) +
                          (activeMdiChild()->textEdit->document()->isModified() ? tr("  <b>Modified</b>  "): " ") +
                          (!hasMdiChildNotReadOnly ? tr(" Read only  "): " ") +
                          (activeMdiChild()->textEdit->overwriteMode() ? tr(" Overwrite  "): tr(" Insert ")));

   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateWindowMenu()
{
   QString text;

   windowMenu->clear();
   windowMenu->addAction(closeAct);
   windowMenu->addAction(closeAllAct);
   windowMenu->addSeparator();
   windowMenu->addAction(tileAct);
   windowMenu->addAction(cascadeAct);
   windowMenu->addSeparator();
   windowMenu->addAction(nextAct);
   windowMenu->addAction(previousAct);
   windowMenu->addAction(separatorAct);

   windowMenu->setAttribute(Qt::WA_AlwaysShowToolTips, true);

   QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
   separatorAct->setVisible(!windows.isEmpty());

   for(int i = 0; i < windows.size(); ++i)
   {
      MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

      if(i < 9)
      {
         text = tr("&%1 %2").arg(i + 1)
                .arg(child->currentFile());
      } else
      {
         text = tr("%1 %2").arg(i + 1)
                .arg(child->currentFile());
      }
      QAction *action = windowMenu->addAction(text);
      action->setCheckable(true);
      action->setChecked(child == activeMdiChild());
      action->setStatusTip(child->getCurrentFileInfo());
      connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
      windowMapper->setMapping(action, windows.at(i));
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

MdiChild *EdytorNc::createMdiChild()
{
    MdiChild *child = new MdiChild();
    mdiArea->addSubWindow(child);

    connect(child->textEdit, SIGNAL(copyAvailable(bool)), this, SLOT(updateMenus()));
    connect(child->textEdit, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));
    connect(child->textEdit, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));
    //connect(child->textEdit, SIGNAL(textChanged()), this, SLOT(updateMenus()));
    connect(child->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateMenus()));
    connect(child->textEdit, SIGNAL(modificationChanged(bool)), this, SLOT(updateStatusBar()));
    connect(child, SIGNAL(message(const QString&, int)), statusBar(), SLOT(message(const QString&, int)));

    //connect(child->textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    //connect(child->textEdit, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

    return child;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createActions()
{
    newAct = new QAction(QIcon(":/images/filenew.png"), tr("&New"), this);
    newAct->setShortcut(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    if(QDir(EXAMPLES_PATH).exists() || QDir(QApplication::applicationDirPath() + "/" + "EXAMPLES").exists())
    {
       openExampleAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open example..."), this);
       openExampleAct->setStatusTip(tr("Open an example file"));
       connect(openExampleAct, SIGNAL(triggered()), this, SLOT(openExample()));
    };

    saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save"), this);
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/filesaveas.png"), tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
    //exitAct->setShortcut(QKeySequence::Quit);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    
    findFilesAct = new QAction(QIcon(":/images/filefind.png"), tr("&Find files..."), this);
    //openAct->setShortcut(tr("Ctrl+"));
    findFilesAct->setCheckable(true);
    findFilesAct->setStatusTip(tr("Find files"));
    connect(findFilesAct, SIGNAL(triggered()), this, SLOT(findInFl()));

    printAct = new QAction(QIcon(":/images/document-print.png"), tr("&Print"), this);
    printAct->setShortcut(QKeySequence::Print);
    printAct->setStatusTip(tr("Print file"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(printFile()));

  

    
    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo last operation"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
    undoAct->setEnabled(FALSE);

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo last operation"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));
    redoAct->setEnabled(FALSE);

    cutAct = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));


    findAct = new QAction(QIcon(":/images/find.png"), tr("&Find"), this);
    findAct->setShortcut(QKeySequence::Find);
    findAct->setStatusTip(tr("Find text"));
    connect(findAct, SIGNAL(triggered()), this, SLOT(createFindToolBar()));

    replaceAct = new QAction(QIcon(":/images/replace.png"), tr("&Replace"), this);
    replaceAct->setShortcut(tr("Ctrl+R"));
    replaceAct->setStatusTip(tr("Find and replace text"));
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(createFindToolBar()));


    deleteAct = new QAction(QIcon(":/images/editdelete.png"), tr("&Delete"), this);
    deleteAct->setShortcut(QKeySequence::Delete);
    deleteAct->setStatusTip(tr("Removes selected text"));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteText()));
    
    selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"), this);
    selAllAct->setShortcut(QKeySequence::SelectAll);
    selAllAct->setStatusTip(tr("Select all text"));
    connect(selAllAct, SIGNAL(triggered()), this, SLOT(selAll()));

    readOnlyAct = new QAction(QIcon(":/images/unlock.png"), tr("Read &only"), this);
    readOnlyAct->setShortcut(tr("F12"));
    readOnlyAct->setCheckable(TRUE);
    readOnlyAct->setStatusTip(tr("Makes text read only"));
    connect(readOnlyAct, SIGNAL(triggered()), this, SLOT(readOnly()));

    configAct = new QAction(QIcon(":/images/configure.png"), tr("Configuration"), this);
    //configAct->setShortcut(tr("Ctrl+R"));
    configAct->setStatusTip(tr("Open configuration dialog"));
    connect(configAct, SIGNAL(triggered()), this, SLOT(config()));





    bhcAct = new QAction(QIcon(":/images/bhc.png"), tr("&Bolt hole circle"), this);
    bhcAct->setShortcut(tr("F8"));
    bhcAct->setStatusTip(tr("Calculate bolt hole's positions"));
    connect(bhcAct, SIGNAL(triggered()), this, SLOT(doBhc()));
    
    insertSpcAct = new QAction(QIcon(":/images/insertspc.png"), tr("&Insert spaces"), this);
    insertSpcAct->setShortcut(tr("F4"));
    insertSpcAct->setStatusTip(tr("Inserts spaces"));
    connect(insertSpcAct, SIGNAL(triggered()), this, SLOT(doInsertSpaces()));

    removeSpcAct = new QAction(QIcon(":/images/removespc.png"), tr("Remove spaces"), this);
    removeSpcAct->setShortcut(tr("F5"));
    removeSpcAct->setStatusTip(tr("Removes spaces"));
    connect(removeSpcAct, SIGNAL(triggered()), this, SLOT(doRemoveSpaces()));

    removeEmptyLinesAct = new QAction(QIcon(":/images/removeemptylines.png"), tr("Remove empty lines"), this);
    //removeEmptyLinesAct->setShortcut(tr("F5"));
    removeEmptyLinesAct->setStatusTip(tr("Removes empty lines"));
    connect(removeEmptyLinesAct, SIGNAL(triggered()), this, SLOT(doRemoveEmptyLines()));

    insertEmptyLinesAct = new QAction(QIcon(":/images/insertemptylines.png"), tr("Insert empty lines"), this);
    //insertEmptyLinesAct->setShortcut(tr("F5"));
    insertEmptyLinesAct->setStatusTip(tr("Insert empty lines"));
    connect(insertEmptyLinesAct, SIGNAL(triggered()), this, SLOT(doInsertEmptyLines()));

    insertDotAct = new QAction(QIcon(":/images/dots.png"), tr("Insert dots"), this);
    insertDotAct->setShortcut(tr("F6"));
    insertDotAct->setStatusTip(tr("Inserts decimal dot"));
    connect(insertDotAct, SIGNAL(triggered()), this, SLOT(doInsertDot()));

    renumberAct = new QAction(QIcon(":/images/renumber.png"), tr("Renumber"), this);
    renumberAct->setShortcut(tr("F7"));
    renumberAct->setStatusTip(tr("Renumber program blocks"));
    connect(renumberAct, SIGNAL(triggered()), this, SLOT(doRenumber()));

    speedFeedAct = new QAction(QIcon(":/images/vcf.png"), tr("Feed's speed's"), this);
    speedFeedAct->setShortcut(tr("F9"));
    speedFeedAct->setStatusTip(tr("Calculate speed, feed, cutting speed"));
    connect(speedFeedAct, SIGNAL(triggered()), this, SLOT(doSpeedFeed()));    

    chamferAct = new QAction(QIcon(":/images/chamfer.png"), tr("Chamfer"), this);
    //chamferAct->setShortcut(tr("F9"));
    chamferAct->setStatusTip(tr("Calculate chamfer"));
    connect(chamferAct, SIGNAL(triggered()), this, SLOT(doChamfer()));

    trianglesAct = new QAction(QIcon(":/images/triangles.png"), tr("Solution of triangles"), this);
    //trianglesAct->setShortcut(tr("F9"));
    trianglesAct->setStatusTip(tr("Solution of triangles"));
    connect(trianglesAct, SIGNAL(triggered()), this, SLOT(doTriangles()));

    convertAct = new QAction(QIcon(":/images/i2m.png"), tr("Convert inch <-> mm"), this);
    //convertAct->setShortcut(tr("F9"));
    convertAct->setStatusTip(tr("Convert inch <-> mm"));
    connect(convertAct, SIGNAL(triggered()), this, SLOT(doConvert()));

    convertProgAct = new QAction(QIcon(":/images/i2mprog.png"), tr("Convert program inch <-> mm"), this);
    //convertProgAct->setShortcut(tr("F9"));
    convertProgAct->setStatusTip(tr("Convert program inch <-> mm"));
    connect(convertProgAct, SIGNAL(triggered()), this, SLOT(doConvertProg()));

    calcAct = new QAction(QIcon(":/images/calc.png"), tr("Calculator"), this);
    //calcAct->setShortcut(tr("F9"));
    calcAct->setStatusTip(tr("Run calculator"));
    connect(calcAct, SIGNAL(triggered()), this, SLOT(doCalc()));

    showSerialToolBarAct = new QAction(QIcon(":/images/serial.png"), tr("Serial port send/receive"), this);
    //showSerialToolBarAct->setShortcut(tr("F9"));
    showSerialToolBarAct->setCheckable(true);
    showSerialToolBarAct->setStatusTip(tr("Serial port send/receive"));
    connect(showSerialToolBarAct, SIGNAL(triggered()), this, SLOT(createSerialToolBar()));

    cmpMacroAct = new QAction(QIcon(":/images/compfile.png"), tr("Compile macro - experimental"), this);
    //cmpMacroAct->setShortcut(tr("F9"));
    cmpMacroAct->setStatusTip(tr("Translate EdytorNC macro into CNC program"));
    connect(cmpMacroAct, SIGNAL(triggered()), this, SLOT(doCmpMacro()));

    diffRAct = new QAction(QIcon(":/images/diffr.png"), tr("Show diff - open current file in right diff window"), this);
    diffRAct->setStatusTip(tr("Show diff - open current file in right diff window"));
    connect(diffRAct, SIGNAL(triggered()), this, SLOT(doDiffR()));

    diffLAct = new QAction(QIcon(":/images/diffl.png"), tr("Show diff - open current file in left diff window"), this);
    diffLAct->setStatusTip(tr("Show diff - open current file in left diff window"));
    connect(diffLAct, SIGNAL(triggered()), this, SLOT(doDiffL()));

    diffAct = new QAction(QIcon(":/images/diff.png"), tr("On/off diff window"), this);
    diffAct->setCheckable(true);
    diffAct->setStatusTip(tr("Show diff window"));
    connect(diffAct, SIGNAL(triggered()), this, SLOT(doDiff()));

    diffEditorAct = new QAction(QIcon(":/images/diff_editor.png"), tr("Show unsaved changes"), this);
    diffEditorAct->setStatusTip(tr("Show diff of currently edited file and file on disk"));
    connect(diffEditorAct, SIGNAL(triggered()), this, SLOT(diffEditorFile()));

    splittAct = new QAction(QIcon(":/images/split_prog.png"), tr("Split file"), this);
    splittAct->setStatusTip(tr("Split file"));
    connect(splittAct, SIGNAL(triggered()), this, SLOT(splitPrograms()));



    closeAct = new QAction(QIcon(":/images/fileclose.png"), tr("Cl&ose"), this);
    //closeAct->setShortcut(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeCurrentWindow()));

    closeAllAct = new QAction(QIcon(":/images/window-close.png"), tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(QIcon(":/images/tile_h.png"), tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(QIcon(":/images/cascade.png"), tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(QIcon(":/images/go-next.png"), tr("Ne&xt"), this);
    nextAct->setShortcut(QKeySequence::Forward);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(QIcon(":/images/go-previous.png"), tr("Pre&vious"), this);
    previousAct->setShortcut(QKeySequence::Back);
    previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(activeWindowChanged(QMdiSubWindow *)));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);



    createGlobalToolTipsAct = new QAction(tr("&Create global cnc tooltips"), this);
    createGlobalToolTipsAct->setStatusTip(tr("Create default global cnc tooltips file"));
    connect(createGlobalToolTipsAct, SIGNAL(triggered()), this, SLOT(createGlobalToolTipsFile()));

    createUserToolTipsAct = new QAction(tr("&Create user cnc tooltips"), this);
    createUserToolTipsAct->setStatusTip(tr("Create/edit user cnc tooltips file"));
    connect(createUserToolTipsAct, SIGNAL(triggered()), this, SLOT(createUserToolTipsFile()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    if(openExampleAct != NULL)
       fileMenu->addAction(openExampleAct);
    fileMenu->addSeparator();
    recentFileMenu = fileMenu->addMenu(tr("&Recent files"));
    recentFileMenu->setIcon(QIcon(":/images/document-open-recent.png"));
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();    
    fileMenu->addAction(findFilesAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
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
    editMenu->addSeparator();
    editMenu->addAction(diffAct);
    editMenu->addSeparator();
    editMenu->addAction(diffLAct);
    editMenu->addAction(diffRAct);
    editMenu->addAction(diffEditorAct);
    editMenu->addSeparator();
    editMenu->addAction(readOnlyAct);
    editMenu->addSeparator();
    editMenu->addAction(configAct);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(showSerialToolBarAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(insertSpcAct);
    toolsMenu->addAction(removeSpcAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(insertEmptyLinesAct);
    toolsMenu->addAction(removeEmptyLinesAct);
    toolsMenu->addAction(splittAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(insertDotAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(renumberAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(bhcAct);
    toolsMenu->addAction(speedFeedAct);
    toolsMenu->addAction(trianglesAct);
    toolsMenu->addAction(chamferAct);
    toolsMenu->addAction(convertAct);
    toolsMenu->addAction(convertProgAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(cmpMacroAct);
    toolsMenu->addSeparator();
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

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("File");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
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
    editToolBar->addAction(diffRAct);
    editToolBar->addAction(diffLAct);
    editToolBar->addAction(diffEditorAct);


    toolsToolBar = new QToolBar(tr("Tools"));
    addToolBar(Qt::LeftToolBarArea, toolsToolBar);
    toolsToolBar->setObjectName("Tools");
    toolsToolBar->addAction(showSerialToolBarAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(insertSpcAct);
    toolsToolBar->addAction(removeSpcAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(removeEmptyLinesAct);
    toolsToolBar->addAction(splittAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(insertDotAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(renumberAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(bhcAct);
    toolsToolBar->addAction(speedFeedAct);
    toolsToolBar->addAction(trianglesAct);
    toolsToolBar->addAction(chamferAct);
    toolsToolBar->addAction(convertAct);
    toolsToolBar->addAction(convertProgAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(cmpMacroAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(calcAct);
    toolsToolBar->addSeparator();

    windowToolBar = addToolBar(tr("Window"));
    windowToolBar->setObjectName("Window");
    windowToolBar->addAction(previousAct);
    windowToolBar->addAction(nextAct);

}

//**************************************************************************************************
//
//**************************************************************************************************

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
   highlightTypeCombo->addItem(tr("TOOLTIPS"), MODE_TOOLTIPS);

   connect(highlightTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setHighLightMode(int)));

   attachHighlightToDirAct = new QAction(QIcon(":/images/attach.png"), tr("Attach current highlight setting to current directory of programs"), this);
   attachHighlightToDirAct->setStatusTip(tr("Attach current highlight setting to current directory of programs"));
   connect(attachHighlightToDirAct, SIGNAL(triggered()), this, SLOT(attachHighlightToDirActClicked()));

   attachHighlightButton = new QToolButton();
   attachHighlightButton->setDefaultAction(attachHighlightToDirAct);

   deAttachHighlightToDirAct = new QAction(QIcon(":/images/deattach.png"), tr("Remove highlight settings from the directory"), this);
   deAttachHighlightToDirAct->setStatusTip(tr("Remove highlight settings from the directory"));
   connect(deAttachHighlightToDirAct, SIGNAL(triggered()), this, SLOT(deAttachHighlightToDirActClicked()));

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

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::setHighLightMode(int mode)
{  
   bool ok;
   bool hasMdiChild = (activeMdiChild() != 0);

   int id = highlightTypeCombo->itemData(mode).toInt(&ok);
   if(hasMdiChild)
   {
      activeMdiChild()->setHighligthMode(id);
      activeMdiChild()->setFocus(Qt::MouseFocusReason);
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::readSettings()
{
    QSettings settings("EdytorNC", "EdytorNC");

    QPoint pos = settings.value("Pos", QPoint(0, 0)).toPoint();
    QSize size = settings.value("Size", QSize(800, 600)).toSize();
    move(pos);
    resize(size);


    if(settings.value("SerialToolbarShown", FALSE).toBool())
    {
       createSerialToolBar();
       showSerialToolBarAct->setChecked(true);
    };

    restoreState(settings.value("State", QByteArray()).toByteArray());

    lastDir = settings.value("LastDir", QString(getenv("HOME"))).toString();

    defaultMdiWindowProperites.extensions = settings.value("Extensions", "*.nc").toStringList();
    defaultMdiWindowProperites.saveExtension = settings.value("SaveExtension", ".nc").toString();
    defaultMdiWindowProperites.saveDirectory = settings.value("SaveDirectory", "").toString();

    defaultMdiWindowProperites.dotAdr = settings.value("DotAddress", "XYZB").toString();
    defaultMdiWindowProperites.dotAftrerCount = settings.value("DotAfterCount", 1000).toInt();
    defaultMdiWindowProperites.atEnd = settings.value("DotAtEnd", TRUE ).toBool();
    defaultMdiWindowProperites.dotAfter = settings.value("DotAfter", FALSE).toBool();

    defaultMdiWindowProperites.i2mAdr = settings.value("I2MAddress", "XYZ").toString();
    defaultMdiWindowProperites.i2mprec = settings.value("I2MPrec", 3).toInt();
    defaultMdiWindowProperites.inch = settings.value("I2M", TRUE).toBool();

    defaultMdiWindowProperites.fontName = settings.value("FontName", "Courier").toString();
    defaultMdiWindowProperites.fontSize = settings.value("FontSize", 12).toInt();
    defaultMdiWindowProperites.intCapsLock = settings.value("IntCapsLock", TRUE).toBool();
    defaultMdiWindowProperites.underlineChanges = settings.value("UnderlineChanges", TRUE).toBool();
    defaultMdiWindowProperites.windowMode = settings.value("WindowMode", 0x0E).toInt();
    defaultMdiWindowProperites.clearUndoHistory = settings.value("ClearUndoRedo", FALSE).toBool();
    defaultMdiWindowProperites.clearUnderlineHistory = settings.value("ClearUnderline", FALSE).toBool();
    defaultMdiWindowProperites.editorToolTips = settings.value("EditorToolTips", TRUE).toBool();
    defaultMdiWindowProperites.startEmpty = settings.value("StartEmpty", FALSE).toBool();

    defaultMdiWindowProperites.lineColor = settings.value("LineColor", 0xFEFFB6).toInt();
    defaultMdiWindowProperites.underlineColor = settings.value("UnderlineColor", 0x00FF00).toInt();

    defaultMdiWindowProperites.defaultReadOnly = settings.value("ViewerMode", FALSE).toBool();
    defaultMdiWindowProperites.defaultHighlightMode = settings.value("DefaultHighlightMode", MODE_AUTO).toInt();


    fileDialogState = settings.value("FileDialogState", QByteArray()).toByteArray();


#ifdef Q_OS_LINUX
   defaultMdiWindowProperites.calcBinary = "kcalc";
#endif

#ifdef Q_OS_WIN32
   defaultMdiWindowProperites.calcBinary = "calc.exe";
#endif

    defaultMdiWindowProperites.calcBinary = settings.value("CalcBinary", defaultMdiWindowProperites.calcBinary).toString();

    m_recentFiles = settings.value("RecentFiles").toStringList();
    updateRecentFilesMenu();

    defaultMdiWindowProperites.maximized = settings.value("MaximizedMdi", TRUE).toBool();

    settings.beginGroup("Highlight");
    defaultMdiWindowProperites.syntaxH = settings.value("HighlightOn", TRUE).toBool();

    defaultMdiWindowProperites.hColors.commentColor = settings.value("CommentColor", 0xde0020).toInt();
    defaultMdiWindowProperites.hColors.gColor = settings.value("GColor", 0x1600ee).toInt();
    defaultMdiWindowProperites.hColors.mColor = settings.value("MColor", 0x80007d).toInt();
    defaultMdiWindowProperites.hColors.nColor = settings.value("NColor", 0x808080).toInt();
    defaultMdiWindowProperites.hColors.lColor = settings.value("LColor", 0x535b5f).toInt();
    defaultMdiWindowProperites.hColors.fsColor = settings.value("FsColor", 0x516600).toInt();
    defaultMdiWindowProperites.hColors.dhtColor = settings.value("DhtColor", 0x660033).toInt();
    defaultMdiWindowProperites.hColors.rColor = settings.value("RColor", 0x24576f).toInt();
    defaultMdiWindowProperites.hColors.macroColor = settings.value("MacroColor", 0x000080).toInt();
    defaultMdiWindowProperites.hColors.keyWordColor = settings.value("KeyWordColor", 0x1d8000).toInt();
    defaultMdiWindowProperites.hColors.progNameColor = settings.value("ProgNameColor", 0x000000).toInt();
    defaultMdiWindowProperites.hColors.operatorColor = settings.value("OperatorColor", 0x9a2200).toInt();
    defaultMdiWindowProperites.hColors.zColor = settings.value("ZColor", 0x000080).toInt();
    defaultMdiWindowProperites.hColors.aColor = settings.value("AColor", 0x000000).toInt();
    defaultMdiWindowProperites.hColors.bColor = settings.value("BColor", 0x000000).toInt();
    settings.endGroup();

    if(!defaultMdiWindowProperites.startEmpty)
    {
       int max = settings.beginReadArray("LastDoc");
       for(int i = 0; i < max; ++i)
       {
          settings.setArrayIndex(i);
          defaultMdiWindowProperites.lastDir = lastDir.absolutePath();

          defaultMdiWindowProperites.fileName = settings.value("OpenedFile").toString();
          if(!defaultMdiWindowProperites.fileName.isEmpty())
          {
             defaultMdiWindowProperites.cursorPos = settings.value("Cursor", 1).toInt();
             defaultMdiWindowProperites.readOnly = settings.value("ReadOnly", FALSE).toBool();
             defaultMdiWindowProperites.geometry = settings.value("Geometry", QByteArray()).toByteArray();
             defaultMdiWindowProperites.hColors.highlightMode = settings.value("HighlightMode", MODE_AUTO).toInt();
             loadFile(defaultMdiWindowProperites, false);
          };
       };
       settings.endArray();
    };

    fileTreeView->header()->restoreState(settings.value("FileTreeViewState", QByteArray()).toByteArray());

    vSplitter->restoreState(settings.value("VSplitterState", QByteArray()).toByteArray());

    currentProjectName = settings.value("CurrentProjectName", "").toString();
    projectLoad(currentProjectName);

    panelState = settings.value("ProjectPanelState", QByteArray()).toByteArray();
    hSplitter->restoreState(panelState);
    panelHidden = settings.value("PanelHidden", false).toBool();
    if(panelHidden)
    {
       vSplitter->hide();
       frame->setMaximumWidth(hideButton->width());
       hideButton->setText(">>");
    };

    tabWidget->setCurrentIndex(settings.value("TabCurrentIndex", 0).toInt()); 
    currentPathCheckBox->setChecked(settings.value("FileBrowserShowCurrentFileDir", false).toBool());
    filePreviewSpinBox->setValue(settings.value("FilePreviewNo", 10).toInt());
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::writeSettings()
{
    MdiChild *mdiChild;
    bool maximized = false;
   
    QSettings settings("EdytorNC", "EdytorNC");


    settings.setValue("Pos", pos());
    settings.setValue("Size", size());


    settings.setValue("State", saveState());

    settings.setValue("LastDir", lastDir.path());
    
    settings.setValue("Extensions",defaultMdiWindowProperites.extensions);
    settings.setValue("SaveExtension",defaultMdiWindowProperites.saveExtension);
    settings.setValue("SaveDirectory",defaultMdiWindowProperites.saveDirectory);

    settings.setValue("DotAddress", defaultMdiWindowProperites.dotAdr);
    settings.setValue("DotAfterCount", defaultMdiWindowProperites.dotAftrerCount);
    settings.setValue("DotAtEnd", defaultMdiWindowProperites.atEnd);
    settings.setValue("DotAfter", defaultMdiWindowProperites.dotAfter);

    settings.setValue("I2MAddress", defaultMdiWindowProperites.i2mAdr);
    settings.setValue("I2MPrec", defaultMdiWindowProperites.i2mprec);
    settings.setValue("I2M", defaultMdiWindowProperites.inch);

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
    
    settings.setValue("FileDialogState", fileDialogState);
    settings.setValue("RecentFiles", m_recentFiles);

    settings.setValue("SerialToolbarShown", (serialToolBar != NULL));

    settings.setValue("CurrentProjectName", currentProjectName);

    settings.setValue("FileTreeViewState", fileTreeView->header()->saveState());
    settings.setValue("VSplitterState", vSplitter->saveState());
    settings.setValue("TabCurrentIndex", tabWidget->currentIndex());
    settings.setValue("FilePreviewNo", filePreviewSpinBox->value());

    if(panelHidden)
      settings.setValue("ProjectPanelState", panelState);
    else
       settings.setValue("ProjectPanelState", hSplitter->saveState());

    settings.setValue("PanelHidden", panelHidden);
    settings.setValue("FileBrowserShowCurrentFileDir", currentPathCheckBox->isChecked());


    settings.beginGroup("Highlight" );
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

    settings.endGroup();

    //cleanup old settings
    settings.remove("LastDoc");

    settings.beginWriteArray("LastDoc");
    int i = 0;
    foreach(const QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder))
    {
        mdiChild = qobject_cast<MdiChild *>(window->widget());
        _editor_properites Opt = mdiChild->getMdiWindowProperites();

        settings.setArrayIndex(i);
        settings.setValue("OpenedFile", Opt.fileName);
        settings.setValue("Cursor", Opt.cursorPos);
        settings.setValue("ReadOnly", Opt.readOnly);
        settings.setValue("Geometry", mdiChild->parentWidget()->saveGeometry());
        settings.setValue("HighlightMode", Opt.hColors.highlightMode);
        if(mdiChild->parentWidget()->isMaximized())
          maximized =  true;

        i++;
    };
    settings.endArray();

    settings.setValue("MaximizedMdi", maximized);
}

//**************************************************************************************************
//
//**************************************************************************************************

MdiChild *EdytorNc::activeMdiChild()
{
    if(QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
      return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

//**************************************************************************************************
//
//**************************************************************************************************

QMdiSubWindow *EdytorNc::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    if(canonicalFilePath.isEmpty())
       canonicalFilePath = fileName;

    foreach(QMdiSubWindow *window, mdiArea->subWindowList())
    {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if(mdiChild->currentFile() == QDir::toNativeSeparators(canonicalFilePath))
          return window;
    }
    return 0;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::setActiveSubWindow(QWidget *window)
{
    if(!window)
      return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::loadFile(_editor_properites options, bool checkAlreadyLoaded)
{
   QFileInfo file;

   if(checkAlreadyLoaded)
   {
      QMdiSubWindow *existing = findMdiChild(options.fileName);
      if(existing)
      {
         mdiArea->setActiveSubWindow(existing);
         return;
      };
   };

   file.setFile(options.fileName);
   if((file.exists()) && (file.isReadable()))
   {
      MdiChild *child = createMdiChild();
      child->newFile();
      child->loadFile(options.fileName);
      child->setMdiWindowProperites(options);
      child->parentWidget()->restoreGeometry(options.geometry);
      if(defaultMdiWindowProperites.maximized)
         child->showMaximized();
      else
         child->showNormal();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateRecentFiles(const QString &filename)
{
    m_recentFiles.prepend(filename);
    if(m_recentFiles.size() > MAX_RECENTFILES)
        m_recentFiles.removeLast();
    m_recentFiles.removeDuplicates();

    updateRecentFilesMenu();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::fileOpenRecent(QAction *act)
{
    defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
    defaultMdiWindowProperites.geometry = QByteArray();
    defaultMdiWindowProperites.cursorPos = 0;
    defaultMdiWindowProperites.editorToolTips = true;
    defaultMdiWindowProperites.fileName = m_recentFiles[act->data().toInt()];
    defaultMdiWindowProperites.hColors.highlightMode =  defaultHighlightMode(QFileInfo(defaultMdiWindowProperites.fileName).canonicalPath());
    loadFile(defaultMdiWindowProperites);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateRecentFilesMenu()
{
    QAction *newAc;
  
    recentFileMenu->clear();
    for(int i = 0; i < MAX_RECENTFILES; ++i)
    {
        if(i < int(m_recentFiles.size()))
        {
            newAc = recentFileMenu->addAction(QIcon(":/images/document-open-recent.png"),
                                              QString( "&%1 - %2" ).arg( i + 1 ).arg(QDir::toNativeSeparators(m_recentFiles[i])));
            connect(recentFileMenu, SIGNAL(triggered(QAction *)), this, SLOT(fileOpenRecent(QAction *)));
            newAc->setData(i);
        };
    }
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::loadFoundedFile(const QString &fileName)
{
    QFileInfo file;

    QMdiSubWindow *existing = findMdiChild(fileName);
    if(existing)
    {
       mdiArea->setActiveSubWindow(existing);
       return;
    };
    file.setFile(fileName);
    if((file.exists()) && (file.isReadable()))
    {
       MdiChild *child = createMdiChild();
       child->newFile();
       child->loadFile(fileName);
       updateRecentFiles(fileName);
       //defaultMdiWindowProperites.maximized = FALSE;
       defaultMdiWindowProperites.cursorPos = 0;
       defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
       defaultMdiWindowProperites.geometry = QByteArray();
       defaultMdiWindowProperites.hColors.highlightMode = defaultHighlightMode(child->filePath());
       defaultMdiWindowProperites.editorToolTips = true;
       child->setMdiWindowProperites(defaultMdiWindowProperites);
       if(defaultMdiWindowProperites.maximized)
         child->showMaximized();
       else
         child->showNormal();
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::messReceived(const QString &text)
{
    QString str = text;
    QStringList list1 = str.split(";", QString::SkipEmptyParts);
    for(int i = 0; i < list1.size(); ++i)
      openFile(list1.at(i));
    emit needToShow();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createFindToolBar()
{
   QString selText;
   QTextCursor cursor;

   if(findToolBar == NULL)
   {
      findToolBar = new QToolBar(tr("Find"));
      addToolBar(Qt::BottomToolBarArea, findToolBar);
      findToolBar->setObjectName("Find");

      findToolBar->setAttribute(Qt::WA_DeleteOnClose);

      findNextAct = new QAction(QIcon(":/images/arrow-right.png"), tr("Find next"), this);
      findNextAct->setShortcut(QKeySequence::FindNext);
      findNextAct->setStatusTip(tr("Find next"));
      connect(findNextAct, SIGNAL(triggered()), this, SLOT(findNext()));

      findPreviousAct = new QAction(QIcon(":/images/arrow-left.png"), tr("Find previous"), this);
      findPreviousAct->setShortcut(QKeySequence::FindPrevious);
      findPreviousAct->setStatusTip(tr("Find previous"));
      connect(findPreviousAct, SIGNAL(triggered()), this, SLOT(findPrevious()));

      replaceNextAct = new QAction(QIcon(":/images/arrow-right.png"), tr("Replace && find next"), this);
      //replaceNextAct->setShortcut(tr("F3"));
      replaceNextAct->setStatusTip(tr("Replace && find next"));
      connect(replaceNextAct, SIGNAL(triggered()), this, SLOT(replaceNext()));

      replacePreviousAct = new QAction(QIcon(":/images/arrow-left.png"), tr("Replace && find previous"), this);
      //replacePreviousAct->setShortcut(tr("F3"));
      replacePreviousAct->setStatusTip(tr("Replace && find previous"));
      connect(replacePreviousAct, SIGNAL(triggered()), this, SLOT(replacePrevious()));

      replaceAllAct = new QAction(QIcon(":/images/arrow-right-double.png"), tr("Replace all"), this);
      //replaceAllAct->setShortcut(tr("F3"));
      replaceAllAct->setStatusTip(tr("Replace all"));
      connect(replaceAllAct, SIGNAL(triggered()), this, SLOT(replaceAll()));

      findCloseAct = new QAction(QIcon(":/images/close_small.png"), tr("Close find toolbar"), this);
      findCloseAct->setStatusTip(tr("Close find toolbar"));
      connect(findCloseAct, SIGNAL(triggered()), this, SLOT(closeFindToolBar()));

      findLabel = new QLabel(tr("Find:"));
      findToolBar->addWidget(findLabel);
      findEdit = new QLineEdit();
      findEdit->setToolTip(tr("<b>Letter$$</b> - matches any number.<p><b>Letter$max$min</b> - matches number &lt;=max &gt;=min.</p>" \
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
      replaceEdit->setToolTip(tr("<b>$$OperatorNumber</b> - do some math on replaced numbers. Operator +-*/" \
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

      QSettings settings("EdytorNC", "EdytorNC");
      mCheckIgnoreComments->setChecked(settings.value("FindIgnoreComments", true).toBool());
      mCheckFindWholeWords->setChecked(settings.value("FindWholeWords", false).toBool());
      mCheckIgnoreCase->setChecked(settings.value("FindIgnoreCase", true).toBool());
   }
   else
     findToolBar->show();

   disconnect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));

   if(!activeMdiChild()->textEdit->textCursor().hasSelection())
   {
      cursor = activeMdiChild()->textEdit->textCursor();
      cursor.select(QTextCursor::WordUnderCursor);
      selText = cursor.selectedText();
      if((selText.size() > 32) || (selText.size() < 2))
        cursor.clearSelection();
      activeMdiChild()->textEdit->setTextCursor(cursor);
   };

   cursor = activeMdiChild()->textEdit->textCursor();

   if(cursor.hasSelection())
   {
      selText = cursor.selectedText();

      if((selText.size() < 32))
        findEdit->setText(selText);
      else
      {
         cursor.clearSelection();
         activeMdiChild()->textEdit->setTextCursor(cursor);
      };
   };

   findEdit->setPalette(QPalette());
   connect(findEdit, SIGNAL(textChanged(QString)), this, SLOT(findTextChanged()));
   findEdit->setFocus(Qt::MouseFocusReason);

   activeMdiChild()->highlightFindText(findEdit->text(),
                                      ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                      (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))), mCheckIgnoreComments->isChecked());

   findEdit->selectAll();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::closeFindToolBar()
{
   if(activeMdiChild())
   {
      activeMdiChild()->setFocus(Qt::MouseFocusReason);
      activeMdiChild()->highlightFindText("");
      activeMdiChild()->textEdit->centerCursor();
   };

   QSettings settings("EdytorNC", "EdytorNC");
   settings.setValue("FindIgnoreComments", mCheckIgnoreComments->isChecked());
   settings.setValue("FindWholeWords", mCheckFindWholeWords->isChecked());
   settings.setValue("FindIgnoreCase", mCheckIgnoreCase->isChecked());

   findToolBar->close();
   findToolBar = NULL;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::findTextChanged()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   QTextCursor cursor;
   int pos;


   if(findEdit->text().contains(QRegExp("\\$\\$")) || findEdit->text().contains(QRegExp("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}")))
   {
      replaceAllAct->setEnabled(false);
   }
   else
      replaceAllAct->setEnabled(true);


   if(hasMdiChild)
   {
      cursor = activeMdiChild()->textEdit->textCursor();
      if(!findEdit->text().isEmpty())
      {
         pos = cursor.position() - findEdit->text().size();
         if(pos < 0)
           pos = 0;
         do
         {
            cursor.movePosition(QTextCursor::Left);  //cursor.movePosition(QTextCursor::StartOfWord)
         }while((pos <= cursor.position()) && (cursor.position() > 0));

         activeMdiChild()->textEdit->setTextCursor(cursor);

         findNext();
      }
      else
      {
         findEdit->setPalette(QPalette());
         cursor.clearSelection();
         activeMdiChild()->textEdit->setTextCursor(cursor);
      };

   };
}

//**************************************************************************************************
//
//**************************************************************************************************

bool EdytorNc::eventFilter(QObject *obj, QEvent *ev)
{
   if((obj == findEdit) || (obj == replaceEdit))
   {
       if( ev->type() == QEvent::KeyPress )
       {
          QKeyEvent *k = (QKeyEvent*) ev;

          if(k->key() == Qt::Key_Comma) //Keypad comma should always prints period
          {
             if((k->modifiers() == Qt::KeypadModifier) || (k->nativeScanCode() == 0x53)) // !!! Qt::KeypadModifier - Not working for keypad comma !!!
             {
                QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, ".", FALSE, 1));
                return true;
             };

          };

          if(defaultMdiWindowProperites.intCapsLock)
          {
             if(k->text()[0].isLower() && (k->modifiers() == Qt::NoModifier))
             {
                QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::NoModifier, k->text().toUpper(), FALSE, 1));
                return true;

             };

             if(k->text()[0].isUpper() && (k->modifiers() == Qt::ShiftModifier))
             {
                QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::ShiftModifier, k->text().toLower(), FALSE, 1));
                return true;
             };
          };
       };

       return false;
   }
   else
   {
      // pass the event on to the parent class
      return eventFilter(obj, ev);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createSerialToolBar()
{
   if(serialToolBar == NULL)
   {
      serialToolBar = new QToolBar(tr("Serial port toolbar"));
      addToolBar(Qt::TopToolBarArea, serialToolBar);
      serialToolBar->setObjectName("SerialToolBar");


      configPortAct = new QAction(QIcon(":/images/serialconfig.png"), tr("Serial port configuration"), this);
      //configPortAct->setShortcut(tr("F3"));
      configPortAct->setStatusTip(tr("Serial port configuration"));
      connect(configPortAct, SIGNAL(triggered()), this, SLOT(serialConfig()));

      receiveAct = new QAction(QIcon(":/images/receive.png"), tr("Receive new file"), this);
      //receiveAct->setShortcut(tr("Shift+F3"));
      receiveAct->setStatusTip(tr("Receive new file"));
      connect(receiveAct, SIGNAL(triggered()), this, SLOT(receiveButtonClicked()));

      sendAct = new QAction(QIcon(":/images/send.png"), tr("Send current file"), this);
      //sendAct->setShortcut(tr("F3"));
      sendAct->setStatusTip(tr("Send current file"));
      connect(sendAct, SIGNAL(triggered()), this, SLOT(sendButtonClicked()));

      attachToDirAct = new QAction(QIcon(":/images/attach.png"), tr("Attach current port settings to current directory of programs"), this);
      //attachToDirAct->setShortcut(tr("F3"));
      attachToDirAct->setStatusTip(tr("Attach current port settings to current directory of programs"));
      connect(attachToDirAct, SIGNAL(triggered()), this, SLOT(attachToDirButtonClicked()));

      deAttachToDirAct = new QAction(QIcon(":/images/deattach.png"), tr("Remove settings from the directory"), this);
      //deAttachToDirAct->setShortcut(tr("F3"));
      deAttachToDirAct->setStatusTip(tr("Remove settings from the directory"));
      connect(deAttachToDirAct, SIGNAL(triggered()), this, SLOT(deAttachToDirButtonClicked()));

      diagAct = new QAction(QIcon(":/images/serialtest.png"), tr("Check serial port settings"), this);
      //diagAct->setShortcut(tr("F3"));
      diagAct->setStatusTip(tr("Check serial port settings"));
      connect(diagAct, SIGNAL(triggered()), this, SLOT(serialConfigTest()));

      serialCloseAct = new QAction(QIcon(":/images/close_small.png"), tr("Close send/receive toolbar"), this);
      serialCloseAct->setStatusTip(tr("Close find toolbar"));
      connect(serialCloseAct, SIGNAL(triggered()), this, SLOT(closeSerialToolbar()));


      configBox = new QComboBox();
      configBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
      configBox->setDuplicatesEnabled(false);


      //serialToolBar->addSeparator();
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
   }
   else
     if(!showSerialToolBarAct->isChecked())
     {
        closeSerialToolbar();
        return;
     }
     else
     {
        serialToolBar->show();
        showSerialToolBarAct->setChecked(TRUE);
     };

   //comPort = new QextSerialPort();
   stop = true;

   loadSerialConfignames();
   configBox->adjustSize();
   updateCurrentSerialConfig();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::closeSerialToolbar()
{
   stop = true;

   serialToolBar->close();
   delete(serialToolBar);
   serialToolBar = NULL;
   showSerialToolBarAct->setChecked(FALSE);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::attachToDirButtonClicked(bool attach)
{
   QFileInfo fileInfo;
   QFile file;
   int i;


   bool hasMdiChild = (activeMdiChild() != 0);
   if(hasMdiChild && (serialToolBar > NULL))
   {
      QDir dir;
      dir.setPath(activeMdiChild()->filePath());
      dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
      dir.setSorting(QDir::Name);
      dir.setNameFilters(QStringList("*.ini"));

      QFileInfoList list = dir.entryInfoList();

      if(!list.isEmpty())
      {
         for(i = 0; i < list.count(); i++)
         {
            fileInfo = (QFileInfo)list.at(i);
            file.setFileName(fileInfo.absoluteFilePath());
            file.remove();
         };
      };

      if(attach)
      {
         file.setFileName(activeMdiChild()->filePath() + "/" + configBox->currentText() + ".ini");
         file.open(QIODevice::ReadWrite);
         file.close();;
      };

   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::deAttachToDirButtonClicked()
{
   attachToDirButtonClicked(false);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::serialConfig()
{
   SPConfigDialog *serialConfigDialog = new SPConfigDialog(this, configBox->currentText());

   if(serialConfigDialog->exec() == QDialog::Accepted)
      loadSerialConfignames();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::loadSerialConfignames()
{
    int id;
    QStringList list;
    QString item;

    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");


    configBox->clear();
    list = settings.value("SettingsList", QStringList(tr("Default"))).toStringList();
    list.sort();
    configBox->addItems(list);
    item = settings.value("CurrentSerialPortSettings", tr("Default")).toString();
    id = configBox->findText(item);
    configBox->setCurrentIndex(id);



    settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::serialConfigTest()
{
   TransmissionDialog *trDialog = new TransmissionDialog(this);

   trDialog->show();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::loadConfig()
{
    QString port, fTx;
    int pos;
    QRegExp exp;
    char chr;
    bool ok;


    stop = true;
    QSettings settings("EdytorNC", "EdytorNC");

    settings.beginGroup("SerialPortConfigs");

#ifdef Q_OS_WIN32
       port = "COM1";
#else
       port = "/dev/ttyS0";
#endif

    settings.beginGroup(configBox->currentText());


    portName = settings.value("PortName", port).toString();
    portSettings.BaudRate = (BaudRateType) settings.value("BaudRate", BAUD9600).toInt();
    portSettings.DataBits = (DataBitsType) settings.value("DataBits", DATA_8).toInt();
    portSettings.StopBits = (StopBitsType) settings.value("StopBits", STOP_2).toInt();
    portSettings.Parity = (ParityType) settings.value("Parity", PAR_NONE).toInt();
    portSettings.FlowControl = (FlowType) settings.value("FlowControl", FLOW_HARDWARE).toInt();
    lineDelay = settings.value("LineDelay", 0).toDouble();
    portSettings.Xon = settings.value("Xon", "17").toString().toInt(&ok, 10);
    portSettings.Xoff = settings.value("Xoff", "19").toString().toInt(&ok, 10);
    sendAtEnd = settings.value("SendAtEnd", "").toString();
    sendAtBegining = settings.value("SendAtBegining", "").toString();
    deleteControlChars = settings.value("DeleteControlChars", true).toBool();
    removeEmptyLines = settings.value("RemoveEmptyLines", true).toBool();
    removeBefore = settings.value("RemoveBefore", false).toBool();

    sendStartDelay = settings.value("SendingStartDelay", 0).toInt();
    doNotShowProgressInEditor = settings.value("DoNotShowProgressInEditor", false).toBool();
    recieveTimeout = settings.value("RecieveTimeout", 0).toInt();


    settings.endGroup();
    settings.endGroup();

    portSettings.Timeout_Millisec = 50;

    exp.setPattern("0x[0-9a-fA-F]{1,2}");
    pos = 0;
    while((pos = sendAtBegining.indexOf(exp, pos)) >= 0)
    {
       fTx = sendAtBegining.mid(pos, exp.matchedLength());
       chr = fTx.toInt(&ok, 16);
       sendAtBegining.replace(pos, exp.matchedLength(), QString(chr));
    };
    sendAtBegining.remove(" ");

    pos = 0;
    while((pos = sendAtEnd.indexOf(exp, pos)) >= 0)
    {
       fTx = sendAtEnd.mid(pos, exp.matchedLength());
       chr = fTx.toInt(&ok, 16);
       sendAtEnd.replace(pos, exp.matchedLength(), QString(chr));
    };
    sendAtEnd.remove(" ");

    //comPort->setTimeout(0,100);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::lineDelaySlot()
{
   readyCont = true;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::sendButtonClicked()
{
   int i, bytesToWrite;
   QString tx;
   QTextCursor cursor, prevCursor;
   MdiChild *activeWindow;
   char controlChar;
   QTimer *sendStartDelayTimer = NULL;


   bytesToWrite = 0;

   activeWindow = activeMdiChild();
   if(activeWindow <= 0)
     return;

   loadConfig();

   comPort = new QextSerialPort(portName, portSettings);

   if(comPort->open(QIODevice::ReadWrite | QIODevice::Unbuffered | QIODevice::Truncate))
     stop = false;
   else
   {
      stop = true;
      showError(E_INVALID_FD);
      delete(comPort);
      return;
   };

   comPort->flush();
   comPort->reset();

   showError(E_NO_ERROR);
   receiveAct->setEnabled(FALSE);
   sendAct->setEnabled(FALSE);
   QApplication::setOverrideCursor(Qt::BusyCursor);

   cursor = activeWindow->textEdit->textCursor();
   prevCursor = cursor;
   cursor.movePosition(QTextCursor::Start);
   activeWindow->textEdit->setTextCursor(cursor);

   tx = sendAtBegining;
   if(removeBefore)
       tx.append(activeWindow->textEdit->toPlainText().mid(activeWindow->textEdit->toPlainText().indexOf("%")));
   else
       tx.append(activeWindow->textEdit->toPlainText());
   tx.append(sendAtEnd);
   if(!tx.contains("\r\n"))
      tx.replace("\n", "\r\n");

   TransProgressDialog progressDialog(this);
   progressDialog.setRange(0, tx.size());
   progressDialog.setModal(true);
   progressDialog.setWindowTitle(tr("Sending..."));
   progressDialog.open(comPort, (portSettings.FlowControl == FLOW_XONXOFF ? portSettings.Xon : 0), (portSettings.FlowControl == FLOW_XONXOFF ? portSettings.Xoff : 0));
   progressDialog.setLabelText(tr("Waiting..."));
   qApp->processEvents();

   if(portSettings.FlowControl == FLOW_HARDWARE)
   {
      ulong status = comPort->lineStatus();
      if(!(status & LS_CTS))
         xoffReceived = true;
   }
   else
      xoffReceived = false;


   if(sendStartDelay > 0 && portSettings.FlowControl != FLOW_HARDWARE)
   {
      sendStartDelayTimer = new QTimer(this);
      connect(sendStartDelayTimer, SIGNAL(timeout()), this, SLOT(sendStartDelayTimeout()));
      sendStartDelayTimer->setInterval(1000);
      xoffReceived = true;
      sendStartDelayTimer->start();
   };

   if((sendStartDelay == 0) && (portSettings.FlowControl == FLOW_XONXOFF))
   {
      xoffReceived = true;
   };

   i = 0;
   while(i < tx.size())
   {
      if(sendStartDelay > 0)
         progressDialog.setLabelText(tr("Start in %1s").arg(sendStartDelay));
      else
         if(xoffReceived)
            progressDialog.setLabelText(tr("Waiting for a signal readiness..."));

      qApp->processEvents();

      if(progressDialog.wasCanceled())
        break;

      if(stop)
        break;

      if(portSettings.FlowControl == FLOW_HARDWARE)
      {
         xoffReceived = !(comPort->lineStatus() & LS_CTS);
      }
      else
      {
         if(portSettings.FlowControl == FLOW_XONXOFF)
         {
            controlChar = 0;
            if(comPort->bytesAvailable() > 0)
            {
               comPort->getChar(&controlChar);
               //qDebug() << "Recived control char: " << QString("%1").arg((int)controlChar, 0, 16);
            };

            if(controlChar == portSettings.Xoff)
               xoffReceived = true;
            if(controlChar == portSettings.Xon)
            {
                sendStartDelay = 0;
                xoffReceived = false;
            };
         }
//         else
//            xoffReceived = false;
      };

      bytesToWrite = comPort->bytesToWrite();

#ifdef Q_OS_UNIX
      usleep(2000);
#endif

      if((bytesToWrite == 0) && (!xoffReceived))
      {
         if(!comPort->putChar(tx[i].toAscii()))
         {
//            qDebug() << comPort->lastError();
//            break;
         };
         if(!doNotShowProgressInEditor)
         {
            if(tx[i].toAscii() != '\r')
               cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            activeWindow->textEdit->setTextCursor(cursor);
         };
         progressDialog.setValue(i);
         progressDialog.setLabelText(tr("Sending byte %1 of %2").arg(i + 1).arg(tx.size()));
         //qApp->processEvents();

         if(lineDelay > 0)
         {
            if(tx[i].toAscii() == '\n')
            {
               readyCont = false;
               QTimer::singleShot(int(lineDelay * 1000), this, SLOT(lineDelaySlot()));
               while(!readyCont)
               {
                  qApp->processEvents();
               };
            };
         };

         i++;
      };
   };

   while(comPort->bytesToWrite() > 0)
   {
      qApp->processEvents();
      //qDebug() << "xoffReceived: " << xoffReceived << " bytes:" << bytesToWrite;
   };

   if(sendStartDelayTimer != NULL)
   {
      sendStartDelayTimer->stop();
      delete(sendStartDelayTimer);
   };

   comPort->flush();
   comPort->close();
   delete(comPort);
   progressDialog.close();
   activeWindow->textEdit->setTextCursor(prevCursor);
   receiveAct->setEnabled(TRUE);
   sendAct->setEnabled(TRUE);
   QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::sendStartDelayTimeout()
{
   if(sendStartDelay > 0)
      sendStartDelay--;
   else
      xoffReceived = false;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::receiveButtonClicked()
{
   QString tx;
   int count, i, j;
   char buf[1024];
   MdiChild *activeWindow;
   QTimer *recieveTimeoutTimer;


   showError(E_NO_ERROR);
   count = 0;
   recieveTimeoutTimer = NULL;

   loadConfig();
   comPort = new QextSerialPort(portName, portSettings);
   if(comPort->open(QIODevice::ReadWrite | QIODevice::Unbuffered | QIODevice::Truncate))
     stop = false;
   else
   {
      stop = true;
      showError(E_INVALID_FD);
      delete(comPort);
      return;
   };
   //comPort->flush();
   //comPort->reset();

   i = configBox->currentIndex();
   newFile();
   activeWindow = activeMdiChild();
   if(activeWindow <= 0)
     return;
   configBox->setCurrentIndex(i);

   receiveAct->setEnabled(FALSE);
   sendAct->setEnabled(FALSE);
   QApplication::setOverrideCursor(Qt::BusyCursor);

   TransProgressDialog progressDialog(this);

   if(recieveTimeout > 0)
   {
      recieveTimeoutTimer = new QTimer(this);
      connect(recieveTimeoutTimer, SIGNAL(timeout()), &progressDialog, SLOT(close()));
      recieveTimeoutTimer->setInterval(recieveTimeout * 1000);
      recieveTimeoutTimer->setSingleShot(true);
   };

   progressDialog.setRange(0, 0);
   progressDialog.setModal(true);
   progressDialog.setWindowTitle(tr("Receiving..."));
   progressDialog.setLabelText(tr("Waiting for data..."));
   progressDialog.open(comPort, (portSettings.FlowControl == FLOW_XONXOFF ? portSettings.Xon : 0), (portSettings.FlowControl == FLOW_XONXOFF ? portSettings.Xoff : 0));
   qApp->processEvents();

   if(portSettings.FlowControl == FLOW_XONXOFF)
   {
      comPort->putChar(portSettings.Xon);
   }
   else
      if(portSettings.FlowControl == FLOW_HARDWARE)
      {
         comPort->setRts(true);
         comPort->setDtr(true);
      };

   tx.clear();
   while(1)
   {
      //progressDialog.setValue(count);

#ifdef Q_OS_UNIX
      usleep(2000);
#endif

      i = comPort->bytesAvailable();
      if(i > 0)
      {
         //qDebug() << "Bytes available: " << i;
         i = comPort->readLine(buf, sizeof(buf) - 1);  //readLine

         if(recieveTimeoutTimer > NULL)
            recieveTimeoutTimer->start();

         qApp->processEvents();

         if(i < 0)
         {
            stop = true;
            if(portSettings.FlowControl == FLOW_XONXOFF)
            {
               comPort->putChar(portSettings.Xoff);
            };
            showError(comPort->lastError());
         };
         buf[i] = '\0';
         count += i;

         if(deleteControlChars)
            for(j = 0; j < i; j++)
            {
               if(((buf[j] > 0x1F) && (buf[j] < 0x7F)) || ((buf[j] == 0x0D)))
                  tx.append(buf[j]);
            }
         else
            for(j = 0; j < i; j++)
            {
               if(buf[j] != 0x0A)
                  tx.append(buf[j]);
            }

         progressDialog.setLabelText(tr("Reciving byte %1").arg(count));

         activeWindow->textEdit->insertPlainText(tx);
         tx.clear();

         if(!doNotShowProgressInEditor)
         {
            activeWindow->textEdit->ensureCursorVisible();
         };
      }
      else
         qApp->processEvents();

      if(stop)
      {
         if(!tx.isEmpty())
            activeWindow->textEdit->insertPlainText(tx);
         break;
      };

      progressDialog.setValue(count);
      //qApp->processEvents();

      if(progressDialog.wasCanceled())
      {
         stop = true;

         if(portSettings.FlowControl == FLOW_XONXOFF)
         {
            comPort->putChar(portSettings.Xoff);
         }
         else
            if(portSettings.FlowControl == FLOW_HARDWARE)
            {
               comPort->setRts(false);
               comPort->setDtr(false);
            };
      };
   };

   comPort->close();
   delete(comPort);
   progressDialog.close();
   receiveAct->setEnabled(TRUE);
   sendAct->setEnabled(TRUE);
   if(recieveTimeoutTimer > NULL)
      delete(recieveTimeoutTimer);


   if(activeWindow)
   {
     if(activeWindow->textEdit->document()->isEmpty())
     {
        activeWindow->parentWidget()->close();
     }
     else
     {
        if (removeEmptyLines)
            activeWindow->doRemoveEmptyLines();
        activeWindow->setHighligthMode(MODE_AUTO);
        if(defaultMdiWindowProperites.defaultReadOnly)
           activeWindow->textEdit->isReadOnly();
     };
   };
   QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::stopButtonClicked()
{
   stop = true;
   qApp->processEvents();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::showError(int error)
{
   QString text;
   QMessageBox msgBox;
   
   switch(error)
   {
      case E_INVALID_FD                   : text = tr("Invalid file descriptor (port was not opened correctly)");
                                            break;
      case E_NO_MEMORY                    : text = tr("Unable to allocate memory tables");
                                            break;
      case E_CAUGHT_NON_BLOCKED_SIGNAL    : text = tr("Caught a non-blocked signal");
                                            break;
      case E_PORT_TIMEOUT                 : text = tr("Operation timed out");
                                            break;
      case E_INVALID_DEVICE               : text = tr("The file opened by the port is not a character device");
                                            break;
      case E_BREAK_CONDITION              : text = tr("The port detected a break condition");
                                            break;
      case E_FRAMING_ERROR                : text = tr("The port detected a framing error (incorrect baud rate settings ?)");
                                            break;
      case E_IO_ERROR                     : text = tr("There was an I/O error while communicating with the port");
                                            break;
      case E_BUFFER_OVERRUN               : text = tr("Character buffer overrun");
                                            break;
      case E_RECEIVE_OVERFLOW             : text = tr("Receive buffer overflow");
                                            break;
      case E_RECEIVE_PARITY_ERROR         : text = tr("The port detected a parity error in the received data");
                                            break;
      case E_TRANSMIT_OVERFLOW            : text = tr("Transmit buffer overflow");
                                            break;
      case E_READ_FAILED                  : text = tr("General read operation failure");
                                            break;
      case E_WRITE_FAILED                 : text = tr("General write operation failure");
                                            break;
      case E_NO_ERROR                     : text = tr("No Error has occured");
                                            statusBar()->showMessage(text);
                                            return;
      default                             : text = tr("Unknown error");
   };

    
   statusBar()->showMessage(text);
   msgBox.setText(text);
   msgBox.setIcon(QMessageBox::Warning);
   msgBox.exec();

}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::doCmpMacro()
{
   if(activeMdiChild())
     activeMdiChild()->compileMacro();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createUserToolTipsFile()
{
   QString fileName;

   if(activeMdiChild())
     fileName = QFileInfo(activeMdiChild()->currentFile()).canonicalPath();
   else
     return;

   fileName += "/cnc_tips.txt";

   if(QFileInfo(fileName).exists())
      openFile(fileName);
   else
   {
         QFile file(fileName);
         if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
           return;

         qDebug() << fileName;

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
   };
   QMdiSubWindow *existing = findMdiChild(fileName);
   if(existing)
   {
      mdiArea->setActiveSubWindow(existing);
   };
};

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createGlobalToolTipsFile()
{
   QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "EdytorNC", "EdytorNC");
   QString config_dir = QFileInfo(cfg.fileName()).absolutePath() + "/";

   QString fileName = config_dir + "cnc_tips_" + QLocale::system().name() + ".txt";

   QSettings settings(fileName, QSettings::IniFormat);


 //******************************************************************//
   settings.beginGroup("OKUMA");
 //******************************************************************//
   settings.setValue("M00", tr("<b>M00</b> - program stop, unconditional"));
   settings.setValue("M01", tr("<b>M01</b> - optional program stop"));
   settings.setValue("M02", tr("<b>M02</b> - end of program"));
   settings.setValue("M03", tr("<b>M03</b> - start spindle CW"));
   settings.setValue("M04", tr("<b>M04</b> - start spindle CCW"));
   settings.setValue("M05", tr("<b>M05</b> - spindle stop"));
   settings.setValue("M06", tr("<b>M06</b> - tool change"));
   settings.setValue("M08", tr("<b>M08</b> - coolant on"));
   settings.setValue("M09", tr("<b>M09</b> - coolant off"));
   settings.setValue("M12", tr("<b>M12</b> - M-tool spindle STOP"));
   settings.setValue("M13", tr("<b>M13</b> - M-tool spindle CW"));
   settings.setValue("M14", tr("<b>M14</b> - M-tool spindle CCW"));
   settings.setValue("M15", tr("<b>M15</b> - B or C-axis positioning, plus direction CW"));
   settings.setValue("M16", tr("<b>M16</b> - B or C-axis positioning, minus direction CCW"));
   settings.setValue("M19", tr("<b>M19</b> - oriented spindle stop"));
   settings.setValue("M20", tr("<b>M20</b> - tailstock barrier OFF"));
   settings.setValue("M21", tr("<b>M21</b> - tailstock barrier ON"));
   settings.setValue("M22", tr("<b>M22</b> - chamfering OFF (for thread cutting cycle)"));
   settings.setValue("M23", tr("<b>M23</b> - chamfering ON (for thread cutting cycle)"));
   settings.setValue("M24", tr("<b>M24</b> - chuck barrier OFF"));
   settings.setValue("M25", tr("<b>M25</b> - chuck barrier ON"));
   settings.setValue("M26", tr("<b>M26</b> - thread lead along Z-axis"));
   settings.setValue("M27", tr("<b>M27</b> - thread lead along X-axis"));
   settings.setValue("M28", tr("<b>M28</b> - tool interference check function OFF"));
   settings.setValue("M29", tr("<b>M28</b> - tool interference check function ON"));
   settings.setValue("M30", tr("<b>M30</b> - end of program"));

   settings.setValue("M40", tr("<b>M40</b> - spindle gear range neutral"));
   settings.setValue("M41", tr("<b>M41</b> - spindle gear range 1"));
   settings.setValue("M42", tr("<b>M42</b> - spindle gear range 2"));
   settings.setValue("M43", tr("<b>M43</b> - spindle gear range 3"));
   settings.setValue("M44", tr("<b>M44</b> - spindle gear range 4"));
   settings.setValue("M48", tr("<b>M48</b> - spindle speed override ignore cancel"));
   settings.setValue("M49", tr("<b>M49</b> - spindle speed override ignore"));

   settings.setValue("M52", tr("<i>v.M</i> <b>M52</b> - mode of return to upper limit level") +
                            tr("<br /><i>v.L</i> <b>M52</b> - "));
   settings.setValue("M53", tr("<i>v.M</i> <b>M53</b> - mode of return to a specified point level set by G71") +
                            tr("<br /><i>v.L</i> <b>M53</b> - "));
   settings.setValue("M54", tr("<i>v.M</i> <b>M54</b> - mode of return to the point R level") +
                            tr("<br /><i>v.L</i> <b>M54</b> - "));

   settings.setValue("M55", tr("<b>M55</b> - tailstock spindle retract"));
   settings.setValue("M56", tr("<b>M56</b> - tailstock spindle advanced"));
   settings.setValue("M58", tr("<b>M58</b> - chucking pressure low"));
   settings.setValue("M59", tr("<b>M59</b> - chucking pressure high"));

   settings.setValue("M60", tr("<b>M60</b> - cancel of M61"));
   settings.setValue("M61", tr("<b>M61</b> - Ignoring fixed rpm arrival in constant speed cutting"));
   settings.setValue("M62", tr("<b>M62</b> - cancel of M64"));
   settings.setValue("M63", tr("<b>M63</b> - ignoring spindle rotation M code answer"));
   settings.setValue("M64", tr("<b>M64</b> - ignoring general M code answer"));
   settings.setValue("M65", tr("<b>M65</b> - ignoring T code answer"));
   settings.setValue("M66", tr("<b>M66</b> - turret indexing position free"));

   settings.setValue("M78", tr("<b>M78</b> - steady rest unclamp"));
   settings.setValue("M79", tr("<b>M79</b> - steady rest clamp"));
   settings.setValue("M83", tr("<b>M83</b> - chuck clamp"));
   settings.setValue("M84", tr("<b>M84</b> - chuck unclamp"));
   settings.setValue("M85", tr("<b>M85</b> - no return to the cutting starting point after the completion of rough turning cycle (LAP)"));
   settings.setValue("M88", tr("<b>M88</b> - air blower OFF"));
   settings.setValue("M89", tr("<b>M89</b> - air blower ON"));
   settings.setValue("M90", tr("<b>M90</b> - door/cover close"));
   settings.setValue("M91", tr("<b>M91</b> - door/cover open"));

   settings.setValue("M98", tr("<b>M98</b> - tailstock spindle thrust low"));
   settings.setValue("M99", tr("<b>M99</b> - tailstock spindle thrust high"));
   settings.setValue("M109", tr("<b>M109</b> - cancel of M110"));
   settings.setValue("M110", tr("<b>M110</b> - C-axis joint"));

   settings.setValue("M122", tr("<b>M122</b> - work rest retraction"));
   settings.setValue("M123", tr("<b>M123</b> - Cwork rest advance"));

   settings.setValue("M130", tr("<b>M130</b> - cutting feed; spindle rotating condition OFF"));
   settings.setValue("M131", tr("<b>M131</b> - cutting feed; spindle rotating condition ON"));
   settings.setValue("M132", tr("<b>M132</b> - single block ineffective"));
   settings.setValue("M133", tr("<b>M133</b> - single block effective"));
   settings.setValue("M136", tr("<b>M136</b> - feedrate override ineffective"));
   settings.setValue("M137", tr("<i>v.M</i> <b>M137</b> - feedrate override effective") +
                             tr("<br /><i>v.L</i> <b>M137</b> - touch setter interlock release ON"));
   settings.setValue("M138", tr("<i>v.M</i> <b>M138</b> - dry run ineffective") +
                             tr("<br /><i>v.L</i> <b>M138</b> - touch setter interlock release OFF"));
   settings.setValue("M139", tr("<b>M139</b> - dry run effective"));
   settings.setValue("M140", tr("<i>v.M</i> <b>M140</b> - slide hold ineffective") +
                             tr("<br /><i>v.L</i> <b>M140</b> - main motor overload monitoring OFF"));
   settings.setValue("M141", tr("<i>v.M</i> <b>M141</b> - slide hold effective") +
                             tr("<br /><i>v.L</i> <b>M141</b> - main motor overload monitoring ON"));
   settings.setValue("M142", tr("<b>M142</b> - coolant pressure low"));
   settings.setValue("M143", tr("<b>M143</b> - coolant pressure high"));
   settings.setValue("M144", tr("<b>M144</b> - additional coolant 1 OFF"));
   settings.setValue("M145", tr("<b>M145</b> - additional coolant 1 ON"));
   settings.setValue("M146", tr("<b>M146</b> - C-axis unclamp"));
   settings.setValue("M147", tr("<b>M147</b> - C-axis clamp"));
   settings.setValue("M152", tr("<b>M152</b> - M-tools spindle interlock ON"));
   settings.setValue("M153", tr("<b>M153</b> - M-tools spindle interlock OFF"));
   settings.setValue("M156", tr("<b>M156</b> - center work interlock OFF"));
   settings.setValue("M157", tr("<b>M157</b> - center work interlock ON"));
   settings.setValue("M161", tr("<b>M161</b> - feedrate override fix (100%)"));
   settings.setValue("M162", tr("<b>M162</b> - cancel of M163"));
   settings.setValue("M163", tr("<b>M163</b> - M-tools spindle speed override fix (100%)"));

   settings.setValue("M184", tr("<b>M184</b> - chuck internal interlock release OFF"));
   settings.setValue("M185", tr("<b>M185</b> - chuck internal interlock release ON"));
   settings.setValue("M186", tr("<b>M186</b> - work rest base unclamp"));
   settings.setValue("M187", tr("<b>M187</b> - work rest base clamp"));
   settings.setValue("M215", tr("<b>M215</b> - load monitor G00 ignore OFF"));
   settings.setValue("M216", tr("<b>M216</b> - load monitor G00 ignore ON"));

   settings.setValue("G00", tr("<b>G00</b> - rapid move - positioning"));
   settings.setValue("G01", tr("<b>G01</b> - linear interpolation"));
   settings.setValue("G02", tr("<b>G02 X Y Z [I J K</b> | <b>L]</b> - circular interpolation CW"));
   settings.setValue("G03", tr("<b>G03 X Y Z [I J K</b> | <b>L]</b> - circular interpolation CCW"));
   settings.setValue("G04", tr("<b>G04 F</b>xxxx - dwell xxxx seconds"));
   settings.setValue("G09", tr("<b>G09</b> - exact stop"));
   settings.setValue("G10", tr("<b>G10</b> - cancel of G11"));
   settings.setValue("G11", tr("<b>G11</b> - parallel and rotation shift of coordinate system"));

   settings.setValue("G13", tr("<b>G13</b> - turret selection: Turret A"));
   settings.setValue("G14", tr("<b>G14</b> - turret selection: Turret B"));
   settings.setValue("G15", tr("<b>G15 H</b>xx - selection of work coordinate system no. xx, modal"));
   settings.setValue("G16", tr("<b>G15 H</b>xx - selection of work coordinate system no. xx, one-shot"));

   settings.setValue("G17", tr("<b>G17</b> - XY plane"));
   settings.setValue("G18", tr("<b>G18</b> - ZX plane"));
   settings.setValue("G19", tr("<b>G19</b> - YZ plane"));
   settings.setValue("G20", tr("<b>G20</b> - inch input confirmation"));
   settings.setValue("G21", tr("<b>G21</b> - metric input confirmation"));

   settings.setValue("G40", tr("<b>G40</b> - tool nose/cutter radius compensation cancel"));
   settings.setValue("G41", tr("<b>G41</b> - tool nose/cutter radius compensation left"));
   settings.setValue("G42", tr("<b>G42</b> - tool nose/cutter radius compensation right"));
   settings.setValue("G50", tr("<i>v.L</i> <b>G50 S</b>xxxx - maximum spindle speed") +
                            tr("<br /><i>v.L</i> <b>G50 X Z</b> - zero point shift"));

   settings.setValue("G53", tr("<i>v.M</i> <b>G53</b> - cancel tool length offset") +
                            tr("<br /><i>v.L</i> <b>G53</b> - "));
   settings.setValue("G54", tr("<i>v.M</i> <b>G54 H</b>xx - tool length offset X-axis, xx - offset no.") +
                            tr("<br /><i>v.L</i> <b>G54</b> - "));
   settings.setValue("G55", tr("<i>v.M</i> <b>G55 H</b>xx - tool length offset Y-axis, xx - offset no.") +
                            tr("<br /><i>v.L</i> <b>G55</b> - "));
   settings.setValue("G56", tr("<i>v.M</i> <b>G56 H</b>xx - tool length offset Z-axis, xx - offset no.") +
                            tr("<br /><i>v.L</i> <b>G56</b> - "));
   settings.setValue("G57", tr("<i>v.M</i> <b>G57 H</b>xx - tool length offset 4-axis, xx - offset no.") +
                            tr("<br /><i>v.L</i> <b>G57</b> - "));
   settings.setValue("G58", tr("<i>v.M</i> <b>G58 H</b>xx - tool length offset 5-axis, xx - offset no.") +
                            tr("<br /><i>v.L</i> <b>G58</b> - "));
   settings.setValue("G59", tr("<i>v.M</i> <b>G59 H</b>xx - tool length offset 6-axis, xx - offset no.") +
                            tr("<br /><i>v.L</i> <b>G59</b> - "));

   settings.setValue("G61", tr("<b>G61</b> - exact stop mode"));
   settings.setValue("G62", tr("<b>G62</b> - programmable mirror image function"));
   settings.setValue("G17", tr("<b>G17</b> - cutting mode"));


   settings.setValue("G71", tr("<i>v.M</i> <b>G71 Z</b>xx - return level xx command") +
                            tr("<br /><i>v.L</i> <b>G71</b> - "));
   settings.setValue("G73", tr("<i>v.M</i> <b>G73</b> - high speed deep hole drilling") +
                            tr("<br /><i>v.L</i> <b>G73</b> - "));
   settings.setValue("G74", tr("<i>v.M</i> <b>G74</b> - reverse tapping") +
                            tr("<br /><i>v.L</i> <b>G74</b> - "));
   settings.setValue("G76", tr("<i>v.M</i> <b>G76</b> - fine boring") +
                            tr("<br /><i>v.L</i> <b>G76</b> - "));
   settings.setValue("G75", tr("<i>v.L</i> <b>G75 G01 [X</b> | <b>Z] L</b>xxxx - chamfering 45deg. xxxx - direction and size"));
   settings.setValue("G76", tr("<i>v.L</i> <b>G76 G01 [X</b> | <b>Z] L</b>xxxx - rounding. xxxx - direction and size"));

   settings.setValue("G80", tr("<i>v.M</i> <b>G80</b> - fixed cycle mode cancel") +
                            tr("<br /><i>v.L</i> <b>G80</b> - end of shape designation (LAP)"));

   settings.setValue("G81", tr("<i>v.M</i> <b>G81 R X Y Z</b> - drilling cycle") +
                            tr("<br /><i>v.L</i> <b>G81</b> - start of longitudinal shape designation (LAP)"));

   settings.setValue("G82", tr("<i>v.M</i> <b>G82 R X Y Z</b> - counter bore cycle") +
                            tr("<br /><i>v.L</i> <b>G82</b> - start of transverse shape designation (LAP)"));

   settings.setValue("G83", tr("<i>v.M</i> <b>G83 R X Y Z</b> - deep hole drilling cycle") +
                            tr("<br /><i>v.L</i> <b>G83</b> - start of blank material shape definition (LAP)"));

   settings.setValue("G84", tr("<i>v.M</i> <b>G84 R X Y Z</b> - tapping cycle") +
                            tr("<br /><i>v.L</i> <b>G84</b> -   change of cutting conditions in bar turning cycle (LAP)"));

   settings.setValue("G85", tr("<i>v.M</i> <b>G85 R X Y Z</b> - boring cycle") +
                            tr("<br /><i>v.L</i> <b>G85</b> - call of rough bar turning cycle (LAP)"));

   settings.setValue("G86", tr("<i>v.M</i> <b>G86 R X Y Z</b> - boring cycle") +
                            tr("<br /><i>v.L</i> <b>G86</b> - call of rough copy turning cycle (LAP)"));

   settings.setValue("G87", tr("<i>v.M</i> <b>G87 R X Y Z</b> - back boring cycle") +
                            tr("<br /><i>v.L</i> <b>G87</b> - call finish turning cycle (LAP)"));

   settings.setValue("G88", tr("<i>v.M</i> <b>G88 R X Y Z</b> - drilling cycle") +
                            tr("<br /><i>v.L</i> <b>G88</b> - call of continuous thread cutting cycle (LAP)"));

   settings.setValue("G89", tr("<i>v.M</i> <b>G89 R X Y Z</b> - boring cycle") +
                            tr("<br /><i>v.L</i> <b>G89</b> - "));


   settings.setValue("G90", tr("<b>G90</b> - absolute programming"));
   settings.setValue("G91", tr("<b>G91</b> - incremental programming"));
   settings.setValue("G92", tr("<b>G92</b> - setting of work coordinate system"));
   settings.setValue("G94", tr("<b>G94</b> - feed per minute"));
   settings.setValue("G95", tr("<b>G95</b> - feed per revolution"));
   settings.setValue("G96", tr("<b>G96 S</b>xx - constant cutting speed xx"));
   settings.setValue("G97", tr("<b>G97 S</b>xx - constant spindle speed xx"));

   settings.setValue("NCYL", tr("<b>NCYL</b> - if specified in fixed cycle, positioning to the definied hole position is performed, but the cycle axis does not operate"));
   settings.setValue("NOEX", tr("<b>NOEX</b> - if specified in fixed cycle, no axis movements may be performed"));

   settings.setValue("SIN", tr("<b>SIN[</b>angle<b>]</b> - sine"));
   settings.setValue("COS", tr("<b>COS[</b>angle<b>]</b> - cosine"));
   settings.setValue("TAN", tr("<b>TAN[</b>angle<b>]</b> - tangent"));
   settings.setValue("ATAN", tr("<b>ATAN[</b>angle<b>]</b> - arctangent 1 or 2"));
   settings.setValue("SQRT", tr("<b>SQRT[</b>val<b>]</b> - square root"));
   settings.setValue("ABS", tr("<b>ABS[</b>val<b>]</b> - absolute value"));
   settings.setValue("BIN", tr("<b>BIN[</b>val<b>]</b> - decimal to binary conversion"));
   settings.setValue("BCD", tr("<b>BCD[</b>val<b>]</b> - binary to decimal conversion"));
   settings.setValue("ROUND", tr("<b>ROUND[</b>val<b>]</b> - integer implementation (rounding)"));
   settings.setValue("FIX", tr("<b>FIX[</b>val<b>]</b> - integer implementation (truncation)"));
   settings.setValue("FUP", tr("<b>FUP[</b>val<b>]</b> - integer implementation (raising)"));
   settings.setValue("DROUND", tr("<b>DROUND[</b>val<b>]</b> - unit integer implementation (rounding)"));
   settings.setValue("DFIX", tr("<b>DFIX[</b>val<b>]</b> - unit integer implementation (truncation)"));
   settings.setValue("DFUP", tr("<b>DFUP[</b>val<b>]</b> - unit integer implementation (raising)"));
   settings.setValue("MOD", tr("<b>MOD[</b>val<b>,</b>yy<b>]</b> - remainder of val/yy"));

   settings.setValue("VDIN", tr("<b>VDIN[</b>xx<b>]</b> - imput variable no. xx"));
   settings.setValue("VDOUT", tr("<b>VDOUT[</b>xx<b>]</b> - output variable no. xx"));
   settings.setValue("VUACM", tr("<b>VUACM[</b>n<b>]='</b>text<b>'</b> - sub message for user definied alarms, n - subscript expression, text - max. 16 chracters"));

   settings.setValue("MODIN", tr("<b>MODIN O</b>nnnn [<b>Q</b>]xx - subprogram call after axis movement, nnnn - prog. name, xx - number of repetitions"));
   settings.setValue("MODOUT", tr("<b>MODOUT</b> - cancels last MODIN command"));

   settings.setValue("OMIT", tr("<b>OMIT</b> - coordinate calculation function, omit"));
   settings.setValue("RSTRT", tr("<b>RSTRT</b> - coordinate calculation function, restart"));

   settings.setValue("LAA", tr("<b>LAA</b> - line at angle"));
   settings.setValue("ARC", tr("<b>ARC</b> - arc"));
   settings.setValue("GRDX", tr("<b>GRDX</b> - grid X"));
   settings.setValue("GRDY", tr("<b>GRDY</b> - grid Y"));
   settings.setValue("DGRDX", tr("<b>DGRDX</b> - double grid X"));
   settings.setValue("DGRDY", tr("<b>DGRDY</b> - double grid Y"));
   settings.setValue("SQRX", tr("<b>SQRX</b> - square X"));
   settings.setValue("SQRY", tr("<b>SQRY</b> - square Y"));
   settings.setValue("BHC", tr("<b>BHC X Y I J K </b> - bolt hole circle, X Y - circle center, I - radius, J - angle of first hole, K - no. of holes"));

   settings.setValue("EQ", tr("<b>EQ</b> - equal to"));
   settings.setValue("NE", tr("<b>NE</b> - not equal to"));
   settings.setValue("GT", tr("<b>GT</b> - greather than"));
   settings.setValue("LE", tr("<b>LE</b> - less than or equal to"));
   settings.setValue("LT", tr("<b>LT</b> - less than"));
   settings.setValue("GE", tr("<b>GE</b> - greather than or equal to"));
   settings.setValue("IF", tr("<b>IF[</b>condition<b>] N</b>xxxx - if condition is true goto block xxxx"));

   settings.setValue("TLFON", tr("<b>TLFON</b> - tool life on"));
   settings.setValue("EMPTY", tr("<b>EMPTY</b> - 'empty' value "));

   settings.setValue("SB", tr("<b>SB=</b>xxxx - M-tool spindle speed xxxx"));
   settings.setValue("TG", tr("<b>TG=</b>xx - tool group no. xx"));
   settings.setValue("OG", tr("<b>OG=</b>xx - offset group no. xx"));
   settings.setValue("GOTO", tr("<b>GOTO N</b>nnnn - jump to nnnn"));
   settings.setValue("CALL", tr("<b>CALL O</b>nnnn [<b>Q</b>xx] - call subprogram nnnn, repeat it xx times"));
   settings.setValue("RTS", tr("<b>RTS</b> - subprogram end"));
   settings.setValue("VLMON", tr("<b>VLMON[</b>xx<b>]=</b>yy - load monitor"));


   settings.setValue("VATOL", tr("<b>VATOL</b> - active tool number, tool kind + tool number"));
   settings.setValue("VNTOL", tr("<b>VNTOL</b> - next tool number, tool kind + tool number"));
   settings.setValue("VMLOK", tr("<b>VMLOK</b> - equal zero if not in machine lock status"));
   settings.setValue("VPPCP", tr("<b>VPPCP</b> - PPC parameter"));
   settings.setValue("VPLNO", tr("<b>VPLNO</b> - PPC pallet number"));
   settings.setValue("VPLDT", tr("<b>VPLDT</b> - PPC parameter bit data"));
   settings.setValue("VTLCN", tr("<b>VTLCN</b> - active tool number"));
   settings.setValue("VTLNN", tr("<b>VTLNN</b> - next tool number"));


   settings.setValue("VMCOD", tr("<b>VMCOD[</b>xx<b>]</b> - present M code of group no. xx"));
   settings.setValue("VSCOD", tr("<b>VSCOD</b> - command value of present spindle speed S"));
   settings.setValue("VDCOD", tr("<b>VDCOD</b> - present cutter radius compensation offset number"));
   settings.setValue("VFCOD", tr("<b>VFCOD</b> - command value of present feedrate F"));
   settings.setValue("VGCOD", tr("<b>VGCOD[</b>xx<b>]</b> - present G code of group no. xx"));
   settings.setValue("VHCOD", tr("<b>VHCOD</b> - present tool length offset number"));
   settings.setValue("VACOD", tr("<b>VACOD</b> - coordinate system number"));

   settings.setValue("VZOFX", tr("<i>v.M</i> <b>VZOFX[</b>xx<b>]</b> - zero offset no. xx of X-axis") +
                              tr("<br /><i>v.L</i> <b>VZOFX</b> - zero offset of X-axis"));
   settings.setValue("VZOFZ", tr("<i>v.M</i> <b>VZOFZ[</b>xx<b>]</b> - zero offset no. xx of Z-axis") +
                              tr("<br /><i>v.L</i> <b>VZOFZ</b> - zero offset of Z-axis"));
   settings.setValue("VZOFY", tr("<i>v.M</i> <b>VZOFY[</b>xx<b>]</b> - zero offset no. xx of Y-axis"));
   settings.setValue("VZOFW", tr("<i>v.L</i> <b>VZOFW</b> - zero offset of W-axis"));
   settings.setValue("VZOFC", tr("<i>v.L</i> <b>VZOFC</b> - zero offset of C-axis"));
   settings.setValue("VZSHZ", tr("<i>v.L</i> <b>VZSHZ</b> - zero shift of Z-axis"));
   settings.setValue("VZSHX", tr("<i>v.L</i> <b>VZSHX</b> - zero shift of X-axis"));
   settings.setValue("VZSHC", tr("<i>v.L</i> <b>VZSHC</b> - zero shift of C-axis"));
   settings.setValue("VTOFZ", tr("<i>v.L</i> <b>VTOFZ[</b>xx<b>]</b> - tool offset no. xx of Z-axis"));
   settings.setValue("VTOFX", tr("<i>v.L</i> <b>VTOFX[</b>xx<b>]</b> - tool offset no. xx of X-axis"));
   settings.setValue("VRSTT", tr("<b>VRSTT</b> - equal zero if not in restart state"));
   settings.setValue("VTOFH", tr("<i>v.M</i> <b>VTOFH[</b>xx<b>]</b> - tool length, offset no. xx"));
   settings.setValue("VTOFD", tr("<i>v.M</i> <b>VTOFD[</b>xx<b>]</b> - cutter radius, offset no. xx"));
   settings.setValue("VNCOM", tr("<b>VNCOM[</b>xx<b>]</b> - communication for automation, no. xx"));




   settings.endGroup();

//******************************************************************//
   settings.beginGroup("FANUC");
//******************************************************************//
   settings.setValue("M00", tr("<b>M00</b> - program stop, unconditional"));
   settings.setValue("M01", tr("<b>M01</b> - optional program stop"));
   settings.setValue("M03", tr("<b>M03</b> - start spindle CW"));
   settings.setValue("M04", tr("<b>M04</b> - start spindle CCW"));
   settings.setValue("M05", tr("<b>M05</b> - spindle stop"));
   settings.setValue("M06", tr("<b>M06</b> - tool change"));
   settings.setValue("M08", tr("<b>M08</b> - coolant on"));
   settings.setValue("M09", tr("<b>M09</b> - coolant off"));
   settings.setValue("M30", tr("<b>M30</b> - end of program"));
   settings.setValue("M98", tr("<b>M98 P</b>xxxx - macro xxxx call"));
   settings.setValue("M99", tr("<b>M99</b> - subprogram end"));

   settings.setValue("G00", tr("<b>G00</b> - rapid move - positioning"));
   settings.setValue("G01", tr("<b>G01</b> - linear interpolation"));
   settings.setValue("G02", tr("<b>G02 {X Y Z</b> | <b>U V W} {I J K</b> | <b>R} F</b> - circular interpolation CW; XYZ - end point (absolute); UW - end point (incremental); IJK - distance from start point to center; R - radius of arc"));
   settings.setValue("G03", tr("<b>G03 {X Y Z</b> | <b>U V W} {I J K</b> | <b>R} F</b> - circular interpolation CCW; XYZ - end point (absolute); UW - end point (incremental); IJK - distance from start point to center; R - radius of arc"));
   settings.setValue("G04", tr("<b>G04 X U P</b> - dwell XU - in seconds; P - in microseconds"));

   settings.setValue("G10", tr("<b>G10 P X Z Y R Q</b> - change of offset value by program"));

   settings.setValue("G17", tr("<b>G17</b> - XY plane"));
   settings.setValue("G18", tr("<b>G18</b> - ZX plane"));
   settings.setValue("G19", tr("<b>G19</b> - YZ plane"));

   settings.setValue("G20", tr("<b>G20</b> - inch input"));
   settings.setValue("G21", tr("<b>G21</b> - metric input"));
   settings.setValue("G27", tr("<b>G27</b> - reference point return check"));
   settings.setValue("G28", tr("<b>G28</b> - reference point return"));
   settings.setValue("G30", tr("<b>G30 P</b>x - x = 2nd, 3rd, 4th reference point return"));
   settings.setValue("G31", tr("<b>G31</b> - skip function"));

   settings.setValue("G40", tr("<b>G40</b> - tool nose/cutter radius compensation cancel"));
   settings.setValue("G41", tr("<b>G41</b> - tool nose/cutter radius compensation left"));
   settings.setValue("G42", tr("<b>G42</b> - tool nose/cutter radius compensation right"));

   settings.setValue("G50", tr("<i>v.T</i> <b>G50 S</b>xxxx - maximum spindle speed") +
                            tr("<br /><b>G50 X Z</b> - zero point shift"));
   settings.setValue("G53", tr("<b>G53</b> - machine coordinate system"));
   settings.setValue("G54", tr("<b>G54</b> - settable zero offset 1"));
   settings.setValue("G55", tr("<b>G55</b> - settable zero offset 2"));
   settings.setValue("G56", tr("<b>G56</b> - settable zero offset 3"));
   settings.setValue("G57", tr("<b>G57</b> - settable zero offset 4"));
   settings.setValue("G58", tr("<b>G58</b> - settable zero offset 5"));
   settings.setValue("G59", tr("<b>G59</b> - settable zero offset 6"));

   settings.setValue("G65", tr("<b>G65 P</b>xxxx - macro xxxx call"));
   settings.setValue("G66", tr("<b>G66 P</b>xxxx - macro xxxx modal call"));
   settings.setValue("G67", tr("<b>G67</b> - macro modal call cancel"));

   settings.setValue("G70", tr("<i>v.T</i> <b>G70 P Q</b> - finishing cycle; P - sequence number of the first block of finishing shape, Q - sequence number of last block"));
   settings.setValue("G71", tr("<i>v.T</i> <b>G71 U R</b> - U - depth of cut, Q - escaping amount") +
                            tr("<br /><i>v.T</i> <b>G71 P Q U W</b> - stock removal in turning; P - first sequence number of finishing shape, Q - last sequence number of finishing shape, U - X finishing allowance, W - Z finishing allowance"));
   settings.setValue("G72", tr("<i>v.T</i> <b>G72 W R</b> - U - depth of cut, Q - escaping amount") +
                            tr("<br /><i>v.T</i> <b>G72 P Q U W</b> - stock removal in facing; P - first sequence number of finishing shape, Q - last sequence number of finishing shape, U - X finishing allowance, W - Z finishing allowance"));
   settings.setValue("G73", tr("<i>v.T</i> <b>G73 W R</b>") +
                            tr("<br /><i>v.T</i> <b>G73 P Q U W</b> - pattern repeating"));
   settings.setValue("G74", tr("<i>v.T</i> <b>G74 R</b>") +
                            tr("<br /><i>v.T</i> <b>G74 {X Z U W} P Q R</b> - end face peck drilling cycle"));
   settings.setValue("G75", tr("<i>v.T</i> <b>G75 R</b>") +
                            tr("<br /><i>v.T</i> <b>G75 {X Z U W} P Q U W</b> - outer/internal diameter drilling cycle"));
   settings.setValue("G76", tr("<i>v.T</i> <b>G76 P Q R</b>") +
                            tr("<br /><i>v.T</i> <b>G76 {X Z U W} P Q U W</b> - multiple thread cutting cycle"));

   settings.setValue("G90", tr("<b>G90</b> - absolute programming"));
   settings.setValue("G91", tr("<b>G91</b> - incremental programming"));
   settings.setValue("G92", tr("<b>G92 X Z</b> - coordinate system setting"));
   settings.setValue("G94", tr("<b>G94</b> - feed per minute"));
   settings.setValue("G95", tr("<b>G95</b> - feed per revolution"));
   settings.setValue("G96", tr("<b>G96 S</b>xx - constant cutting speed xx"));
   settings.setValue("G97", tr("<b>G97 S</b>xx - constant spindle speed xx"));
   settings.setValue("G98", tr("<b>G98</b> - return to initial level"));
   settings.setValue("G99", tr("<b>G99</b> - return to R level"));

   settings.setValue("EQ", tr("<b>EQ</b> - equal to"));
   settings.setValue("NE", tr("<b>NE</b> - not equal to"));
   settings.setValue("GT", tr("<b>GT</b> - greather than"));
   settings.setValue("LE", tr("<b>LE</b> - less than or equal to"));
   settings.setValue("LT", tr("<b>LT</b> - less than"));
   settings.setValue("GE", tr("<b>GE</b> - greather than or equal to"));
   settings.setValue("IF", tr("<b>IF[</b>condition<b>]</b>do something - if condition is true do something"));
   settings.setValue("GOTO", tr("<b>GOTO</b>nnnn - jump to block nnnn"));

   settings.setValue("WHILE", tr("<b>WHILE[</b>condition<b>] DO</b>n <br />...<br />commands<br />... <br /><b>END</b>n  - loop - while condition true do commands beetwen DOn and ENDn"));
   settings.setValue("END", tr("<b>END</b>n - end of WHILE DOn loop"));

   settings.setValue("EOR", tr("<b>EOR</b> - exclusive OR"));
   settings.setValue("OR", tr("<b>OR</b> - logical OR"));
   settings.setValue("AND", tr("<b>AND</b> - logical AND"));
   settings.setValue("NOT", tr("<b>NOT</b> - negation"));

   settings.setValue("SIN", tr("<b>SIN[</b>angle<b>]</b> - sine"));
   settings.setValue("COS", tr("<b>COS[</b>angle<b>]</b> - cosine"));
   settings.setValue("TAN", tr("<b>TAN[</b>angle<b>]</b> - tangent"));
   settings.setValue("ATAN", tr("<b>ATAN[</b>angle<b>]</b> - arctangent 1 or 2"));
   settings.setValue("SQRT", tr("<b>SQRT[</b>val<b>]</b> - square root"));
   settings.setValue("ABS", tr("<b>ABS[</b>val<b>]</b> - absolute value"));
   settings.setValue("BIN", tr("<b>BIN[</b>val<b>]</b> - decimal to binary conversion"));
   settings.setValue("BCD", tr("<b>BCD[</b>val<b>]</b> - binary to decimal conversion"));
   settings.setValue("ROUND", tr("<b>ROUND[</b>val<b>]</b> - integer implementation (rounding)"));
   settings.setValue("FIX", tr("<b>FIX[</b>val<b>]</b> - integer implementation (truncation)"));
   settings.setValue("FUP", tr("<b>FUP[</b>val<b>]</b> - integer implementation (raising)"));




   settings.endGroup();

//******************************************************************//
   settings.beginGroup("SINUMERIK");
//******************************************************************//
   settings.setValue("M00", tr("<b>M00</b> - program stop, unconditional"));
   settings.setValue("M01", tr("<b>M01</b> - optional program stop"));
   settings.setValue("M03", tr("<b>M03</b> - start spindle CW"));
   settings.setValue("M04", tr("<b>M04</b> - start spindle CCW"));
   settings.setValue("M05", tr("<b>M05</b> - spindle stop"));
   settings.setValue("M06", tr("<b>M06</b> - tool change"));
   settings.setValue("M08", tr("<b>M08</b> - coolant on"));
   settings.setValue("M09", tr("<b>M09</b> - coolant off"));
   settings.setValue("M30", tr("<b>M30</b> - end of program"));
   settings.setValue("M51", tr("<b>M51</b> - thro spindle coolant on"));

   settings.setValue("G00", tr("<b>G00</b> - rapid move - positioning"));
   settings.setValue("G01", tr("<b>G01</b> - linear interpolation"));
   settings.setValue("G02", tr("<b>G02</b> - circular interpolation CW"));
   settings.setValue("G03", tr("<b>G03</b> - circular interpolation CCW"));
   settings.setValue("G04", tr("<b>G04 F</b>xxxx - dwell xxxx seconds"));

   settings.setValue("G17", tr("<b>G17</b> - XY plane"));
   settings.setValue("G18", tr("<b>G18</b> - ZX plane"));
   settings.setValue("G19", tr("<b>G19</b> - YZ plane"));

   settings.setValue("G40", tr("<b>G40</b> - tool nose/cutter radius compensation cancel"));
   settings.setValue("G41", tr("<b>G41</b> - tool nose/cutter radius compensation left"));
   settings.setValue("G42", tr("<b>G42</b> - tool nose/cutter radius compensation right"));

   settings.setValue("G53", tr("<b>G53</b> - machine coordinate system"));
   settings.setValue("G54", tr("<b>G54</b> - settable zero offset 1"));
   settings.setValue("G55", tr("<b>G55</b> - settable zero offset 2"));
   settings.setValue("G56", tr("<b>G56</b> - settable zero offset 3"));
   settings.setValue("G57", tr("<b>G57</b> - settable zero offset 4"));
   settings.setValue("G58", tr("<b>G58</b> - programmable offset, absolute axial substitution"));
   settings.setValue("G59", tr("<b>G59</b> - programmable offset, additive axial substitution"));

   settings.setValue("G90", tr("<b>G90</b> - absolute programming"));
   settings.setValue("G91", tr("<b>G91</b> - incremental programming"));
   settings.setValue("G94", tr("<b>G94</b> - feed per minute"));
   settings.setValue("G95", tr("<b>G95</b> - feed per revolution"));
   settings.setValue("G96", tr("<b>G96 S</b>xx - constant cutting speed xx"));
   settings.setValue("G97", tr("<b>G97 S</b>xx - constant spindle speed xx"));


   settings.setValue("@100", tr("<b>@100 K</b>nnnn - jump to block number nnnn"));
   settings.setValue("@121", tr("<b>@121 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy not equal to xx jump to nnnn"));
   settings.setValue("@122", tr("<b>@122 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy equal to xx jump to nnnn"));
   settings.setValue("@123", tr("<b>@123 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is less than or equal to xx jump to nnnn"));
   settings.setValue("@124", tr("<b>@124 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is less than xx jump to nnnn"));
   settings.setValue("@125", tr("<b>@125 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is greather than or equal to xx jump to nnnn"));
   settings.setValue("@126", tr("<b>@126 R</b>|<b>K</b>yy <b>R</b>|<b>K</b>xx <b>K</b>nnnn - if yy is greather than xx jump to nnnn"));

   settings.setValue("@620", tr("<b>@620 R</b>xx - increment value in param. xx"));
   settings.setValue("@621", tr("<b>@621 R</b>xx - decrement value in param. xx"));
   settings.setValue("@622", tr("<b>@622 R</b>xx - 'trunc' value in param. xx"));
   settings.setValue("@630", tr("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - sine of angle yy is stored in param. xx"));
   settings.setValue("@631", tr("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - cosine of angle yy is stored in param. xx"));
   settings.setValue("@632", tr("<b>@630 R</b>xx <b>R</b>|<b>K</b>yy - tangent of angle yy is stored in param. xx"));
   settings.setValue("@634", tr("<b>@634 R</b>xx <b>R</b>|<b>K</b>yy - arc sine of angle yy is stored in param. xx"));

   settings.setValue("@714", tr("<b>@714</b> - stop decoding, until buffer is empty"));



   settings.endGroup();

//******************************************************************//
   settings.beginGroup("SINUMERIK_840");
//******************************************************************//
   settings.setValue("M00", tr("<b>M00</b> - program stop, unconditional"));
   settings.setValue("M01", tr("<b>M01</b> - optional program stop"));
   settings.setValue("M03", tr("<b>M03</b> - start spindle CW"));
   settings.setValue("M04", tr("<b>M04</b> - start spindle CCW"));
   settings.setValue("M05", tr("<b>M05</b> - spindle stop"));
   settings.setValue("M06", tr("<b>M06</b> - tool change"));
   settings.setValue("M07", tr("<b>M07</b> - thro spindle coolant on"));
   settings.setValue("M08", tr("<b>M08</b> - coolant on"));
   settings.setValue("M09", tr("<b>M09</b> - coolant off"));
   settings.setValue("M30", tr("<b>M30</b> - end of program"));
   settings.setValue("M50", tr("<b>M50</b> - shower coolant on"));

   settings.setValue("G00", tr("<b>G00</b> - rapid move - positioning"));
   settings.setValue("G01", tr("<b>G01</b> - linear interpolation"));
   settings.setValue("G02", tr("<b>G02</b> - circular interpolation CW"));
   settings.setValue("G03", tr("<b>G03</b> - circular interpolation CCW"));
   settings.setValue("G04", tr("<b>G04 F</b>xxxx - dwell xxxx seconds"));

   settings.setValue("G17", tr("<b>G17</b> - XY plane"));
   settings.setValue("G18", tr("<b>G18</b> - ZX plane"));
   settings.setValue("G19", tr("<b>G19</b> - YZ plane"));

   settings.setValue("G53", tr("<b>G53</b> - Suppression of current frames: Programmable frame including system frame for TOROT and TOFRAME and active settable frame G54 ... G599."));
   settings.setValue("G54", tr("<b>G54</b> - settable zero offset 1"));
   settings.setValue("G55", tr("<b>G55</b> - settable zero offset 2"));
   settings.setValue("G56", tr("<b>G56</b> - settable zero offset 3"));
   settings.setValue("G57", tr("<b>G57</b> - settable zero offset 4"));
   settings.setValue("G58", tr("<b>G58</b> - programmable offset, absolute axial substitution"));
   settings.setValue("G59", tr("<b>G59</b> - programmable offset, additive axial substitution"));


   settings.setValue("G40", tr("<b>G40</b> - tool nose/cutter radius compensation cancel"));
   settings.setValue("G41", tr("<b>G41</b> - tool nose/cutter radius compensation left"));
   settings.setValue("G42", tr("<b>G42</b> - tool nose/cutter radius compensation right"));

   settings.setValue("G60", tr("<b>G60</b> - Velocity reduction, exact positioning"));
   settings.setValue("G64", tr("<b>G64</b> - Continuous-path mode"));

   settings.setValue("G90", tr("<b>G90</b> - absolute programming"));
   settings.setValue("G91", tr("<b>G91</b> - incremental programming"));
   settings.setValue("G94", tr("<b>G94</b> - feed per minute"));
   settings.setValue("G95", tr("<b>G95</b> - feed per revolution"));
   settings.setValue("G96", tr("<b>G96 S</b>xx - constant cutting speed xx"));
   settings.setValue("G97", tr("<b>G97 S</b>xx - constant spindle speed xx"));

   settings.setValue("G153", tr("<b>G153</b> - suppression as for G53 and including all channel-specific and/or NCU-global basic frame"));
   settings.setValue("G500", tr("<b>G500</b> - deactivate all settable G54-G57 frames if G500 does not contain a value"));

   settings.setValue("SUPA", tr("<b>SUPA</b> - suppression as for G153 and including system frames for actual-value setting, scratching, zero offset external, PAROT including handwheel offsets (DRF), [zero offset external], overlaid motion"));

   settings.setValue("IF", tr("<b>IF(</b>condition<b>)</b> nnnn - if condition is true goto block nnnn or label nnnn:"));
   settings.setValue("GOTO", tr("<b>GOTO</b> nnnn - jump forward and if block not found jump backward to block nnnn or label nnnn:"));
   settings.setValue("GOTOF", tr("<b>GOTOF</b> nnnn - jump forward to block nnnn or label nnnn:"));
   settings.setValue("GOTOB", tr("<b>GOTOB</b> nnnn - jump backward to block nnnn or label nnnn:"));

   settings.setValue("STOPRE", tr("<b>STOPRE</b> - stop decoding, until buffer is empty"));

   settings.setValue("DIAMOF", tr("<b>DIAMOF</b> - diameter programming OFF; radius programming for G90/G91"));
   settings.setValue("DIAMON", tr("<b>DIAMON</b> - diameter programming ON; diameter programming for G90/G91"));
   settings.setValue("DIAM", tr("<b>DIAM90</b> - diameter programming for G90; radius programming for G91"));


   settings.setValue("RET", tr("<b>RET</b> - subprogram end"));




   settings.endGroup();


//******************************************************************//
   settings.beginGroup("HEIDENHAIN");
//******************************************************************//
   settings.setValue("M00", tr("<b>M00</b> - program stop, unconditional"));
   settings.setValue("M01", tr("<b>M01</b> - optional program stop"));
   settings.setValue("M03", tr("<b>M03</b> - start spindle CW"));
   settings.setValue("M04", tr("<b>M04</b> - start spindle CCW"));
   settings.setValue("M05", tr("<b>M05</b> - spindle stop"));
   settings.setValue("M06", tr("<b>M06</b> - tool change"));
   settings.setValue("M07", tr("<b>M07</b> - thro spindle coolant on"));
   settings.setValue("M08", tr("<b>M08</b> - coolant on"));
   settings.setValue("M09", tr("<b>M09</b> - coolant off"));
   settings.setValue("M30", tr("<b>M30</b> - end of program"));
   settings.setValue("M50", tr("<b>M50</b> - shower coolant on"));

   settings.setValue("G00", tr("<b>G00</b> - rapid move - positioning"));
   settings.setValue("G01", tr("<b>G01</b> - linear interpolation"));
   settings.setValue("G02", tr("<b>G02</b> - circular interpolation CW"));
   settings.setValue("G03", tr("<b>G03</b> - circular interpolation CCW"));
   settings.setValue("G04", tr("<b>G04 F</b>xxxx - dwell xxxx seconds"));








   settings.endGroup();



//******************************************************************//
   settings.beginGroup("HEIDENHAIN_ISO");
//******************************************************************//
   settings.setValue("M00", tr("<b>M00</b> - program stop, unconditional"));
   settings.setValue("M01", tr("<b>M01</b> - optional program stop"));
   settings.setValue("M03", tr("<b>M03</b> - start spindle CW"));
   settings.setValue("M04", tr("<b>M04</b> - start spindle CCW"));
   settings.setValue("M05", tr("<b>M05</b> - spindle stop"));
   settings.setValue("M06", tr("<b>M06</b> - tool change"));
   settings.setValue("M07", tr("<b>M07</b> - thro spindle coolant on"));
   settings.setValue("M08", tr("<b>M08</b> - coolant on"));
   settings.setValue("M09", tr("<b>M09</b> - coolant off"));
   settings.setValue("M30", tr("<b>M30</b> - end of program"));
   settings.setValue("M50", tr("<b>M50</b> - shower coolant on"));

   settings.setValue("G00", tr("<b>G00</b> - rapid move - positioning"));
   settings.setValue("G01", tr("<b>G01</b> - linear interpolation"));
   settings.setValue("G02", tr("<b>G02</b> - circular interpolation CW"));
   settings.setValue("G03", tr("<b>G03</b> - circular interpolation CCW"));
   settings.setValue("G04", tr("<b>G04 F</b>xxxx - dwell xxxx seconds"));

   settings.setValue("G17", tr("<b>G17</b> - XY plane"));
   settings.setValue("G18", tr("<b>G18</b> - ZX plane"));
   settings.setValue("G19", tr("<b>G19</b> - YZ plane"));





   settings.endGroup();

   if(QFileInfo(fileName).exists())
      openFile(fileName);
   QMdiSubWindow *existing = findMdiChild(fileName);
   if(existing)
   {
      mdiArea->setActiveSubWindow(existing);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::attachHighlighterToDirButtonClicked(bool attach)
{
   QFileInfo fileInfo;
   QFile file;
   int i;

   bool hasMdiChild = (activeMdiChild() != 0);
   if(hasMdiChild)
   {
      QDir dir;
      dir.setPath(activeMdiChild()->filePath());
      dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
      dir.setSorting(QDir::Name);
      dir.setNameFilters(QStringList("*.cfg"));

      QFileInfoList list = dir.entryInfoList();

      if(!list.isEmpty())
      {
         for(i = 0; i < list.count(); i++)
         {
            fileInfo = (QFileInfo)list.at(i);
            file.setFileName(fileInfo.absoluteFilePath());
            file.remove();
         };
      };

      if(attach)
      {
         file.setFileName(activeMdiChild()->filePath() + "/" + highlightTypeCombo->currentText() + ".cfg");
         file.open(QIODevice::ReadWrite);
         file.close();;
      };

   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::attachHighlightToDirActClicked()
{
   attachHighlighterToDirButtonClicked(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::deAttachHighlightToDirActClicked()
{
   attachHighlighterToDirButtonClicked(false);
}

//**************************************************************************************************
//
//**************************************************************************************************

int EdytorNc::defaultHighlightMode(QString filePath)
{
   int id;
   QDir dir;
   bool ok;

   dir.setPath(filePath);
   dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
   dir.setSorting(QDir::Name);
   dir.setNameFilters(QStringList("*.cfg"));

   QFileInfoList list = dir.entryInfoList();

   if(!list.isEmpty())
   {
      QFileInfo name = list.at(0);
      id = highlightTypeCombo->findText(name.baseName());
      if(id >= 0)
      {
         //highlightTypeCombo->setCurrentIndex(id);
         return(highlightTypeCombo->itemData(id).toInt(&ok));
      };
   };

   return MODE_AUTO;
};

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectAdd()
{
   QFileInfo file;
   QStandardItem *item;
   QIcon icon;

   if(currentProject == NULL)
      return;

#ifdef Q_OS_LINUX
   QString filters = tr("All files (*.* *);;"
                        "CNC programs files *.nc (*.nc);;"
                        "CNC programs files *.nc *.min *.anc *.cnc (*.nc *.min *.anc *.cnc);;"
                        "Documents *.odf *.odt *.pdf *.doc *.docx  *.xls *.xlsx (*.odf *.odt *.pdf *.doc *.docx  *.xls *.xlsx);;"
                        "Drawings *.dwg *.dxf (*.dwg *.dxf);;"
                        "Pictures *.jpg *.bmp *.svg (*.jpg *.bmp *.svg);;"
                        "Text files *.txt (*.txt)");
#endif

#ifdef Q_OS_WIN32
   QString filters = tr("All files (*.* *);;"
                        "CNC programs files (*.nc);;"
                        "CNC programs files (*.nc *.min *.anc *.cnc);;"
                        "Documents (*.odf *.odt *.pdf *.doc *.docx  *.xls *.xlsx);;"
                        "Drawings (*.dwg *.dxf);;"
                        "Pictures (*.jpg *.bmp *.svg);;"
                        "Text files (*.txt)");
#endif



   QStringList files = QFileDialog::getOpenFileNames(
                        this,
                        tr("Add files to project"),
                        lastDir.absolutePath(),
                        filters, 0);


   QStringList list = files;

   if(list.isEmpty())
      return;

   QStringList::Iterator it = list.begin();

   QStandardItem *parentItem = currentProject;

   if(it != list.end())
   {
      file.setFile(*it);
      if((file.absoluteDir().exists()) && (file.absoluteDir().isReadable()))
      {

         QList<QStandardItem *> items = model->findItems(QDir::toNativeSeparators(file.absoluteDir().canonicalPath()), Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive, 0);

         if(!items.isEmpty())
         {
            item = items.at(0);
            if(item->text() != file.absoluteDir().canonicalPath())
            {
               item = new QStandardItem(QIcon(":/images/folder.png"), QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
               parentItem->appendRow(item);
            };
         }
         else
         {
            item = new QStandardItem(QIcon(":/images/folder.png"), QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
            parentItem->appendRow(item);
         };

         parentItem = item;

      }
      else
         return;
   };

   QFileSystemModel *fModel = new QFileSystemModel;

   while(it != list.end())
   {
      file.setFile(*it);

      if((file.exists()) && (file.isReadable()))
      {
         icon = fModel->iconProvider()->icon(file);

         if(icon.isNull())
            icon = QIcon(":/images/ncfile.png");

         item = new QStandardItem(icon, file.fileName());
         parentItem->appendRow(item);
      };
      ++it;
   };
   projectTreeView->expandAll(); //model->indexFromItem(currentProject));

   currentProjectModified = true;
   statusBar()->showMessage(tr("Project opened"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectSave()
{
   QString path, fileName;
   int fileCount;
   QStandardItem *item;


   if(currentProjectName.isEmpty() || currentProject == NULL)
      return;

   QSettings settings(currentProjectName, QSettings::IniFormat);

   settings.remove("ProjectFiles");
   settings.beginWriteArray("ProjectFiles");

   fileCount = 0;
   for(int i = 0; i < currentProject->rowCount(); i++)
   {
      item = currentProject->child(i, 0);
      path = item->text();
      for(int j = 0; j < item->rowCount(); j++)
      {
         fileName = item->child(j, 0)->text();

         //qDebug() << path;
         //qDebug() << fileName;
         settings.setArrayIndex(fileCount);
         settings.setValue("File", QFileInfo(path, fileName).absoluteFilePath());
         fileCount++;
      };
   };

   settings.endArray();

   if(settings.status() == QSettings::NoError)
   {
      currentProjectModified = false;
      statusBar()->showMessage(tr("Project saved"), 5000);
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectSaveAs()
{
   QString fileName = projectSelectName();

   if(fileName.isEmpty())
      return;

   currentProjectName = fileName;
   QStandardItem *parentItem = model->invisibleRootItem();
   parentItem->child(0, 0)->setText(QFileInfo(currentProjectName).fileName());
   parentItem->child(0, 0)->setToolTip(QDir::toNativeSeparators(QFileInfo(currentProjectName).absoluteFilePath()));
   projectSave();
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectNew()
{

   if(!maybeSaveProject())
      return;

   QString fileName = projectSelectName();

   if(fileName.isEmpty())
      return;

   currentProjectName = fileName;


   QStandardItem *parentItem = model->invisibleRootItem();
   QStandardItem *item = new QStandardItem(QIcon(":/images/edytornc.png"), QFileInfo(currentProjectName).fileName());

   parentItem->appendRow(item);

   currentProject = item;
   currentProjectModified = true;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectTreeViewDoubleClicked(const QModelIndex & index)
{
   QFileInfo file;

   if((!index.isValid()))
      return;

   QStandardItem *item = model->itemFromIndex(index);

   if(item == NULL)
      return;

   if(item->hasChildren())
      return;

   file.setFile(item->parent()->text(), item->text());

   if((file.exists()) && (file.isReadable()))
   {
      if(file.suffix() == "nc")
      {
         defaultMdiWindowProperites.fileName = file.absoluteFilePath();
         defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
         defaultMdiWindowProperites.geometry = QByteArray();
         defaultMdiWindowProperites.cursorPos = 0;
         defaultMdiWindowProperites.editorToolTips = true;
         defaultMdiWindowProperites.hColors.highlightMode =  defaultHighlightMode(QFileInfo(defaultMdiWindowProperites.fileName).canonicalPath());
         loadFile(defaultMdiWindowProperites);
      }
      else
      {
         QDesktopServices::openUrl(QUrl("file:///" + file.absoluteFilePath(), QUrl::TolerantMode));
      };
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::fileTreeViewDoubleClicked(const QModelIndex & index)
{
   QFileInfo file;

   if(!index.isValid())
      return;

   file.setFile(dirModel->filePath(index));

   if((file.exists()) && (file.isReadable()))
   {
      if(file.suffix() == "nc")
      {
         defaultMdiWindowProperites.fileName = file.absoluteFilePath();
         defaultMdiWindowProperites.readOnly = defaultMdiWindowProperites.defaultReadOnly;
         defaultMdiWindowProperites.geometry = QByteArray();
         defaultMdiWindowProperites.cursorPos = 0;
         defaultMdiWindowProperites.editorToolTips = true;
         defaultMdiWindowProperites.hColors.highlightMode =  defaultHighlightMode(QFileInfo(defaultMdiWindowProperites.fileName).canonicalPath());
         loadFile(defaultMdiWindowProperites);
      }
      else
      {
         QDesktopServices::openUrl(QUrl("file:///" + file.absoluteFilePath(), QUrl::TolerantMode));
      };
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

QString EdytorNc::projectSelectName()
{

#ifdef Q_OS_LINUX
   QString filters = tr("EdytorNC project file *.ncp (*.ncp)");
#endif

#ifdef Q_OS_WIN32
   QString filters = tr("EdytorNC project file (*.ncp)");
#endif

   QString file = QFileDialog::getSaveFileName(
                         this,
                         tr("Select the project name and location..."),
                         currentProjectName,
                         filters);

   return file;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectOpen()
{

   if(!maybeSaveProject())
      return;


#ifdef Q_OS_LINUX
   QString filters = tr("EdytorNC project file *.ncp (*.ncp)");
#endif

#ifdef Q_OS_WIN32
   QString filters = tr("EdytorNC project file (*.ncp)");
#endif

   QString fileName = QFileDialog::getOpenFileName(
                         this,
                         tr("Open the project file..."),
                         currentProjectName,
                         filters);

   if(fileName.isEmpty())
      return;

   projectLoad(fileName);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::hidePanel()
{
   hSplitter->setUpdatesEnabled(false);

   if(!panelHidden)
   {
      panelState = hSplitter->saveState();
      frame->setMaximumWidth(hideButton->width());
      vSplitter->hide();
      //openFileTableWidget->hide();
      hideButton->setText(">>");
      panelHidden = true;
   }
   else
   {
      panelHidden = false;
      fileTreeViewChangeRootDir();
      frame->setMaximumWidth(16777215);
      vSplitter->show();
      //openFileTableWidget->show();
      hideButton->setText("<<");
      hSplitter->restoreState(panelState);
   };

   hSplitter->updateGeometry();
   hSplitter->setUpdatesEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectTreeRemoveItem()
{
   QStandardItem *item;

   QModelIndexList list = projectTreeView->selectionModel()->selectedIndexes();
   QModelIndexList::Iterator it = list.begin();


   while(it != list.end())
   {
      item = model->itemFromIndex(*it);

      if(item == NULL)
         return;

      if(!item->hasChildren())
      {
         currentProjectModified = model->removeRow(item->row(), model->indexFromItem(item->parent()));
      };

      ++it;
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::projectLoad(QString projectName)
{
   QFileInfo file;
   QIcon icon;


   if(projectName.isEmpty())
      return;

   currentProjectName = projectName;

   model->clear();

   QSettings settings(currentProjectName, QSettings::IniFormat);

   QStandardItem *parentItem = model->invisibleRootItem();
   QStandardItem *item = new QStandardItem(QIcon(":/images/edytornc.png"), QFileInfo(currentProjectName).fileName());
   item->setToolTip(QDir::toNativeSeparators(currentProjectName));

   parentItem->appendRow(item);

   currentProject = item;
   parentItem = item;

   QFileSystemModel *fModel = new QFileSystemModel;

   int max = settings.beginReadArray("ProjectFiles");
   for(int i = 0; i < max; ++i)
   {
      settings.setArrayIndex(i);
      file.setFile(settings.value("File", "").toString());

      if((file.absoluteDir().exists()) && (file.absoluteDir().isReadable()))
      {
         QList<QStandardItem *> items = model->findItems(file.absoluteDir().canonicalPath(), Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive, 0);

         if(!items.isEmpty())
         {
            item = items.at(0);
            if(item->text() != file.absoluteDir().canonicalPath())
            {
               item = new QStandardItem(QIcon(":/images/folder.png"), file.absoluteDir().canonicalPath());
               item->setToolTip(QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
               currentProject->appendRow(item);
            };
         }
         else
         {
            item = new QStandardItem(QIcon(":/images/folder.png"), file.absoluteDir().canonicalPath());
            item->setToolTip(QDir::toNativeSeparators(file.absoluteDir().canonicalPath()));
            currentProject->appendRow(item);
         };

         parentItem = item;

         if((file.exists()) && (file.isReadable()))
         {
            icon = fModel->iconProvider()->icon(file);

            if(icon.isNull())
               icon = QIcon(":/images/ncfile.png");

            item = new QStandardItem(icon, file.fileName());
            item->setToolTip(file.fileName());
            parentItem->appendRow(item);
         };
      };
   };
   settings.endArray();

   projectTreeView->expandAll();

   currentProjectModified = false;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool EdytorNc::maybeSaveProject()
{
   if(currentProjectModified)
   {
      QMessageBox msgBox;
      msgBox.setText(tr("<b>Project: \"%1\"\n has been modified.</b>").arg(currentProjectName));
      msgBox.setInformativeText(tr("Do you want to save your changes ?"));
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Save);
      msgBox.setIcon(QMessageBox::Warning);
      int ret = msgBox.exec();
      switch (ret)
      {
         case QMessageBox::Save    : projectSave();
                                     return true;
                                     break;
         case QMessageBox::Discard : currentProjectModified = false;
                                     return true;
                                     break;
         case QMessageBox::Cancel  : return false;
                                     break;
         default                   : return true;
                                     break;
      } ;
   };
   return true;
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::createFileBrowseTabs()
{
   dirModel = new QFileSystemModel();
   dirModel->setResolveSymlinks(true);
   
   //dirModel->setRootPath(lastDir.absolutePath());
   //fileTreeViewChangeRootDir();

   dirModel->setNameFilters(defaultMdiWindowProperites.extensions); //QStringList("*.nc")
   dirModel->setNameFilterDisables(false);

   fileTreeView->setModel(dirModel);
   fileTreeViewChangeRootDir();

   connect(fileTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(fileTreeViewDoubleClicked(QModelIndex)));
   connect(openFileTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(openFileTableWidgetClicked(int, int)));
   openFileTableWidget->setToolTip(tr("Open files"));
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::updateOpenFileList()
{
   QTableWidgetItem *newItem;
   QFileInfo file;
   QStringList labels;

   openFileTableWidget->setUpdatesEnabled(false);

   openFileTableWidget->clear();
   labels << tr("Info") << tr("File Name") << "";
   openFileTableWidget->setHorizontalHeaderLabels(labels);
   openFileTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

   QList<QMdiSubWindow *> windows = mdiArea->subWindowList();

   openFileTableWidget->setRowCount(windows.size());
   for (int i = 0; i < windows.size(); ++i)
   {
       MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

       file.setFile(child->currentFile());

       newItem = new QTableWidgetItem(file.fileName() + (child->textEdit->document()->isModified() ? "*": ""));

       if(file.canonicalFilePath().isEmpty())
         newItem->setToolTip(child->currentFile());
       else
           newItem->setToolTip(QDir::toNativeSeparators(file.canonicalFilePath()));
       openFileTableWidget->setItem(i, 1, newItem);

       newItem = new QTableWidgetItem(child->getCurrentFileInfo());
       newItem->setToolTip(child->getCurrentFileInfo() + " --> " + QDir::toNativeSeparators(file.canonicalFilePath()));
       openFileTableWidget->setItem(i, 0, newItem);

       newItem = new QTableWidgetItem(QIcon(":/images/fileclose_small.png"), "", QTableWidgetItem::UserType);
       newItem->setToolTip(tr("Close"));
       openFileTableWidget->setItem(i, 2, newItem);

       if(child == activeMdiChild())
       {
          openFileTableWidget->selectRow(i);
       };
   };
   
   openFileTableWidget->setVisible(false);
   openFileTableWidget->resizeRowsToContents();

   openFileTableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
   openFileTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
   openFileTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

   //openFileTableWidget->resizeColumnsToContents();
   openFileTableWidget->setVisible(true);

   openFileTableWidget->setUpdatesEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::openFileTableWidgetClicked(int x, int y)
{
   QTableWidgetItem *item = openFileTableWidget->item(x, 1);

   QMdiSubWindow *existing = findMdiChild(item->toolTip());
   if(existing)
   {
      if(y == 2)
      {
         existing->close();
         updateOpenFileList();
      }
      else
         mdiArea->setActiveSubWindow(existing);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::fileTreeViewChangeRootDir()
{
   QString path;

   if(panelHidden)  //if((!isVisible()) || panelHidden)
      return;

   if(tabWidget->currentIndex() != 1)
      return;

   if(fileTreeView == NULL || dirModel == NULL)
      return;

   if(currentPathCheckBox->isChecked() && (activeMdiChild() != 0))
   {
      path = activeMdiChild()->currentFile();
      path = QFileInfo(path).canonicalPath();
   }
   else
      path = lastDir.canonicalPath();

   if(path.isEmpty())
      return;

   if(dirModel->rootPath() == path)
      return;

   fileTreeView->setRootIndex(dirModel->index(path));
   dirModel->setRootPath(path);
   //fileTreeView->setToolTip(path);
   fileTreeView->setSortingEnabled(true);
   fileTreeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
   fileTreeView->resizeColumnToContents(0);
   fileTreeView->resizeColumnToContents(1);
   fileTreeView->setColumnHidden(2, true);
   fileTreeView->resizeColumnToContents(3);
}

//**************************************************************************************************
//
//**************************************************************************************************

bool EdytorNc::event(QEvent *event)
{
   QString key, text;
   QModelIndex index;
   QFile file;
   QString fileName;
   char buf[1024];
   qint64 lineLength;


   if((event->type() == QEvent::ToolTip))
   {
      if(panelHidden)
         return true;

      QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);


      QPoint pos = fileTreeView->viewport()->mapFromGlobal(helpEvent->globalPos());

      if((pos.y() >= fileTreeView->viewport()->height()) ||
         (pos.x() >= fileTreeView->viewport()->width()) || (tabWidget->currentIndex() != 1))
      {
         return true;
      };

      index = fileTreeView->indexAt(pos);

      if(!index.isValid())
         return true;

      fileName = dirModel->filePath(index);
      file.setFileName(fileName);
      text = "<b>" + QDir::toNativeSeparators(fileName) + "</b>";

      if(filePreviewSpinBox->value() > 0)
      {
         text.append("<br />");
         if(file.open(QIODevice::ReadOnly))
         {
            for(int i = 0; i < filePreviewSpinBox->value(); i++)
            {
               lineLength = file.readLine(buf, sizeof(buf));
               if (lineLength != -1)
               {
                  text.append(buf);
               };
            };
            file.close();
            if(text.endsWith('\n'))
               text.remove(text.size() - 1, 1);
         };
      };

      if(!text.isEmpty())
      {
         if(text.length() < fileName.size())
            key = "<p style='white-space:normal'>";
         else
            key = "<p style='white-space:pre'>";
         QToolTip::showText(helpEvent->globalPos(), key + text, this, QRect());
      }
      else
      {
         QToolTip::hideText();
         event->ignore();
      };
      return true;
   };
   return QWidget::event(event);
}

//**************************************************************************************************
//
//**************************************************************************************************

void EdytorNc::splitPrograms()
{
   MdiChild *activeWindow = activeMdiChild();
   if(activeWindow <= 0)
     return;

   QApplication::setOverrideCursor(Qt::BusyCursor);

   QStringList list = activeWindow->splitFile();

   if(list.size() <= 1)
   {
      QApplication::restoreOverrideCursor();
      return;
   };

   QStringList::const_iterator it = list.constBegin();
   while(it != list.constEnd())
   {
      newFile();
      activeWindow = activeMdiChild();
      if(activeWindow <= 0)
      {
         QApplication::restoreOverrideCursor();
         return;
      };
      activeWindow->textEdit->setUndoRedoEnabled(false);  //clear undo/redo history
      activeWindow->textEdit->setPlainText(*it);
      activeWindow->textEdit->setUndoRedoEnabled(true);
      it++;
   };
   QApplication::restoreOverrideCursor();
}
