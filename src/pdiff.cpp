/***************************************************************************
                         pdiff.cpp  -  Implementation for class KDiff3App
                         ---------------
    begin                : Mon March 18 20:04:50 CET 2002
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
//#include "mergeresultwindow.h"
//#include "directorymergewindow.h"
//#include "smalldialogs.h"

#include <iostream>
#include <algorithm>
#include <ctype.h>

#include <qclipboard.h>
#include <qscrollbar.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qsplitter.h>
#include <qdir.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <QDropEvent>
#include <QUrl>
#include <QProcess>



#include <assert.h>

#include "kdiff3.h"
#include "optiondialog.h"
#include "fileaccess.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "gnudiff_diff.h"

bool g_bIgnoreWhiteSpace = true;
bool g_bIgnoreTrivialMatches = true;


bool KDiff3App::runDiff( const LineData* p1, int size1, const LineData* p2, int size2, DiffList& diffList )
{
   ProgressProxy pp;
   static GnuDiff gnuDiff;  // All values are initialized with zeros.

   pp.setCurrent(0);

   diffList.clear();
   if ( p1[0].pLine==0 || p2[0].pLine==0 || size1==0 || size2==0 )
   {
      Diff d( 0,0,0);
      if ( p1[0].pLine==0 && p2[0].pLine==0 && size1 == size2 )
         d.nofEquals = size1;
      else
      {
         d.diff1=size1;
         d.diff2=size2;
      }

      diffList.push_back(d);
   }
   else
   {
      GnuDiff::comparison comparisonInput;
      memset( &comparisonInput, 0, sizeof(comparisonInput) );
      comparisonInput.parent = 0;
      comparisonInput.file[0].buffer = p1[0].pLine;//ptr to buffer
      comparisonInput.file[0].buffered = (p1[size1-1].pLine-p1[0].pLine+p1[size1-1].size); // size of buffer
      comparisonInput.file[1].buffer = p2[0].pLine;//ptr to buffer
      comparisonInput.file[1].buffered = (p2[size2-1].pLine-p2[0].pLine+p2[size2-1].size); // size of buffer

      gnuDiff.ignore_white_space = GnuDiff::IGNORE_ALL_SPACE;  // I think nobody needs anything else ...
      gnuDiff.bIgnoreWhiteSpace = true;
      gnuDiff.bIgnoreNumbers    = false; //m_pOptionDialog->m_bIgnoreNumbers;
      gnuDiff.minimal = false; //m_pOptionDialog->m_bTryHard;
      gnuDiff.ignore_case = false;
      GnuDiff::change* script = gnuDiff.diff_2_files( &comparisonInput );

      int equalLinesAtStart =  comparisonInput.file[0].prefix_lines;
      int currentLine1 = 0;
      int currentLine2 = 0;
      GnuDiff::change* p=0;
      for (GnuDiff::change* e = script; e; e = p)
      {
         Diff d(0,0,0);
         d.nofEquals = e->line0 - currentLine1;
         assert( d.nofEquals == e->line1 - currentLine2 );
         d.diff1 = e->deleted;
         d.diff2 = e->inserted;
         currentLine1 += d.nofEquals + d.diff1;
         currentLine2 += d.nofEquals + d.diff2;
         diffList.push_back(d);

         p = e->link;
         free (e);
      }

      if ( diffList.empty() )
      {
         Diff d(0,0,0);
         d.nofEquals = min2(size1,size2);
         d.diff1 = size1 - d.nofEquals;
         d.diff2 = size2 - d.nofEquals;
         diffList.push_back(d);
/*         Diff d(0,0,0);
         d.nofEquals = equalLinesAtStart;
         if ( gnuDiff.files[0].missing_newline != gnuDiff.files[1].missing_newline )
         {
            d.diff1 = gnuDiff.files[0].missing_newline ? 0 : 1;
            d.diff2 = gnuDiff.files[1].missing_newline ? 0 : 1;
            ++d.nofEquals;
         }
         else if ( !gnuDiff.files[0].missing_newline )
         {
            ++d.nofEquals;
         }
         diffList.push_back(d);
*/
      }
      else
      {
         diffList.front().nofEquals += equalLinesAtStart;   
         currentLine1 += equalLinesAtStart;
         currentLine2 += equalLinesAtStart;

         int nofEquals = min2(size1-currentLine1,size2-currentLine2);
         if ( nofEquals==0 )
         {
            diffList.back().diff1 += size1-currentLine1;
            diffList.back().diff2 += size2-currentLine2;
         }
         else
         {
            Diff d( nofEquals,size1-currentLine1-nofEquals,size2-currentLine2-nofEquals);
            diffList.push_back(d);
         }

         /*
         if ( gnuDiff.files[0].missing_newline != gnuDiff.files[1].missing_newline )
         {
            diffList.back().diff1 += gnuDiff.files[0].missing_newline ? 0 : 1;
            diffList.back().diff2 += gnuDiff.files[1].missing_newline ? 0 : 1;
         }
         else if ( !gnuDiff.files[0].missing_newline )
         {
            ++ diffList.back().nofEquals;
         }
         */
      }
   }

#ifndef NDEBUG
   // Verify difflist
   {
      int l1=0;
      int l2=0;
      DiffList::iterator i;
      for( i = diffList.begin(); i!=diffList.end(); ++i )
      {
         l1+= i->nofEquals + i->diff1;
         l2+= i->nofEquals + i->diff2;
      }

      //if( l1!=p1-p1start || l2!=p2-p2start )
      if( l1!=size1 || l2!=size2 )
         assert( false );
   }
#endif

   pp.setCurrent(1.0);

   return true;
}

bool KDiff3App::runDiff( const LineData* p1, int size1, const LineData* p2, int size2, DiffList& diffList,
                         int winIdx1, int winIdx2 )
{
   diffList.clear();
   DiffList diffList2;

   int l1begin = 0;
   int l2begin = 0;
   ManualDiffHelpList::const_iterator i;
   for( i = m_manualDiffHelpList.begin(); i!=m_manualDiffHelpList.end(); ++i )
   {
      const ManualDiffHelpEntry& mdhe = *i;

      int l1end = winIdx1 == 1 ? mdhe.lineA1 : winIdx1==2 ? mdhe.lineB1 : mdhe.lineC1 ;
      int l2end = winIdx2 == 1 ? mdhe.lineA1 : winIdx2==2 ? mdhe.lineB1 : mdhe.lineC1 ;

      if ( l1end>=0 && l2end>=0 )
      {
         runDiff( p1+l1begin, l1end-l1begin, p2+l2begin, l2end-l2begin, diffList2 );
         diffList.splice( diffList.end(), diffList2 );
         l1begin = l1end;
         l2begin = l2end;

         l1end = winIdx1 == 1 ? mdhe.lineA2 : winIdx1==2 ? mdhe.lineB2 : mdhe.lineC2 ;
         l2end = winIdx2 == 1 ? mdhe.lineA2 : winIdx2==2 ? mdhe.lineB2 : mdhe.lineC2 ;

         if ( l1end>=0 && l2end>=0 )
         {
            ++l1end; // point to line after last selected line
            ++l2end;
            runDiff( p1+l1begin, l1end-l1begin, p2+l2begin, l2end-l2begin, diffList2 );
            diffList.splice( diffList.end(), diffList2 );
            l1begin = l1end;
            l2begin = l2end;
         }
      }
   }
   runDiff( p1+l1begin, size1-l1begin, p2+l2begin, size2-l2begin, diffList2 );
   diffList.splice( diffList.end(), diffList2 );
   return true;
}

