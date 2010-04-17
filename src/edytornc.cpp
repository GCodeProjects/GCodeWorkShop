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

#include <QtGui>

#include "edytornc.h"
#include "mdichild.h"


#define EXAMPLES_PATH             "/usr/share/edytornc/EXAMPLES"


//**************************************************************************************************
//
//**************************************************************************************************

edytornc::edytornc()
{
    setAttribute(Qt::WA_DeleteOnClose);

    findToolBar = NULL;
    serialToolBar = NULL;
    diffApp = NULL;
    findFiles = NULL;

    openExampleAct = NULL;


    clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(updateMenus()));

    splitter = new QSplitter(Qt::Vertical, this);
    setCentralWidget(splitter);

    mdiArea = new QMdiArea;
    //setCentralWidget(mdiArea);

    splitter->addWidget(mdiArea);
    splitter->setChildrenCollapsible(false);

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));
    


    createActions();
    createMenus();
    createToolBars();

    updateMenus();

    readSettings();

    setWindowTitle(tr("EdytorNC"));
    setWindowIcon(QIcon(":/images/edytornc.png"));
    createStatusBar();
    
    if(tabbedView)
      mdiArea->setViewMode(QMdiArea::TabbedView);
    else
      mdiArea->setViewMode(QMdiArea::SubWindowView);
    
}

//**************************************************************************************************
//
//**************************************************************************************************

edytornc::~edytornc()
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

void edytornc::closeEvent(QCloseEvent *event)
{
    setUpdatesEnabled(FALSE);
    writeSettings();

    foreach(QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder))
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
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::newFile()
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

