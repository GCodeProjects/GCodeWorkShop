/***************************************************************************
 *   Copyright (C) 2008 by Artur Kozioł                                    *
 *   artkoz@poczta.onet.pl                                                 *
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

#include <QtGui>

#include "edytornc.h"
#include "mdichild.h"




//**************************************************************************************************
//
//**************************************************************************************************

edytornc::edytornc()
{
    setAttribute(Qt::WA_DeleteOnClose);

    clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(updateMenus()));

    mdiArea = new QMdiArea;
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    searchOptions.options = 0;
    searchOptions.cursor.setPosition(0);
    searchOptions.fromCursor = TRUE;

    readSettings();

    setWindowTitle(tr("EdytorNC"));
    setWindowIcon(QIcon(":/images/edytornc.png"));
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

    //foreach (QMdiSubWindow *window, mdiArea->subWindowList()) 
   // {
   //     MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
   //     if (mdiChild->currentFile() == canonicalFilePath)
   //         return window;
   // }

    writeSettings();
    mdiArea->closeAllSubWindows();

    if(currentMdiChild()) 
    {
       event->ignore();
    } 
    else 
    {  
       event->accept();
    }
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    defaultMdiWindowProperites.cursorPosX = 0;
    defaultMdiWindowProperites.readOnly = FALSE;
    child->setMdiWindowProperites(defaultMdiWindowProperites);
    child->show();
}


//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::open()
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
                child->setMdiWindowProperites(defaultMdiWindowProperites);
                child->show();
                updateRecentFiles(*it);
             } 
             else 
             {
                child->close();
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
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::cut()
{
    if(activeMdiChild())
      activeMdiChild()->cut();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::copy()
{
    if(activeMdiChild())
      activeMdiChild()->copy();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::findInFl()
{

   if((findFiles = findChild<FindInFiles *>()) != 0)
   { 
      return;
   };
   findFiles = new FindInFiles(this);

   if(defaultMdiWindowProperites.syntaxH)
     findFiles->setHighlightColors(defaultMdiWindowProperites.hColors);

   if(activeMdiChild())
     findFiles->setDir(QFileInfo(activeMdiChild()->currentFile()).canonicalPath());

   connect(findFiles, SIGNAL(fileClicket(QString)), this, SLOT(loadFoundedFile(QString)));
   findFiles->show();


}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::find()
{
   if(currentMdiChild())
   {  
      searchOptions.expr = currentMdiChild()->textCursor().selectedText();
   }
   else
     return;

   FindDialog *findDialog = new FindDialog(this);

   findDialog->setFindOpt(searchOptions);
   if(findDialog->exec() == QDialog::Accepted)
   {
      findDialog->getFindOpt(searchOptions);
      findSlot(searchOptions, true);
   };

   delete findDialog;

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::findSlot(SSearchOptions &s, bool findFirst)
{
    
    MdiChild *mdiChild = currentMdiChild();  
    if(!mdiChild) 
      return;

    if((s.fromCursor || !findFirst) || (s.options & QTextDocument::FindBackward))
    {
       s.cursor = currentMdiChild()->textCursor();
       s.cursor = mdiChild->document()->find(s.expr, s.cursor, s.options);
    }
    else
    {
       s.cursor.setPosition(0);
       s.cursor = mdiChild->document()->find(s.expr, s.cursor, s.options);
    };

    searchOptions = s;
    
    if(!s.cursor.isNull())
    {  
       mdiChild->setTextCursor(s.cursor);
    }
    else
      QMessageBox::information( this, tr("EdytorNC"), QString(tr("Can't find : '%1'.")).arg(searchOptions.expr));

    findNextAct->setEnabled(!s.cursor.isNull());
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::findNext()
{
   findSlot(searchOptions, false);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replaceSlot(bool &found)
{
   MdiChild *mdiChild = currentMdiChild();
   if(mdiChild)
   {
      if(searchOptions.cursor != currentMdiChild()->textCursor()) 
      {
         replaceFindSlot(found);
         return;
      };
      if(!searchOptions.cursor.isNull())
      {  
         //searchOptions.cursor = currentMdiChild()->textCursor();
         searchOptions.cursor.beginEditBlock();
         searchOptions.cursor.removeSelectedText();
         searchOptions.cursor.insertText(searchOptions.replaceText);
         searchOptions.cursor.endEditBlock();
         mdiChild->setTextCursor(searchOptions.cursor);
      };
      replaceFindSlot(found);
      if(found)
        mdiChild->setTextCursor(searchOptions.cursor);  
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replaceFindSlot(bool &found)
{
   QTextCursor curTmp;    

   MdiChild *mdiChild = currentMdiChild();
   if(mdiChild)
   { 
      curTmp = searchOptions.cursor;
      searchOptions.cursor = mdiChild->document()->find(searchOptions.expr, currentMdiChild()->textCursor(), 
                                                        searchOptions.options); 
      found = !searchOptions.cursor.isNull();
      if(found)
        mdiChild->setTextCursor(searchOptions.cursor);
      else 
        searchOptions.cursor = curTmp; 
         
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replaceAllSlot(bool &found)
{
   MdiChild *mdiChild = currentMdiChild();
   
   if(mdiChild)
   {
      if(searchOptions.cursor != currentMdiChild()->textCursor()) 
      {
         replaceFindSlot(found);
         return;
      };  
      do
      {
         if(!searchOptions.cursor.isNull())
         {  
            searchOptions.cursor = currentMdiChild()->textCursor();
            searchOptions.cursor.beginEditBlock();
            searchOptions.cursor.removeSelectedText();
            searchOptions.cursor.insertText(searchOptions.replaceText);
            searchOptions.cursor.endEditBlock();
            mdiChild->setTextCursor(searchOptions.cursor);
         };
         replaceFindSlot(found);;
         if(found)
         mdiChild->setTextCursor(searchOptions.cursor); 
      }while(found);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::replace()
{
    QString tx;

    MdiChild *mdiChild = currentMdiChild();

    if(!mdiChild)
      return;


    tx = mdiChild->textCursor().selectedText();
    if(!tx.isEmpty())
      searchOptions.expr = tx;

    ReplaceDialog *replaceDialog = new ReplaceDialog(this);

    replaceDialog->setFindOpt(searchOptions);
    if(replaceDialog->exec() == QDialog::Accepted)
    {
       replaceDialog->getFindOpt(searchOptions);

       if(searchOptions.fromCursor)
       {
          searchOptions.cursor = currentMdiChild()->textCursor();
          searchOptions.cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
          searchOptions.cursor = mdiChild->document()->find(searchOptions.expr, searchOptions.cursor, searchOptions.options);
       }
       else
       {
          searchOptions.cursor.setPosition(0);
          searchOptions.cursor = mdiChild->document()->find(searchOptions.expr, searchOptions.cursor, searchOptions.options);
       }

       if(!searchOptions.cursor.isNull())
       {
          mdiChild->setTextCursor(searchOptions.cursor);
          ReplaceConfirmDialog *confirmationDialog = new ReplaceConfirmDialog(&QString(tr("Searching text '%1' was found. What to do now ?").arg(searchOptions.expr)), this);

          connect( confirmationDialog, SIGNAL(replaceSignal(bool &)), this, SLOT(replaceSlot(bool &)));
          connect( confirmationDialog, SIGNAL(replaceAllSignal(bool &)), this, SLOT(replaceAllSlot(bool &)));
          connect( confirmationDialog, SIGNAL(findNextSignal(bool &)), this, SLOT(replaceFindSlot(bool &)));

          confirmationDialog->show();
       }
       else
         QMessageBox::information( this, tr("EdytorNc"), QString(tr("Unable to find : '%1'.")).arg(searchOptions.expr));
    };

    delete replaceDialog;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::selAll()
{
   if(activeMdiChild())
      activeMdiChild()->selectAll();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::zoomIn()
{
   if(activeMdiChild())
      activeMdiChild()->zoomIn();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::zoomOut()
{
   if(activeMdiChild())
      activeMdiChild()->zoomOut();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::config()
{
   
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::readOnly()
{
    if(activeMdiChild())
      activeMdiChild()->setReadOnly(readOnlyAct->isChecked());
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

   int startAt, inc, from, prec, mode;
   bool renumEmpty, renumComm;
   MdiChild *child;
   
   child = activeMdiChild();
   
   RenumberDialog *renumberDialog = new RenumberDialog(this);
   //renumberDialog->setState(editorOpt.dotAdr, editorOpt.atEnd, editorOpt.dotAfter, editorOpt.dotAftrerCount);

   if(renumberDialog->exec() == QDialog::Accepted)
   {
      renumberDialog->getState(mode, startAt, from, prec, inc, renumEmpty, renumComm);
      if(child)
      {
         inc = child->doRenumber(mode, startAt, from, prec, inc, renumEmpty, renumComm);
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

      feedsDialog->show();
      feedsDialog->move((geometry().x() + width() - 10) - feedsDialog->width(), geometry().y()+35);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doChamfer()
{
   
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

      triangleDialog->show();
      triangleDialog->move((geometry().x() + width() - 10) - triangleDialog->width(), geometry().y()+35);
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

      i2MDialog->show();
      i2MDialog->move((geometry().x() + width() - 10) - i2MDialog->width(), geometry().y()+35);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doConvertProg()
{
   
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::doCalc()
{
   proc = findChild<QProcess *>();
   QString name = "";

#ifdef Q_OS_LINUX
   name = "kcalc";
#endif

#ifdef Q_OS_WIN32
   name = "calc.exe";
#endif


   //proc = (QProcess *) qt_find_obj_child(this, "QProcess", "_calc_procces_");
   if(!proc && !name.isNull())
   {
      proc = new QProcess(this);
      proc->start(name);


      //proc->setCommunication(QProcess::Stdout);
      //proc->start();
   }
   else
     if(!proc->pid() && !name.isNull())
       proc->start(name);

}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::deleteText()
{
    if(activeMdiChild())
      activeMdiChild()->textCursor().removeSelectedText();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::undo()
{
    if (activeMdiChild())
        activeMdiChild()->document()->undo();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::redo()
{
    if (activeMdiChild())
        activeMdiChild()->document()->redo();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::activeWindowChanged(QMdiSubWindow * window)
{
    if(activeMdiChild())
      statusBar()->showMessage(activeMdiChild()->currentFile(), 0);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::about()
{
   QMessageBox::about(this, tr("About EdytorNC"),
                            tr("The <b>EdytorNC</b> is text editor for CNC programmers."
                               "<P>Copyright (C) 1998 - 2009 by <urllink href='mailto:artkoz@poczta.onet.pl'>Artur Koziol</urllink> (artkoz@poczta.onet.pl)"
                               "<P>http://sourceforge.net/projects/edytornc"
                               "<P><i>This program is free software; you can redistribute it and/or modify"
                               "it under the terms of the GNU General Public License.</i>"));
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    bool hasMdiChildNotReadOnly = (hasMdiChild && !activeMdiChild()->isReadOnly());
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
    if(!hasMdiChild)
      findNextAct->setEnabled(FALSE);
    replaceAct->setEnabled(hasMdiChildNotReadOnly);
    zoomInAct->setEnabled(hasMdiChild);
    zoomOutAct->setEnabled(hasMdiChild);
    readOnlyAct->setEnabled(hasMdiChild);
    renumberAct->setEnabled(hasMdiChildNotReadOnly);
    insertDotAct->setEnabled(hasMdiChildNotReadOnly);
    insertSpcAct->setEnabled(hasMdiChildNotReadOnly);
    removeSpcAct->setEnabled(hasMdiChildNotReadOnly);
    convertProgAct->setEnabled(hasMdiChildNotReadOnly);


    if(activeMdiChild() && activeMdiChild()->isReadOnly())
    {
       readOnlyAct->setChecked(TRUE);
       readOnlyAct->setIcon(QIcon(":/images/lock.png"));
    }
    else
    {
       readOnlyAct->setChecked(FALSE);
       readOnlyAct->setIcon(QIcon(":/images/unlock.png"));
    };

    bool hasSelection = (activeMdiChild() && activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection && hasMdiChildNotReadOnly);
    copyAct->setEnabled(hasSelection);
    deleteAct->setEnabled(hasSelection && hasMdiChildNotReadOnly);
 
    pasteAct->setEnabled((!clipboard->text().isEmpty()) && hasMdiChildNotReadOnly);


    //undoAct->setEnabled(activeMdiChild() && activeMdiChild()->document()->isUndoAvailable());
    //redoAct->setEnabled(activeMdiChild() && activeMdiChild()->document()->isRedoAvailable());
 

    
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
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
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
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(redoAvailable(bool)), redoAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(undoAvailable(bool)), undoAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(textChanged()), this, SLOT(updateMenus()));
    connect(child, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

    return child;
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::createActions()
{
    newAct = new QAction(QIcon(":/images/filenew.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/filesaveas.png"), tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    
    findFilesAct = new QAction(QIcon(":/images/filefind.png"), tr("&Find files..."), this);
    //openAct->setShortcut(tr("Ctrl+"));
    findFilesAct->setStatusTip(tr("Find files"));
    connect(findFilesAct, SIGNAL(triggered()), this, SLOT(findInFl()));

  
    
    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    undoAct->setStatusTip(tr("Undo last operation"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
    undoAct->setEnabled(FALSE);

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcut(tr("Ctrl+Shift+Z"));
    redoAct->setStatusTip(tr("Redo last operation"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));
    redoAct->setEnabled(FALSE);

    cutAct = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));


    findAct = new QAction(QIcon(":/images/find.png"), tr("&Find"), this);
    findAct->setShortcut(tr("Ctrl+F"));
    findAct->setStatusTip(tr("Find text"));
    connect(findAct, SIGNAL(triggered()), this, SLOT(find()));

    findNextAct = new QAction(QIcon(":/images/findnext.png"), tr("Find &next"), this);
    findNextAct->setShortcut(tr("F3"));
    findNextAct->setStatusTip(tr("Find next text"));
    findNextAct->setEnabled(FALSE);
    connect(findNextAct, SIGNAL(triggered()), this, SLOT(findNext()));

    replaceAct = new QAction(QIcon(":/images/replace.png"), tr("&Replace"), this);
    replaceAct->setShortcut(tr("Ctrl+R"));
    replaceAct->setStatusTip(tr("Find and replace text"));
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(replace()));



    deleteAct = new QAction(QIcon(":/images/editdelete.png"), tr("&Delete"), this);
    //deleteAct->setShortcut(tr("Del"));
    deleteAct->setStatusTip(tr("Removes selected text"));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteText()));
    
    selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"), this);
    selAllAct->setShortcut(tr("Ctrl+A"));
    selAllAct->setStatusTip(tr("Select all text"));
    connect(selAllAct, SIGNAL(triggered()), this, SLOT(selAll()));

    readOnlyAct = new QAction(QIcon(":/images/unlock.png"), tr("Read &only"), this);
    readOnlyAct->setShortcut(tr("F12"));
    readOnlyAct->setCheckable(TRUE);
    readOnlyAct->setStatusTip(tr("Makes text read only"));
    connect(readOnlyAct, SIGNAL(triggered()), this, SLOT(readOnly()));

    zoomInAct = new QAction(QIcon(":/images/zoom-in.png"), tr("&Zoom in"), this);
    zoomInAct->setShortcut(tr("Ctrl+PgUp"));
    zoomInAct->setStatusTip(tr("Incrase font size"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom out"), this);
    zoomOutAct->setShortcut(tr("Ctrl+PgDown"));
    zoomOutAct->setStatusTip(tr("Decrease font size"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    configAct = new QAction(QIcon(":/images/configure.png"), tr("Configuration"), this);
    //configAct->setShortcut(tr("Ctrl+R"));
    configAct->setStatusTip(tr("Open configuration dialog"));
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(config()));




    bhcAct = new QAction(QIcon(":/images/bhc.png"), tr("&BHC"), this);
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

    insertDotAct = new QAction(QIcon(":/images/dots.png"), tr("Insert dots"), this);
    insertDotAct->setShortcut(tr("F6"));
    insertDotAct->setStatusTip(tr("Incrase font size"));
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
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(activeWindowChanged(QMdiSubWindow *)));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);



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
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(findFilesAct);
    fileMenu->addSeparator();
    recentFileMenu = fileMenu->addMenu(tr("&Recent files"));
    recentFileMenu->setIcon(QIcon(":/images/document-open-recent.png"));
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
    editMenu->addAction(findNextAct);
    editMenu->addAction(replaceAct);
    editMenu->addSeparator();
    editMenu->addAction(readOnlyAct);
    editMenu->addSeparator();
    editMenu->addAction(zoomInAct);
    editMenu->addAction(zoomOutAct);
    editMenu->addSeparator();
    editMenu->addAction(configAct);



    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(insertSpcAct);
    toolsMenu->addAction(removeSpcAct);
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
    toolsMenu->addAction(calcAct);
 


    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
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
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(findFilesAct);

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
    editToolBar->addAction(findNextAct);
    editToolBar->addSeparator();
    editToolBar->addAction(replaceAct);
    editToolBar->addSeparator();
    editToolBar->addAction(zoomInAct);
    editToolBar->addAction(zoomOutAct);


    toolsToolBar = new QToolBar(tr("Tools"));
    addToolBar(Qt::RightToolBarArea,toolsToolBar);
    toolsToolBar->setObjectName("Tools");
    toolsToolBar->addAction(insertSpcAct);
    toolsToolBar->addAction(removeSpcAct);
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
    toolsToolBar->addAction(calcAct);


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
    statusBar()->showMessage(tr("Ready"));
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::readSettings()
{
    QSettings settings("Trolltech", "EdytorNC");

    QPoint pos = settings.value("Pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("Size", QSize(500, 700)).toSize();
    move(pos);
    resize(size);


    restoreState(settings.value("State", QByteArray()).toByteArray());


    //lastdir = settings.readEntry("LastDir", QString(getenv("HOME")) );

    openFileFilter = settings.value("FileOpenFilter", "*.nc").toString();

    defaultMdiWindowProperites.dotAdr = settings.value("DotAddress", "XYZB").toString();
    defaultMdiWindowProperites.dotAftrerCount = settings.value("DotAfterCount", 1000).toInt();
    defaultMdiWindowProperites.atEnd = settings.value("DotAtEnd", TRUE ).toBool();
    defaultMdiWindowProperites.dotAfter = settings.value("DotAfter", FALSE).toBool();

    defaultMdiWindowProperites.i2mAdr = settings.value("I2MAddress", "XYZB").toString();
    defaultMdiWindowProperites.i2mprec = settings.value("I2MPrec", 3).toInt();
    defaultMdiWindowProperites.inch = settings.value("I2M", TRUE ).toBool();

    defaultMdiWindowProperites.fontName = settings.value("FontName", "Courier").toString();
    defaultMdiWindowProperites.fontSize = settings.value("FontSize", 14 ).toInt();
    defaultMdiWindowProperites.intCapsLock = settings.value("IntCapsLock", TRUE ).toBool();

    fileDialogState = settings.value("FileDialogState", QByteArray()).toByteArray(); 

    m_recentFiles = settings.value( "RecentFiles").toStringList();
    updateRecentFilesMenu();


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


    settings.endGroup();


    settings.beginGroup("LastDoc" );

    defaultMdiWindowProperites.cursorPosX = 0;
    defaultMdiWindowProperites.readOnly = FALSE;

    int max = settings.value("OpenedFileCount", 0 ).toInt();
    for(int i = 1; i < max; ++i) 
    {
        //defaultMdiWindowProperites.lastDir = lastdir.absPath(); 

        defaultMdiWindowProperites.fileName = settings.value( "OpenedFile_" + QString::number(i) ).toString();
        if(!defaultMdiWindowProperites.fileName.isEmpty())
        {
           defaultMdiWindowProperites.cursorPosX = settings.value( "CursorX_" + QString::number(i), 1).toInt();
           //defaultMdiWindowProperites.cursorPosY = settings.value( "CursorY_" + QString::number(i), 1).toInt();
           defaultMdiWindowProperites.readOnly = settings.value( "ReadOnly_" + QString::number(i), FALSE).toBool();
           defaultMdiWindowProperites.geometry = settings.value("Geometry_" + QString::number(i), QByteArray()).toByteArray();
           loadFile(defaultMdiWindowProperites);

           //pos = settings.value("Pos_" + QString::number(i), QPoint(0, 0)).toPoint();
           //size = settings.value("Size_" + QString::number(i), QSize(200, 200)).toSize();
           
           
           
           //QMdiSubWindow *existing = findMdiChild(defaultMdiWindowProperites.fileName);
           //if(existing)
           //{
              //existing->move(pos);
              //existing->resize(size);
              //existing->restoreGeometry(geom);
           //};

           
        };
        
        settings.remove( "OpenedFile_" + QString::number(i));
        settings.remove( "CursorX_" + QString::number(i));
        settings.remove( "CursorY_" + QString::number(i));
        settings.remove( "ReadOnly_" + QString::number(i));
        settings.remove( "Pos_" + QString::number(i));
        settings.remove( "Size_" + QString::number(i));
        settings.remove( "Geometry_" + QString::number(i));

    };

    //QMdiSubWindow *existing = findMdiChild(defaultMdiWindowProperites.fileName);
    //if(existing)
    //{
       //if(settings.value("MdiMaximized", TRUE).toBool())
         //existing->showMaximized(); 
    //}; 
    

    settings.endGroup();



}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::writeSettings()
{
    int i = 1;
    MdiChild *mdiChild;
   
    QSettings settings("Trolltech", "EdytorNC");

    settings.setValue("Pos", pos());
    settings.setValue("Size", size());


    settings.setValue("State", saveState());


    //settings.setValue("LastDir", lastdir.path());
    
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
    
    settings.setValue("FileDialogState", fileDialogState);
    settings.setValue( "RecentFiles", m_recentFiles);

    //settings.endGroup();

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

    settings.endGroup();

    settings.beginGroup("LastDoc" );

    foreach (QMdiSubWindow *window, mdiArea->subWindowList(QMdiArea::StackingOrder)) 
    {
        mdiChild = qobject_cast<MdiChild *>(window->widget());
        _editor_properites Opt = mdiChild->getMdiWindowProperites();

        settings.setValue( "OpenedFile_" + QString::number(i), Opt.fileName);
        settings.setValue( "CursorX_" + QString::number(i), Opt.cursorPosX);
        //settings.setValue("CursorY_" + QString::number(i), Opt.cursorPosY);
        settings.setValue( "ReadOnly_" + QString::number(i), Opt.readOnly);
   
        //settings.setValue("Pos_" + QString::number(i), mdiChild->pos());
        //settings.setValue("Size_" + QString::number(i), mdiChild->size());
        //settings.setValue("Pos_" + QString::number(i), Opt.winPos);

        settings.setValue("Geometry_" + QString::number(i), mdiChild->saveGeometry());


        //settings.setValue("MdiMaximized", mdiChild->isMaximized());
        i++;
    };
    //if(activeMdiChild())
      //settings.setValue("MdiMaximized", activeMdiChild()->isMaximized());
    
    settings.setValue( "OpenedFileCount", (i)); 
    settings.endGroup();

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

MdiChild *edytornc::currentMdiChild()
{
    if(QMdiSubWindow *currentSubWindow = mdiArea->currentSubWindow())
      return qobject_cast<MdiChild *>(currentSubWindow->widget());
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

void edytornc::loadFile(_editor_properites options)
{
    QFileInfo file;

    QMdiSubWindow *existing = findMdiChild(options.fileName);
    if(existing)
    {
       mdiArea->setActiveSubWindow(existing);
       return;
    };
    file.setFile(options.fileName);
    if((file.exists()) && (file.isReadable()))
    {
       MdiChild *child = createMdiChild();
       child->newFile();
       child->loadFile(options.fileName);
       child->setMdiWindowProperites(options);
       child->show();
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateRecentFiles( const QString &filename )
{
    //if(m_recentFiles.indexOf( filename ) != m_recentFiles.end())
      //return;

    m_recentFiles.prepend( filename );
    if ( m_recentFiles.size() > MAX_RECENTFILES )
        m_recentFiles.removeLast();

    updateRecentFilesMenu();
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::fileOpenRecent( QAction *act )
{
    defaultMdiWindowProperites.readOnly = FALSE;
    defaultMdiWindowProperites.cursorPosX = 0;
    //defaultMdiWindowProperites.cursorPosY = 0;
    defaultMdiWindowProperites.fileName = m_recentFiles[act->data().toInt()];
    loadFile(defaultMdiWindowProperites);
}

//**************************************************************************************************
//
//**************************************************************************************************

void edytornc::updateRecentFilesMenu()
{
    QAction *newAct;
  
  
    recentFileMenu->clear();
    for ( int i = 0; i < MAX_RECENTFILES; ++i )
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
       defaultMdiWindowProperites.cursorPosX = 0;
       defaultMdiWindowProperites.readOnly = FALSE;
       child->setMdiWindowProperites(defaultMdiWindowProperites);
       child->show();
    };
}

//**************************************************************************************************
//
//**************************************************************************************************



//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************

