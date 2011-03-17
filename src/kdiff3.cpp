/***************************************************************************
    orginal file:        kdiff3.cpp  -  description
                             -------------------
    begin                : Don Jul 11 12:31:29 CEST 2002
    copyright            : (C) 2002-2007 by Joachim Eibl
    email                : joachim.eibl at gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "difftextwindow.h"

#include <iostream>
#include <assert.h>

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qsplitter.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <QMenu>
#include <QLabel>
#include <QTextEdit>
#include <QLayout>
#include <QPaintDevice>
#include <QStatusBar>
#include <QDesktopWidget>
#include <QPrinter>
#include <QPrintDialog>



// application specific includes
#include "kdiff3.h"
#include "optiondialog.h"
#include "fileaccess.h"



bool KDiff3App::isFileSaved()
{
   return m_bFileSaved;
}

bool KDiff3App::isDirComparison()
{
   return m_bDirCompare;
}

KDiff3App::KDiff3App(QWidget* pParent, const char* , QStringList extensions): QWidget(pParent)
{
   setObjectName( "KDiff3App" );
   setAttribute(Qt::WA_DeleteOnClose);

   setUpdatesEnabled(false);

   m_pCornerWidget = 0;
   m_pDiffTextWindow1 = 0;
   m_pDiffTextWindow2 = 0;
   m_pDiffTextWindowFrame1 = 0;
   m_pDiffTextWindowFrame2 = 0;
   m_pDiffWindowSplitter = 0;
   m_pOverview = 0;
   m_bTripleDiff = false;
   m_pMergeResultWindow = 0;
   m_bOutputModified = false;
   m_bFileSaved = false;
   m_bTimerBlock = false;
   m_pHScrollBar = 0;
   m_pDiffVScrollBar = 0;
   m_pMergeVScrollBar = 0;
   m_extensions = extensions;

   // Needed before any file operations via FileAccess happen.
   if (!g_pProgressDialog)
   {
      g_pProgressDialog = new ProgressDialog(0);
      g_pProgressDialog->setStayHidden( true );
   }

   // All default values must be set before calling readOptions().
   m_pOptionDialog = new OptionDialog(this);


   m_sd1.setOptionDialog(m_pOptionDialog);
   m_sd2.setOptionDialog(m_pOptionDialog);


   slotRefresh();
   initView();
}


void KDiff3App::completeInit( const QString& fn1, const QString& fn2)
{
   if(!fn1.isEmpty())
   {
      m_sd1.setFilename(fn1);
      m_sd1.setAliasName(fn1);

   }
   else
   {
      if(!fn2.isEmpty() && m_sd1.isEmpty())
      {
         QString fileName1 = QFileInfo(fn2).canonicalPath();
         m_sd1.setFilename(fileName1);
         m_sd1.setAliasName(fileName1);
      };
   };

   if(!fn2.isEmpty())
   {
      m_sd2.setFilename(fn2);
      m_sd2.setAliasName(fn2);
   }
   else
   {
      if(!fn1.isEmpty() && m_sd2.isEmpty())
      {
         QString fileName2 = QFileInfo(fn1).canonicalPath();
         m_sd2.setFilename(fileName2);
         m_sd2.setAliasName(fileName2);
      };
   };

   init(false);
   slotUpdateAvailabilities();
}

KDiff3App::~KDiff3App()
{

}



