/***************************************************************************
    orginal file:        kdiff3.h  -  description
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

#ifndef KDIFF3_H
#define KDIFF3_H

#include "diff.h"
#include "mergeresultwindow.h"

//#include <config-kdiff3.h>

// include files for Qt
#include <QSplitter>
#include <QScrollBar>




// forward declaration of the KDiff3 classes
class OptionDialog;
class FindDialog;
class ManualDiffHelpDialog;
class DiffTextWindow;
class DiffTextWindowFrame;
//class MergeResultWindow;
//class WindowTitleWidget;


class QScrollBar;
class QSplitter;


class ReversibleScrollBar : public QScrollBar
{
   Q_OBJECT
   bool* m_pbRightToLeftLanguage;
   int m_realVal;
public:
   ReversibleScrollBar( Qt::Orientation o, bool* pbRightToLeftLanguage )
      : QScrollBar( o )
   {
      m_pbRightToLeftLanguage=pbRightToLeftLanguage;
      m_realVal=0;
      connect( this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
   }
   void setAgain(){ setValue(m_realVal); }
public slots:
   void slotValueChanged(int i)
   {
      m_realVal = i;
      if(m_pbRightToLeftLanguage && *m_pbRightToLeftLanguage)
         m_realVal = maximum()-(i-minimum());
      emit valueChanged2(m_realVal);
   }
   void setValue(int i)
   {
      if(m_pbRightToLeftLanguage && *m_pbRightToLeftLanguage)
         QScrollBar::setValue( maximum()-(i-minimum())  );
      else
         QScrollBar::setValue( i );
   }
signals:
   void valueChanged2(int);
};

class KDiff3App : public QWidget
{
   Q_OBJECT

public:
   /** constructor of KDiff3App, calls all init functions to create the application.
     */
   KDiff3App( QWidget* parent, const char* name);
   ~KDiff3App();

   bool isPart();

   // Finish initialisation (virtual, so that it can be called from the shell too.)
   virtual void completeInit(const QString& fn1="", const QString& fn2="");

   virtual bool isFileSaved();
   virtual bool isDirComparison();

signals:
   void createNewInstance( const QString& fn1, const QString& fn2, const QString& fn3 );

protected:
   void initDirectoryMergeActions();
   /** sets up the statusbar for the main window by initialzing a statuslabel. */


   /** creates the centerwidget of the KMainWindow instance and sets it as the view */
   void initView();

public slots:

   /** open a file and load it into the document*/
   void slotFileOpen();
   void slotFileOpen2( QString fn1, QString fn2, QString ofn,
                       QString an1, QString an2, TotalDiffStatus* pTotalDiffStatus );

   void slotFileNameChanged(const QString& fileName, int winIdx);


   /** put the marked text/object into the clipboard
     */
   void slotEditCopy();
   /** paste the clipboard into the document
     */
   void slotEditPaste();


private slots:


private:

   ReversibleScrollBar* m_pHScrollBar;
   QScrollBar* m_pDiffVScrollBar;
   QScrollBar* m_pMergeVScrollBar;

   DiffTextWindow* m_pDiffTextWindow1;
   DiffTextWindow* m_pDiffTextWindow2;
   DiffTextWindowFrame* m_pDiffTextWindowFrame1;
   DiffTextWindowFrame* m_pDiffTextWindowFrame2;
   QSplitter* m_pDiffWindowSplitter;

   MergeResultWindow* m_pMergeResultWindow;
   bool m_bTripleDiff;
   bool m_bDirCompare;

   Overview* m_pOverview;

   QWidget* m_pCornerWidget;

   TotalDiffStatus m_totalDiffStatus;

   SourceData m_sd1;
   SourceData m_sd2;

   QString m_outputFilename;
   bool m_bDefaultFilename;

   DiffList m_diffList12;
   DiffList m_diffList23;
   DiffList m_diffList13;

   DiffBufferInfo m_diffBufferInfo;
   Diff3LineList m_diff3LineList;
   Diff3LineVector m_diff3LineVector;
   ManualDiffHelpList m_manualDiffHelpList;

   int m_neededLines;
   int m_DTWHeight;
   bool m_bOutputModified;
   bool m_bFileSaved;
   bool m_bTimerBlock;      // Synchronisation

   OptionDialog* m_pOptionDialog;
   FindDialog*   m_pFindDialog;

   void init( bool bAuto=false, TotalDiffStatus* pTotalDiffStatus=0, bool bLoadFiles=true );


   virtual bool eventFilter( QObject* o, QEvent* e );
   virtual void resizeEvent(QResizeEvent*);

   bool improveFilenames(bool bCreateNewInstance);

   bool runDiff( const LineData* p1, int size1, const LineData* p2, int size2, DiffList& diffList, int winIdx1, int winIdx2 );
   bool runDiff( const LineData* p1, int size1, const LineData* p2, int size2, DiffList& diffList );
   bool canContinue();

   void choose(int choice);

   bool m_bAutoFlag;
   bool m_bAutoMode;
   void recalcWordWrap(int nofVisibleColumns=-1);
   void setHScrollBarRange();

public slots:
   void resizeDiffTextWindow(int newWidth, int newHeight);
   void resizeMergeResultWindow();
   void slotRecalcWordWrap();


   void scrollDiffTextWindow( int deltaX, int deltaY );
   void scrollMergeResultWindow( int deltaX, int deltaY );
   void setDiff3Line( int line );


   void slotUpdateAvailabilities();

   void slotRefresh();
   void slotSelectionEnd();
   void slotSelectionStart();
   void slotAfterFirstPaint();

   void slotReload();

   void slotWinFocusNext();
   void slotWinFocusPrev();
   void slotWinToggleSplitterOrientation();
   void slotOverviewNormal();
   void slotOverviewAB();

   void slotSplitDiff();
   void slotJoinDiffs();
   void slotAddManualDiffHelp();

   void slotNoRelevantChangesDetected();
};

#endif // KDIFF3_H