void edytornc::open()
{
    QFileInfo file;
    QMdiSubWindow *existing;

    existing = 0;

#ifdef Q_OS_LINUX
    QString filters = tr("CNC programs files *.nc (*.nc);;"
                         "CNC programs files *.nc *.min *.anc *.cnc (*.nc *.min *.anc *.cnc);;"
                         "Text files *.txt (*.txt);; All files (*.* *)");
#endif

#ifdef Q_OS_WIN32
  QString filters = tr("CNC programs files (*.nc);;"
                       "CNC programs files (*.nc *.min *.anc *.cnc);;"
                       "Text files (*.txt);; All files (*.* *)");
#endif



    QStringList files = QFileDialog::getOpenFileNames(
                         this,
                         tr("Select one or more files to open"),
                         lastDir.absolutePath(),
                         filters, &openFileFilter);


    QStringList list = files;
    QStringList::Iterator it = list.begin();
    while(it != list.end())
    {
       file.setFile(*it);
       existing = findMdiChild(*it);

       if((file.exists()) && (file.isReadable()) && !existing)
       {
          lastDir = file.absoluteDir();
          MdiChild *child = createMdiChild();
          if(child->loadFile(*it))
          {
             defaultMdiWindowProperites.cursorPos = 0;
             defaultMdiWindowProperites.readOnly = FALSE;
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

void edytornc::openExample()
{
   QFileInfo file;
   QMdiSubWindow *existing;
   QString dir;

   existing = 0;


   if(QDir(EXAMPLES_PATH).exists())
      dir = EXAMPLES_PATH;
   else
      dir = QApplication::applicationDirPath() + "/" + "EXAMPLES";



   QString filters = tr("CNC programs files *.nc (*.nc);;"
                        "All files (*.* *)");




   QStringList files = QFileDialog::getOpenFileNames(
                        this,
                        tr("Select one or more files to open"),
                        dir,
                        filters, &openFileFilter);


   QStringList list = files;
   QStringList::Iterator it = list.begin();
   while(it != list.end())
   {
      file.setFile(*it);
      existing = findMdiChild(*it);

      if((file.exists()) && (file.isReadable()) && !existing)
      {
         lastDir = file.absoluteDir();
         MdiChild *child = createMdiChild();
         if(child->loadFile(*it))
         {
            defaultMdiWindowProperites.cursorPos = 0;
            defaultMdiWindowProperites.readOnly = FALSE;
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

void edytornc::openFile(const QString fileName)
{
   QFileInfo file;

   file.setFile(fileName);

   QMdiSubWindow *existing = findMdiChild(fileName);

   if((file.exists()) && (file.isReadable()) && !existing)
   {
      lastDir = file.absoluteDir();
      MdiChild *child = createMdiChild();
      if(child->loadFile(fileName))
      {
         defaultMdiWindowProperites.cursorPos = 0;
         defaultMdiWindowProperites.readOnly = FALSE;
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

void edytornc::openWithPreview()
{

    CustomFDialog *fileDialog;
    QStringList filters;
    QFileInfo file;



    fileDialog = new CustomFDialog(this, fdShowPreview + fdExistingFiles);


    filters << tr("CNC programs files (*.nc)")
            << tr("CNC programs files (*.nc *.min *.anc *.cnc)")
            << tr("Text files (*.txt)")
            << tr("All files (*.* *)");

    fileDialog->setNameFilters(filters);

    if(defaultMdiWindowProperites.syntaxH)
      fileDialog->setHighlightColors(defaultMdiWindowProperites.hColors);

    fileDialog->restoreState(fileDialogState);
    fileDialog->selectNameFilter(openFileFilter);


    if (fileDialog->exec() == QDialog::Accepted)
    {
       fileDialogState = fileDialog->saveState();
       openFileFilter = fileDialog->selectedNameFilter();


       QStringList list = fileDialog->selectedFiles();
       QStringList::Iterator it = list.begin();
       while( it != list.end() )
       {
          file.setFile (*it);
          QMdiSubWindow *existing = findMdiChild(*it);

          if((file.exists ()) && (file.isReadable ()) && !existing)
          {
             MdiChild *child = createMdiChild();
             if(child->loadFile(*it))
             {
                defaultMdiWindowProperites.cursorPos = 0;
                defaultMdiWindowProperites.readOnly = FALSE;
                //defaultMdiWindowProperites.maximized = FALSE;
                defaultMdiWindowProperites.geometry = QByteArray();
                defaultMdiWindowProperites.hColors.highlightMode = defaultHighlightMode(child->filePath());
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
       statusBar()->showMessage(tr("File loaded"), 2000);
    };

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::save()
{
    if(activeMdiChild() && activeMdiChild()->save())
      statusBar()->showMessage(tr("File saved"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::saveAs()
{
    if(activeMdiChild() && activeMdiChild()->saveAs())
      statusBar()->showMessage(tr("File saved"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::printFile()
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

void edytornc::cut()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->cut();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::copy()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->copy();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::findInFl()
{
   if(findFiles == NULL)
   {
      findFiles = new FindInFiles(splitter);
      qApp->processEvents();
      splitter->updateGeometry();

      if(defaultMdiWindowProperites.syntaxH)
         findFiles->setHighlightColors(defaultMdiWindowProperites.hColors);

      if(activeMdiChild())
         findFiles->setDir(QFileInfo(activeMdiChild()->currentFile()).canonicalPath());

      connect(findFiles, SIGNAL(fileClicket(QString)), this, SLOT(loadFoundedFile(QString)));

      qApp->processEvents();

      findFiles->hideDialog(false);

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

bool edytornc::findNext()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   bool found = false;
   QTextCursor cursor, cursorOld;
   QPalette palette;

   if(!findEdit->text().isEmpty() && hasMdiChild)
   {
      activeMdiChild()->highlightFindText(findEdit->text(),
                                          ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                          (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
      found = activeMdiChild()->textEdit->find(findEdit->text(),
                                ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
      if(!found)
      {
         cursor = activeMdiChild()->textEdit->textCursor();
         cursorOld = cursor;
         cursor.movePosition(QTextCursor::Start);
         activeMdiChild()->textEdit->setTextCursor(cursor);
         found = activeMdiChild()->textEdit->find(findEdit->text(),
                                   ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                   (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
         if(!found)
           activeMdiChild()->textEdit->setTextCursor(cursorOld);
      };


      palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

      if(found)
        findEdit->setPalette(QPalette());
      else
        findEdit->setPalette(palette);
      
      return found;
   };
   return false;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool edytornc::findPrevious()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   bool found = false;
   QTextCursor cursor, cursorOld;
   QPalette palette;

   if(!findEdit->text().isEmpty() && hasMdiChild)
   {
      activeMdiChild()->highlightFindText(findEdit->text(),
                                          ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                          (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
      found = activeMdiChild()->textEdit->find(findEdit->text(), QTextDocument::FindBackward |
                               ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                               (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
      if(!found)
      {
         cursor = activeMdiChild()->textEdit->textCursor();
         cursorOld = cursor;
         cursor.movePosition(QTextCursor::End);
         activeMdiChild()->textEdit->setTextCursor(cursor);
         found = activeMdiChild()->textEdit->find(findEdit->text(), QTextDocument::FindBackward |
                                   ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                   (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
         if(!found)
           activeMdiChild()->textEdit->setTextCursor(cursorOld);
      };

      palette.setColor(QPalette::Base, QColor(Qt::red).lighter(160));

      if(found)
        findEdit->setPalette(QPalette());
      else
        findEdit->setPalette(palette);

      return found;
   };
   return false;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replaceNext()
{
   bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->textEdit->isReadOnly());
   bool found = false;

   if(hasMdiChildNotReadOnly) //!replaceEdit->text().isEmpty() &&
   {
      if(activeMdiChild()->textEdit->textCursor().selectedText() == findEdit->text())
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
         cr.insertText(replaceEdit->text());
         cr.endEditBlock();
         activeMdiChild()->textEdit->setTextCursor(cr);
         findNext();
      };

   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replacePrevious()
{
   bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->textEdit->isReadOnly());
   bool found = false;

   if(hasMdiChildNotReadOnly) //!replaceEdit->text().isEmpty() &&
   {
      if(activeMdiChild()->textEdit->textCursor().selectedText() == findEdit->text())
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
         cr.insertText(replaceEdit->text());
         cr.endEditBlock();
         activeMdiChild()->textEdit->setTextCursor(cr);
         findPrevious();
      };

   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replaceAll()
{
   bool hasMdiChildNotReadOnly = ((activeMdiChild() != 0) && !activeMdiChild()->textEdit->isReadOnly());
   bool found = false;
   QTextCursor startCursor, cr;
   QTextCharFormat format;

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
         if(startCursor.position() == activeMdiChild()->textEdit->textCursor().position())
           break;
         qApp->processEvents();
      };
      QApplication::restoreOverrideCursor();
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::selAll()
{
   if(activeMdiChild())
      activeMdiChild()->textEdit->selectAll();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::config()
{
   _editor_properites opt;
   MdiChild *mdiChild;

   defaultMdiWindowProperites.tabbedMode = tabbedView;
   SetupDialog *setUpDialog = new SetupDialog(this, &defaultMdiWindowProperites);


   if(setUpDialog->exec() == QDialog::Accepted)
   {
      defaultMdiWindowProperites = setUpDialog->getSettings();

      foreach(QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder))
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

         tabbedView = defaultMdiWindowProperites.tabbedMode;
         if(tabbedView)
           mdiArea->setViewMode(QMdiArea::TabbedView);
         else
           mdiArea->setViewMode(QMdiArea::SubWindowView);

         mdiChild->setMdiWindowProperites(opt);
      };

   };
   delete setUpDialog;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::readOnly()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->setReadOnly(readOnlyAct->isChecked());
    updateMenus();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doBhc()
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

void edytornc::doInsertSpaces()
{
   if(activeMdiChild())
      activeMdiChild()->doInsertSpace();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doRemoveSpaces()
{
   if(activeMdiChild())
      activeMdiChild()->doRemoveSpace();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doDiffL()
{
   if(diffApp == NULL)
   {
      diffApp = new KDiff3App(splitter, "DiffApp");
   };

   if(diffApp != NULL)
   {
      diffAct->setChecked(true);
      if(activeMdiChild())
      {
         QString fileName = activeMdiChild()->currentFile();
         diffApp->completeInit("", fileName);
      };
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doDiffR()
{
   if(diffApp == NULL)
   {
      diffApp = new KDiff3App(splitter, "DiffApp");
   };

   if(diffApp != NULL)
   {
      diffAct->setChecked(true);
      if(activeMdiChild())
      {
         QString fileName = activeMdiChild()->currentFile();
         diffApp->completeInit(fileName, "");
      };
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doDiff()
{
   if(diffApp == NULL)
   {
      diffApp = new KDiff3App(splitter, "DiffApp");

      if(activeMdiChild())
      {
         QString fileName = activeMdiChild()->currentFile();
         diffApp->completeInit(QFileInfo(fileName).canonicalPath(), QFileInfo(fileName).canonicalPath());
      };
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

void edytornc::doRemoveEmptyLines()
{
   if(activeMdiChild())
      activeMdiChild()->doRemoveEmptyLines();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doInsertDot()
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

void edytornc::doRenumber()
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

void edytornc::doSpeedFeed()
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

void edytornc::doChamfer()
{
   ChamferDialog *chamferDialog = new ChamferDialog(this);
   chamferDialog->move((geometry().x() + width()) - chamferDialog->width(), geometry().y()+35);
   chamferDialog->show();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doTriangles()
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

void edytornc::doConvert()
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

void edytornc::doConvertProg()
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

void edytornc::doCalc()
{
   if(defaultMdiWindowProperites.calcBinary.isNull()  || defaultMdiWindowProperites.calcBinary.isEmpty())
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

void edytornc::deleteText()
{
    if(activeMdiChild())
      activeMdiChild()->textEdit->textCursor().removeSelectedText();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::paste()
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

void edytornc::undo()
{
   if(activeMdiChild())
   {
      activeMdiChild()->doUndo();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::redo()
{
   if(activeMdiChild())
   {
      activeMdiChild()->doRedo();
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::activeWindowChanged(QMdiSubWindow *window)
{
   Q_UNUSED(window);
   MdiChild *mdiChild;

   if(mdiArea->subWindowList().count() <= 1)
     defaultMdiWindowProperites.maximized = TRUE;

   mdiChild = activeMdiChild();
   if(mdiChild)
   {
      defaultMdiWindowProperites.maximized = mdiChild->parentWidget()->isMaximized();
      statusBar()->showMessage(mdiChild->currentFile(), 0);
   };
   updateCurrentSerialConfig();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::about()
{
   QMessageBox::about(this, tr("About EdytorNC"),
                            tr("The <b>EdytorNC</b> is text editor for CNC programmers.") +
                            tr("<P>Version: ") +
                               "2010.04" +
                            tr("<P>Copyright (C) 1998 - 2010 by <a href=\"mailto:artkoz@poczta.onet.pl\">Artur Koziol</a>") +
                            tr("<P>Catalan translation and deb package thanks to Jordi Sayol") +
                            tr("<br />German translation thanks to Michael Numberger") +
                            tr("<P><a href=\"http://sourceforge.net/projects/edytornc/\">http://sourceforge.net/projects/edytornc</a>") +
                            tr("<P>") +
                            tr("<P>Cross platform installer made by <a href=\"http://installbuilder.bitrock.com/\">BitRock InstallBuilder for Qt</a>") +
                            tr("<P>") +
                            tr("<P>EdytorNC wins <a href=\"http://www.softpedia.com/progClean/EdytorNC-Clean-144736.html/\">\"100% FREE award granted by Softpedia\"</a>") +
                            tr("<P>") +
                            tr("<P>EdytorNC contains pieces of code from other Open Source projects.") +
                            tr("<P>") +
                            tr("<P><i>EdytorNC is free software; you can redistribute it and/or modify"
                               "it under the terms of the GNU General Public License  as published by"
                               "the Free Software Foundation; either version 2 of the License, or"
                               "(at your option) any later version.</i>") +
                            tr("<P><i>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
                               "INCLUDING THE WARRANTY OF DESIGN,"
                               "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</i>"));
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateMenus()
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

   replaceAct->setEnabled(hasMdiChildNotReadOnly);
   readOnlyAct->setEnabled(hasMdiChild);
   renumberAct->setEnabled(hasMdiChildNotReadOnly);
   insertDotAct->setEnabled(hasMdiChildNotReadOnly);
   insertSpcAct->setEnabled(hasMdiChildNotReadOnly);
   removeSpcAct->setEnabled(hasMdiChildNotReadOnly);
   removeEmptyLinesAct->setEnabled(hasMdiChildNotReadOnly);
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

   updateStatusBar();

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateCurrentSerialConfig()
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

void edytornc::cancelUnderline()
{
   bool hasMdiChild = (activeMdiChild() != 0);

   if(hasMdiChild)
   {
//      QTextCharFormat format = activeMdiChild()->textEdit->currentCharFormat();
//
//      if((format.underlineStyle() != QTextCharFormat::NoUnderline) &&
//         !activeMdiChild()->textEdit->textCursor().hasSelection())
//      {
//         format.setUnderlineStyle(QTextCharFormat::NoUnderline);
//         activeMdiChild()->textEdit->setCurrentCharFormat(format);
//      };

      if(findToolBar != NULL)
        activeMdiChild()->highlightFindText(findEdit->text(),
                                           ((mCheckFindWholeWords->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)) |
                                           (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));
      else
        activeMdiChild()->highlightFindText("");
   };

   updateStatusBar();

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateStatusBar()
{
   QTextBlock b, cb;
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
      column = activeMdiChild()->textEdit->textCursor().position() - b.position();
      cb = activeMdiChild()->textEdit->textCursor().block();
      for(b = activeMdiChild()->textEdit->document()->begin(); b != activeMdiChild()->textEdit->document()->end(); b = b.next())
      {
         if(b == cb)
         {
            break;
         };
         line++;
      };

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

void edytornc::updateWindowMenu()
{
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

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) 
    {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->currentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->currentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

MdiChild *edytornc::createMdiChild()
{
    MdiChild *child = new MdiChild();
    mdiArea->addSubWindow(child);

    connect(child->textEdit, SIGNAL(copyAvailable(bool)), this, SLOT(updateMenus()));
    connect(child->textEdit, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));
    connect(child->textEdit, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));
    connect(child->textEdit, SIGNAL(textChanged()), this, SLOT(updateMenus()));
    connect(child->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cancelUnderline()));
    connect(child->textEdit, SIGNAL(modificationChanged(bool)), this, SLOT(updateStatusBar()));
    connect(child, SIGNAL(message(const QString&, int)), statusBar(), SLOT(message(const QString&, int)));

    //connect(child->textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    //connect(child->textEdit, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

    return child;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::createActions()
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

    openWPvAct = new QAction(QIcon(":/images/fileopen_preview.png"), tr("&Open file with preview"), this);
    openWPvAct->setStatusTip(tr("Open an existing file (Openfile dialog with preview)"));
    connect(openWPvAct, SIGNAL(triggered()), this, SLOT(openWithPreview()));

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




    closeAct = new QAction(QIcon(":/images/fileclose.png"), tr("Cl&ose"), this);
    closeAct->setShortcut(tr("Ctrl+F4"));
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

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
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(QIcon(":/images/go-previous.png"), tr("Pre&vious"), this);
    previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(activeWindowChanged(QMdiSubWindow *)));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);



    createToolTipsAct = new QAction(tr("&Create cnc tooltips"), this);
    createToolTipsAct->setStatusTip(tr("Create default cnc tooltips file"));
    connect(createToolTipsAct, SIGNAL(triggered()), this, SLOT(createToolTipsFile()));

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

void edytornc::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(openWPvAct);
    if(openExampleAct != NULL)
       fileMenu->addAction(openExampleAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();    
    fileMenu->addAction(findFilesAct);
    fileMenu->addSeparator();
    recentFileMenu = fileMenu->addMenu(tr("&Recent files"));
    recentFileMenu->setIcon(QIcon(":/images/document-open-recent.png"));
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
    editMenu->addAction(readOnlyAct);
    editMenu->addSeparator();
    editMenu->addAction(configAct);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(showSerialToolBarAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(insertSpcAct);
    toolsMenu->addAction(removeSpcAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(removeEmptyLinesAct);
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
    helpMenu->addAction(createToolTipsAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("File");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    //fileToolBar->addAction(openWPvAct);
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


    toolsToolBar = new QToolBar(tr("Tools"));
    addToolBar(Qt::LeftToolBarArea, toolsToolBar);
    toolsToolBar->setObjectName("Tools");
    toolsToolBar->addAction(showSerialToolBarAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(insertSpcAct);
    toolsToolBar->addAction(removeSpcAct);
    toolsToolBar->addSeparator();
    toolsToolBar->addAction(removeEmptyLinesAct);
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

void edytornc::createStatusBar()
{

   labelStat1 = new QLabel("    ");
   
   labelStat1->setFrameShadow(QFrame::Sunken);
   labelStat1->setFrameShape(QFrame::Box);

   //QLabel *highlightLabel = new QLabel();
   //highlightLabel->setText(tr("Highlight:"));
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

void edytornc::setHighLightMode(int mode)
{  
   bool ok;
   bool hasMdiChild = (activeMdiChild() != 0);

   int id = highlightTypeCombo->itemData(mode).toInt(&ok);
   if(hasMdiChild)
   {
      activeMdiChild()->setHighligthMode(id);
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::readSettings()
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

    openFileFilter = settings.value("FileOpenFilter", "*.nc").toString();

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
    tabbedView = settings.value("TabbedView", FALSE).toBool();
    defaultMdiWindowProperites.clearUndoHistory = settings.value("ClearUndoRedo", FALSE).toBool();
    defaultMdiWindowProperites.clearUnderlineHistory = settings.value("ClearUnderline", FALSE).toBool();
    defaultMdiWindowProperites.editorToolTips = settings.value("EditorToolTips", TRUE).toBool();

    defaultMdiWindowProperites.lineColor = settings.value("LineColor", 0xFEFFB6).toInt();
    defaultMdiWindowProperites.underlineColor = settings.value("UnderlineColor", 0x00FF00).toInt();


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


    settings.beginGroup("Highlight" );
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

    int max = settings.beginReadArray("LastDoc");
    for(int i = 0; i < max; ++i)
    {

       settings.setArrayIndex(i);
       defaultMdiWindowProperites.lastDir = lastDir.absolutePath();

       defaultMdiWindowProperites.fileName = settings.value("OpenedFile_" + QString::number(i)).toString();
       if(!defaultMdiWindowProperites.fileName.isEmpty())
       {
          defaultMdiWindowProperites.cursorPos = settings.value("Cursor_" + QString::number(i), 1).toInt();
          defaultMdiWindowProperites.readOnly = settings.value( "ReadOnly_" + QString::number(i), FALSE).toBool();
          defaultMdiWindowProperites.geometry = settings.value("Geometry_" + QString::number(i), QByteArray()).toByteArray();
          defaultMdiWindowProperites.hColors.highlightMode = settings.value("HighlightMode_" + QString::number(i), MODE_AUTO).toInt();
          loadFile(defaultMdiWindowProperites, false);
           
       };
        
    };
    settings.endArray();

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::writeSettings()
{
    MdiChild *mdiChild;
    bool maximized = false;
   
    QSettings settings("EdytorNC", "EdytorNC");

    //cleanup old settings
    settings.remove("LastDoc");


    settings.setValue("Pos", pos());
    settings.setValue("Size", size());


    settings.setValue("State", saveState());

    settings.setValue("LastDir", lastDir.path());
    
    settings.setValue("FileOpenFilter", openFileFilter);

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
    settings.setValue("TabbedView", tabbedView);
    settings.setValue("LineColor", defaultMdiWindowProperites.lineColor);
    settings.setValue("UnderlineColor", defaultMdiWindowProperites.underlineColor);
    settings.setValue("CalcBinary", defaultMdiWindowProperites.calcBinary);
    settings.setValue("ClearUndoRedo", defaultMdiWindowProperites.clearUndoHistory);
    settings.setValue("ClearUnderline", defaultMdiWindowProperites.clearUnderlineHistory);
    settings.setValue("EditorToolTips", defaultMdiWindowProperites.editorToolTips);
    
    settings.setValue("FileDialogState", fileDialogState);
    settings.setValue("RecentFiles", m_recentFiles);

    settings.setValue("SerialToolbarShown", (serialToolBar != NULL));


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


    settings.beginWriteArray("LastDoc");
    int i = 0;
    foreach(QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder))
    {
        mdiChild = qobject_cast<MdiChild *>(window->widget());
        _editor_properites Opt = mdiChild->getMdiWindowProperites();

        settings.setArrayIndex(i);
        settings.setValue("OpenedFile_" + QString::number(i), Opt.fileName);
        settings.setValue("Cursor_" + QString::number(i), Opt.cursorPos);
        settings.setValue("ReadOnly_" + QString::number(i), Opt.readOnly);
        settings.setValue("Geometry_" + QString::number(i), mdiChild->parentWidget()->saveGeometry());
        settings.setValue("HighlightMode_" + QString::number(i), Opt.hColors.highlightMode);
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

MdiChild *edytornc::activeMdiChild()
{
    if(QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
      return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

//**************************************************************************************************
//
//**************************************************************************************************

QMdiSubWindow *edytornc::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) 
    {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::setActiveSubWindow(QWidget *window)
{
    if(!window)
      return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::loadFile(_editor_properites options, bool checkAlreadyLoaded)
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

void edytornc::updateRecentFiles(const QString &filename)
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

void edytornc::fileOpenRecent(QAction *act)
{
    defaultMdiWindowProperites.readOnly = FALSE;
    //defaultMdiWindowProperites.maximized = FALSE;
    defaultMdiWindowProperites.cursorPos = 0;
    defaultMdiWindowProperites.editorToolTips = true;
    defaultMdiWindowProperites.fileName = m_recentFiles[act->data().toInt()];
    defaultMdiWindowProperites.hColors.highlightMode =  defaultHighlightMode(QFileInfo(defaultMdiWindowProperites.fileName).canonicalPath());
    loadFile(defaultMdiWindowProperites);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateRecentFilesMenu()
{
    QAction *newAct;
  
  
    recentFileMenu->clear();
    for(int i = 0; i < MAX_RECENTFILES; ++i)
    {
        if(i < int(m_recentFiles.size()))
        {
            newAct = recentFileMenu->addAction(QIcon(":/images/document-open-recent.png"), 
                                               QString( "&%1 - %2" ).arg( i + 1 ).arg(m_recentFiles[i]));
            connect(recentFileMenu, SIGNAL(triggered(QAction *)), this, SLOT(fileOpenRecent(QAction *)));
            newAct->setData(i);
        };
    }
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::loadFoundedFile(const QString &fileName)
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
       defaultMdiWindowProperites.readOnly = FALSE;
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

void edytornc::messReceived(const QString &text)
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

void edytornc::createFindToolBar()
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
      findEdit->installEventFilter(this);
      findToolBar->addWidget(findEdit);
      findToolBar->addAction(findPreviousAct);
      findToolBar->addAction(findNextAct);   
      findToolBar->addSeparator();

      replaceLabel = new QLabel(tr("Replace with:"));
      findToolBar->addWidget(replaceLabel);
      replaceEdit = new QLineEdit();
      replaceEdit->installEventFilter(this);
      findToolBar->addWidget(replaceEdit);
      findToolBar->addAction(replacePreviousAct);
      findToolBar->addAction(replaceNextAct);
      findToolBar->addAction(replaceAllAct);
      findToolBar->addSeparator();

      mCheckIgnoreCase = new QCheckBox(tr("Ignore c&ase"));
      mCheckIgnoreCase->setChecked(TRUE);
      connect(mCheckIgnoreCase, SIGNAL(clicked()), this, SLOT(findTextChanged()));
      findToolBar->addWidget(mCheckIgnoreCase);
      mCheckFindWholeWords = new QCheckBox(tr("&Whole words only"));
      connect(mCheckFindWholeWords, SIGNAL(clicked()), this, SLOT(findTextChanged()));
      findToolBar->addWidget(mCheckFindWholeWords);
      findToolBar->addSeparator();
      findToolBar->addAction(findCloseAct);
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
                                      (!mCheckIgnoreCase->isChecked() ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags(0))));

   findEdit->selectAll();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::closeFindToolBar()
{
   if(activeMdiChild())
   {
      activeMdiChild()->setFocus(Qt::MouseFocusReason);
      activeMdiChild()->highlightFindText("");
      activeMdiChild()->textEdit->centerCursor();
   };
   findToolBar->close();
   findToolBar = NULL;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::findTextChanged()
{
   bool hasMdiChild = (activeMdiChild() != 0);
   QTextCursor cursor;
   int pos;

   if(hasMdiChild)
   {
      if(!findEdit->text().isEmpty())
      {
         cursor = activeMdiChild()->textEdit->textCursor();
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

bool edytornc::eventFilter(QObject *obj, QEvent *ev)
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

       return FALSE;
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

void edytornc::createSerialToolBar()
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

void edytornc::closeSerialToolbar()
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

void edytornc::attachToDirButtonClicked(bool attach)
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

void edytornc::deAttachToDirButtonClicked()
{
   attachToDirButtonClicked(false);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::serialConfig()
{
   SPConfigDialog *serialConfigDialog = new SPConfigDialog(this, configBox->currentText());

   if(serialConfigDialog->exec() == QDialog::Accepted)
      loadSerialConfignames();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::loadSerialConfignames()
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

void edytornc::serialConfigTest()
{
   TransmissionDialog *trDialog = new TransmissionDialog(this);

   trDialog->show();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::loadConfig()
{
    QString port, fTx, nTx;
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

    sendStartDelay = settings.value("SendingStartDelay", 0).toInt();


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

void edytornc::lineDelaySlot()
{
   readyCont = true;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::sendButtonClicked()
{
   int i, bytesToWrite;
   QString tx;
   QTextCursor cursor, prevCursor;
   MdiChild *activeWindow;
   char controlChar;
   QTimer *sendStartDelayTimer = NULL;


   bytesToWrite = 0;

   activeWindow = activeMdiChild();
   if(!(activeWindow != 0))
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


   qDebug() << "xoffReceived: " << xoffReceived << "sendStartDelayTimer: " << sendStartDelay;

   i = 0;
   while(i < tx.size())
   {
      if(sendStartDelay > 0)
         progressDialog.setLabelText(tr("Start in %1s").arg(sendStartDelay));
      else
         if(xoffReceived)
            progressDialog.setLabelText(tr("Waiting for a signal readiness..."));;

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
               qDebug() << "Recived control char: " << QString("%1").arg((int)controlChar, 0, 16);
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
            //showError(comPort->lastError());
            //break;
         };

         if(tx[i].toAscii() != '\r')
           cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

         activeWindow->textEdit->setTextCursor(cursor);
         progressDialog.setValue(i);
         progressDialog.setLabelText(tr("Sending byte %1 of %2").arg(i + 1).arg(tx.size()));
         qApp->processEvents();

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
      qDebug() << "xoffReceived: " << xoffReceived << " bytes:" << bytesToWrite;
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

void edytornc::sendStartDelayTimeout()
{
   if(sendStartDelay > 0)
      sendStartDelay--;
   else
      xoffReceived = false;

   qDebug() << "xoffReceived: " << xoffReceived << "sendStartDelayTimer: " << sendStartDelay;

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::receiveButtonClicked()
{
   QString tx;
   int count, i, j;
   char buf[1024];
   MdiChild *activeWindow;


   showError(E_NO_ERROR);
   count = 0;

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

   i = configBox->currentIndex();
   newFile();
   activeWindow = activeMdiChild();
   if(!(activeWindow != 0))
     return;
   configBox->setCurrentIndex(i);

   receiveAct->setEnabled(FALSE);
   sendAct->setEnabled(FALSE);
   QApplication::setOverrideCursor(Qt::BusyCursor);

   TransProgressDialog progressDialog(this);
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
         qDebug() << "Bytes available: " << i;
         i = comPort->readLine(buf, sizeof(buf) - 1);  //readLine

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

         activeWindow->textEdit->ensureCursorVisible();
         qApp->processEvents();

      };

      if(stop)
      {
         if(!tx.isEmpty())
            activeWindow->textEdit->insertPlainText(tx);
         break;
      };

      progressDialog.setValue(count);
      qApp->processEvents();

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
   QApplication::restoreOverrideCursor();

   if(activeWindow)
     if(activeWindow->textEdit->document()->isEmpty())
     {
        activeWindow->parentWidget()->close();
     };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::stopButtonClicked()
{
   stop = true;
   qApp->processEvents();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::showError(int error)
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

void edytornc::doCmpMacro()
{
   if(activeMdiChild())
     activeMdiChild()->compileMacro();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::createToolTipsFile()
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
   settings.setValue("M44", tr("<b>M42</b> - spindle gear range 4"));
   settings.setValue("M48", tr("<b>M48</b> - spindle speed override ignore cancel"));
   settings.setValue("M49", tr("<b>M49</b> - spindle speed override ignore"));

   settings.setValue("M52", tr("<i>v.M</i> <b>M52</b> - mode of return to upper limit level") +
                            tr("<br /><i>v.L</i> <b>M52</b> - "));
   settings.setValue("M53", tr("<i>v.M</i> <b>M53</b> - mode of return to a specified point level set by G71") +
                            tr("<br /><i>v.L</i> <b>M52</b> - "));
   settings.setValue("M54", tr("<i>v.M</i> <b>M54</b> - mode of return to the point R level") +
                            tr("<br /><i>v.L</i> <b>M52</b> - "));

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

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::attachHighlighterToDirButtonClicked(bool attach)
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

void edytornc::attachHighlightToDirActClicked()
{
   attachHighlighterToDirButtonClicked(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::deAttachHighlightToDirActClicked()
{
   attachHighlighterToDirButtonClicked(false);
}

//**************************************************************************************************
//
//**************************************************************************************************

int edytornc::defaultHighlightMode(QString filePath)
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

