/***************************************************************************
     orginal file:       pdiff.cpp  -  Implementation for class KDiff3App
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
#include <QPoint>
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
#include "difftextwindow.h"

bool g_bIgnoreWhiteSpace = true;
bool g_bIgnoreTrivialMatches = true;

bool KDiff3App::runDiff(const LineData *p1, int size1, const LineData *p2, int size2,
                        DiffList &diffList)
{
    ProgressProxy pp;
    static GnuDiff gnuDiff;  // All values are initialized with zeros.

    pp.setCurrent(0);

    diffList.clear();

    if (p1[0].pLine == 0 || p2[0].pLine == 0 || size1 == 0 || size2 == 0) {
        Diff d(0, 0, 0);

        if (p1[0].pLine == 0 && p2[0].pLine == 0 && size1 == size2) {
            d.nofEquals = size1;
        } else {
            d.diff1 = size1;
            d.diff2 = size2;
        }

        diffList.push_back(d);
    } else {
        GnuDiff::comparison comparisonInput;
        memset(&comparisonInput, 0, sizeof(comparisonInput));
        comparisonInput.parent = 0;
        comparisonInput.file[0].buffer = p1[0].pLine;//ptr to buffer
        comparisonInput.file[0].buffered = (p1[size1 - 1].pLine - p1[0].pLine + p1[size1 -
                                            1].size); // size of buffer
        comparisonInput.file[1].buffer = p2[0].pLine;//ptr to buffer
        comparisonInput.file[1].buffered = (p2[size2 - 1].pLine - p2[0].pLine + p2[size2 -
                                            1].size); // size of buffer

        gnuDiff.ignore_white_space =
            GnuDiff::IGNORE_ALL_SPACE;  // I think nobody needs anything else ...
        gnuDiff.bIgnoreWhiteSpace = true;
        gnuDiff.bIgnoreNumbers    = false; //m_pOptionDialog->m_bIgnoreNumbers;
        gnuDiff.minimal = false; //m_pOptionDialog->m_bTryHard;
        gnuDiff.ignore_case = false;
        GnuDiff::change *script = gnuDiff.diff_2_files(&comparisonInput);

        int equalLinesAtStart =  comparisonInput.file[0].prefix_lines;
        int currentLine1 = 0;
        int currentLine2 = 0;
        GnuDiff::change *p = 0;

        for (GnuDiff::change *e = script; e; e = p) {
            Diff d(0, 0, 0);
            d.nofEquals = e->line0 - currentLine1;
            assert(d.nofEquals == e->line1 - currentLine2);
            d.diff1 = e->deleted;
            d.diff2 = e->inserted;
            currentLine1 += d.nofEquals + d.diff1;
            currentLine2 += d.nofEquals + d.diff2;
            diffList.push_back(d);

            p = e->link;
            free(e);
        }

        if (diffList.empty()) {
            Diff d(0, 0, 0);
            d.nofEquals = min2(size1, size2);
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
        } else {
            diffList.front().nofEquals += equalLinesAtStart;
            currentLine1 += equalLinesAtStart;
            currentLine2 += equalLinesAtStart;

            int nofEquals = min2(size1 - currentLine1, size2 - currentLine2);

            if (nofEquals == 0) {
                diffList.back().diff1 += size1 - currentLine1;
                diffList.back().diff2 += size2 - currentLine2;
            } else {
                Diff d(nofEquals, size1 - currentLine1 - nofEquals, size2 - currentLine2 - nofEquals);
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
        int l1 = 0;
        int l2 = 0;
        DiffList::iterator i;

        for (i = diffList.begin(); i != diffList.end(); ++i) {
            l1 += i->nofEquals + i->diff1;
            l2 += i->nofEquals + i->diff2;
        }

        //if( l1!=p1-p1start || l2!=p2-p2start )
        if (l1 != size1 || l2 != size2) {
            assert(false);
        }
    }
#endif

    pp.setCurrent(1.0);

    return true;
}

bool KDiff3App::runDiff(const LineData *p1, int size1, const LineData *p2, int size2,
                        DiffList &diffList,
                        int winIdx1, int winIdx2)
{
    diffList.clear();
    DiffList diffList2;

    int l1begin = 0;
    int l2begin = 0;
    ManualDiffHelpList::const_iterator i;

    for (i = m_manualDiffHelpList.begin(); i != m_manualDiffHelpList.end(); ++i) {
        const ManualDiffHelpEntry &mdhe = *i;

        int l1end = winIdx1 == 1 ? mdhe.lineA1 : winIdx1 == 2 ? mdhe.lineB1 : mdhe.lineC1 ;
        int l2end = winIdx2 == 1 ? mdhe.lineA1 : winIdx2 == 2 ? mdhe.lineB1 : mdhe.lineC1 ;

        if (l1end >= 0 && l2end >= 0) {
            runDiff(p1 + l1begin, l1end - l1begin, p2 + l2begin, l2end - l2begin, diffList2);
            diffList.splice(diffList.end(), diffList2);
            l1begin = l1end;
            l2begin = l2end;

            l1end = winIdx1 == 1 ? mdhe.lineA2 : winIdx1 == 2 ? mdhe.lineB2 : mdhe.lineC2 ;
            l2end = winIdx2 == 1 ? mdhe.lineA2 : winIdx2 == 2 ? mdhe.lineB2 : mdhe.lineC2 ;

            if (l1end >= 0 && l2end >= 0) {
                ++l1end; // point to line after last selected line
                ++l2end;
                runDiff(p1 + l1begin, l1end - l1begin, p2 + l2begin, l2end - l2begin, diffList2);
                diffList.splice(diffList.end(), diffList2);
                l1begin = l1end;
                l2begin = l2end;
            }
        }
    }

    runDiff(p1 + l1begin, size1 - l1begin, p2 + l2begin, size2 - l2begin, diffList2);
    diffList.splice(diffList.end(), diffList2);
    return true;
}

bool KDiff3App::init(bool bAuto, TotalDiffStatus *pTotalDiffStatus, bool bLoadFiles)
{
    ProgressProxy pp;

    QTextCodec *m_pEncodingB = QTextCodec::codecForName("UTF-8");

    // When doing a full analysis in the directory-comparison, then the statistics-results
    // will be stored in the given TotalDiffStatus. Otherwise it will be 0.
    bool bGUI = pTotalDiffStatus == 0;

    if (pTotalDiffStatus == 0) {
        pTotalDiffStatus = &m_totalDiffStatus;
    }

    bool bVisibleMergeResultWindow = ! m_outputFilename.isEmpty();

    // Because of the progressdialog paintevents can occur, but data is invalid,
    // so painting must be suppressed.
    if (m_pDiffTextWindow1) {
        m_pDiffTextWindow1->setPaintingAllowed(false);
    }

    if (m_pDiffTextWindow2) {
        m_pDiffTextWindow2->setPaintingAllowed(false);
    }

    m_diff3LineList.clear();

    if (bLoadFiles) {
        m_manualDiffHelpList.clear();

        pp.setMaxNofSteps(4);    // Read 2 files, 1 comparison, 1 finediff

        // First get all input data.
        pp.setInformation(tr("Loading A"));
        m_sd1.readAndPreprocess(m_pEncodingB, true);
        pp.step();

        pp.setInformation(tr("Loading B"));
        m_sd2.readAndPreprocess(m_pEncodingB, true);
        pp.step();
    } else {
        pp.setMaxNofSteps(2);    // 1 comparison, 1 finediff
    }

    pTotalDiffStatus->reset();
    // Run the diff.

    pTotalDiffStatus->bBinaryAEqB = m_sd1.isBinaryEqualWith(m_sd2);
    pp.setInformation(tr("Diff: A <-> B"));

    runDiff(m_sd1.getLineDataForDiff(), m_sd1.getSizeLines(), m_sd2.getLineDataForDiff(),
            m_sd2.getSizeLines(), m_diffList12, 1, 2);

    pp.step();

    pp.setInformation(tr("Linediff: A <-> B"));
    calcDiff3LineListUsingAB(&m_diffList12, m_diff3LineList);
    fineDiff(m_diff3LineList, 1, m_sd1.getLineDataForDisplay(), m_sd2.getLineDataForDisplay(),
             pTotalDiffStatus->bTextAEqB);

    if (m_sd1.getSizeBytes() == 0) {
        pTotalDiffStatus->bTextAEqB = false;
    }

    pp.step();

    m_diffBufferInfo.init(&m_diff3LineList, &m_diff3LineVector,
                          m_sd1.getLineDataForDiff(), m_sd1.getSizeLines(),
                          m_sd2.getLineDataForDiff(), m_sd2.getSizeLines());
    calcWhiteDiff3Lines(m_diff3LineList, m_sd1.getLineDataForDiff(), m_sd2.getLineDataForDiff());
    calcDiff3LineVector(m_diff3LineList, m_diff3LineVector);

    // Calc needed lines for display
    m_neededLines = static_cast<int>(m_diff3LineList.size());

    setVisible(bGUI);

    m_bTripleDiff = false; // m_sd3.isEmpty()

    if (bGUI) {

        const ManualDiffHelpList *pMDHL = &m_manualDiffHelpList;

        //qDebug() << "m_sd1.getAliasName()" << m_sd1.getAliasName();

        m_pDiffTextWindow1->init(m_sd1.getAliasName(), m_sd1.getEncoding(),
                                 m_sd1.getLineDataForDisplay(), m_sd1.getSizeLines(), &m_diff3LineVector, pMDHL, m_bTripleDiff);
        m_pDiffTextWindow2->init(m_sd2.getAliasName(), m_sd2.getEncoding(),
                                 m_sd2.getLineDataForDisplay(), m_sd2.getSizeLines(), &m_diff3LineVector, pMDHL, m_bTripleDiff);
    }

    m_pMergeResultWindow->init(
        m_sd1.getLineDataForDisplay(), m_sd1.getSizeLines(),
        m_sd2.getLineDataForDisplay(), m_sd2.getSizeLines(), &m_diff3LineList, pTotalDiffStatus);

    m_pOverview->init(&m_diff3LineList, m_bTripleDiff);
    m_pHScrollBar->setValue(0);

    m_pDiffTextWindow1->setPaintingAllowed(true);
    m_pDiffTextWindow2->setPaintingAllowed(true);
    m_pOverview->setPaintingAllowed(true);

    setHScrollBarRange();

    if (bLoadFiles) {
        /*if ( bVisibleMergeResultWindow && !bAuto )
           m_pMergeResultWindow->showNrOfConflicts();
        else */if (!bAuto &&
                   // Avoid showing this message during startup without parameters.
                   !(m_sd1.getAliasName().isEmpty() && m_sd2.getAliasName().isEmpty()) &&
                   (m_sd1.isValid() && m_sd2.isValid())
                  ) {
            QString totalInfo;

            if (pTotalDiffStatus->bBinaryAEqB && pTotalDiffStatus->bBinaryAEqC) {
                totalInfo += tr("All input files are binary equal.");
            } else  if (pTotalDiffStatus->bTextAEqB && pTotalDiffStatus->bTextAEqC) {
                totalInfo += tr("All input files contain the same text, but are not binary equal.");
            } else {
                if (pTotalDiffStatus->bBinaryAEqB) {
                    totalInfo += tr("Files %1 and %2 are binary equal.\n").arg(QString("A"), QString("B"));
                } else if (pTotalDiffStatus->bTextAEqB) {
                    totalInfo += tr("Files %1 and %2 have equal text, but are not binary equal. \n").arg(
                                     QString("A"), QString("B"));
                }

                if (pTotalDiffStatus->bBinaryAEqC) {
                    totalInfo += tr("Files %1 and %2 are binary equal.\n").arg(QString("A"), QString("C"));
                } else if (pTotalDiffStatus->bTextAEqC) {
                    totalInfo += tr("Files %1 and %2 have equal text, but are not binary equal. \n").arg(
                                     QString("A"), QString("C"));
                }

                if (pTotalDiffStatus->bBinaryBEqC) {
                    totalInfo += tr("Files %1 and %2 are binary equal.\n").arg(QString("B"), QString("C"));
                } else if (pTotalDiffStatus->bTextBEqC) {
                    totalInfo += tr("Files %1 and %2 have equal text, but are not binary equal. \n").arg(
                                     QString("B"), QString("C"));
                }
            }

            //QMessageBox::information( this, tr("Diff"), totalInfo );
            //         if ( !totalInfo.isEmpty() )
            //            KMessageBox::information( this, totalInfo );
        }

        if (bVisibleMergeResultWindow && (!m_sd1.isText() || !m_sd2.isText())) {
            QMessageBox::information(this, tr("Diff"), tr(
                                         "Some inputfiles don't seem to be pure textfiles.\n"
                                         "Note that the KDiff3-merge was not meant for binary data.\n"
                                         "Continue at your own risk."));
        }
    }

    QTimer::singleShot(10, this, SLOT(slotAfterFirstPaint()));

    /* if ( bVisibleMergeResultWindow && m_pMergeResultWindow )
     {
        m_pMergeResultWindow->setFocus();
     }

     else */if (m_pDiffTextWindow1) {
        m_pDiffTextWindow1->setFocus();
    }

    return pTotalDiffStatus->bTextAEqB;
}


