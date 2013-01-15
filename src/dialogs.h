/***************************************************************************
 *   Copyright (C) 2006-2013 by Artur Kozioł                               *
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

#ifndef DIALOGS_H
#define DIALOGS_H


#include <QtGui>
#include <QTextDocument>

#include "commoninc.h"

#include "ui_i2mdialog.h"
#include "ui_feedsdialog.h"
#include "ui_renumberdialog.h"
#include "ui_dotdialog.h"
#include "ui_triangledialog.h"
#include "ui_bhctabform.h"
#include "ui_bhcdialog.h"
#include "ui_chamferdialog.h"
#include "ui_i2mprogdialog.h"
#include "ui_setupdialog.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


QString removeZeros(QString str);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class I2MDialog : public QDialog, private Ui::I2MDialog

{
  Q_OBJECT

  public:
    I2MDialog(QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~I2MDialog();


  public:


  protected slots:
    void inputChanged();
    void checkBoxToggled();


  private slots:

  private:

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class FeedsDialog : public QDialog, private Ui::FeedsDialog
{
  Q_OBJECT

  public:
    FeedsDialog( QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~FeedsDialog();


  public:


  protected slots:
    void inputChanged();

  private slots:
    void computeButtonClicked();
    void computeVcButtonClicked();
    void setDefButton();

  private:

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class RenumberDialog : public QDialog, private Ui::RenumberDialog
{
  Q_OBJECT

  public:
    RenumberDialog(QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~RenumberDialog();

  public slots:
    void getState(int &mode, int &startAt, int &from, int &prec, int &inc, int &to, bool &renumEmpty, bool &renumComm, bool &renumMarked);


  protected:

  signals :

  private slots:
    void okButtonClicked();
    void atEndClicked();
    void divideClicked();
    void renumClicked();
    void allLinesClicked();
    void removeAllClicked();
    void mRenumHeClicked();

  private:

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class DotDialog : public QDialog, private Ui::DotDialog
{
  Q_OBJECT

  public:
    DotDialog(QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~DotDialog();

  public slots:
    void getState(QString &text, bool &at_end, bool &div, int &x);
    void setState(const QString &txt, bool at_end, bool div, int x);



  protected:

  signals :


  private slots:
    void inputChanged(const QString &text);
    void okButtonClicked();
    void atEndClicked();
    void divideClicked();
    void spinBoxVal(int val);

  private:

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class TriangleDialog : public QDialog, private Ui::TriangleDialog
{
  Q_OBJECT

  public:
    TriangleDialog( QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~TriangleDialog();


  public:


  protected slots:
    void inputChanged();
    void checkBoxToggled();
    void rightTriangleCheckBoxToggled();


  private slots:
    void computeButtonClicked();
    int option1();
    int option2();
    int option3();
    void option4();

  private:
    uint mode;
    QPixmap *pic1;
    QPixmap *pic2;
    QDoubleValidator *aAInputValid;
    QDoubleValidator *aBInputValid;
    QDoubleValidator *aCInputValid;

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class BHCTab : public QWidget, public Ui::BHCTab
{
  Q_OBJECT

  public:
    BHCTab( QWidget * parent = 0);

    ~BHCTab();


  public:


  protected slots:
    void contextMenuReq(const QPoint & pos);
    void inputChk();

  signals:
    void commonChk();

  private slots:
    void copySelection();
    void sellAll();
    void inputChanged();

  private:
    QMenu *contextMenu;
    QAbstractItemModel *model;
    QAction *addCommentsId;


};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class BHCDraw : public QWidget
{
  Q_OBJECT

  public:
    BHCDraw(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::Tool);

    ~BHCDraw();


  public slots:
    void clear();
    void printText(int x, int y, int line, const QString &text, QColor color);
    void drawHole(qreal ang, qreal dia, bool first = FALSE, bool last = FALSE, QColor color = Qt::green);
    void init(int w, int h);
    void drawLines(qreal dia, qreal ang, QColor cl);
    void setScale(double sc);


  private slots:
    void drawLines();

  protected:
    void paintEvent(QPaintEvent *);
    void focusOutEvent(QFocusEvent *);
    void mousePressEvent(QMouseEvent *event);

  private:
    double scale;
    QPixmap *pm;

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class BHCDialog : public QDialog, private Ui::BHCDialog
{
  Q_OBJECT

  public:
    BHCDialog( QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~BHCDialog();


  public slots:

  protected:
    //void windowActivationChange(bool oldActive);

  private slots:
    void comChk();
    void computeButtonClicked();
    void closeButtonClicked();

  private:
    QTabWidget *tabBar;
    BHCDraw *drawing;

    int parentHeight;
    int parentWidth;
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class ChamferDialog : public QDialog, private Ui::ChamferDialog
{
  Q_OBJECT

  public:
    ChamferDialog(QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~ChamferDialog();


  public:


  protected slots:
    void inputChanged();
    void checkBoxToggled();


  private slots:
    void computeButtonClicked();

  private:

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class I2MProgDialog : public QDialog, private Ui::I2MProgDialog
{
  Q_OBJECT

  public:
    I2MProgDialog(QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog);

    ~I2MProgDialog();


  public slots:
    void getState(QString &txt, int &x, bool &in);
    void setState(const QString &txt, int x, bool in);

  protected slots:

  private slots:
    void inputChanged();
    void okButtonClicked();

  private:

};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class SetupDialog : public QDialog, private Ui::SetupDialog
{
  Q_OBJECT

  public:
    SetupDialog( QWidget* parent = 0, const _editor_properites* prop = 0, Qt::WindowFlags f = Qt::Dialog);

    ~SetupDialog();


  public slots:
    _editor_properites getSettings();

  protected slots:


  private slots:
    void on_btnBrowseDirectory_clicked();
    void on_btnDeleteExtension_clicked();
    void on_btnAddExtension_clicked();
    void changeFont();
    void changeColor(QAbstractButton *button);
    void setDefaultProp();
    void browseButtonClicked();


  private:
    _editor_properites editProp;
    QButtonGroup *colorButtons;



};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/













#endif