void KDiff3App::init( bool bAuto, TotalDiffStatus* pTotalDiffStatus, bool bLoadFiles )
{
   ProgressProxy pp;

   QTextCodec *m_pEncodingB = QTextCodec::codecForName("UTF-8");

   // When doing a full analysis in the directory-comparison, then the statistics-results
   // will be stored in the given TotalDiffStatus. Otherwise it will be 0.
   bool bGUI = pTotalDiffStatus == 0;
   if (pTotalDiffStatus==0) 
      pTotalDiffStatus = &m_totalDiffStatus;

   bool bPreserveCarriageReturn = true; //m_pOptionDialog->m_bPreserveCarriageReturn;

   bool bVisibleMergeResultWindow = ! m_outputFilename.isEmpty();
//   if ( bVisibleMergeResultWindow && bGUI )
//   {
//      bPreserveCarriageReturn = false;
//
//      QString msg;
//
////      if ( !m_pOptionDialog->m_PreProcessorCmd.isEmpty() )
////      {
////         msg += "- " + tr("PreprocessorCmd: ") + m_pOptionDialog->m_PreProcessorCmd + "\n";
////      }
////      if ( !msg.isEmpty() )
////      {
////         int result = KMessageBox::warningYesNo( this,
////                               tr("The following option(s) you selected might change data:\n") + msg +
////                               tr("\nMost likely this is not wanted during a merge.\n"
////                                    "Do you want to disable these settings or continue with these settings active?"),
////                               tr("Option Unsafe for Merging"),
////                               KGuiItem( tr("Use These Options During Merge") ),
////                               KGuiItem( tr("Disable Unsafe Options") )
////                               );
////
////         if (result == KMessageBox::No )
////         {
////            m_pOptionDialog->m_PreProcessorCmd = "";
////         }
////      }
//   }

   // Because of the progressdialog paintevents can occur, but data is invalid,
   // so painting must be suppressed.
   if (m_pDiffTextWindow1) m_pDiffTextWindow1->setPaintingAllowed( false );
   if (m_pDiffTextWindow2) m_pDiffTextWindow2->setPaintingAllowed( false );
   //if (m_pDiffTextWindow3) m_pDiffTextWindow3->setPaintingAllowed( false );
   //if (m_pOverview)        m_pOverview->setPaintingAllowed( false );
   //if (m_pMergeResultWindow) m_pMergeResultWindow->setPaintingAllowed( false );

   m_diff3LineList.clear();

   if ( bLoadFiles )
   {
      m_manualDiffHelpList.clear();

      pp.setMaxNofSteps( 4 );  // Read 2 files, 1 comparison, 1 finediff




      // First get all input data.
      pp.setInformation(tr("Loading A"));
      m_sd1.readAndPreprocess(m_pEncodingB, true );
      pp.step();

      pp.setInformation(tr("Loading B"));
      m_sd2.readAndPreprocess(m_pEncodingB, true );
      pp.step();
   }
   else
   {
      pp.setMaxNofSteps( 2 );  // 1 comparison, 1 finediff
   }

   pTotalDiffStatus->reset();
   // Run the diff.

      pTotalDiffStatus->bBinaryAEqB = m_sd1.isBinaryEqualWith( m_sd2 );
      pp.setInformation(tr("Diff: A <-> B"));

      runDiff( m_sd1.getLineDataForDiff(), m_sd1.getSizeLines(), m_sd2.getLineDataForDiff(), m_sd2.getSizeLines(), m_diffList12,1,2 );

      pp.step();

      pp.setInformation(tr("Linediff: A <-> B"));
      calcDiff3LineListUsingAB( &m_diffList12, m_diff3LineList );
      fineDiff( m_diff3LineList, 1, m_sd1.getLineDataForDisplay(), m_sd2.getLineDataForDisplay(), pTotalDiffStatus->bTextAEqB );
      if ( m_sd1.getSizeBytes()==0 ) pTotalDiffStatus->bTextAEqB=false;

      pp.step();


   m_diffBufferInfo.init( &m_diff3LineList, &m_diff3LineVector,
      m_sd1.getLineDataForDiff(), m_sd1.getSizeLines(),
      m_sd2.getLineDataForDiff(), m_sd2.getSizeLines());
   calcWhiteDiff3Lines( m_diff3LineList, m_sd1.getLineDataForDiff(), m_sd2.getLineDataForDiff());
   calcDiff3LineVector( m_diff3LineList, m_diff3LineVector );

   // Calc needed lines for display
   m_neededLines = m_diff3LineList.size();

   QList<int> oldHeights;
//   if ( m_pDirectoryMergeSplitter->isVisible() )
//      oldHeights = m_pMainSplitter->sizes();

   //initView();

//   if ( m_pDirectoryMergeSplitter->isVisible() )
//   {
//      if (oldHeights.count() < 2)
//         oldHeights.append(0);
//      if (oldHeights[1]==0)    // Distribute the available space evenly between the two widgets.
//      {
//         oldHeights[1] = oldHeights[0]/2;
//         oldHeights[0] -= oldHeights[1];
//      }
//      if ( oldHeights[0]==0 && oldHeights[1]==0 )
//      {
//         oldHeights[1] = 100;
//         oldHeights[0] = 100;
//      }
//      m_pMainSplitter->setSizes( oldHeights );
//   }

   setVisible( bGUI );

   m_bTripleDiff = false; // m_sd3.isEmpty()

   //m_pMergeResultWindowTitle->setEncodings( m_sd1.getEncoding(), m_sd2.getEncoding(), m_sd3.getEncoding() );
   //if ( ! m_pOptionDialog->m_bAutoSelectOutEncoding )
      //m_pMergeResultWindowTitle->setEncoding( m_pOptionDialog->m_pEncodingOut );

   //m_pMergeResultWindowTitle->setLineEndStyles( m_sd1.getLineEndStyle(), m_sd2.getLineEndStyle(), m_sd3.getLineEndStyle() );

   if ( bGUI )
   {
      
      const ManualDiffHelpList* pMDHL = &m_manualDiffHelpList;

      qDebug() << "m_sd1.getAliasName()" << m_sd1.getAliasName();

      m_pDiffTextWindow1->init( m_sd1.getAliasName(), m_sd1.getEncoding(),
         m_sd1.getLineDataForDisplay(), m_sd1.getSizeLines(), &m_diff3LineVector, pMDHL, m_bTripleDiff );
      m_pDiffTextWindow2->init( m_sd2.getAliasName(), m_sd2.getEncoding(),
         m_sd2.getLineDataForDisplay(), m_sd2.getSizeLines(), &m_diff3LineVector, pMDHL, m_bTripleDiff );

//      m_pDiffTextWindow3->init( m_sd3.getAliasName(), m_sd3.getEncoding(), m_sd3.getLineEndStyle(),
//         m_sd3.getLineDataForDisplay(), m_sd3.getSizeLines(), &m_diff3LineVector, pMDHL, m_bTripleDiff );

      //m_pDiffTextWindowFrame3->setVisible(m_bTripleDiff);
   }

   //m_bOutputModified = bVisibleMergeResultWindow;

//   m_pMergeResultWindow->init(
//      m_sd1.getLineDataForDisplay(), m_sd1.getSizeLines(),
//      m_sd2.getLineDataForDisplay(), m_sd2.getSizeLines(),
//      m_bTripleDiff ? m_sd3.getLineDataForDisplay() : 0, m_sd3.getSizeLines(),
//      &m_diff3LineList,
//      pTotalDiffStatus
//      );
//   m_pMergeResultWindowTitle->setFileName( m_outputFilename.isEmpty() ? QString("unnamed.txt") : m_outputFilename );

//   if ( !bGUI )
//   {
//      // We now have all needed information. The rest below is only for GUI-activation.
//      m_sd1.reset();
//      m_sd2.reset();
//      m_sd3.reset();
//      return;
//   }

   //m_pOverview->init(&m_diff3LineList, m_bTripleDiff );
   //m_pDiffVScrollBar->setValue( 0 );
   m_pHScrollBar->setValue( 0 );
   //m_pMergeVScrollBar->setValue( 0 );

   m_pDiffTextWindow1->setPaintingAllowed( true );
   m_pDiffTextWindow2->setPaintingAllowed( true );
   //m_pDiffTextWindow3->setPaintingAllowed( true );
   //m_pOverview->setPaintingAllowed( true );
   //m_pMergeResultWindow->setPaintingAllowed( true );


//   if ( !bVisibleMergeResultWindow )
//      m_pMergeWindowFrame->hide();
//   else
//      m_pMergeWindowFrame->show();

   setHScrollBarRange();

   // Try to create a meaningful but not too long caption
//   if ( !isPart() )
//   {
//      // 1. If the filenames are equal then show only one filename
//      QString caption;
//      QString a1 = m_sd1.getAliasName();
//      QString a2 = m_sd2.getAliasName();
//      QString a3 = m_sd3.getAliasName();
//      QString f1, f2, f3;
//      int p1,p2,p3;
//      if ( !a1.isEmpty() &&  (p1=a1.lastIndexOf('/'))>=0 )
//         f1 = a1.mid( p1+1 );
//      if ( !a2.isEmpty() &&  (p2=a2.lastIndexOf('/'))>=0 )
//         f2 = a2.mid( p2+1 );
//      if ( !a3.isEmpty() &&  (p3=a3.lastIndexOf('/'))>=0 )
//         f3 = a3.mid( p3+1 );
//      if ( !f1.isEmpty() )
//      {
//         if ( ( f2.isEmpty() && f3.isEmpty() ) ||
//              (f2.isEmpty() && f1==f3) || ( f3.isEmpty() && f1==f2 ) || (f1==f2 && f1==f3))
//            caption = ".../"+f1;
//      }
//      else if ( ! f2.isEmpty() )
//      {
//         if ( f3.isEmpty() || f2==f3 )
//            caption = ".../"+f2;
//      }
//      else if ( ! f3.isEmpty() )
//         caption = ".../"+f3;
//
//      // 2. If the files don't have the same name then show all names
//      if ( caption.isEmpty() && (!f1.isEmpty() || !f2.isEmpty() || !f3.isEmpty()) )
//      {
//         caption = ( f1.isEmpty()? QString("") : QString(".../")+f1 );
//         caption += QString(caption.isEmpty() || f2.isEmpty() ? "" : " <-> ") + ( f2.isEmpty()? QString("") : QString(".../")+f2 );
//         caption += QString(caption.isEmpty() || f3.isEmpty() ? "" : " <-> ") + ( f3.isEmpty()? QString("") : QString(".../")+f3 ) ;
//      }
//
//      //m_pKDiff3Shell->setWindowTitle( caption.isEmpty() ? QString("KDiff3") : caption+QString(" - KDiff3"));
//   }

   if ( bLoadFiles )
   {
      /*if ( bVisibleMergeResultWindow && !bAuto )
         m_pMergeResultWindow->showNrOfConflicts();
      else */if ( !bAuto &&
         // Avoid showing this message during startup without parameters.
         !( m_sd1.getAliasName().isEmpty() && m_sd2.getAliasName().isEmpty()) &&
         ( m_sd1.isValid() && m_sd2.isValid())
         )
      {
         QString totalInfo;
         if ( pTotalDiffStatus->bBinaryAEqB && pTotalDiffStatus->bBinaryAEqC )
            totalInfo += tr("All input files are binary equal.");
         else  if ( pTotalDiffStatus->bTextAEqB && pTotalDiffStatus->bTextAEqC )
            totalInfo += tr("All input files contain the same text, but are not binary equal.");
         else {
            if    ( pTotalDiffStatus->bBinaryAEqB ) totalInfo += tr("Files %1 and %2 are binary equal.\n").arg(QString("A"),QString("B"));
            else if ( pTotalDiffStatus->bTextAEqB ) totalInfo += tr("Files %1 and %2 have equal text, but are not binary equal. \n").arg(QString("A"),QString("B"));
            if    ( pTotalDiffStatus->bBinaryAEqC ) totalInfo += tr("Files %1 and %2 are binary equal.\n").arg(QString("A"),QString("C"));
            else if ( pTotalDiffStatus->bTextAEqC ) totalInfo += tr("Files %1 and %2 have equal text, but are not binary equal. \n").arg(QString("A"),QString("C"));
            if    ( pTotalDiffStatus->bBinaryBEqC ) totalInfo += tr("Files %1 and %2 are binary equal.\n").arg(QString("B"),QString("C"));
            else if ( pTotalDiffStatus->bTextBEqC ) totalInfo += tr("Files %1 and %2 have equal text, but are not binary equal. \n").arg(QString("B"),QString("C"));
         }

//         if ( !totalInfo.isEmpty() )
//            KMessageBox::information( this, totalInfo );
      }

      if ( bVisibleMergeResultWindow && (!m_sd1.isText() || !m_sd2.isText()) )
      {
         QMessageBox::information( this, tr("Diff"), tr(
            "Some inputfiles don't seem to be pure textfiles.\n"
            "Note that the KDiff3-merge was not meant for binary data.\n"
            "Continue at your own risk.") );
      }
   }

   QTimer::singleShot( 10, this, SLOT(slotAfterFirstPaint()) );

  /* if ( bVisibleMergeResultWindow && m_pMergeResultWindow )
   {
      m_pMergeResultWindow->setFocus();
   }
   else */if(m_pDiffTextWindow1)
   {
      m_pDiffTextWindow1->setFocus();
   }
}


void KDiff3App::setHScrollBarRange()
{
   int w1 = m_pDiffTextWindow1!=0 && m_pDiffTextWindow1->isVisible() ? m_pDiffTextWindow1->getNofColumns() : 0;
   int w2 = m_pDiffTextWindow2!=0 && m_pDiffTextWindow2->isVisible() ? m_pDiffTextWindow2->getNofColumns() : 0;
   //int w3 = m_pDiffTextWindow3!=0 && m_pDiffTextWindow3->isVisible() ? m_pDiffTextWindow3->getNofColumns() : 0;

   //int wm = m_pMergeResultWindow!=0 && m_pMergeResultWindow->isVisible() ? m_pMergeResultWindow->getNofColumns() : 0;

   int v1 = m_pDiffTextWindow1!=0 && m_pDiffTextWindow1->isVisible() ? m_pDiffTextWindow1->getNofVisibleColumns() : 0;
   int v2 = m_pDiffTextWindow2!=0 && m_pDiffTextWindow2->isVisible() ? m_pDiffTextWindow2->getNofVisibleColumns() : 0;
   //int v3 = m_pDiffTextWindow3!=0 && m_pDiffTextWindow3->isVisible() ? m_pDiffTextWindow3->getNofVisibleColumns() : 0;
   //int vm = m_pMergeResultWindow!=0 && m_pMergeResultWindow->isVisible() ? m_pMergeResultWindow->getNofVisibleColumns() : 0;

   // Find the minimum, but don't consider 0.
   int pageStep = 0;
   if ( (pageStep==0 || pageStep>v1) && v1>0 )
      pageStep = v1;
   if ( (pageStep==0 || pageStep>v2) && v2>0  )
      pageStep = v2;
//   if ( (pageStep==0 || pageStep>v3) && v3>0  )
//      pageStep = v3;
//   if ( (pageStep==0 || pageStep>vm) && vm>0  )
//      pageStep = vm;

   int rangeMax = 0;
   if ( w1>v1 && w1-v1>rangeMax && v1>0 )
      rangeMax = w1-v1;
   if ( w2>v2 && w2-v2>rangeMax && v2>0 )
      rangeMax = w2-v2;
//   if ( w3>v3 && w3-v3>rangeMax && v3>0 )
//      rangeMax = w3-v3;
//   if ( wm>vm && wm-vm>rangeMax && vm>0 )
//      rangeMax = wm-vm;

   m_pHScrollBar->setRange(0, rangeMax );
   m_pHScrollBar->setPageStep( pageStep );
}

void KDiff3App::resizeDiffTextWindow(int /*newWidth*/, int newHeight)
{
   m_DTWHeight = newHeight;

   recalcWordWrap();

   m_pDiffVScrollBar->setRange(0, max2(0, m_neededLines+1 - newHeight) );
   m_pDiffVScrollBar->setPageStep( newHeight );
   //m_pOverview->setRange( m_pDiffVScrollBar->value(), m_pDiffVScrollBar->pageStep() );

   setHScrollBarRange();
}

void KDiff3App::resizeMergeResultWindow()
{
//   //MergeResultWindow* p = m_pMergeResultWindow;
//   m_pMergeVScrollBar->setRange(0, max2(0, p->getNofLines() - p->getNofVisibleLines()) );
//   m_pMergeVScrollBar->setPageStep( p->getNofVisibleLines() );
//
//   setHScrollBarRange();
}

void KDiff3App::scrollDiffTextWindow( int deltaX, int deltaY )
{
   if ( deltaY!= 0 )
   {
      m_pDiffVScrollBar->setValue( m_pDiffVScrollBar->value() + deltaY );
      //m_pOverview->setRange( m_pDiffVScrollBar->value(), m_pDiffVScrollBar->pageStep() );
   }
   if ( deltaX!= 0)
      m_pHScrollBar->QScrollBar::setValue( m_pHScrollBar->value() + deltaX );
}

void KDiff3App::scrollMergeResultWindow( int deltaX, int deltaY )
{
   if ( deltaY!= 0 )
      m_pMergeVScrollBar->setValue( m_pMergeVScrollBar->value() + deltaY );
   if ( deltaX!= 0)
      m_pHScrollBar->setValue( m_pHScrollBar->value() + deltaX );
}

void KDiff3App::setDiff3Line( int line )
{
   m_pDiffVScrollBar->setValue( line );
}