void KDiff3App::setHScrollBarRange()
{
    int w1 = m_pDiffTextWindow1 != 0
             && m_pDiffTextWindow1->isVisible() ? m_pDiffTextWindow1->getNofColumns() : 0;
    int w2 = m_pDiffTextWindow2 != 0
             && m_pDiffTextWindow2->isVisible() ? m_pDiffTextWindow2->getNofColumns() : 0;

    int v1 = m_pDiffTextWindow1 != 0
             && m_pDiffTextWindow1->isVisible() ? m_pDiffTextWindow1->getNofVisibleColumns() : 0;
    int v2 = m_pDiffTextWindow2 != 0
             && m_pDiffTextWindow2->isVisible() ? m_pDiffTextWindow2->getNofVisibleColumns() : 0;

    // Find the minimum, but don't consider 0.
    int pageStep = 0;

    if ((pageStep == 0 || pageStep > v1) && v1 > 0) {
        pageStep = v1;
    }

    if ((pageStep == 0 || pageStep > v2) && v2 > 0) {
        pageStep = v2;
    }

    int rangeMax = 0;

    if (w1 > v1 && w1 - v1 > rangeMax && v1 > 0) {
        rangeMax = w1 - v1;
    }

    if (w2 > v2 && w2 - v2 > rangeMax && v2 > 0) {
        rangeMax = w2 - v2;
    }

    m_pHScrollBar->setRange(0, rangeMax);
    m_pHScrollBar->setPageStep(pageStep);
}