//void KDiff3App::sourceMask( int srcMask, int enabledMask )
//{
////   chooseA->blockSignals(true);
////   chooseB->blockSignals(true);
////   chooseC->blockSignals(true);
////   chooseA->setChecked( (srcMask & 1) != 0 );
////   chooseB->setChecked( (srcMask & 2) != 0 );
////   chooseC->setChecked( (srcMask & 4) != 0 );
////   chooseA->blockSignals(false);
////   chooseB->blockSignals(false);
////   chooseC->blockSignals(false);
////   chooseA->setEnabled( (enabledMask & 1) != 0 );
////   chooseB->setEnabled( (enabledMask & 2) != 0 );
////   chooseC->setEnabled( (enabledMask & 4) != 0 );
//}



// Function uses setMinSize( sizeHint ) before adding the widget.
// void addWidget(QBoxLayout* layout, QWidget* widget);
template <class W, class L>
void addWidget( L* layout, W* widget)
{
   QSize s = widget->sizeHint();
   widget->setMinimumSize( QSize(max2(s.width(),0),max2(s.height(),0) ) );
   layout->addWidget( widget );
}

void KDiff3App::initView()
{
   // set the main widget here
//   if ( m_pMainWidget != 0 )
//   {
//      return;
//      //delete m_pMainWidget;
//   }
   //m_pMainWidget = new QWidget(this); // Contains vertical splitter and horiz scrollbar
   //m_pMainSplitter->addWidget( m_pMainWidget );
   //m_pMainWidget->setObjectName("MainWidget");
   QVBoxLayout* pVLayout = new QVBoxLayout();
   pVLayout->setMargin(0);
   pVLayout->setSpacing(0);
   setLayout(pVLayout);

//   QSplitter* pVSplitter = new QSplitter();
//   pVSplitter->setObjectName("VSplitter");
//   pVSplitter->setOpaqueResize(false);
//   pVSplitter->setOrientation( Qt::Vertical );
//   pVLayout->addWidget( pVSplitter );
//   pVSplitter->show();

   QWidget* pDiffWindowFrame = new QWidget(); // Contains diff windows, overview and vert scrollbar
   pDiffWindowFrame->setObjectName("DiffWindowFrame");
   QHBoxLayout* pDiffHLayout = new QHBoxLayout( pDiffWindowFrame );
   pDiffHLayout->setMargin(0);
   pDiffHLayout->setSpacing(0);
   //pDiffWindowFrame->show();
   pVLayout->addWidget(pDiffWindowFrame);
   //layout()->addWidget(pDiffWindowFrame);


   m_pDiffWindowSplitter = new QSplitter();
   m_pDiffWindowSplitter->setObjectName("DiffWindowSplitter");
   m_pDiffWindowSplitter->setOpaqueResize(false);

   m_pDiffWindowSplitter->setOrientation( true ?  Qt::Horizontal : Qt::Vertical ); //m_pOptionDialog->m_bHorizDiffWindowSplitting
   pDiffHLayout->addWidget( m_pDiffWindowSplitter );
   //m_pDiffWindowSplitter->show();

   //m_pOverview = new Overview( m_pOptionDialog );
   //m_pOverview->setObjectName("Overview");
   //pDiffHLayout->addWidget(m_pOverview);
   //connect( m_pOverview, SIGNAL(setLine(int)), this, SLOT(setDiff3Line(int)) );
   //connect( m_pOverview, SIGNAL(afterFirstPaint()), this, SLOT(slotAfterFirstPaint()));

   m_pDiffVScrollBar = new QScrollBar( Qt::Vertical, pDiffWindowFrame );
   pDiffHLayout->addWidget( m_pDiffVScrollBar );

   m_pDiffTextWindowFrame1 = new DiffTextWindowFrame( m_pDiffWindowSplitter, m_pOptionDialog, 1 );
   m_pDiffWindowSplitter->addWidget(m_pDiffTextWindowFrame1);
   //m_pDiffTextWindowFrame1->show();
   m_pDiffTextWindowFrame2 = new DiffTextWindowFrame( m_pDiffWindowSplitter, m_pOptionDialog, 2 );
   m_pDiffWindowSplitter->addWidget(m_pDiffTextWindowFrame2);
   //m_pDiffTextWindowFrame2->show();
   //m_pDiffTextWindowFrame3 = new DiffTextWindowFrame( m_pDiffWindowSplitter, 0, m_pOptionDialog, 3 );
   //m_pDiffWindowSplitter->addWidget(m_pDiffTextWindowFrame3);
   m_pDiffTextWindow1 = m_pDiffTextWindowFrame1->getDiffTextWindow();
   m_pDiffTextWindow2 = m_pDiffTextWindowFrame2->getDiffTextWindow();
   //m_pDiffTextWindow3 = m_pDiffTextWindowFrame3->getDiffTextWindow();
   connect(m_pDiffTextWindowFrame1, SIGNAL(fileNameChanged(const QString&,int)), this, SLOT(slotFileNameChanged(const QString&,int)));
   connect(m_pDiffTextWindowFrame2, SIGNAL(fileNameChanged(const QString&,int)), this, SLOT(slotFileNameChanged(const QString&,int)));
   //connect(m_pDiffTextWindowFrame3, SIGNAL(fileNameChanged(const QString&,int)), this, SLOT(slotFileNameChanged(const QString&,int)));

   // Merge window
//   m_pMergeWindowFrame = new QWidget( pVSplitter );
//   m_pMergeWindowFrame->setObjectName("MergeWindowFrame");
//   pVSplitter->addWidget(m_pMergeWindowFrame);
//   QHBoxLayout* pMergeHLayout = new QHBoxLayout( m_pMergeWindowFrame );
//   pMergeHLayout->setMargin(0);
//   pMergeHLayout->setSpacing(0);
//   QVBoxLayout* pMergeVLayout = new QVBoxLayout();
//   pMergeHLayout->addLayout( pMergeVLayout, 1 );
//
//   //m_pMergeResultWindowTitle = new WindowTitleWidget(m_pOptionDialog);
//   //pMergeVLayout->addWidget( m_pMergeResultWindowTitle );
//
//   //m_pMergeResultWindow = new MergeResultWindow( m_pMergeWindowFrame, m_pOptionDialog, statusBar() );
//   //pMergeVLayout->addWidget( m_pMergeResultWindow, 1 );
//
//   m_pMergeVScrollBar = new QScrollBar( Qt::Vertical, m_pMergeWindowFrame );
//   pMergeHLayout->addWidget( m_pMergeVScrollBar );

   //m_pMainSplitter->addWidget(m_pMainWidget);

   //m_pMainWidget->show();

   //autoAdvance->setEnabled(true);

//   QList<int> sizes = pVSplitter->sizes();
//   int total = sizes[0] + sizes[1];
//   if ( total<10 )
//      total = 100;
//   sizes[0]=total/2; sizes[1]=total/2;
//   pVSplitter->setSizes( sizes );

   //m_pMergeResultWindow->installEventFilter( this );       // for Cut/Copy/Paste-shortcuts
   //m_pMergeResultWindow->installEventFilter( m_pMergeResultWindowTitle ); // for focus tracking

   QHBoxLayout* pHScrollBarLayout = new QHBoxLayout();
   pVLayout->addLayout( pHScrollBarLayout );
   //layout()->addItem(pHScrollBarLayout);

   bool bj;
   m_pHScrollBar = new ReversibleScrollBar( Qt::Horizontal, &bj ); //m_pOptionDialog->m_bHorizDiffWindowSplitting
   pHScrollBarLayout->addWidget( m_pHScrollBar );
   m_pCornerWidget = new QWidget( this );
   pHScrollBarLayout->addWidget( m_pCornerWidget );


   //connect( m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pOverview, SLOT(setFirstLine(int)));
   connect( m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pDiffTextWindow1, SLOT(setFirstLine(int)));
   connect( m_pHScrollBar, SIGNAL(valueChanged2(int)), m_pDiffTextWindow1, SLOT(setFirstColumn(int)));
   connect( m_pDiffTextWindow1, SIGNAL(newSelection()), this, SLOT(slotSelectionStart()));
   connect( m_pDiffTextWindow1, SIGNAL(selectionEnd()), this, SLOT(slotSelectionEnd()));
   connect( m_pDiffTextWindow1, SIGNAL(scroll(int,int)), this, SLOT(scrollDiffTextWindow(int,int)));
   m_pDiffTextWindow1->installEventFilter( this );

   connect( m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pDiffTextWindow2, SLOT(setFirstLine(int)));
   connect( m_pHScrollBar, SIGNAL(valueChanged2(int)), m_pDiffTextWindow2, SLOT(setFirstColumn(int)));
   connect( m_pDiffTextWindow2, SIGNAL(newSelection()), this, SLOT(slotSelectionStart()));
   connect( m_pDiffTextWindow2, SIGNAL(selectionEnd()), this, SLOT(slotSelectionEnd()));
   connect( m_pDiffTextWindow2, SIGNAL(scroll(int,int)), this, SLOT(scrollDiffTextWindow(int,int)));
   m_pDiffTextWindow2->installEventFilter( this );

//   connect( m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pDiffTextWindow3, SLOT(setFirstLine(int)));
//   connect( m_pHScrollBar, SIGNAL(valueChanged2(int)), m_pDiffTextWindow3, SLOT(setFirstColumn(int)));
//   connect( m_pDiffTextWindow3, SIGNAL(newSelection()), this, SLOT(slotSelectionStart()));
//   connect( m_pDiffTextWindow3, SIGNAL(selectionEnd()), this, SLOT(slotSelectionEnd()));
//   connect( m_pDiffTextWindow3, SIGNAL(scroll(int,int)), this, SLOT(scrollDiffTextWindow(int,int)));
//   m_pDiffTextWindow3->installEventFilter( this );


   //MergeResultWindow* p = m_pMergeResultWindow;
   //connect( m_pMergeVScrollBar, SIGNAL(valueChanged(int)), p, SLOT(setFirstLine(int)));

   //connect( m_pHScrollBar,      SIGNAL(valueChanged2(int)), p, SLOT(setFirstColumn(int)));
   //connect( p, SIGNAL(scroll(int,int)), this, SLOT(scrollMergeResultWindow(int,int)));
   //connect( p, SIGNAL(sourceMask(int,int)), this, SLOT(sourceMask(int,int)));
   //connect( p, SIGNAL( resizeSignal() ),this, SLOT(resizeMergeResultWindow()));
   //connect( p, SIGNAL( selectionEnd() ), this, SLOT( slotSelectionEnd() ) );
   //connect( p, SIGNAL( newSelection() ), this, SLOT( slotSelectionStart() ) );
   //connect( p, SIGNAL( modifiedChanged(bool) ), this, SLOT( slotOutputModified(bool) ) );
   //connect( p, SIGNAL( modifiedChanged(bool) ), m_pMergeResultWindowTitle, SLOT( slotSetModified(bool) ) );
   //connect( p, SIGNAL( updateAvailabilities() ), this, SLOT( slotUpdateAvailabilities() ) );
   //connect( p, SIGNAL( showPopupMenu(const QPoint&) ), this, SLOT(showPopupMenu(const QPoint&)));
   //connect( p, SIGNAL( noRelevantChangesDetected() ), this, SLOT(slotNoRelevantChangesDetected()));
   //sourceMask(0,0);


   //connect( p, SIGNAL(setFastSelectorRange(int,int)), m_pDiffTextWindow1, SLOT(setFastSelectorRange(int,int)));
   //connect( p, SIGNAL(setFastSelectorRange(int,int)), m_pDiffTextWindow2, SLOT(setFastSelectorRange(int,int)));
   //connect( p, SIGNAL(setFastSelectorRange(int,int)), m_pDiffTextWindow3, SLOT(setFastSelectorRange(int,int)));
   //connect(m_pDiffTextWindow1, SIGNAL(setFastSelectorLine(int)), p, SLOT(slotSetFastSelectorLine(int)));
   //connect(m_pDiffTextWindow2, SIGNAL(setFastSelectorLine(int)), p, SLOT(slotSetFastSelectorLine(int)));
   //connect(m_pDiffTextWindow3, SIGNAL(setFastSelectorLine(int)), p, SLOT(slotSetFastSelectorLine(int)));
   //connect(m_pDiffTextWindow1, SIGNAL(gotFocus()), p, SLOT(updateSourceMask()));
   //connect(m_pDiffTextWindow2, SIGNAL(gotFocus()), p, SLOT(updateSourceMask()));
   //connect(m_pDiffTextWindow3, SIGNAL(gotFocus()), p, SLOT(updateSourceMask()));
   //connect(m_pDirectoryMergeInfo, SIGNAL(gotFocus()), p, SLOT(updateSourceMask()));

   connect( m_pDiffTextWindow1, SIGNAL( resizeSignal(int,int) ),this, SLOT(resizeDiffTextWindow(int,int)));
   // The following two connects cause the wordwrap to be recalced thrice, just to make sure. Better than forgetting one.
   connect( m_pDiffTextWindow2, SIGNAL( resizeSignal(int,int) ),this, SLOT(slotRecalcWordWrap()));
   //connect( m_pDiffTextWindow3, SIGNAL( resizeSignal(int,int) ),this, SLOT(slotRecalcWordWrap()));

   m_pDiffTextWindow1->setFocus();
   setMinimumSize(50,50);
   m_pCornerWidget->setFixedSize( m_pDiffVScrollBar->width(), m_pHScrollBar->height() );
//   showWindowA->setChecked( true );
//   showWindowB->setChecked( true );
//   showWindowC->setChecked( true );
}