void KDiff3App::resizeDiffTextWindow(int /*newWidth*/, int newHeight)
{
    m_DTWHeight = newHeight;

    recalcWordWrap();

    m_pDiffVScrollBar->setRange(0, max2(0, m_neededLines + 1 - newHeight));
    m_pDiffVScrollBar->setPageStep(newHeight);
    m_pOverview->setRange(m_pDiffVScrollBar->value(), m_pDiffVScrollBar->pageStep());

    setHScrollBarRange();
}

void KDiff3App::resizeMergeResultWindow()
{
}

void KDiff3App::scrollDiffTextWindow(int deltaX, int deltaY)
{
    if (deltaY != 0) {
        m_pDiffVScrollBar->setValue(m_pDiffVScrollBar->value() + deltaY);
        m_pOverview->setRange(m_pDiffVScrollBar->value(), m_pDiffVScrollBar->pageStep());
    }

    if (deltaX != 0) {
        m_pHScrollBar->QScrollBar::setValue(m_pHScrollBar->value() + deltaX);
    }
}

void KDiff3App::scrollMergeResultWindow(int deltaX, int deltaY)
{
    if (deltaY != 0) {
        m_pMergeVScrollBar->setValue(m_pMergeVScrollBar->value() + deltaY);
    }

    if (deltaX != 0) {
        m_pHScrollBar->setValue(m_pHScrollBar->value() + deltaX);
    }
}

void KDiff3App::setDiff3Line(int line)
{
    m_pDiffVScrollBar->setValue(line);
}

// Function uses setMinSize( sizeHint ) before adding the widget.
// void addWidget(QBoxLayout* layout, QWidget* widget);
template <class W, class L>
void addWidget(L *layout, W *widget)
{
    QSize s = widget->sizeHint();
    widget->setMinimumSize(QSize(max2(s.width(), 0), max2(s.height(), 0)));
    layout->addWidget(widget);
}