static int calcManualDiffFirstDiff3LineIdx( const Diff3LineVector& d3lv, const ManualDiffHelpEntry& mdhe )
{
   unsigned int i;
   for( i = 0; i<d3lv.size(); ++i )
   {
      const Diff3Line& d3l = *d3lv[i];
      if ( (mdhe.lineA1>=0  &&  mdhe.lineA1==d3l.lineA) ||
           (mdhe.lineB1>=0  &&  mdhe.lineB1==d3l.lineB) ||
           (mdhe.lineC1>=0  &&  mdhe.lineC1==d3l.lineC) )
         return i;
   }
   return -1;
}

void KDiff3App::slotAfterFirstPaint()
{
   int newHeight = m_pDiffTextWindow1->getNofVisibleLines();
   /*int newWidth  = m_pDiffTextWindow1->getNofVisibleColumns();*/
   m_DTWHeight = newHeight;

   recalcWordWrap();

   m_pDiffVScrollBar->setRange(0, max2(0, m_neededLines+1 - newHeight) );
   m_pDiffVScrollBar->setPageStep( newHeight );
   //m_pOverview->setRange( m_pDiffVScrollBar->value(), m_pDiffVScrollBar->pageStep() );

   int d3l=-1;
   if ( ! m_manualDiffHelpList.empty() )
      d3l = calcManualDiffFirstDiff3LineIdx( m_diff3LineVector, m_manualDiffHelpList.front() );
   if ( d3l>=0 && m_pDiffTextWindow1 )
   {
      int line = m_pDiffTextWindow1->convertDiff3LineIdxToLine( d3l );
      m_pDiffVScrollBar->setValue( max2(0,line-1) );
   }
   else
   {
//      //m_pMergeResultWindow->slotGoTop();
//      if ( ! m_outputFilename.isEmpty() && ! m_pMergeResultWindow->isUnsolvedConflictAtCurrent() )
//         m_pMergeResultWindow->slotGoNextUnsolvedConflict();
   }

   if (m_pCornerWidget)
      m_pCornerWidget->setFixedSize( m_pDiffVScrollBar->width(), m_pHScrollBar->height() );

   slotUpdateAvailabilities();
   setUpdatesEnabled(true);
   // Workaround for a Qt-bug
   QList<QTreeView*> treeViews = findChildren<QTreeView*>();
   foreach( QTreeView* pTreeView, treeViews )
   {
      pTreeView->setUpdatesEnabled(true);
   }
}

void KDiff3App::resizeEvent(QResizeEvent* e)
{
   QWidget::resizeEvent(e);
   if (m_pCornerWidget)
      m_pCornerWidget->setFixedSize( m_pDiffVScrollBar->width(), m_pHScrollBar->height() );
}


bool KDiff3App::eventFilter( QObject* o, QEvent* e )
{
//   if( o == m_pMergeResultWindow )
//   {
//      if ( e->type() == QEvent::KeyPress )
//      {  // key press
//         QKeyEvent *k = (QKeyEvent*)e;
//         if (k->key()==Qt::Key_Insert &&  (k->QInputEvent::modifiers() & Qt::ControlModifier)!=0 )
//         {
//            slotEditCopy();
//            return true;
//         }
//         if (k->key()==Qt::Key_Insert &&  (k->QInputEvent::modifiers() & Qt::ShiftModifier)!=0 )
//         {
//            slotEditPaste();
//            return true;
//         }
//         if (k->key()==Qt::Key_Delete &&  (k->QInputEvent::modifiers() & Qt::ShiftModifier)!=0 )
//         {
//            slotEditCut();
//            return true;
//         }
//         if ( k->key()==Qt::Key_Escape && m_pKDiff3Shell && m_pOptionDialog->m_bEscapeKeyQuits )
//         {
//            m_pKDiff3Shell->close();
//            return true;
//         }
//      }
//      return QSplitter::eventFilter( o, e );    // standard event processing
//   }

   if ( e->type() == QEvent::KeyPress )   // key press
   {
       QKeyEvent *k = (QKeyEvent*)e;
//       if ( k->key()==Qt::Key_Escape && m_pKDiff3Shell && m_pOptionDialog->m_bEscapeKeyQuits )
//       {
//          m_pKDiff3Shell->close();
//          return true;
//       }

       bool bCtrl = (k->QInputEvent::modifiers() & Qt::ControlModifier) != 0;
       if (k->key()==Qt::Key_Insert &&  bCtrl )
       {
          slotEditCopy();
          return true;
       }
//       if (k->key()==Qt::Key_Insert &&  (k->QInputEvent::modifiers() & Qt::ShiftModifier)!=0 )
//       {
//          //slotEditPaste();
//          return true;
//       }
       int deltaX=0;
       int deltaY=0;
       int pageSize =  m_DTWHeight;
       switch( k->key() )
       {
       case Qt::Key_Down:     if (!bCtrl) ++deltaY; break;
       case Qt::Key_Up:       if (!bCtrl) --deltaY; break;
       case Qt::Key_PageDown: if (!bCtrl) deltaY+=pageSize; break;
       case Qt::Key_PageUp:   if (!bCtrl) deltaY-=pageSize; break;
       case Qt::Key_Left:     if (!bCtrl) --deltaX;  break;
       case Qt::Key_Right:    if (!bCtrl) ++deltaX;  break;
       case Qt::Key_Home: if ( bCtrl )  m_pDiffVScrollBar->setValue( 0 );
                      else          m_pHScrollBar->setValue( 0 );
                      break;
       case Qt::Key_End:  if ( bCtrl )  m_pDiffVScrollBar->setValue( m_pDiffVScrollBar->maximum() );
                      else          m_pHScrollBar->setValue( m_pHScrollBar->maximum() );
                      break;
       default: break;
       }

       scrollDiffTextWindow( deltaX, deltaY );

       return true;                        // eat event
   }
   else if (e->type() == QEvent::Wheel )   // wheel event
   {
       QWheelEvent *w = (QWheelEvent*)e;
       w->accept();

       int deltaX=0;

       int d=w->delta();
       int deltaY = -d/120 * QApplication::wheelScrollLines();

       scrollDiffTextWindow( deltaX, deltaY );
       return true;
   }
   else if (e->type() == QEvent::Drop )
   {
      QDropEvent* pDropEvent = static_cast<QDropEvent*>(e);
      pDropEvent->accept();

      if ( pDropEvent->mimeData()->hasUrls() )
      {

         QList<QUrl> urlList = pDropEvent->mimeData()->urls();
         if ( canContinue() && !urlList.isEmpty() )
         {
            raise();
            QString filename = urlList.first().toLocalFile();
            if      ( o == m_pDiffTextWindow1 ) m_sd1.setFilename( filename );
            else if ( o == m_pDiffTextWindow2 ) m_sd2.setFilename( filename );
            //else if ( o == m_pDiffTextWindow3 ) m_sd3.setFilename( filename );
            init();
         }

      }
      else if ( pDropEvent->mimeData()->hasText() )
      {
         QString text = pDropEvent->mimeData()->text();
         if ( canContinue() )
         {
            raise();
            if      ( o == m_pDiffTextWindow1 ) m_sd1.setData(text);
            else if ( o == m_pDiffTextWindow2 ) m_sd2.setData(text);
            //else if ( o == m_pDiffTextWindow3 ) m_sd3.setData(text);
            init();
         }
      }

      return true;
   }
   return QWidget::eventFilter( o, e );    // standard event processing
}




void KDiff3App::slotFileOpen()
{
//   //if ( !canContinue() ) return;
//
////   if ( m_pDirectoryMergeWindow->isDirectoryMergeInProgress() )
////   {
////      int result = KMessageBox::warningYesNo(this,
////         tr("You are currently doing a directory merge. Are you sure, you want to abort?"),
////         tr("Warning"),
////         KGuiItem( tr("Abort") ),
////         KGuiItem( tr("Continue Merging") ) );
////      if ( result!=KMessageBox::Yes )
////         return;
////   }
//
//
//   //slotStatusMsg(tr("Opening files..."));
//
//   for(;;)
//   {
////      OpenDialog d(this,
////         //QDir::toNativeSeparators( m_bDirCompare ? m_pDirectoryMergeWindow->getDirNameA() : m_sd1.isFromBuffer() ? QString("") : m_sd1.getAliasName() ),
////         //QDir::toNativeSeparators( m_bDirCompare ? m_pDirectoryMergeWindow->getDirNameB() : m_sd2.isFromBuffer() ? QString("") : m_sd2.getAliasName() ),
////         //QDir::toNativeSeparators( m_bDirCompare ? m_pDirectoryMergeWindow->getDirNameC() : m_sd3.isFromBuffer() ? QString("") : m_sd3.getAliasName() ),
////         //m_bDirCompare ? ! m_pDirectoryMergeWindow->getDirNameDest().isEmpty() : !m_outputFilename.isEmpty(),
////         //QDir::toNativeSeparators( m_bDirCompare ? m_pDirectoryMergeWindow->getDirNameDest() : m_bDefaultFilename ? QString("") : m_outputFilename ),
////         SLOT(slotConfigure()), m_pOptionDialog );
//      int status = d.exec();
//      if ( status == QDialog::Accepted )
//      {
//         m_sd1.setFilename( d.m_pLineA->currentText() );
//         m_sd2.setFilename( d.m_pLineB->currentText() );
//         m_sd3.setFilename( d.m_pLineC->currentText() );
//
//         if( d.m_pMerge->isChecked() )
//         {
//            if ( d.m_pLineOut->currentText().isEmpty() )
//            {
//               m_outputFilename = "unnamed.txt";
//               m_bDefaultFilename = true;
//            }
//            else
//            {
//               m_outputFilename = d.m_pLineOut->currentText();
//               m_bDefaultFilename = false;
//            }
//         }
//         else
//            m_outputFilename = "";
//
//         bool bSuccess = improveFilenames(false);
//         if ( !bSuccess )
//            continue;
//
//         if ( m_bDirCompare )
//         {
//            m_pDirectoryMergeSplitter->show();
//            if ( m_pMainWidget!=0 )
//            {
//               m_pMainWidget->hide();
//            }
//            break;
//         }
//         else
//         {
//            m_pDirectoryMergeSplitter->hide();
//            init();
//
//            if ( (! m_sd1.isEmpty() && !m_sd1.hasData())  ||
//                 (! m_sd2.isEmpty() && !m_sd2.hasData())  ||
//                 (! m_sd3.isEmpty() && !m_sd3.hasData()) )
//            {
//               QString text( tr("Opening of these files failed:") );
//               text += "\n\n";
//               if ( ! m_sd1.isEmpty() && !m_sd1.hasData() )
//                  text += " - " + m_sd1.getAliasName() + "\n";
//               if ( ! m_sd2.isEmpty() && !m_sd2.hasData() )
//                  text += " - " + m_sd2.getAliasName() + "\n";
//               if ( ! m_sd3.isEmpty() && !m_sd3.hasData() )
//                  text += " - " + m_sd3.getAliasName() + "\n";
//
//               //KMessageBox::sorry( this, text, tr("File open error") );
//               continue;
//            }
//         }
//      }
//      break;
//   }
//
//   slotUpdateAvailabilities();
//   //slotStatusMsg(tr("Ready."));
}


void KDiff3App::slotFileOpen2(QString fn1, QString fn2, QString ofn,
                              QString an1, QString an2, TotalDiffStatus* pTotalDiffStatus )
{
   //if ( !canContinue() ) return;

   if(fn1=="" && fn2=="" && ofn=="")
   {
      return;
   }

   //slotStatusMsg(tr("Opening files..."));

   m_sd1.setFilename( fn1 );
   m_sd2.setFilename( fn2 );
   //m_sd3.setFilename( fn3 );

   m_sd1.setAliasName( an1 );
   m_sd2.setAliasName( an2 );
   //m_sd3.setAliasName( an3 );



   if ( ! ofn.isEmpty() )
   {
      m_outputFilename = ofn;
      m_bDefaultFilename = false;
   }
   else
   {
      m_outputFilename = "";
      m_bDefaultFilename = true;
   }



      init( false, pTotalDiffStatus );

      if ( pTotalDiffStatus!=0 )
         return;

      if ( (! m_sd1.isEmpty() && ! m_sd1.hasData())  ||
           (! m_sd2.isEmpty() && ! m_sd2.hasData()))
      {
         QString text( tr("Opening of these files failed:") );
         text += "\n\n";
         if ( ! m_sd1.isEmpty() && !m_sd1.hasData() )
            text += " - " + m_sd1.getAliasName() + "\n";
         if ( ! m_sd2.isEmpty() && !m_sd2.hasData() )
            text += " - " + m_sd2.getAliasName() + "\n";

         //KMessageBox::sorry( this, text, tr("File open error") );
      }
}


void KDiff3App::slotFileNameChanged(const QString& fileName, int winIdx)
{
   QString fn1 = m_sd1.getFilename();
   QString an1 = m_sd1.getAliasName();
   QString fn2 = m_sd2.getFilename();
   QString an2 = m_sd2.getAliasName();

   if (winIdx==1) { fn1 = fileName; an1 = fileName; }
   if (winIdx==2) { fn2 = fileName; an2 = fileName; }
   //if (winIdx==3) { fn3 = fileName; an3 = ""; }

   slotFileOpen2( fn1, fn2, m_outputFilename, an1, an2, 0 );
   slotRefresh();
}


//void KDiff3App::slotEditCut()
//{
////   slotStatusMsg(tr("Cutting selection..."));
////
////   QString s;
//////   if ( m_pMergeResultWindow!=0 )
//////   {
//////      s = m_pMergeResultWindow->getSelection();
//////      m_pMergeResultWindow->deleteSelection();
//////
//////      m_pMergeResultWindow->update();
//////   }
////
////   if ( !s.isNull() )
////   {
////      QApplication::clipboard()->setText( s, QClipboard::Clipboard );
////   }
////
////   slotStatusMsg(tr("Ready."));
//}

void KDiff3App::slotEditCopy()
{
   //slotStatusMsg(tr("Copying selection to clipboard..."));
   QString s;
   if (               m_pDiffTextWindow1!=0 )   s = m_pDiffTextWindow1->getSelection();
   if ( s.isNull() && m_pDiffTextWindow2!=0 )   s = m_pDiffTextWindow2->getSelection();
   //if ( s.isNull() && m_pDiffTextWindow3!=0 )   s = m_pDiffTextWindow3->getSelection();
   //if ( s.isNull() && m_pMergeResultWindow!=0 ) s = m_pMergeResultWindow->getSelection();
   if ( !s.isNull() )
   {
      QApplication::clipboard()->setText( s, QClipboard::Clipboard );
   }

   //slotStatusMsg(tr("Ready."));
}

//void KDiff3App::slotEditPaste()
//{
//   slotStatusMsg(tr("Inserting clipboard contents..."));
//
//   if ( m_pMergeResultWindow!=0 && m_pMergeResultWindow->isVisible() )
//   {
//      m_pMergeResultWindow->pasteClipboard(false);
//   }
//   else if ( canContinue() )
//   {
//      if ( m_pDiffTextWindow1->hasFocus() )
//      {
//         m_sd1.setData( QApplication::clipboard()->text(QClipboard::Clipboard) );
//         init();
//      }
//      else if ( m_pDiffTextWindow2->hasFocus() )
//      {
//         m_sd2.setData( QApplication::clipboard()->text(QClipboard::Clipboard) );
//         init();
//      }
//      else if ( m_pDiffTextWindow3->hasFocus() )
//      {
//         m_sd3.setData( QApplication::clipboard()->text(QClipboard::Clipboard) );
//         init();
//      }
//   }
//
//   slotStatusMsg(tr("Ready."));
//}

//void KDiff3App::slotEditSelectAll()
//{
////   int l=0,p=0; // needed as dummy return values
////   if  ( m_pMergeResultWindow && m_pMergeResultWindow->hasFocus() ) { m_pMergeResultWindow->setSelection( 0,0,m_pMergeResultWindow->getNofLines(),0); }
////   else if ( m_pDiffTextWindow1 && m_pDiffTextWindow1->hasFocus() ) { m_pDiffTextWindow1  ->setSelection( 0,0,m_pDiffTextWindow1->getNofLines(),0,l,p);   }
////   else if ( m_pDiffTextWindow2 && m_pDiffTextWindow2->hasFocus() ) { m_pDiffTextWindow2  ->setSelection( 0,0,m_pDiffTextWindow2->getNofLines(),0,l,p);   }
////   else if ( m_pDiffTextWindow3 && m_pDiffTextWindow3->hasFocus() ) { m_pDiffTextWindow3  ->setSelection( 0,0,m_pDiffTextWindow3->getNofLines(),0,l,p);   }
////
////   slotStatusMsg(tr("Ready."));
//}

//void KDiff3App::slotGoCurrent()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoCurrent();
//}
//void KDiff3App::slotGoTop()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoTop();
//}
//void KDiff3App::slotGoBottom()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoBottom();
//}
//void KDiff3App::slotGoPrevUnsolvedConflict()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoPrevUnsolvedConflict();
//}
//void KDiff3App::slotGoNextUnsolvedConflict()
//{
//   m_bTimerBlock = false;
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoNextUnsolvedConflict();
//}
//void KDiff3App::slotGoPrevConflict()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoPrevConflict();
//}
//void KDiff3App::slotGoNextConflict()
//{
//   m_bTimerBlock = false;
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoNextConflict();
//}
//void KDiff3App::slotGoPrevDelta()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoPrevDelta();
//}
//void KDiff3App::slotGoNextDelta()
//{
//   //if (m_pMergeResultWindow)  m_pMergeResultWindow->slotGoNextDelta();
//}

//void KDiff3App::choose( int choice )
//{
//   if (!m_bTimerBlock )
//   {
//      if ( m_pDirectoryMergeWindow && m_pDirectoryMergeWindow->hasFocus() )
//      {
//         if (choice==A) m_pDirectoryMergeWindow->slotCurrentChooseA();
//         if (choice==B) m_pDirectoryMergeWindow->slotCurrentChooseB();
//         if (choice==C) m_pDirectoryMergeWindow->slotCurrentChooseC();
//
//         chooseA->setChecked(false);
//         chooseB->setChecked(false);
//         chooseC->setChecked(false);
//      }
//      else if ( m_pMergeResultWindow )
//      {
//         m_pMergeResultWindow->choose( choice );
//         if ( autoAdvance->isChecked() )
//         {
//            m_bTimerBlock = true;
//            QTimer::singleShot( m_pOptionDialog->m_autoAdvanceDelay, this, SLOT( slotGoNextUnsolvedConflict() ) );
//         }
//      }
//   }
//}

//void KDiff3App::slotChooseA() { choose( A ); }
//void KDiff3App::slotChooseB() { choose( B ); }
//void KDiff3App::slotChooseC() { choose( C ); }

// bConflictsOnly automatically choose for conflicts only (true) or for everywhere
//static void mergeChooseGlobal( MergeResultWindow* pMRW, int selector, bool bConflictsOnly, bool bWhiteSpaceOnly )
//{
//   if ( pMRW )
//   {
//      pMRW->chooseGlobal(selector, bConflictsOnly, bWhiteSpaceOnly );
//   }
//}

//void KDiff3App::slotChooseAEverywhere() {  mergeChooseGlobal( m_pMergeResultWindow, A, false, false ); }
//void KDiff3App::slotChooseBEverywhere() {  mergeChooseGlobal( m_pMergeResultWindow, B, false, false ); }
//void KDiff3App::slotChooseCEverywhere() {  mergeChooseGlobal( m_pMergeResultWindow, C, false, false ); }
//void KDiff3App::slotChooseAForUnsolvedConflicts() {  mergeChooseGlobal( m_pMergeResultWindow, A, true, false ); }
//void KDiff3App::slotChooseBForUnsolvedConflicts() {  mergeChooseGlobal( m_pMergeResultWindow, B, true, false ); }
//void KDiff3App::slotChooseCForUnsolvedConflicts() {  mergeChooseGlobal( m_pMergeResultWindow, C, true, false ); }
//void KDiff3App::slotChooseAForUnsolvedWhiteSpaceConflicts() {  mergeChooseGlobal( m_pMergeResultWindow, A, true, true ); }
//void KDiff3App::slotChooseBForUnsolvedWhiteSpaceConflicts() {  mergeChooseGlobal( m_pMergeResultWindow, B, true, true ); }
//void KDiff3App::slotChooseCForUnsolvedWhiteSpaceConflicts() {  mergeChooseGlobal( m_pMergeResultWindow, C, true, true ); }


//void KDiff3App::slotAutoSolve()
//{
////   if (m_pMergeResultWindow )
////   {
////      m_pMergeResultWindow->slotAutoSolve();
////      // m_pMergeWindowFrame->show(); incompatible with bPreserveCarriageReturn
////      m_pMergeResultWindow->showNrOfConflicts();
////      slotUpdateAvailabilities();
////   }
//}

//void KDiff3App::slotUnsolve()
//{
////   if (m_pMergeResultWindow )
////   {
////      m_pMergeResultWindow->slotUnsolve();
////   }
//}
//
//void KDiff3App::slotMergeHistory()
//{
////   if (m_pMergeResultWindow )
////   {
////      m_pMergeResultWindow->slotMergeHistory();
////   }
//}
//
//void KDiff3App::slotRegExpAutoMerge()
//{
////   if (m_pMergeResultWindow )
////   {
////      m_pMergeResultWindow->slotRegExpAutoMerge();
////   }
//}

void KDiff3App::slotSplitDiff()
{
   int firstLine = -1;
   int lastLine = -1;
   DiffTextWindow* pDTW=0;
   if (                m_pDiffTextWindow1 ) { pDTW=m_pDiffTextWindow1; pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords); }
   if ( firstLine<0 && m_pDiffTextWindow2 ) { pDTW=m_pDiffTextWindow2; pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords); }
   //if ( firstLine<0 && m_pDiffTextWindow3 ) { pDTW=m_pDiffTextWindow3; pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords); }
//   if ( pDTW && firstLine>=0 && m_pMergeResultWindow)
//   {
//      pDTW->resetSelection();
//
//      m_pMergeResultWindow->slotSplitDiff( firstLine, lastLine );
//   }
}

void KDiff3App::slotJoinDiffs()
{
   int firstLine = -1;
   int lastLine = -1;
   DiffTextWindow* pDTW=0;
   if (                m_pDiffTextWindow1 ) { pDTW=m_pDiffTextWindow1; pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords); }
   if ( firstLine<0 && m_pDiffTextWindow2 ) { pDTW=m_pDiffTextWindow2; pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords); }
   //if ( firstLine<0 && m_pDiffTextWindow3 ) { pDTW=m_pDiffTextWindow3; pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords); }
//   if ( pDTW && firstLine>=0 && m_pMergeResultWindow)
//   {
//      pDTW->resetSelection();
//
//      m_pMergeResultWindow->slotJoinDiffs( firstLine, lastLine );
//   }
}

//void KDiff3App::slotConfigure()
//{
////   m_pOptionDialog->setState();
////   m_pOptionDialog->incrementInitialSize ( QSize(0,40) );
////   m_pOptionDialog->exec();
////   slotRefresh();
//}

//void KDiff3App::slotConfigureKeys()
//{
//    //KShortcutsDialog::configure(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this);
//}

void KDiff3App::slotRefresh()
{
   if (m_pDiffTextWindow1!=0)
   {
      m_pDiffTextWindow1->setFont(m_pOptionDialog->m_font); //m_pOptionDialog->m_font
      m_pDiffTextWindow1->update();
   }
   if (m_pDiffTextWindow2!=0)
   {
      m_pDiffTextWindow2->setFont(m_pOptionDialog->m_font);
      m_pDiffTextWindow2->update();
   }
//   if (m_pDiffTextWindow3!=0)
//   {
//      m_pDiffTextWindow3->setFont(QFont("Curier"));
//      m_pDiffTextWindow3->update();
//   }
//   if (m_pMergeResultWindow!=0)
//   {
//      m_pMergeResultWindow->setFont(m_pOptionDialog->m_font);
//      m_pMergeResultWindow->update();
//   }
   if (m_pHScrollBar!=0)
   {
      m_pHScrollBar->setAgain();
   }
   if ( m_pDiffWindowSplitter!=0 )
   {
      m_pDiffWindowSplitter->setOrientation( true ? Qt::Horizontal : Qt::Vertical ); //m_pOptionDialog->m_bHorizDiffWindowSplitting
   }
//   if ( m_pDirectoryMergeWindow )
//   {
//      m_pDirectoryMergeWindow->updateFileVisibilities();
//   }
}