void KDiff3App::initView()
{

    contextMenu = new QMenu(this);

    QAction *copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(slotEditCopy()));

    //   QAction *selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"), this);
    //   selAllAct->setShortcut(QKeySequence::SelectAll);
    //   selAllAct->setStatusTip(tr("Select all"));
    //   connect(selAllAct, SIGNAL(triggered()), this, SLOT(sellAll()));

    contextMenu->addAction(copyAct);
    //contextMenu->addSeparator();
    //contextMenu->addAction(selAllAct);

    QVBoxLayout *pVLayout = new QVBoxLayout();
    pVLayout->setContentsMargins(0, 0, 0, 0);
    pVLayout->setSpacing(0);
    setLayout(pVLayout);

    QWidget *pDiffWindowFrame = new QWidget(); // Contains diff windows, overview and vert scrollbar
    pDiffWindowFrame->setObjectName("DiffWindowFrame");
    QHBoxLayout *pDiffHLayout = new QHBoxLayout(pDiffWindowFrame);
    pDiffHLayout->setContentsMargins(0, 0, 0, 0);
    pDiffHLayout->setSpacing(0);
    pVLayout->addWidget(pDiffWindowFrame);

    m_pDiffWindowSplitter = new QSplitter();
    m_pDiffWindowSplitter->setObjectName("DiffWindowSplitter");
    m_pDiffWindowSplitter->setOpaqueResize(false);

    m_pDiffWindowSplitter->setOrientation(true ?  Qt::Horizontal :
                                          Qt::Vertical);   //m_pOptionDialog->m_bHorizDiffWindowSplitting
    pDiffHLayout->addWidget(m_pDiffWindowSplitter);

    m_pOverview = new Overview(m_pOptionDialog);
    m_pOverview->setObjectName("Overview");
    pDiffHLayout->addWidget(m_pOverview);
    connect(m_pOverview, SIGNAL(setLine(int)), this, SLOT(setDiff3Line(int)));

    m_pDiffVScrollBar = new QScrollBar(Qt::Vertical, pDiffWindowFrame);
    pDiffHLayout->addWidget(m_pDiffVScrollBar);

    m_pDiffTextWindowFrame1 = new DiffTextWindowFrame(m_pDiffWindowSplitter, m_pOptionDialog, 1,
            m_extensions);
    m_pDiffWindowSplitter->addWidget(m_pDiffTextWindowFrame1);

    m_pDiffTextWindowFrame2 = new DiffTextWindowFrame(m_pDiffWindowSplitter, m_pOptionDialog, 2,
            m_extensions);
    m_pDiffWindowSplitter->addWidget(m_pDiffTextWindowFrame2);

    m_pDiffTextWindow1 = m_pDiffTextWindowFrame1->getDiffTextWindow();
    m_pDiffTextWindow2 = m_pDiffTextWindowFrame2->getDiffTextWindow();

    connect(m_pDiffTextWindowFrame1, SIGNAL(fileNameChanged(const QString &, int)), this,
            SLOT(slotFileNameChanged(const QString &, int)));
    connect(m_pDiffTextWindowFrame2, SIGNAL(fileNameChanged(const QString &, int)), this,
            SLOT(slotFileNameChanged(const QString &, int)));

    m_pMergeResultWindow = new MergeResultWindow(this, m_pOptionDialog);

    QHBoxLayout *pHScrollBarLayout = new QHBoxLayout();
    pVLayout->addLayout(pHScrollBarLayout);
    //layout()->addItem(pHScrollBarLayout);

    bool bj;
    m_pHScrollBar = new ReversibleScrollBar(Qt::Horizontal,
                                            &bj);   //m_pOptionDialog->m_bHorizDiffWindowSplitting
    pHScrollBarLayout->addWidget(m_pHScrollBar);
    m_pCornerWidget = new QWidget(this);
    pHScrollBarLayout->addWidget(m_pCornerWidget);

    connect(m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pOverview, SLOT(setFirstLine(int)));
    connect(m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pDiffTextWindow1,
            SLOT(setFirstLine(int)));
    connect(m_pHScrollBar, SIGNAL(valueChanged2(int)), m_pDiffTextWindow1,
            SLOT(setFirstColumn(int)));
    connect(m_pDiffTextWindow1, SIGNAL(newSelection()), this, SLOT(slotSelectionStart()));
    connect(m_pDiffTextWindow1, SIGNAL(selectionEnd()), this, SLOT(slotSelectionEnd()));
    connect(m_pDiffTextWindow1, SIGNAL(scroll(int, int)), this, SLOT(scrollDiffTextWindow(int,
            int)));
    connect(m_pDiffTextWindow1, SIGNAL(lineClicked(QString, int)), this, SIGNAL(lineClicked(QString,
            int)));
    connect(m_pDiffTextWindow1, SIGNAL(showPopupMenu(const QPoint &)), this,
            SLOT(showPopupMenu(const QPoint &)));
    m_pDiffTextWindow1->installEventFilter(this);

    connect(m_pDiffVScrollBar, SIGNAL(valueChanged(int)), m_pDiffTextWindow2,
            SLOT(setFirstLine(int)));
    connect(m_pHScrollBar, SIGNAL(valueChanged2(int)), m_pDiffTextWindow2,
            SLOT(setFirstColumn(int)));
    connect(m_pDiffTextWindow2, SIGNAL(newSelection()), this, SLOT(slotSelectionStart()));
    connect(m_pDiffTextWindow2, SIGNAL(selectionEnd()), this, SLOT(slotSelectionEnd()));
    connect(m_pDiffTextWindow2, SIGNAL(scroll(int, int)), this, SLOT(scrollDiffTextWindow(int,
            int)));
    connect(m_pDiffTextWindow2, SIGNAL(lineClicked(QString, int)), this, SIGNAL(lineClicked(QString,
            int)));
    connect(m_pDiffTextWindow2, SIGNAL(showPopupMenu(const QPoint &)), this,
            SLOT(showPopupMenu(const QPoint &)));
    m_pDiffTextWindow2->installEventFilter(this);

    MergeResultWindow *p = m_pMergeResultWindow;

    connect(p, SIGNAL(selectionEnd()), this, SLOT(slotSelectionEnd()));

    //connect( p, SIGNAL( showPopupMenu(const QPoint&) ), this, SLOT(showPopupMenu(const QPoint&)));

    connect(p, SIGNAL(setFastSelectorRange(int, int)), m_pDiffTextWindow1,
            SLOT(setFastSelectorRange(int, int)));
    connect(p, SIGNAL(setFastSelectorRange(int, int)), m_pDiffTextWindow2,
            SLOT(setFastSelectorRange(int, int)));

    connect(m_pDiffTextWindow1, SIGNAL(setFastSelectorLine(int)), p,
            SLOT(slotSetFastSelectorLine(int)));
    connect(m_pDiffTextWindow2, SIGNAL(setFastSelectorLine(int)), p,
            SLOT(slotSetFastSelectorLine(int)));

    connect(m_pDiffTextWindow1, SIGNAL(gotFocus()), p, SLOT(updateSourceMask()));
    connect(m_pDiffTextWindow2, SIGNAL(gotFocus()), p, SLOT(updateSourceMask()));

    connect(m_pDiffTextWindow1, SIGNAL(resizeSignal(int, int)), this, SLOT(resizeDiffTextWindow(int,
            int)));
    // The following two connects cause the wordwrap to be recalced thrice, just to make sure. Better than forgetting one.
    connect(m_pDiffTextWindow2, SIGNAL(resizeSignal(int, int)), this, SLOT(slotRecalcWordWrap()));

    m_pDiffTextWindow1->setFocus();
    setMinimumSize(50, 50);
    m_pCornerWidget->setFixedSize(m_pDiffVScrollBar->width(), m_pHScrollBar->height());
}