void KDiff3App::slotSelectionStart()
{
   //editCopy->setEnabled( false );
   //editCut->setEnabled( false );

   const QObject* s = sender();
   if (m_pDiffTextWindow1 && s!=m_pDiffTextWindow1)   m_pDiffTextWindow1->resetSelection();
   if (m_pDiffTextWindow2 && s!=m_pDiffTextWindow2)   m_pDiffTextWindow2->resetSelection();
   //if (m_pDiffTextWindow3 && s!=m_pDiffTextWindow3)   m_pDiffTextWindow3->resetSelection();
   //if (m_pMergeResultWindow && s!=m_pMergeResultWindow) m_pMergeResultWindow->resetSelection();
}

void KDiff3App::slotSelectionEnd()
{
   //const QObject* s = sender();
   //editCopy->setEnabled(true);
   //editCut->setEnabled( s==m_pMergeResultWindow );
   if ( false ) //m_pOptionDialog->m_bAutoCopySelection
   {
      slotEditCopy();
   }
   else
   {
       QClipboard *clipBoard = QApplication::clipboard();

       if (clipBoard->supportsSelection ())
       {
           QString s;
           if (               m_pDiffTextWindow1!=0 )   s = m_pDiffTextWindow1->getSelection();
           if ( s.isNull() && m_pDiffTextWindow2!=0 )   s = m_pDiffTextWindow2->getSelection();
           //if ( s.isNull() && m_pDiffTextWindow3!=0 )   s = m_pDiffTextWindow3->getSelection();
           //if ( s.isNull() && m_pMergeResultWindow!=0 ) s = m_pMergeResultWindow->getSelection();
           if ( !s.isNull() )
           {
               clipBoard->setText( s, QClipboard::Selection );
           }
       }
   }
}

//void KDiff3App::slotClipboardChanged()
//{
//   QString s = QApplication::clipboard()->text();
//   //editPaste->setEnabled(!s.isEmpty());
//}

//void KDiff3App::slotOutputModified(bool bModified)
//{
//   if ( bModified && !m_bOutputModified )
//   {
//      m_bOutputModified=true;
//      slotUpdateAvailabilities();
//   }
//}

//void KDiff3App::slotAutoAdvanceToggled()
//{
//   //m_pOptionDialog->m_bAutoAdvance = autoAdvance->isChecked();
//}

//void KDiff3App::slotWordWrapToggled()
//{
//   //m_pOptionDialog->m_bWordWrap = wordWrap->isChecked();
//   //recalcWordWrap();
//}

void KDiff3App::slotRecalcWordWrap()
{
   recalcWordWrap();
}

void KDiff3App::recalcWordWrap(int nofVisibleColumns) // nofVisibleColumns is >=0 only for printing, otherwise the really visible width is used
{
   bool bPrinting = nofVisibleColumns>=0;
   int firstD3LIdx = 0;
   if( m_pDiffTextWindow1 ) 
      firstD3LIdx = m_pDiffTextWindow1->convertLineToDiff3LineIdx( m_pDiffTextWindow1->getFirstLine() );

   // Convert selection to D3L-coords (converting back happens in DiffTextWindow::recalcWordWrap()
   if ( m_pDiffTextWindow1 )
      m_pDiffTextWindow1->convertSelectionToD3LCoords();
   if ( m_pDiffTextWindow2 )
      m_pDiffTextWindow2->convertSelectionToD3LCoords();
//   if ( m_pDiffTextWindow3 )
//      m_pDiffTextWindow3->convertSelectionToD3LCoords();


   if ( !m_diff3LineList.empty() && false ) //m_pOptionDialog->m_bWordWrap
   {
      Diff3LineList::iterator i;
      int sumOfLines=0;
      for ( i=m_diff3LineList.begin(); i!=m_diff3LineList.end(); ++i )
      {
         Diff3Line& d3l = *i;
         d3l.linesNeededForDisplay = 1;
         d3l.sumLinesNeededForDisplay = sumOfLines;
         sumOfLines += d3l.linesNeededForDisplay;
      }

      // Let every window calc how many lines will be needed.
      if ( m_pDiffTextWindow1 )
         m_pDiffTextWindow1->recalcWordWrap(true,0,nofVisibleColumns);
      if ( m_pDiffTextWindow2 )
         m_pDiffTextWindow2->recalcWordWrap(true,0,nofVisibleColumns);
//      if ( m_pDiffTextWindow3 )
//         m_pDiffTextWindow3->recalcWordWrap(true,0,nofVisibleColumns);

      sumOfLines=0;
      for ( i=m_diff3LineList.begin(); i!=m_diff3LineList.end(); ++i )
      {
         Diff3Line& d3l = *i;
         d3l.sumLinesNeededForDisplay = sumOfLines;
         sumOfLines += d3l.linesNeededForDisplay;
      }

      // Finish the initialisation:
      if ( m_pDiffTextWindow1 )
         m_pDiffTextWindow1->recalcWordWrap(true,sumOfLines,nofVisibleColumns);
      if ( m_pDiffTextWindow2 )
         m_pDiffTextWindow2->recalcWordWrap(true,sumOfLines,nofVisibleColumns);
//      if ( m_pDiffTextWindow3 )
//         m_pDiffTextWindow3->recalcWordWrap(true,sumOfLines,nofVisibleColumns);

      m_neededLines = sumOfLines;
   }
   else
   {
      m_neededLines = m_diff3LineVector.size();
      if ( m_pDiffTextWindow1 )
         m_pDiffTextWindow1->recalcWordWrap(false,0,0);
      if ( m_pDiffTextWindow2 )
         m_pDiffTextWindow2->recalcWordWrap(false,0,0);
//      if ( m_pDiffTextWindow3 )
//         m_pDiffTextWindow3->recalcWordWrap(false,0,0);
   }
   if (bPrinting)
      return;

   /*if (m_pOverview)
      m_pOverview->slotRedraw()*/;
   if ( m_pDiffTextWindow1 )
   {
      m_pDiffTextWindow1->setFirstLine( m_pDiffTextWindow1->convertDiff3LineIdxToLine( firstD3LIdx ) );
      m_pDiffTextWindow1->update();
   }
   if ( m_pDiffTextWindow2 )
   {
      m_pDiffTextWindow2->setFirstLine( m_pDiffTextWindow2->convertDiff3LineIdxToLine( firstD3LIdx ) );
      m_pDiffTextWindow2->update();
   }
//   if ( m_pDiffTextWindow3 )
//   {
//      m_pDiffTextWindow3->setFirstLine( m_pDiffTextWindow3->convertDiff3LineIdxToLine( firstD3LIdx ) );
//      m_pDiffTextWindow3->update();
//   }

   if ( m_pDiffVScrollBar )
      m_pDiffVScrollBar->setRange(0, max2(0, m_neededLines+1 - m_DTWHeight) );
   if ( m_pDiffTextWindow1 )
   {
      m_pDiffVScrollBar->setValue( m_pDiffTextWindow1->convertDiff3LineIdxToLine( firstD3LIdx ) );

      setHScrollBarRange();
      m_pHScrollBar->setValue(0);
   }
}

//void KDiff3App::slotShowWhiteSpaceToggled()
//{
//   //m_pOptionDialog->m_bShowWhiteSpaceCharacters = showWhiteSpaceCharacters->isChecked();
//   //m_pOptionDialog->m_bShowWhiteSpace = showWhiteSpace->isChecked();
//   //showWhiteSpaceCharacters->setEnabled( showWhiteSpace->isChecked() );
//   if ( m_pDiffTextWindow1!=0 )
//      m_pDiffTextWindow1->update();
//   if ( m_pDiffTextWindow2!=0 )
//      m_pDiffTextWindow2->update();
//   if ( m_pDiffTextWindow3!=0 )
//      m_pDiffTextWindow3->update();
//   //if ( m_pOverview!=0 )
//      //m_pOverview->slotRedraw();
//}

//void KDiff3App::slotShowLineNumbersToggled()
//{
//   //m_pOptionDialog->m_bShowLineNumbers = showLineNumbers->isChecked();
//   if ( m_pDiffTextWindow1!=0 )
//      m_pDiffTextWindow1->update();
//   if ( m_pDiffTextWindow2!=0 )
//      m_pDiffTextWindow2->update();
//   if ( m_pDiffTextWindow3!=0 )
//      m_pDiffTextWindow3->update();
//}

/// Return true for success, else false
//bool KDiff3App::improveFilenames( bool bCreateNewInstance )
//{
//   m_bDirCompare = false;
//
//   FileAccess f1(m_sd1.getFilename());
//   FileAccess f2(m_sd2.getFilename());
//   FileAccess f3(m_sd3.getFilename());
//   FileAccess f4(m_outputFilename);
//
//   if ( f1.isFile()  &&  f1.exists() )
//   {
//      if ( f2.isDir() )
//      {
//         f2.addPath( f1.fileName() );
//         if ( f2.isFile() && f2.exists() )
//            m_sd2.setFileAccess( f2 );
//      }
//      if ( f3.isDir() )
//      {
//         f3.addPath( f1.fileName() );
//         if ( f3.isFile() && f3.exists() )
//            m_sd3.setFileAccess( f3 );
//      }
//      if ( f4.isDir() )
//      {
//         f4.addPath( f1.fileName() );
//         if ( f4.isFile() && f4.exists() )
//            m_outputFilename = f4.absoluteFilePath();
//      }
//   }
//   else if ( f1.isDir() )
//   {
//      m_bDirCompare = true;
//      if (bCreateNewInstance)
//      {
//         emit createNewInstance( f1.absoluteFilePath(), f2.absoluteFilePath(), f3.absoluteFilePath() );
//      }
//      else
//      {
//         FileAccess destDir;
//         if (!m_bDefaultFilename) destDir = f4;
//         m_pDirectoryMergeSplitter->show();
//         if (m_pMainWidget!=0) m_pMainWidget->hide();
//         setUpdatesEnabled(true);
//
////         bool bSuccess = m_pDirectoryMergeWindow->init(
////            f1, f2, f3,
////            destDir,  // Destdirname
////            !m_outputFilename.isEmpty()
////            );
//
//         m_bDirCompare = true;  // This seems redundant but it might have been reset during full analysis.
//
//         if (bSuccess)
//         {
//            m_sd1.reset();
//            if (m_pDiffTextWindow1!=0) m_pDiffTextWindow1->init(0,0,eLineEndStyleDos,0,0,0,0,false);
//            m_sd2.reset();
//            if (m_pDiffTextWindow2!=0) m_pDiffTextWindow2->init(0,0,eLineEndStyleDos,0,0,0,0,false);
//            m_sd3.reset();
//            if (m_pDiffTextWindow3!=0) m_pDiffTextWindow3->init(0,0,eLineEndStyleDos,0,0,0,0,false);
//         }
//         slotUpdateAvailabilities();
//         return bSuccess;
//      }
//   }
//   return true;
//}

void KDiff3App::slotReload()
{
   //if ( !canContinue() ) return;

   init();
}

bool KDiff3App::canContinue()
{
   // First test if anything must be saved.
//   if(m_bOutputModified)
//   {
//      int result = KMessageBox::warningYesNoCancel(this,
//         tr("The merge result hasn't been saved."),
//         tr("Warning"),
//         KGuiItem( tr("Save && Continue") ),
//         KGuiItem( tr("Continue Without Saving") ) );
//      if ( result==KMessageBox::Cancel )
//         return false;
//      else if ( result==KMessageBox::Yes )
//      {
//         slotFileSave();
//         if ( m_bOutputModified )
//         {
//            KMessageBox::sorry(this, tr("Saving the merge result failed."), tr("Warning") );
//            return false;
//         }
//      }
//   }
//
//   m_bOutputModified = false;
   return true;
}

//void KDiff3App::slotCheckIfCanContinue( bool* pbContinue )
//{
//   if (pbContinue!=0) *pbContinue = canContinue();
//}


//void KDiff3App::slotDirShowBoth()
//{
////   if( dirShowBoth->isChecked() )
////   {
////      if ( m_pDirectoryMergeSplitter )
////         m_pDirectoryMergeSplitter->setVisible( m_bDirCompare );
////
////      if ( m_pMainWidget!=0 )
////         m_pMainWidget->show();
////   }
////   else
////   {
////      if ( m_pMainWidget!=0 )
////      {
////         m_pMainWidget->show();
////         m_pDirectoryMergeSplitter->hide();
////      }
////      else if ( m_bDirCompare )
////      {
////         m_pDirectoryMergeSplitter->show();
////      }
////   }
////
////   slotUpdateAvailabilities();
//}


//void KDiff3App::slotDirViewToggle()
//{
////   if ( m_bDirCompare )
////   {
////      if( ! m_pDirectoryMergeSplitter->isVisible() )
////      {
////         m_pDirectoryMergeSplitter->show();
////         if (m_pMainWidget!=0)
////            m_pMainWidget->hide();
////      }
////      else
////      {
////         if (m_pMainWidget!=0)
////         {
////            m_pDirectoryMergeSplitter->hide();
////            m_pMainWidget->show();
////         }
////      }
////   }
////   slotUpdateAvailabilities();
//}

//void KDiff3App::slotShowWindowAToggled()
//{
////   if ( m_pDiffTextWindow1!=0 )
////   {
////      m_pDiffTextWindowFrame1->setVisible( showWindowA->isChecked() );
////      slotUpdateAvailabilities();
////   }
//}

//void KDiff3App::slotShowWindowBToggled()
//{
////   if ( m_pDiffTextWindow2!=0 )
////   {
////      m_pDiffTextWindowFrame2->setVisible( showWindowB->isChecked() );
////      slotUpdateAvailabilities();
////   }
//}
//
//void KDiff3App::slotShowWindowCToggled()
//{
////   if ( m_pDiffTextWindow3!=0 )
////   {
////      m_pDiffTextWindowFrame3->setVisible( showWindowC->isChecked() );
////      slotUpdateAvailabilities();
////   }
//}

//void KDiff3App::slotEditFind()
//{
//   m_pFindDialog->currentLine = 0;
//   m_pFindDialog->currentPos = 0;
//   m_pFindDialog->currentWindow = 1;
//
//   if ( QDialog::Accepted == m_pFindDialog->exec() )
//   {
//      slotEditFindNext();
//   }
//}

//void KDiff3App::slotEditFindNext()
//{
//   QString s = m_pFindDialog->m_pSearchString->text();
//   if ( s.isEmpty() )
//   {
//      slotEditFind();
//      return;
//   }
//
//   bool bDirDown = true;
//   bool bCaseSensitive = m_pFindDialog->m_pCaseSensitive->isChecked();
//
//   int d3vLine = m_pFindDialog->currentLine;
//   int posInLine = m_pFindDialog->currentPos;
//   int l=0;
//   int p=0;
//   if ( m_pFindDialog->currentWindow == 1 )
//   {
//      if ( m_pFindDialog->m_pSearchInA->isChecked() && m_pDiffTextWindow1!=0  &&
//           m_pDiffTextWindow1->findString( s, d3vLine, posInLine, bDirDown, bCaseSensitive ) )
//      {
//         m_pDiffTextWindow1->setSelection( d3vLine, posInLine, d3vLine, posInLine+s.length(), l, p );
//         m_pDiffVScrollBar->setValue(l-m_pDiffVScrollBar->pageStep()/2);
//         m_pHScrollBar->setValue( max2( 0, p+(int)s.length()-m_pHScrollBar->pageStep()) );
//         m_pFindDialog->currentLine = d3vLine;
//         m_pFindDialog->currentPos = posInLine + 1;
//         return;
//      }
//      m_pFindDialog->currentWindow = 2;
//      m_pFindDialog->currentLine = 0;
//      m_pFindDialog->currentPos = 0;
//   }
//
//   d3vLine = m_pFindDialog->currentLine;
//   posInLine = m_pFindDialog->currentPos;
//   if ( m_pFindDialog->currentWindow == 2 )
//   {
//      if ( m_pFindDialog->m_pSearchInB->isChecked() && m_pDiffTextWindow2!=0 &&
//           m_pDiffTextWindow2->findString( s, d3vLine, posInLine, bDirDown, bCaseSensitive ) )
//      {
//         m_pDiffTextWindow2->setSelection( d3vLine, posInLine, d3vLine, posInLine+s.length(),l,p );
//         m_pDiffVScrollBar->setValue(l-m_pDiffVScrollBar->pageStep()/2);
//         m_pHScrollBar->setValue( max2( 0, p+(int)s.length()-m_pHScrollBar->pageStep()) );
//         m_pFindDialog->currentLine = d3vLine;
//         m_pFindDialog->currentPos = posInLine + 1;
//         return;
//      }
//      m_pFindDialog->currentWindow = 3;
//      m_pFindDialog->currentLine = 0;
//      m_pFindDialog->currentPos = 0;
//   }
//
//   d3vLine = m_pFindDialog->currentLine;
//   posInLine = m_pFindDialog->currentPos;
//   if ( m_pFindDialog->currentWindow == 3 )
//   {
//      if ( m_pFindDialog->m_pSearchInC->isChecked() && m_pDiffTextWindow3!=0 &&
//           m_pDiffTextWindow3->findString( s, d3vLine, posInLine, bDirDown, bCaseSensitive ) )
//      {
//         m_pDiffTextWindow3->setSelection( d3vLine, posInLine, d3vLine, posInLine+s.length(),l,p );
//         m_pDiffVScrollBar->setValue(l-m_pDiffVScrollBar->pageStep()/2);
//         m_pHScrollBar->setValue( max2( 0, p+(int)s.length()-m_pHScrollBar->pageStep()) );
//         m_pFindDialog->currentLine = d3vLine;
//         m_pFindDialog->currentPos = posInLine + 1;
//         return;
//      }
//      m_pFindDialog->currentWindow = 4;
//      m_pFindDialog->currentLine = 0;
//      m_pFindDialog->currentPos = 0;
//   }
//
//   d3vLine = m_pFindDialog->currentLine;
//   posInLine = m_pFindDialog->currentPos;
//   if ( m_pFindDialog->currentWindow == 4 )
//   {
//      if ( m_pFindDialog->m_pSearchInOutput->isChecked() && m_pMergeResultWindow!=0 && m_pMergeResultWindow->isVisible() &&
//           m_pMergeResultWindow->findString( s, d3vLine, posInLine, bDirDown, bCaseSensitive ) )
//      {
//         m_pMergeResultWindow->setSelection( d3vLine, posInLine, d3vLine, posInLine+s.length() );
//         m_pMergeVScrollBar->setValue(d3vLine - m_pMergeVScrollBar->pageStep()/2);
//         m_pHScrollBar->setValue( max2( 0, posInLine+(int)s.length()-m_pHScrollBar->pageStep()) );
//         m_pFindDialog->currentLine = d3vLine;
//         m_pFindDialog->currentPos = posInLine + 1;
//         return;
//      }
//      m_pFindDialog->currentWindow = 5;
//      m_pFindDialog->currentLine = 0;
//      m_pFindDialog->currentPos = 0;
//   }
//
//   KMessageBox::information(this,tr("Search complete."),tr("Search Complete"));
//   m_pFindDialog->currentWindow = 1;
//   m_pFindDialog->currentLine = 0;
//   m_pFindDialog->currentPos = 0;
//}

//void KDiff3App::slotMergeCurrentFile()
//{
//   if ( m_bDirCompare  &&  m_pDirectoryMergeWindow->isVisible()  &&  m_pDirectoryMergeWindow->isFileSelected() )
//   {
//      m_pDirectoryMergeWindow->mergeCurrentFile();
//   }
//   else if ( m_pMainWidget != 0 && m_pMainWidget->isVisible() )
//   {
//      if ( !canContinue() ) return;
//      if ( m_outputFilename.isEmpty() )
//      {
//         if ( !m_sd3.isEmpty() && !m_sd3.isFromBuffer() )
//         {
//            m_outputFilename =  m_sd3.getFilename();
//         }
//         else if ( !m_sd2.isEmpty() && !m_sd2.isFromBuffer() )
//         {
//            m_outputFilename =  m_sd2.getFilename();
//         }
//         else if ( !m_sd1.isEmpty() && !m_sd1.isFromBuffer() )
//         {
//            m_outputFilename =  m_sd1.getFilename();
//         }
//         else
//         {
//            m_outputFilename = "unnamed.txt";
//            m_bDefaultFilename = true;
//         }
//      }
//      init();
//   }
//}

void KDiff3App::slotWinFocusNext()
{
   QWidget* focus = qApp->focusWidget();
//   if ( focus == m_pDirectoryMergeWindow && m_pDirectoryMergeWindow->isVisible() && ! dirShowBoth->isChecked() )
//   {
//      slotDirViewToggle();
//   }

   std::list<QWidget*> visibleWidgetList;
   if ( m_pDiffTextWindow1 && m_pDiffTextWindow1->isVisible() ) visibleWidgetList.push_back(m_pDiffTextWindow1);
   if ( m_pDiffTextWindow2 && m_pDiffTextWindow2->isVisible() ) visibleWidgetList.push_back(m_pDiffTextWindow2);
   //if ( m_pDiffTextWindow3 && m_pDiffTextWindow3->isVisible() ) visibleWidgetList.push_back(m_pDiffTextWindow3);
   //if ( m_pMergeResultWindow && m_pMergeResultWindow->isVisible() ) visibleWidgetList.push_back(m_pMergeResultWindow);
   //if ( m_bDirCompare /*m_pDirectoryMergeWindow->isVisible()*/ ) visibleWidgetList.push_back(m_pDirectoryMergeWindow);
   //if ( m_pDirectoryMergeInfo->isVisible() ) visibleWidgetList.push_back(m_pDirectoryMergeInfo->getInfoList());

   std::list<QWidget*>::iterator i = std::find( visibleWidgetList.begin(),  visibleWidgetList.end(), focus);
   ++i;
   if ( i==visibleWidgetList.end() ) 
      i = visibleWidgetList.begin();
   if ( i!=visibleWidgetList.end() )
   {
//      if ( *i == m_pDirectoryMergeWindow  && ! dirShowBoth->isChecked() )
//      {
//         slotDirViewToggle();
//      }
      (*i)->setFocus();
   }
}

void KDiff3App::slotWinFocusPrev()
{
   QWidget* focus = qApp->focusWidget();
//   if ( focus == m_pDirectoryMergeWindow  && m_pDirectoryMergeWindow->isVisible() && ! dirShowBoth->isChecked() )
//   {
//      slotDirViewToggle();
//   }

   std::list<QWidget*> visibleWidgetList;
   if ( m_pDiffTextWindow1 && m_pDiffTextWindow1->isVisible() ) visibleWidgetList.push_back(m_pDiffTextWindow1);
   if ( m_pDiffTextWindow2 && m_pDiffTextWindow2->isVisible() ) visibleWidgetList.push_back(m_pDiffTextWindow2);
   //if ( m_pDiffTextWindow3 && m_pDiffTextWindow3->isVisible() ) visibleWidgetList.push_back(m_pDiffTextWindow3);
   //if ( m_pMergeResultWindow && m_pMergeResultWindow->isVisible() ) visibleWidgetList.push_back(m_pMergeResultWindow);
   //if (m_bDirCompare /* m_pDirectoryMergeWindow->isVisible() */ ) visibleWidgetList.push_back(m_pDirectoryMergeWindow);
   //if ( m_pDirectoryMergeInfo->isVisible() ) visibleWidgetList.push_back(m_pDirectoryMergeInfo->getInfoList());

   std::list<QWidget*>::iterator i = std::find( visibleWidgetList.begin(),  visibleWidgetList.end(), focus);
   if ( i==visibleWidgetList.begin() )
      i=visibleWidgetList.end();
   --i;
   if ( i!=visibleWidgetList.end() )
   {
//      if ( *i == m_pDirectoryMergeWindow  && ! dirShowBoth->isChecked() )
//      {
//         slotDirViewToggle();
//      }
      (*i)->setFocus();
   }
}

void KDiff3App::slotWinToggleSplitterOrientation()
{
   if ( m_pDiffWindowSplitter!=0 )
   {
      m_pDiffWindowSplitter->setOrientation(
            m_pDiffWindowSplitter->orientation()==Qt::Vertical ? Qt::Horizontal : Qt::Vertical
         );

      //m_pOptionDialog->m_bHorizDiffWindowSplitting = m_pDiffWindowSplitter->orientation()==Qt::Horizontal;
   }
}

//void KDiff3App::slotOverviewNormal()
//{
////   if ( m_pOverview != 0 )
////      m_pOverview->setOverviewMode( Overview::eOMNormal );
////   if ( m_pMergeResultWindow !=0 )
////      m_pMergeResultWindow->setOverviewMode( Overview::eOMNormal );
////   slotUpdateAvailabilities();
//}