void KDiff3App::showPopupMenu(QPoint pos)
{
    contextMenu->popup(pos);
}

static int calcManualDiffFirstDiff3LineIdx(const Diff3LineVector &d3lv,
        const ManualDiffHelpEntry &mdhe)
{
    unsigned int i;

    for (i = 0; i < d3lv.size(); ++i) {
        const Diff3Line &d3l = *d3lv[i];

        if ((mdhe.lineA1 >= 0  &&  mdhe.lineA1 == d3l.lineA) ||
                (mdhe.lineB1 >= 0  &&  mdhe.lineB1 == d3l.lineB) ||
                (mdhe.lineC1 >= 0  &&  mdhe.lineC1 == d3l.lineC)) {
            return i;
        }
    }

    return -1;
}

void KDiff3App::slotAfterFirstPaint()
{
    int newHeight = m_pDiffTextWindow1->getNofVisibleLines();
    /*int newWidth  = m_pDiffTextWindow1->getNofVisibleColumns();*/
    m_DTWHeight = newHeight;

    recalcWordWrap();

    m_pDiffVScrollBar->setRange(0, max2(0, m_neededLines + 1 - newHeight));
    m_pDiffVScrollBar->setPageStep(newHeight);
    m_pOverview->setRange(m_pDiffVScrollBar->value(), m_pDiffVScrollBar->pageStep());

    int d3l = -1;

    if (! m_manualDiffHelpList.empty()) {
        d3l = calcManualDiffFirstDiff3LineIdx(m_diff3LineVector, m_manualDiffHelpList.front());
    }

    if (d3l >= 0 && m_pDiffTextWindow1) {
        int line = m_pDiffTextWindow1->convertDiff3LineIdxToLine(d3l);
        m_pDiffVScrollBar->setValue(max2(0, line - 1));
    }

    if (m_pCornerWidget) {
        m_pCornerWidget->setFixedSize(m_pDiffVScrollBar->width(), m_pHScrollBar->height());
    }

    slotUpdateAvailabilities();
    setUpdatesEnabled(true);
    // Workaround for a Qt-bug
    QList<QTreeView *> treeViews = findChildren<QTreeView *>();

    foreach (QTreeView *pTreeView, treeViews) {
        pTreeView->setUpdatesEnabled(true);
    }
}

void KDiff3App::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if (m_pCornerWidget) {
        m_pCornerWidget->setFixedSize(m_pDiffVScrollBar->width(), m_pHScrollBar->height());
    }
}


bool KDiff3App::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {   // key press
        QKeyEvent *k = (QKeyEvent *)e;

        bool bCtrl = (k->QInputEvent::modifiers() & Qt::ControlModifier) != 0;

        if (k->key() == Qt::Key_C &&  bCtrl) {
            slotEditCopy();
            return true;
        }

        if (k->key() == Qt::Key_V &&  bCtrl) {
            slotEditPaste();
            return true;
        }

        int deltaX = 0;
        int deltaY = 0;
        int pageSize =  m_DTWHeight;

        switch (k->key()) {
        case Qt::Key_Down:
            if (!bCtrl) {
                ++deltaY;
            }

            break;

        case Qt::Key_Up:
            if (!bCtrl) {
                --deltaY;
            }

            break;

        case Qt::Key_PageDown:
            if (!bCtrl) {
                deltaY += pageSize;
            }

            break;

        case Qt::Key_PageUp:
            if (!bCtrl) {
                deltaY -= pageSize;
            }

            break;

        case Qt::Key_Left:
            if (!bCtrl) {
                --deltaX;
            }

            break;

        case Qt::Key_Right:
            if (!bCtrl) {
                ++deltaX;
            }

            break;

        case Qt::Key_Home:
            if (bCtrl) {
                m_pDiffVScrollBar->setValue(0);
            } else {
                m_pHScrollBar->setValue(0);
            }

            break;

        case Qt::Key_End:
            if (bCtrl) {
                m_pDiffVScrollBar->setValue(m_pDiffVScrollBar->maximum());
            } else {
                m_pHScrollBar->setValue(m_pHScrollBar->maximum());
            }

            break;

        default:
            break;
        }

        scrollDiffTextWindow(deltaX, deltaY);

        return true;                        // eat event
    } else if (e->type() == QEvent::Wheel) { // wheel event
        QWheelEvent *w = (QWheelEvent *)e;
        w->accept();

        QPoint d = w->pixelDelta();

        if (d.isNull()) {
            d = - w->angleDelta() / 120 * QApplication::wheelScrollLines();
        }

        scrollDiffTextWindow(d.x(), d.y());
        return true;
    } else if (e->type() == QEvent::Drop) {
        QDropEvent *pDropEvent = static_cast<QDropEvent *>(e);
        pDropEvent->accept();

        if (pDropEvent->mimeData()->hasUrls()) {

            QList<QUrl> urlList = pDropEvent->mimeData()->urls();

            if (canContinue() && !urlList.isEmpty()) {
                raise();
                QString filename = urlList.first().toLocalFile();

                if (o == m_pDiffTextWindow1) {
                    m_sd1.setFilename(filename);
                } else if (o == m_pDiffTextWindow2) {
                    m_sd2.setFilename(filename);
                }

                init();
            }

        } else if (pDropEvent->mimeData()->hasText()) {
            QString text = pDropEvent->mimeData()->text();

            if (canContinue()) {
                raise();

                if (o == m_pDiffTextWindow1) {
                    m_sd1.setData(text);
                } else if (o == m_pDiffTextWindow2) {
                    m_sd2.setData(text);
                }

                init();
            }
        }

        return true;
    }

    return QWidget::eventFilter(o, e);      // standard event processing
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
                              QString an1, QString an2, TotalDiffStatus *pTotalDiffStatus)
{
    //if ( !canContinue() ) return;

    if (fn1 == "" && fn2 == "" && ofn == "") {
        return;
    }

    //slotStatusMsg(tr("Opening files..."));

    m_sd1.setFilename(fn1);
    m_sd2.setFilename(fn2);
    //m_sd3.setFilename( fn3 );

    m_sd1.setAliasName(an1);
    m_sd2.setAliasName(an2);
    //m_sd3.setAliasName( an3 );

    if (! ofn.isEmpty()) {
        m_outputFilename = ofn;
        m_bDefaultFilename = false;
    } else {
        m_outputFilename = "";
        m_bDefaultFilename = true;
    }

    init(false, pTotalDiffStatus);

    if (pTotalDiffStatus != 0) {
        return;
    }

    if ((! m_sd1.isEmpty() && ! m_sd1.hasData())  ||
            (! m_sd2.isEmpty() && ! m_sd2.hasData())) {
        QString text(tr("Opening of these files failed:"));
        text += "\n\n";

        if (! m_sd1.isEmpty() && !m_sd1.hasData()) {
            text += " - " + m_sd1.getAliasName() + "\n";
        }

        if (! m_sd2.isEmpty() && !m_sd2.hasData()) {
            text += " - " + m_sd2.getAliasName() + "\n";
        }

        //KMessageBox::sorry( this, text, tr("File open error") );
    }
}

void KDiff3App::slotFileNameChanged(const QString &fileName, int winIdx)
{
    QString fn1 = m_sd1.getFilename();
    QString an1 = m_sd1.getAliasName();
    QString fn2 = m_sd2.getFilename();
    QString an2 = m_sd2.getAliasName();

    if (winIdx == 1) {
        fn1 = fileName;
        an1 = fileName;
    }

    if (winIdx == 2) {
        fn2 = fileName;
        an2 = fileName;
    }

    //if (winIdx==3) { fn3 = fileName; an3 = ""; }

    slotFileOpen2(fn1, fn2, m_outputFilename, an1, an2, 0);
    slotRefresh();
}

void KDiff3App::slotEditCopy()
{
    //slotStatusMsg(tr("Copying selection to clipboard..."));
    QString s;

    if (m_pDiffTextWindow1 != 0) {
        s = m_pDiffTextWindow1->getSelection();
    }

    if (s.isNull() && m_pDiffTextWindow2 != 0) {
        s = m_pDiffTextWindow2->getSelection();
    }

    if (!s.isNull()) {
        QApplication::clipboard()->setText(s, QClipboard::Clipboard);
    }
}

void KDiff3App::slotEditPaste()
{
    if (m_pDiffTextWindow1->hasFocus()) {
        m_sd1.setData(QApplication::clipboard()->text(QClipboard::Clipboard));
        init();
    } else if (m_pDiffTextWindow2->hasFocus()) {
        m_sd2.setData(QApplication::clipboard()->text(QClipboard::Clipboard));
        init();
    }
}

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

void KDiff3App::slotSplitDiff()
{
    int firstLine = -1;
    int lastLine = -1;
    DiffTextWindow *pDTW = 0;

    if (m_pDiffTextWindow1) {
        pDTW = m_pDiffTextWindow1;
        pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords);
    }

    if (firstLine < 0 && m_pDiffTextWindow2) {
        pDTW = m_pDiffTextWindow2;
        pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords);
    }

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
    DiffTextWindow *pDTW = 0;

    if (m_pDiffTextWindow1) {
        pDTW = m_pDiffTextWindow1;
        pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords);
    }

    if (firstLine < 0 && m_pDiffTextWindow2) {
        pDTW = m_pDiffTextWindow2;
        pDTW->getSelectionRange(&firstLine, &lastLine, eD3LLineCoords);
    }

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
    if (m_pDiffTextWindow1 != 0) {
        m_pDiffTextWindow1->setFont(m_pOptionDialog->m_font); //m_pOptionDialog->m_font
        m_pDiffTextWindow1->update();
    }

    if (m_pDiffTextWindow2 != 0) {
        m_pDiffTextWindow2->setFont(m_pOptionDialog->m_font);
        m_pDiffTextWindow2->update();
    }

    if (m_pHScrollBar != 0) {
        m_pHScrollBar->setAgain();
    }

    if (m_pDiffWindowSplitter != 0) {
        m_pDiffWindowSplitter->setOrientation(true ? Qt::Horizontal :
                                              Qt::Vertical);   //m_pOptionDialog->m_bHorizDiffWindowSplitting
    }
}

void KDiff3App::slotSelectionStart()
{
    //editCopy->setEnabled( false );
    //editCut->setEnabled( false );

    const QObject *s = sender();

    if (m_pDiffTextWindow1 && s != m_pDiffTextWindow1) {
        m_pDiffTextWindow1->resetSelection();
    }

    if (m_pDiffTextWindow2 && s != m_pDiffTextWindow2) {
        m_pDiffTextWindow2->resetSelection();
    }

    //if (m_pDiffTextWindow3 && s!=m_pDiffTextWindow3)   m_pDiffTextWindow3->resetSelection();
    //if (m_pMergeResultWindow && s!=m_pMergeResultWindow) m_pMergeResultWindow->resetSelection();
}