//void KDiff3App::slotOverviewAB()
//{
////   if ( m_pOverview != 0 )
////      m_pOverview->setOverviewMode( Overview::eOMAvsB );
////   m_pMergeResultWindow->setOverviewMode( Overview::eOMAvsB );
////   slotUpdateAvailabilities();
//}

//void KDiff3App::slotOverviewAC()
//{
////   if ( m_pOverview != 0 )
////      m_pOverview->setOverviewMode( Overview::eOMAvsC );
////   if ( m_pMergeResultWindow !=0 )
////      m_pMergeResultWindow->setOverviewMode( Overview::eOMAvsC );
////   slotUpdateAvailabilities();
//}

//void KDiff3App::slotOverviewBC()
//{
////   if ( m_pOverview != 0 )
////      m_pOverview->setOverviewMode( Overview::eOMBvsC );
////   if ( m_pMergeResultWindow !=0 )
////      m_pMergeResultWindow->setOverviewMode( Overview::eOMBvsC );
////   slotUpdateAvailabilities();
//}

void KDiff3App::slotNoRelevantChangesDetected()
{
   if ( m_bTripleDiff &&  ! m_outputFilename.isEmpty() )
   {
      //KMessageBox::information( this, "No relevant changes detected", "KDiff3" );
      if (false) //!m_pOptionDialog->m_IrrelevantMergeCmd.isEmpty()
      {
         QString cmd = ""; //m_pOptionDialog->m_IrrelevantMergeCmd + " \"" + m_sd1.getAliasName()+ "\" \"" + m_sd2.getAliasName() + "\" \"" + m_sd3.getAliasName();
         QProcess process;
         process.start( cmd );
         process.waitForFinished(-1);
         //::system( cmd.local8Bit() );
      }
   }
}

static void insertManualDiffHelp( ManualDiffHelpList* pManualDiffHelpList, int winIdx, int firstLine, int lastLine )
{
   // The manual diff help list must be sorted and compact.
   // "Compact" means that upper items can't be empty if lower items contain data.

   // First insert the new item without regarding compactness.
   // If the new item overlaps with previous items then the previous items will be removed.

   ManualDiffHelpEntry mdhe;
   mdhe.firstLine( winIdx ) = firstLine;
   mdhe.lastLine( winIdx ) = lastLine;

   ManualDiffHelpList::iterator i;
   for( i=pManualDiffHelpList->begin(); i!=pManualDiffHelpList->end(); ++i )
   {
      int& l1 = i->firstLine( winIdx );
      int& l2 = i->lastLine( winIdx );
      if (l1>=0 && l2>=0)
      {
         if ( (firstLine<=l1 && lastLine>=l1)  ||  (firstLine <=l2 && lastLine>=l2) )
         {
            // overlap
            l1 = -1;
            l2 = -1;
         }
         if ( firstLine<l1 && lastLine<l1 )
         {
            // insert before this position
            pManualDiffHelpList->insert( i, mdhe );
            break;
         }
      }
   }
   if ( i == pManualDiffHelpList->end() )
   {
      pManualDiffHelpList->insert( i, mdhe );
   }

   // Now make the list compact
   for( int wIdx=1; wIdx<=3; ++wIdx )
   {
      ManualDiffHelpList::iterator iEmpty = pManualDiffHelpList->begin();
      for( i=pManualDiffHelpList->begin(); i!=pManualDiffHelpList->end(); ++i )
      {
         if ( iEmpty->firstLine(wIdx) >= 0 )
         {
            ++iEmpty;
            continue;
         }
         if ( i->firstLine(wIdx)>=0 )  // Current item is not empty -> move it to the empty place
         {
            iEmpty->firstLine(wIdx) = i->firstLine(wIdx);
            iEmpty->lastLine(wIdx) = i->lastLine(wIdx);
            i->firstLine(wIdx) = -1;
            i->lastLine(wIdx) = -1;
            ++iEmpty;
         }
      }
   }
   pManualDiffHelpList->remove( ManualDiffHelpEntry() ); // Remove all completely empty items.
}

void KDiff3App::slotAddManualDiffHelp()
{
   int firstLine = -1;
   int lastLine = -1;
   int winIdx = -1;
   if (                m_pDiffTextWindow1 ) { m_pDiffTextWindow1->getSelectionRange(&firstLine, &lastLine, eFileCoords); winIdx=1; }
   if ( firstLine<0 && m_pDiffTextWindow2 ) { m_pDiffTextWindow2->getSelectionRange(&firstLine, &lastLine, eFileCoords); winIdx=2; }
   //if ( firstLine<0 && m_pDiffTextWindow3 ) { m_pDiffTextWindow3->getSelectionRange(&firstLine, &lastLine, eFileCoords); winIdx=3; }

//   if ( firstLine<0 || lastLine <0 || lastLine<firstLine )
//      KMessageBox::information( this, tr("Nothing is selected in either diff input window."), tr("Error while adding manual diff range") );
//   else
//   {
//   /*
//      ManualDiffHelpEntry mdhe;
//      if (!m_manualDiffHelpList.empty()) mdhe = m_manualDiffHelpList.front();
//      if ( winIdx==1 ) { mdhe.lineA1 = firstLine; mdhe.lineA2 = lastLine; }
//      if ( winIdx==2 ) { mdhe.lineB1 = firstLine; mdhe.lineB2 = lastLine; }
//      if ( winIdx==3 ) { mdhe.lineC1 = firstLine; mdhe.lineC2 = lastLine; }
//      m_manualDiffHelpList.clear();
//      m_manualDiffHelpList.push_back( mdhe );
//      */
//
//      insertManualDiffHelp( &m_manualDiffHelpList, winIdx, firstLine, lastLine );
//
//      init( false, 0, false ); // Init without reload
//      slotRefresh();
//   }
}

//void KDiff3App::slotClearManualDiffHelpList()
//{
//   m_manualDiffHelpList.clear();
//   init( false, 0, false ); // Init without reload
//   slotRefresh();
//}

void KDiff3App::slotUpdateAvailabilities()
{
//   if (m_pMainSplitter==0)
//      return;

   bool bTextDataAvailable = ( m_sd1.hasData() || m_sd2.hasData());

//   if( dirShowBoth->isChecked() )
//   {
//      if ( m_pDirectoryMergeSplitter!=0 )
//         m_pDirectoryMergeSplitter->setVisible( m_bDirCompare );
//
//      if ( m_pMainWidget!=0 && !m_pMainWidget->isVisible() &&
//           bTextDataAvailable && !m_pDirectoryMergeWindow->isScanning()
//         )
//         m_pMainWidget->show();
//   }


//   bool bDiffWindowVisible = m_pMainWidget != 0 && m_pMainWidget->isVisible();
//   bool bMergeEditorVisible = m_pMergeWindowFrame !=0  &&  m_pMergeWindowFrame->isVisible();

   //m_pDirectoryMergeWindow->updateAvailabilities( m_bDirCompare, bDiffWindowVisible, chooseA, chooseB, chooseC );

   //dirShowBoth->setEnabled( m_bDirCompare );
//   dirViewToggle->setEnabled(
//      m_bDirCompare &&
//      ((!m_pDirectoryMergeSplitter->isVisible()  &&  m_pMainWidget!=0 && m_pMainWidget->isVisible()) ||
//        (m_pDirectoryMergeSplitter->isVisible()  &&  m_pMainWidget!=0 && !m_pMainWidget->isVisible() && bTextDataAvailable) )
//      );

   //bool bDirWindowHasFocus = m_pDirectoryMergeSplitter->isVisible() && m_pDirectoryMergeWindow->hasFocus();

   //showWhiteSpaceCharacters->setEnabled( bDiffWindowVisible );
   //autoAdvance->setEnabled( bMergeEditorVisible );
   //autoSolve->setEnabled( bMergeEditorVisible  &&  m_bTripleDiff );
   //unsolve->setEnabled( bMergeEditorVisible );
//   if ( !bDirWindowHasFocus )
//   {
//      chooseA->setEnabled( bMergeEditorVisible );
//      chooseB->setEnabled( bMergeEditorVisible );
//      chooseC->setEnabled( bMergeEditorVisible  &&  m_bTripleDiff );
//   }
   //chooseAEverywhere->setEnabled( bMergeEditorVisible );
   //chooseBEverywhere->setEnabled( bMergeEditorVisible );
   //chooseCEverywhere->setEnabled( bMergeEditorVisible  &&  m_bTripleDiff );
   //chooseAForUnsolvedConflicts->setEnabled( bMergeEditorVisible );
   //chooseBForUnsolvedConflicts->setEnabled( bMergeEditorVisible );
   //chooseCForUnsolvedConflicts->setEnabled( bMergeEditorVisible  &&  m_bTripleDiff );
   //chooseAForUnsolvedWhiteSpaceConflicts->setEnabled( bMergeEditorVisible );
   //chooseBForUnsolvedWhiteSpaceConflicts->setEnabled( bMergeEditorVisible );
   //chooseCForUnsolvedWhiteSpaceConflicts->setEnabled( bMergeEditorVisible  &&  m_bTripleDiff );
   //mergeHistory->setEnabled( bMergeEditorVisible );
   //mergeRegExp->setEnabled( bMergeEditorVisible );
   //showWindowA->setEnabled( bDiffWindowVisible && ( m_pDiffTextWindow2->isVisible() || m_pDiffTextWindow3->isVisible() ) );
   //showWindowB->setEnabled( bDiffWindowVisible && ( m_pDiffTextWindow1->isVisible() || m_pDiffTextWindow3->isVisible() ));
   //showWindowC->setEnabled( bDiffWindowVisible &&  m_bTripleDiff && ( m_pDiffTextWindow1->isVisible() || m_pDiffTextWindow2->isVisible() ) );
   //editFind->setEnabled( bDiffWindowVisible );
   //editFindNext->setEnabled( bDiffWindowVisible );
   //m_pFindDialog->m_pSearchInC->setEnabled( m_bTripleDiff );
   //m_pFindDialog->m_pSearchInOutput->setEnabled( bMergeEditorVisible );

   //bool bSavable = bMergeEditorVisible && m_pMergeResultWindow->getNrOfUnsolvedConflicts()==0;
   //fileSave->setEnabled( m_bOutputModified && bSavable );
   //fileSaveAs->setEnabled( bSavable );

   //goTop->setEnabled( bDiffWindowVisible &&  m_pMergeResultWindow->isDeltaAboveCurrent() );
   //goBottom->setEnabled( bDiffWindowVisible &&  m_pMergeResultWindow->isDeltaBelowCurrent() );
   //goCurrent->setEnabled( bDiffWindowVisible );
   //goPrevUnsolvedConflict->setEnabled( bMergeEditorVisible &&  m_pMergeResultWindow->isUnsolvedConflictAboveCurrent() );
   //goNextUnsolvedConflict->setEnabled( bMergeEditorVisible &&  m_pMergeResultWindow->isUnsolvedConflictBelowCurrent() );
   //goPrevConflict->setEnabled( bDiffWindowVisible &&  m_pMergeResultWindow->isConflictAboveCurrent() );
   //goNextConflict->setEnabled( bDiffWindowVisible &&  m_pMergeResultWindow->isConflictBelowCurrent() );
   //goPrevDelta->setEnabled( bDiffWindowVisible &&  m_pMergeResultWindow->isDeltaAboveCurrent() );
   //goNextDelta->setEnabled( bDiffWindowVisible &&  m_pMergeResultWindow->isDeltaBelowCurrent() );

   //overviewModeNormal->setEnabled( m_bTripleDiff && bDiffWindowVisible );
   //overviewModeAB->setEnabled( m_bTripleDiff && bDiffWindowVisible );
   //overviewModeAC->setEnabled( m_bTripleDiff && bDiffWindowVisible );
   //overviewModeBC->setEnabled( m_bTripleDiff && bDiffWindowVisible );
   //Overview::e_OverviewMode overviewMode = m_pOverview==0 ? Overview::eOMNormal : m_pOverview->getOverviewMode();
   //overviewModeNormal->setChecked( overviewMode == Overview::eOMNormal );
   //overviewModeAB->setChecked( overviewMode == Overview::eOMAvsB );
   //overviewModeAC->setChecked( overviewMode == Overview::eOMAvsC );
   //overviewModeBC->setChecked( overviewMode == Overview::eOMBvsC );

   //winToggleSplitOrientation->setEnabled( bDiffWindowVisible && m_pDiffWindowSplitter!=0 );
}