void KDiff3App::slotSelectionEnd()
{
    //const QObject* s = sender();
    //editCopy->setEnabled(true);
    //editCut->setEnabled( s==m_pMergeResultWindow );
    if (false) { //m_pOptionDialog->m_bAutoCopySelection
        slotEditCopy();
    } else {
        QClipboard *clipBoard = QApplication::clipboard();

        if (clipBoard->supportsSelection()) {
            QString s;

            if (m_pDiffTextWindow1 != 0) {
                s = m_pDiffTextWindow1->getSelection();
            }

            if (s.isNull() && m_pDiffTextWindow2 != 0) {
                s = m_pDiffTextWindow2->getSelection();
            }

            //if ( s.isNull() && m_pDiffTextWindow3!=0 )   s = m_pDiffTextWindow3->getSelection();
            //if ( s.isNull() && m_pMergeResultWindow!=0 ) s = m_pMergeResultWindow->getSelection();
            if (!s.isNull()) {
                clipBoard->setText(s, QClipboard::Selection);
            }
        }
    }
}

void KDiff3App::slotRecalcWordWrap()
{
    recalcWordWrap();
}

void KDiff3App::recalcWordWrap(int
                               nofVisibleColumns) // nofVisibleColumns is >=0 only for printing, otherwise the really visible width is used
{
    bool bPrinting = nofVisibleColumns >= 0;
    int firstD3LIdx = 0;

    if (m_pDiffTextWindow1) {
        firstD3LIdx = m_pDiffTextWindow1->convertLineToDiff3LineIdx(
                          m_pDiffTextWindow1->getFirstLine());
    }

    // Convert selection to D3L-coords (converting back happens in DiffTextWindow::recalcWordWrap()
    if (m_pDiffTextWindow1) {
        m_pDiffTextWindow1->convertSelectionToD3LCoords();
    }

    if (m_pDiffTextWindow2) {
        m_pDiffTextWindow2->convertSelectionToD3LCoords();
    }

    //   if ( m_pDiffTextWindow3 )
    //      m_pDiffTextWindow3->convertSelectionToD3LCoords();


    if (!m_diff3LineList.empty() && false) { //m_pOptionDialog->m_bWordWrap
        Diff3LineList::iterator i;
        int sumOfLines = 0;

        for (i = m_diff3LineList.begin(); i != m_diff3LineList.end(); ++i) {
            Diff3Line &d3l = *i;
            d3l.linesNeededForDisplay = 1;
            d3l.sumLinesNeededForDisplay = sumOfLines;
            sumOfLines += d3l.linesNeededForDisplay;
        }

        // Let every window calc how many lines will be needed.
        if (m_pDiffTextWindow1) {
            m_pDiffTextWindow1->recalcWordWrap(true, 0, nofVisibleColumns);
        }

        if (m_pDiffTextWindow2) {
            m_pDiffTextWindow2->recalcWordWrap(true, 0, nofVisibleColumns);
        }

        //      if ( m_pDiffTextWindow3 )
        //         m_pDiffTextWindow3->recalcWordWrap(true,0,nofVisibleColumns);

        sumOfLines = 0;

        for (i = m_diff3LineList.begin(); i != m_diff3LineList.end(); ++i) {
            Diff3Line &d3l = *i;
            d3l.sumLinesNeededForDisplay = sumOfLines;
            sumOfLines += d3l.linesNeededForDisplay;
        }

        // Finish the initialisation:
        if (m_pDiffTextWindow1) {
            m_pDiffTextWindow1->recalcWordWrap(true, sumOfLines, nofVisibleColumns);
        }

        if (m_pDiffTextWindow2) {
            m_pDiffTextWindow2->recalcWordWrap(true, sumOfLines, nofVisibleColumns);
        }

        //      if ( m_pDiffTextWindow3 )
        //         m_pDiffTextWindow3->recalcWordWrap(true,sumOfLines,nofVisibleColumns);

        m_neededLines = sumOfLines;
    } else {
        m_neededLines = static_cast<int>(m_diff3LineVector.size());

        if (m_pDiffTextWindow1) {
            m_pDiffTextWindow1->recalcWordWrap(false, 0, 0);
        }

        if (m_pDiffTextWindow2) {
            m_pDiffTextWindow2->recalcWordWrap(false, 0, 0);
        }

        //      if ( m_pDiffTextWindow3 )
        //         m_pDiffTextWindow3->recalcWordWrap(false,0,0);
    }

    if (bPrinting) {
        return;
    }

    if (m_pOverview) {
        m_pOverview->slotRedraw();
    }

    if (m_pDiffTextWindow1) {
        m_pDiffTextWindow1->setFirstLine(m_pDiffTextWindow1->convertDiff3LineIdxToLine(firstD3LIdx));
        m_pDiffTextWindow1->update();
    }

    if (m_pDiffTextWindow2) {
        m_pDiffTextWindow2->setFirstLine(m_pDiffTextWindow2->convertDiff3LineIdxToLine(firstD3LIdx));
        m_pDiffTextWindow2->update();
    }

    //   if ( m_pDiffTextWindow3 )
    //   {
    //      m_pDiffTextWindow3->setFirstLine( m_pDiffTextWindow3->convertDiff3LineIdxToLine( firstD3LIdx ) );
    //      m_pDiffTextWindow3->update();
    //   }

    if (m_pDiffVScrollBar) {
        m_pDiffVScrollBar->setRange(0, max2(0, m_neededLines + 1 - m_DTWHeight));
    }

    if (m_pDiffTextWindow1) {
        m_pDiffVScrollBar->setValue(m_pDiffTextWindow1->convertDiff3LineIdxToLine(firstD3LIdx));

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
//   //if ( !=0 )
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

void KDiff3App::slotWinFocusNext()
{
    QWidget *focus = qApp->focusWidget();


    std::list<QWidget *> visibleWidgetList;

    if (m_pDiffTextWindow1 && m_pDiffTextWindow1->isVisible()) {
        visibleWidgetList.push_back(m_pDiffTextWindow1);
    }

    if (m_pDiffTextWindow2 && m_pDiffTextWindow2->isVisible()) {
        visibleWidgetList.push_back(m_pDiffTextWindow2);
    }

    std::list<QWidget *>::iterator i = std::find(visibleWidgetList.begin(),
                                       visibleWidgetList.end(), focus);
    ++i;

    if (i == visibleWidgetList.end()) {
        i = visibleWidgetList.begin();
    }

    if (i != visibleWidgetList.end()) {
        (*i)->setFocus();
    }
}

void KDiff3App::slotWinFocusPrev()
{
    QWidget *focus = qApp->focusWidget();

    std::list<QWidget *> visibleWidgetList;

    if (m_pDiffTextWindow1 && m_pDiffTextWindow1->isVisible()) {
        visibleWidgetList.push_back(m_pDiffTextWindow1);
    }

    if (m_pDiffTextWindow2 && m_pDiffTextWindow2->isVisible()) {
        visibleWidgetList.push_back(m_pDiffTextWindow2);
    }

    std::list<QWidget *>::iterator i = std::find(visibleWidgetList.begin(),
                                       visibleWidgetList.end(), focus);

    if (i == visibleWidgetList.begin()) {
        i = visibleWidgetList.end();
    }

    --i;

    if (i != visibleWidgetList.end()) {
        //      if ( *i == m_pDirectoryMergeWindow  && ! dirShowBoth->isChecked() )
        //      {
        //         slotDirViewToggle();
        //      }
        (*i)->setFocus();
    }
}

void KDiff3App::slotWinToggleSplitterOrientation()
{
    if (m_pDiffWindowSplitter != 0) {
        m_pDiffWindowSplitter->setOrientation(
            m_pDiffWindowSplitter->orientation() == Qt::Vertical ? Qt::Horizontal : Qt::Vertical
        );

        //m_pOptionDialog->m_bHorizDiffWindowSplitting = m_pDiffWindowSplitter->orientation()==Qt::Horizontal;
    }
}

void KDiff3App::slotOverviewNormal()
{
    if (m_pOverview != 0) {
        m_pOverview->setOverviewMode(Overview::eOMNormal);
    }

    //   if (  !=0 )
    //      ->setOverviewMode( Overview::eOMNormal );
    slotUpdateAvailabilities();
}

void KDiff3App::slotOverviewAB()
{
    if (m_pOverview != 0) {
        m_pOverview->setOverviewMode(Overview::eOMAvsB);
    }

    //->setOverviewMode( Overview::eOMAvsB );
    slotUpdateAvailabilities();
}

void KDiff3App::slotNoRelevantChangesDetected()
{
    if (m_bTripleDiff &&  ! m_outputFilename.isEmpty()) {
        //KMessageBox::information( this, "No relevant changes detected", "KDiff3" );
        if (false) { //!m_pOptionDialog->m_IrrelevantMergeCmd.isEmpty()
            // NOTE: this branch always is not reached
            QString cmd =
                ""; //m_pOptionDialog->m_IrrelevantMergeCmd + " \"" + m_sd1.getAliasName()+ "\" \"" + m_sd2.getAliasName() + "\" \"" + m_sd3.getAliasName();
            QProcess process;
            process.start(cmd, QStringList());
            process.waitForFinished(-1);
            //::system( cmd.local8Bit() );
        }
    }
}

//static void insertManualDiffHelp( ManualDiffHelpList* pManualDiffHelpList, int winIdx, int firstLine, int lastLine )
//{
//   // The manual diff help list must be sorted and compact.
//   // "Compact" means that upper items can't be empty if lower items contain data.
//
//   // First insert the new item without regarding compactness.
//   // If the new item overlaps with previous items then the previous items will be removed.
//
//   ManualDiffHelpEntry mdhe;
//   mdhe.firstLine( winIdx ) = firstLine;
//   mdhe.lastLine( winIdx ) = lastLine;
//
//   ManualDiffHelpList::iterator i;
//   for( i=pManualDiffHelpList->begin(); i!=pManualDiffHelpList->end(); ++i )
//   {
//      int& l1 = i->firstLine( winIdx );
//      int& l2 = i->lastLine( winIdx );
//      if (l1>=0 && l2>=0)
//      {
//         if ( (firstLine<=l1 && lastLine>=l1)  ||  (firstLine <=l2 && lastLine>=l2) )
//         {
//            // overlap
//            l1 = -1;
//            l2 = -1;
//         }
//         if ( firstLine<l1 && lastLine<l1 )
//         {
//            // insert before this position
//            pManualDiffHelpList->insert( i, mdhe );
//            break;
//         }
//      }
//   }
//   if ( i == pManualDiffHelpList->end() )
//   {
//      pManualDiffHelpList->insert( i, mdhe );
//   }
//
//   // Now make the list compact
//   for( int wIdx=1; wIdx<=3; ++wIdx )
//   {
//      ManualDiffHelpList::iterator iEmpty = pManualDiffHelpList->begin();
//      for( i=pManualDiffHelpList->begin(); i!=pManualDiffHelpList->end(); ++i )
//      {
//         if ( iEmpty->firstLine(wIdx) >= 0 )
//         {
//            ++iEmpty;
//            continue;
//         }
//         if ( i->firstLine(wIdx)>=0 )  // Current item is not empty -> move it to the empty place
//         {
//            iEmpty->firstLine(wIdx) = i->firstLine(wIdx);
//            iEmpty->lastLine(wIdx) = i->lastLine(wIdx);
//            i->firstLine(wIdx) = -1;
//            i->lastLine(wIdx) = -1;
//            ++iEmpty;
//         }
//      }
//   }
//   pManualDiffHelpList->remove( ManualDiffHelpEntry() ); // Remove all completely empty items.
//}

void KDiff3App::slotAddManualDiffHelp()
{
    int firstLine = -1;
    int lastLine = -1;
    //int winIdx = -1;

    if (m_pDiffTextWindow1) {
        m_pDiffTextWindow1->getSelectionRange(&firstLine, &lastLine, eFileCoords);
        //winIdx = 1;
    }

    if (firstLine < 0 && m_pDiffTextWindow2) {
        m_pDiffTextWindow2->getSelectionRange(&firstLine, &lastLine, eFileCoords);
        //winIdx = 2;
    }
}

void KDiff3App::slotUpdateAvailabilities()
{
}
