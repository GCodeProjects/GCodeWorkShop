/***************************************************************************
 *   Copyright (C) 2006-2015 by Artur Kozioł                               *
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

#include "dialogs.h"


//**************************************************************************************************
//
//**************************************************************************************************

QString removeZeros(QString str)
{
   QRegExp exp;
   int pos;

   pos = 1;
   exp.setPattern("[\\d]{0,}[-.]{0,1}[-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$"); //[\\d]+[.][-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$"

   while((pos = str.indexOf(exp, pos)) > 0)
   {
      if((str.at(pos + exp.matchedLength() - 1) == '0'))
        str.remove(pos + exp.matchedLength() - 1, 1);
      else
        pos += exp.matchedLength();
   };

   if(str == "-0.")
      str = "0.";

   return(str);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

I2MDialog::I2MDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{

   setupUi(this);

   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Inch to metric"));

   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("Inch2mm");
   inchCheckBox->setChecked(settings.value("Inch", true).toBool());
   mmCheckBox->setChecked(!settings.value("Inch", true).toBool());
   settings.endGroup();

   inputChanged();
   checkBoxToggled();

   //setMaximumSize(width(), height());

   connect(inchInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(inchCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(mmCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(closePushButton, SIGNAL(clicked()), SLOT(close()));

   setFocusProxy(inchInput);

}

//**************************************************************************************************
//
//**************************************************************************************************

I2MDialog::~I2MDialog()
{
    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("Inch2mm");
    settings.setValue("Inch", inchCheckBox->isChecked());
    settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

void I2MDialog::inputChanged()
{
   double a, b, c;
   bool ok, dot, sl;
   int i;
   QString tx, rs, ls, ms;

   tx = inchInput->text();
   if(tx.isNull() || tx.isEmpty())
     return;
   tx.replace(',', '.');
   dot = sl = false;
   for(i = 0; i <= (tx.length() - 1); i++)
   {
      if((tx.at(i) == '.'))
      {
         if(dot || sl)
           tx.remove(i, 1);
         dot = true;
      };

      if((tx.at(i) == '/'))
      {
         if(sl)
           tx.remove(i, 1);
         sl = true;
      };

      if(!(tx.at(i).isDigit()) && !(tx.at(i) == '.') && !(tx.at(i) == '/'))
        tx.remove(i, 1);
   };

   if(tx.at(0) == '.')
     tx.insert(0, '0');
   inchInput->setText(tx);

   i = tx.indexOf('/');
   if(i > 0)
   {
      rs = tx.mid(i + 1, tx.length() - i);
      ls = tx.left(i);
      i = tx.indexOf('.');
      if(i > 0)
      {
         ms = ls.left(i);
         ls.remove(0, i + 1);
      }
      else
        ms = "0";

      a = rs.toDouble(&ok);
      b = ls.toDouble(&ok);
      c = ms.toDouble(&ok);

      c = c + (b / a);
      tx = QString("%1").arg(c, 0, 'f', 3);

   };


   a = tx.toDouble(&ok);
   if(!ok)
   {
      mmInput->setText("----");
      return;
   };

   if(inchCheckBox->isChecked())
   {
      if(a > 1000)
      {
         mmInput->setText("----");
         return;
      };
      b = a * 25.4;
      mmInput->setText(QString("%1").arg(b, 0, 'f', 4));

   }
   else
   {
      if(a > 25000)
      {
         mmInput->setText("----");
         return;
      };
      b = a / 25.4;
      mmInput->setText(QString("%1").arg(b, 0, 'f', 4));

   }

}

//**************************************************************************************************
//
//**************************************************************************************************

void I2MDialog::checkBoxToggled()
{

   if(inchCheckBox->isChecked())
   {
      inchLabel->setText(tr("Inch"));
      mmLabel->setText(tr("mm"));
   }
   else
   {
      inchLabel->setText(tr("mm"));
      mmLabel->setText(tr("Inch"));
   }

   inputChanged();

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

FeedsDialog::FeedsDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
   setupUi(this);

   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Cutting parameters"));

   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("FeedSpeedDialog");
   mmCheckBox->setChecked(settings.value("MM", true).toBool());
   inchCheckBox->setChecked(!settings.value("MM", true).toBool());
   settings.endGroup();

   QValidator *vcInputValid = new QIntValidator(1, 9999, this );
   vcInput->setValidator(vcInputValid);

   QValidator *fzInputValid = new QDoubleValidator(0.0001, 999, 4, this );
   fzInput->setValidator(fzInputValid);
   fzInput->installEventFilter(this);

   QValidator *dInputValid = new QDoubleValidator(0.01, 9000, 4, this );
   dInput->setValidator(dInputValid);
   dInput->installEventFilter(this);

   QValidator *zInputValid = new QIntValidator(1, 500, this );
   zInput->setValidator(zInputValid);

   QValidator *sInputValid = new QIntValidator(1, 99999, this );
   sInput->setValidator(sInputValid);

   QValidator *fInputValid = new QDoubleValidator(0.01, 99999, 4, this );
   fInput->setValidator(fInputValid);
   fInput->installEventFilter(this);


   connect(vcInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
   connect(fzInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
   connect(sInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
   connect(fInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
   connect(dInput, SIGNAL(editingFinished()), SLOT(setDefButton()));
   connect(zInput, SIGNAL(editingFinished()), SLOT(setDefButton()));

   connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
   connect(closeButton, SIGNAL(clicked()), SLOT(close()));
   connect(computeVcButton, SIGNAL(clicked()), SLOT(computeVcButtonClicked()));


   inputChanged();
   checkBoxChanged();

   setMaximumSize(width(), height());

   connect(vcInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(fzInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(dInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(zInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(sInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(fInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));

   connect(mmCheckBox, SIGNAL(stateChanged(int)), SLOT(checkBoxChanged()));
   connect(inchCheckBox, SIGNAL(stateChanged(int)), SLOT(checkBoxChanged()));

}

//**************************************************************************************************
//
//**************************************************************************************************

FeedsDialog::~FeedsDialog()
{
    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("FeedSpeedDialog");
    settings.setValue("MM", mmCheckBox->isChecked());
    settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

bool FeedsDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if(qobject_cast<QLineEdit *>(obj))
    {
        if(ev->type() == QEvent::KeyPress)
        {
            QKeyEvent *k = (QKeyEvent*) ev;

            if(QLocale().decimalPoint() == '.')
                if(k->key() == Qt::Key_Comma)
                {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, ".", false, 1));
                    return true;
                };

            if(QLocale().decimalPoint() == ',')
                if(k->key() == Qt::Key_Period)
                {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier, ",", false, 1));
                    return true;
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

void FeedsDialog::setDefButton()
{
   if(fInput->hasFocus() || sInput->hasFocus())
   {
     computeButton->setDefault(false);
     computeVcButton->setDefault(true);
   }
   else
     if(vcInput->hasFocus() || fzInput->hasFocus())
     {
        computeVcButton->setDefault(false);
        computeButton->setDefault(true);
     };

}

//**************************************************************************************************
//
//**************************************************************************************************

void FeedsDialog::checkBoxChanged()
{
   if(mmCheckBox->isChecked())
   {
       vcInput->setToolTip(tr("Cutting speed (m/min)"));
       fzInput->setToolTip(tr("Feed (chip load) per tooth (mm)"));
       dInput->setToolTip(tr("Tool diameter (mm)"));
       zInput->setToolTip(tr("Number of teeth on the cutter"));
       fInput->setToolTip(tr("Table feed (mm/min)"));
       sInput->setToolTip(tr("Spindle speed (revs/min)"));
       fInput->setMaxLength(9);
       fzInput->setMaxLength(8);
   }
   else
     if(inchCheckBox->isChecked())
     {
         vcInput->setToolTip(tr("Cutting speed (ft/min)"));
         fzInput->setToolTip(tr("Feed (chip load) per tooth (inches)"));
         dInput->setToolTip(tr("Tool diameter (inches)"));
         zInput->setToolTip(tr("Number of teeth on the cutter"));
         fInput->setToolTip(tr("Table feed (inches/min)"));
         sInput->setToolTip(tr("Spindle speed (revs/min)"));
         fInput->setMaxLength(10);
         fzInput->setMaxLength(9);
     };

}

//**************************************************************************************************
//
//**************************************************************************************************

void FeedsDialog::computeButtonClicked()
{
   double Vc, Fz, D, z, S, F;
   bool ok;

   Vc = QLocale().toDouble(vcInput->text(), &ok);
   Fz = QLocale().toDouble(fzInput->text(), &ok);
   D = QLocale().toDouble(dInput->text(), &ok);
   z = QLocale().toDouble(zInput->text(), &ok);

   if(mmCheckBox->isChecked())
   {
       S = (Vc * 1000) / (M_PI * D);
       F = S * (Fz * z);

       sInput->setText(QString("%1").arg(round(S)));
       fInput->setText(QString("%1").arg(F, 0, 'f', 3));
   };

   if(inchCheckBox->isChecked())
   {
       S = (Vc * 12) / (M_PI * D);
       F = S * (Fz * z);

       sInput->setText(QString("%1").arg(round(S)));
       fInput->setText(QString("%1").arg(F, 0, 'f', 4));
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void FeedsDialog::computeVcButtonClicked()
{
   double Vc, Fz, D, z, S, F;
   bool ok;

   S = sInput->text().toInt(&ok);
   F = QLocale().toDouble(fInput->text(), &ok);
   D = QLocale().toDouble(dInput->text(), &ok);
   z = QLocale().toDouble(zInput->text(), &ok);

   if(mmCheckBox->isChecked())
   {
       Vc = (M_PI * D * S) / 1000;
       Fz = (F / S) / z;

       vcInput->setText(QString("%1").arg(round(round(Vc))));
       fzInput->setText(QString("%1").arg(Fz, 0, 'f', 3));
   };

   if(inchCheckBox->isChecked())
   {
       Vc = (M_PI * D * S) / 12;
       Fz = (F / S) / z;

       vcInput->setText(QString("%1").arg(round(round(Vc))));
       fzInput->setText(QString("%1").arg(Fz, 0, 'f', 4));
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

void FeedsDialog::inputChanged()
{
   bool ena, ena1;
   QPalette palette;


   palette.setColor(vcInput->foregroundRole(), Qt::red);

   if(vcInput->hasAcceptableInput())
     vcInput->setPalette(QPalette());
   else
     vcInput->setPalette(palette);

   if(fzInput->hasAcceptableInput())
     fzInput->setPalette(QPalette());
   else
     fzInput->setPalette(palette);

   if(zInput->hasAcceptableInput())
     zInput->setPalette(QPalette());
   else
     zInput->setPalette(palette);

   if(dInput->hasAcceptableInput())
     dInput->setPalette(QPalette());
   else
     dInput->setPalette(palette);

   if(sInput->hasAcceptableInput())
     sInput->setPalette(QPalette());
   else
     sInput->setPalette(palette);

   if(fInput->hasAcceptableInput())
     fInput->setPalette(QPalette());
   else
     fInput->setPalette(palette);

   ena = vcInput->hasAcceptableInput() && fzInput->hasAcceptableInput() && zInput->hasAcceptableInput() && dInput->hasAcceptableInput();
   ena1 = sInput->hasAcceptableInput() && fInput->hasAcceptableInput() && zInput->hasAcceptableInput() && dInput->hasAcceptableInput();

   computeButton->setEnabled(ena);
   computeVcButton->setEnabled(ena1);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

RenumberDialog::RenumberDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{

   setupUi(this);

   //setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Renumber lines"));

   connect(mRenumLines, SIGNAL(clicked()), this, SLOT(renumClicked()));
   connect(mAllLines, SIGNAL(clicked()), this, SLOT(allLinesClicked()));
   connect(mRemoveAll, SIGNAL(clicked()), this, SLOT(removeAllClicked()));
   connect(mCheckDivide, SIGNAL(clicked()), this, SLOT(divideClicked()));
   connect(okButton, SIGNAL(clicked()), SLOT(okButtonClicked()));
   connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
   connect(mRenumHe, SIGNAL(clicked()), this, SLOT(mRenumHeClicked()));

   setMaximumSize(width(), height());
   //okButton->setEnabled(formInput->hasAcceptableInput());


   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("RenumberDialog" );


   startAtInput->setValue(settings.value("StartAt", 10).toInt());
   formInput->setValue(settings.value("From", 10).toInt());
   incInput->setValue(settings.value("Inc", 10).toInt());
   toInput->setValue(settings.value("To", 8000).toInt());
   mSpinBox->setValue(settings.value("Prec", 4).toInt());

   mRenumEmpty->setChecked(settings.value("RenumEmpty", false).toBool());
   mRenumWithComm->setChecked(settings.value("RenumWithComm", false).toBool());
   mRenumMarked->setChecked(settings.value("RenumMarked", true).toBool());
   mCheckDivide->setChecked(settings.value("CheckDivide", false).toBool());

   if(mCheckDivide->isChecked())
     divideClicked();

   int mode = settings.value("Mode", 1).toInt();
   switch(mode)
   {
      case 1 :  mRenumLines->setChecked(true);
                renumClicked();
                break;
      case 2 :  mAllLines->setChecked(true);
                allLinesClicked();
                break;
      case 3 :  mRemoveAll->setChecked(true);
                removeAllClicked();
                break;
      case 4 :  mRenumHe->setChecked(true);
                mRenumHeClicked();
                break;
   };

   settings.endGroup();


}

//**************************************************************************************************
//
//**************************************************************************************************

RenumberDialog::~RenumberDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::okButtonClicked()
{

   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("RenumberDialog" );

   settings.setValue("StartAt", startAtInput->value());
   settings.setValue("From", formInput->value());
   settings.setValue("Inc", incInput->value());
   settings.setValue("To", toInput->value());
   settings.setValue("Prec", mSpinBox->value());

   settings.setValue("RenumEmpty", mRenumEmpty->isChecked());
   settings.setValue("RenumWithComm", mRenumWithComm->isChecked());
   settings.setValue("RenumMarked", mRenumMarked->isChecked());
   settings.setValue("CheckDivide", mCheckDivide->isChecked());


   int mode = 1;
   if(mAllLines->isChecked())
     mode = 2;
   else
     if(mRemoveAll->isChecked())
       mode = 3;
     else
       if(mRenumHe->isChecked())
         mode = 4;

   settings.setValue("Mode", mode);

   settings.endGroup();


   accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::getState(int &mode, int &startAt, int &from, int &prec, int &inc, int &to, bool &renumEmpty, bool &renumComm, bool &renumMarked)
{
   startAt = startAtInput->value();
   from = formInput->value();
   inc = incInput->value();
   renumEmpty = mRenumEmpty->isChecked();
   renumComm = !mRenumWithComm->isChecked();
   renumMarked = mRenumMarked->isChecked();

   to = toInput->value();

   if(mCheckDivide->isChecked())
     prec = mSpinBox->value();
   else
     prec = 0;

   mode = 1;
   if(mAllLines->isChecked())
     mode = 2;
   else
     if(mRemoveAll->isChecked())
       mode = 3;
     else
       if(mRenumHe->isChecked())
         mode = 4;

}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::atEndClicked()
{
   mCheckDivide->setChecked(!mRemoveAll->isChecked());
   mSpinBox->setEnabled(mCheckDivide->isChecked());

}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::divideClicked()
{
   mSpinBox->setEnabled(mCheckDivide->isChecked());
}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::renumClicked()
{
   formInput->setEnabled(true);
   startAtInput->setEnabled(true);
   incInput->setEnabled(true);
   toInput->setEnabled(true);

   //startAtInput->setValue(10);
   //incInput->setValue(10);
   //toInput->setValue(8000);

   mRenumLines->setChecked(true);
   mAllLines->setChecked(false);
   mRemoveAll->setChecked(false);

   mRenumWithComm->setEnabled(false);
   mRenumEmpty->setEnabled(false);

   mCheckDivide->setEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::mRenumHeClicked()
{
   formInput->setEnabled(false);
   startAtInput->setEnabled(true);
   incInput->setEnabled(true);
   toInput->setEnabled(false);

   //startAtInput->setValue(0);
   //incInput->setValue(1);

   //mRenumLines->setChecked(true);
   //mAllLines->setChecked(false);
   //mRemoveAll->setChecked(false);

   mRenumWithComm->setEnabled(false);
   mRenumEmpty->setEnabled(true);

   mCheckDivide->setEnabled(false);
}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::allLinesClicked()
{
   formInput->setEnabled(false);
   startAtInput->setEnabled(true);
   incInput->setEnabled(true);
   toInput->setEnabled(false);

   //startAtInput->setValue(10);
   //incInput->setValue(10);

   //mRenumLines->setChecked(false);
   //mAllLines->setChecked(true);
   //mRemoveAll->setChecked(false);

   mRenumWithComm->setEnabled(true);
   mRenumEmpty->setEnabled(true);

   mCheckDivide->setEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

void RenumberDialog::removeAllClicked()
{
   formInput->setEnabled(false);
   startAtInput->setEnabled(false);
   incInput->setEnabled(false);
   toInput->setEnabled(false);

   //mRenumLines->setChecked(false);
  // mAllLines->setChecked(false);
   //mRemoveAll->setChecked(true);

   mRenumWithComm->setEnabled(false);
   mRenumEmpty->setEnabled(false);

   mCheckDivide->setEnabled(false);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

DotDialog::DotDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
   setupUi(this);

   setWindowTitle(tr("Insert dots"));

   connect(mInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged(const QString &)) );
   connect(mCheckAtEnd, SIGNAL(clicked()), this, SLOT(atEndClicked()));
   connect(mCheckDivide, SIGNAL(clicked()), this, SLOT(divideClicked()));
   connect(mSpinBox, SIGNAL(valueChanged(int)), this, SLOT(spinBoxVal(int)));
   connect(okButton, SIGNAL(clicked()), SLOT(okButtonClicked()) );
   connect(cancelButton, SIGNAL(clicked()), SLOT(close()) );

   setMaximumSize(width(), height());
   okButton->setEnabled(mInput->hasAcceptableInput());
}

//**************************************************************************************************
//
//**************************************************************************************************

DotDialog::~DotDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::okButtonClicked()
{
   accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::getState(QString &txt, bool &at_end, bool &div, int &x)
{
   txt = mInput->text();
   txt.remove(' ');
   at_end = mCheckAtEnd->isChecked();
   div = mCheckDivide->isChecked();
   x = mSpinBox->value();
}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::setState(const QString &txt, bool at_end, bool div, int x)
{
   mInput->setText(txt);
   mInput->selectAll();

   mCheckAtEnd->setChecked(at_end);
   mCheckDivide->setChecked(div);
   mSpinBox->setEnabled(mCheckDivide->isChecked());
   mSpinBox->setValue(x);
}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::inputChanged(const QString &text)
{
   Q_UNUSED(text);
   okButton->setEnabled(mInput->hasAcceptableInput());
}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::atEndClicked()
{
   mCheckDivide->setChecked(!mCheckAtEnd->isChecked());
   mSpinBox->setEnabled(mCheckDivide->isChecked());

}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::divideClicked()
{
   mCheckAtEnd->setChecked(!mCheckDivide->isChecked());
   mSpinBox->setEnabled(mCheckDivide->isChecked());
}

//**************************************************************************************************
//
//**************************************************************************************************

void DotDialog::spinBoxVal(int val)
{
  if((val == 99))
    mSpinBox->setValue(10);
  else
    if((val == 999))
      mSpinBox->setValue(10);
    else
      if((val == 9999))
        mSpinBox->setValue(100);

  if((val == 11))
    mSpinBox->setValue(100);
  else
    if((val == 101))
      mSpinBox->setValue(1000);
    else
      if((val == 1001))
        mSpinBox->setValue(10000);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

TriangleDialog::TriangleDialog( QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{

   setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Solution of triangles"));



   pic1 = new QPixmap(":/images/trig.png");
   pic2 = new QPixmap(":/images/trig1.png");

   picLabel->setPixmap(*pic1);
   picLabel->adjustSize();


   connect(rightTriangleCheckBox, SIGNAL(toggled(bool)), SLOT(rightTriangleCheckBoxToggled()));
   connect(aCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(bCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(cCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(aACheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(aBCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(aCCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
   connect(closeButton, SIGNAL(clicked()), SLOT(close()));


   QValidator *aInputValid = new QDoubleValidator(0.001, 9999, 3, this);
   aInput->setValidator(aInputValid);
   aInput->installEventFilter(this);

   QValidator *bInputValid = new QDoubleValidator(0.001, 9999, 3, this);
   bInput->setValidator(bInputValid);
   bInput->installEventFilter(this);

   QValidator *cInputValid = new QDoubleValidator(0.001, 9999, 3, this);
   cInput->setValidator(cInputValid);
   cInput->installEventFilter(this);

   aAInputValid = new QDoubleValidator(0.001, 179.999, 3, this);
   aAInput->setValidator(aAInputValid);
   aAInput->installEventFilter(this);

   aBInputValid = new QDoubleValidator(0.001, 179.999, 3, this);
   aBInput->setValidator(aBInputValid);
   aBInput->installEventFilter(this);

   aCInputValid = new QDoubleValidator(0.001, 179.999, 3, this);
   aCInput->setValidator(aCInputValid);
   aCInput->installEventFilter(this);


   setMaximumSize(width(), height());

   connect(aInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(bInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(cInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(aAInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(aBInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(aCInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));

   rightTriangleCheckBoxToggled();
   checkBoxToggled();
}

//**************************************************************************************************
//
//**************************************************************************************************

TriangleDialog::~TriangleDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

bool TriangleDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if(qobject_cast<QLineEdit *>(obj))
    {
        if(ev->type() == QEvent::KeyPress)
        {
            QKeyEvent *k = (QKeyEvent*) ev;

            if(QLocale().decimalPoint() == '.')
                if(k->key() == Qt::Key_Comma)
                {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, ".", false, 1));
                    return true;
                };

            if(QLocale().decimalPoint() == ',')
                if(k->key() == Qt::Key_Period)
                {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier, ",", false, 1));
                    return true;
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

void TriangleDialog::inputChanged()
{
   QPalette paletteRed, paletteBlue;

   paletteRed.setColor(aInput->foregroundRole(), Qt::red);
   paletteBlue.setColor(aInput->foregroundRole(), Qt::blue);

   if(aInput->hasAcceptableInput())
   {
      if(aInput->isReadOnly())
        aInput->setPalette(paletteBlue);
      else
        aInput->setPalette(QPalette());
   }
   else
     aInput->setPalette(paletteRed);

   if(bInput->hasAcceptableInput())
   {
      if(bInput->isReadOnly())
        bInput->setPalette(paletteBlue);
      else
        bInput->setPalette(QPalette());
   }
   else
     bInput->setPalette(paletteRed);

   if(cInput->hasAcceptableInput())
   {
      if(cInput->isReadOnly())
        cInput->setPalette(paletteBlue);
      else
        cInput->setPalette(QPalette());
   }
   else
     cInput->setPalette(paletteRed);

   if(aAInput->hasAcceptableInput())
   {
      if(aAInput->isReadOnly())
        aAInput->setPalette(paletteBlue);
      else
        aAInput->setPalette(QPalette());
   }
   else
     aAInput->setPalette(paletteRed);

   if(aBInput->hasAcceptableInput())
   {
      if(aBInput->isReadOnly())
        aBInput->setPalette(paletteBlue);
      else
        aBInput->setPalette(QPalette());
   }
   else
     aBInput->setPalette(paletteRed);

   if(aCInput->hasAcceptableInput())
   {
      if(aCInput->isReadOnly())
        aCInput->setPalette(paletteBlue);
      else
        aCInput->setPalette(QPalette());
   }
   else
     aCInput->setPalette(paletteRed);

}

//**************************************************************************************************
//
//**************************************************************************************************

void TriangleDialog::computeButtonClicked()
{
   double angle1, angle2, angle3;
   bool ok;

   angle1 = QLocale().toDouble(aAInput->text(), &ok);
   angle2 = QLocale().toDouble(aBInput->text(), &ok);
   angle3 = QLocale().toDouble(aCInput->text(), &ok);

   if(!aCheckBox->isChecked())
      aInput->setText("--");

   if(!bCheckBox->isChecked())
      bInput->setText("--");

   if(!cCheckBox->isChecked())
      cInput->setText("--");

   if(!aACheckBox->isChecked())
      aAInput->setText("--");

   if(!aBCheckBox->isChecked())
      aBInput->setText("--");

   if(!aCCheckBox->isChecked())
      aCInput->setText("--");

   if((angle1 + angle2 + angle3) > 180)
      return;

   if(option1() == 0) return;
   if(option2() == 0) return;
   if(option3() == 0) return;
   option4();

}

//**************************************************************************************************
//
//**************************************************************************************************

int TriangleDialog::option1()  //any one side and two angles known
{
   double side1, side2, side3, angle1, angle2, angle3;
   bool ok;

   switch(mode)
   {
      case 0x31 : side1 = QLocale().toDouble(aInput->text(), &ok);
                  angle1 = QLocale().toDouble(aAInput->text(), &ok);
                  angle2 = QLocale().toDouble(aBInput->text(), &ok);
                  angle3 = 180 - (angle1 + angle2);
                  break;
      case 0x51 : side1 = QLocale().toDouble(aInput->text(), &ok);
                  angle1 = QLocale().toDouble(aAInput->text(), &ok);
                  angle2 = QLocale().toDouble(aCInput->text(), &ok);
                  angle3 = 180 - (angle1 + angle2);
                  break;
      case 0x61 : side1 = QLocale().toDouble(aInput->text(), &ok);
                  angle3 = QLocale().toDouble(aBInput->text(), &ok);
                  angle2 = QLocale().toDouble(aCInput->text(), &ok);
                  angle1 = 180 - (angle2 + angle3);
                  break;


      case 0x32 : side1 = QLocale().toDouble(bInput->text(), &ok);
                  angle1 = QLocale().toDouble(aBInput->text(), &ok);
                  angle2 = QLocale().toDouble(aAInput->text(), &ok);
                  angle3 = 180 - (angle1 + angle2);
                  break;
      case 0x52 : side1 = QLocale().toDouble(bInput->text(), &ok);
                  angle3 = QLocale().toDouble(aAInput->text(), &ok);
                  angle2 = QLocale().toDouble(aCInput->text(), &ok);
                  angle1 = 180 - (angle2 + angle3);
                  break;
      case 0x62 : side1 = QLocale().toDouble(bInput->text(), &ok);
                  angle1 = QLocale().toDouble(aBInput->text(), &ok);
                  angle2 = QLocale().toDouble(aCInput->text(), &ok);
                  angle3 = 180 - (angle1 + angle2);
                  break;


      case 0x34 : side1 = QLocale().toDouble(cInput->text(), &ok);
                  angle3 = QLocale().toDouble(aAInput->text(), &ok);
                  angle2 = QLocale().toDouble(aBInput->text(), &ok);
                  angle1 = 180 - (angle2 + angle3);
                  break;
      case 0x54 : side1 = QLocale().toDouble(cInput->text(), &ok);
                  angle1 = QLocale().toDouble(aCInput->text(), &ok);
                  angle2 = QLocale().toDouble(aAInput->text(), &ok);
                  angle3 = 180 - (angle1 + angle2);
                  break;
      case 0x64 : side1 = QLocale().toDouble(cInput->text(), &ok);
                  angle1 = QLocale().toDouble(aCInput->text(), &ok);
                  angle2 = QLocale().toDouble(aBInput->text(), &ok);
                  angle3 = 180 - (angle1 + angle2);
                  break;

      default : return(-1);
   };


   side2 = (side1 * sin((M_PI/180) * angle2)) / sin((M_PI/180) * angle1);
   side3 = (side1 * sin((M_PI/180) * angle3)) / sin((M_PI/180) * angle1);

   switch(mode)
   {
      case 0x31 : bInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x51 : bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x61 : bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
                  break;

      case 0x32 : aInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x52 : aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
                  break;
      case 0x62 : aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  cInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;

      case 0x34 : bInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
                  break;
      case 0x54 : bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x64 : bInput->setText(QString("%1").arg(side2, 0, 'f', 3));
                  aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
   };


   return(0);


}

//**************************************************************************************************
//
//**************************************************************************************************

int TriangleDialog::option2()  // two sides and the included angle
{
   double side1, side2, side3, angle1, angle2, angle3;
   bool ok;


   switch(mode)
   {
      case 0x43 : side1 = QLocale().toDouble(aInput->text(), &ok);
                  side2 = QLocale().toDouble(bInput->text(), &ok);
                  angle1 = QLocale().toDouble(aCInput->text(), &ok);
                  break;
      case 0x25 : side2 = QLocale().toDouble(aInput->text(), &ok);
                  side1 = QLocale().toDouble(cInput->text(), &ok);
                  angle1 = QLocale().toDouble(aBInput->text(), &ok);
                  break;
      case 0x16 : side1 = QLocale().toDouble(bInput->text(), &ok);
                  side2 = QLocale().toDouble(cInput->text(), &ok);
                  angle1 = QLocale().toDouble(aAInput->text(), &ok);
                  break;
      default : return(-1);
   };

   angle2 = (side1 * sin((M_PI/180) * angle1)) / (side2 - side1 * cos((M_PI/180) * angle1));
   angle2 = atan(angle2) / (M_PI/180);
   side3 = (side1 * sin((M_PI/180) * angle1)) / sin((M_PI/180) * angle2);
   angle3 = 180 - (angle1 + angle2);

   switch(mode)
   {
      case 0x43 : cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x25 : bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  break;
      case 0x16 : aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
   };


   return(0);


}

//**************************************************************************************************
//
//**************************************************************************************************

int TriangleDialog::option3()  // two sides and the opposite angle
{
   double side1, side2, side3, angle1, angle2, angle3;
   bool ok;


   switch(mode)
   {
      case 0x13 : side1 = QLocale().toDouble(aInput->text(), &ok);
                  side2 = QLocale().toDouble(bInput->text(), &ok);
                  angle1 = QLocale().toDouble(aAInput->text(), &ok);
                  break;
      case 0x15 : side1 = QLocale().toDouble(aInput->text(), &ok);
                  side2 = QLocale().toDouble(cInput->text(), &ok);
                  angle1 = QLocale().toDouble(aAInput->text(), &ok);
                  break;
      case 0x46 : side1 = QLocale().toDouble(cInput->text(), &ok);
                  side2 = QLocale().toDouble(bInput->text(), &ok);
                  angle1 = QLocale().toDouble(aCInput->text(), &ok);
                  break;
      case 0x45 : side1 = QLocale().toDouble(cInput->text(), &ok);
                  side2 = QLocale().toDouble(aInput->text(), &ok);
                  angle1 = QLocale().toDouble(aCInput->text(), &ok);
                  break;
      case 0x23 : side1 = QLocale().toDouble(bInput->text(), &ok);
                  side2 = QLocale().toDouble(aInput->text(), &ok);
                  angle1 = QLocale().toDouble(aBInput->text(), &ok);
                  break;
      case 0x26 : side1 = QLocale().toDouble(bInput->text(), &ok);
                  side2 = QLocale().toDouble(cInput->text(), &ok);
                  angle1 = QLocale().toDouble(aBInput->text(), &ok);
                  break;
      default : return(-1);
   };



   angle2 = (side2 * sin((M_PI/180) * angle1)) / side1;
   angle2 = asin(angle2) / (M_PI/180);
   angle3 = 180 - (angle1 + angle2);
   side3 = (side1 * sin((M_PI/180) * angle3)) / sin((M_PI/180) * angle1);


   switch(mode)
   {
      case 0x13 : cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x15 : bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  break;
      case 0x46 : aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
      case 0x45 : bInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aBInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  break;
      case 0x23 : cInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  break;
      case 0x26 : aInput->setText(QString("%1").arg(side3, 0, 'f', 3));
                  aCInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
                  aAInput->setText(QString("%1").arg(angle3, 0, 'f', 3));
                  break;
   };


   return(0);


}

//**************************************************************************************************
//
//**************************************************************************************************

void TriangleDialog::option4()  // all sides given
{
   double side1, side2, side3, angle1, angle2, angle3;
   bool ok;

   if(mode != 0x07)
     return;

   side1 = QLocale().toDouble(aInput->text(), &ok);
   side2 = QLocale().toDouble(bInput->text(), &ok);
   side3 = QLocale().toDouble(cInput->text(), &ok);



   angle1 = ((side2 * side2) + (side3 * side3) - (side1 * side1)) / (2 * side2 * side3);
   angle1 = acos(angle1) / (M_PI/180);

   angle2 = (side2 * sin((M_PI/180) * angle1)) / side1;


   angle2 = asin(angle2) / (M_PI/180);
   angle3 = 180 - (angle1 + angle2);


   aAInput->setText(QString("%1").arg(angle1, 0, 'f', 3));
   aBInput->setText(QString("%1").arg(angle2, 0, 'f', 3));
   aCInput->setText(QString("%1").arg(angle3, 0, 'f', 3));

}

//**************************************************************************************************
//
//**************************************************************************************************

void TriangleDialog::rightTriangleCheckBoxToggled()
{

  if(rightTriangleCheckBox->isChecked())
  {
     aACheckBox->setChecked(true);
     aAInput->setText("90");
     aACheckBox->setEnabled(false);
     picLabel->setPixmap(*pic2);
  }
  else
  {
     aACheckBox->setChecked(false);
     aACheckBox->setEnabled(true);
     picLabel->setPixmap(*pic1);
  };

}

//**************************************************************************************************
//
//**************************************************************************************************

void TriangleDialog::checkBoxToggled()
{
   QPalette palette;


   mode = 0;

   if(aCheckBox->isChecked())
     mode |= 0x01;

   if(bCheckBox->isChecked())
     mode |= 0x02;

   if(cCheckBox->isChecked())
     mode |= 0x04;

   if(aACheckBox->isChecked())
     mode |= 0x10;

   if(aBCheckBox->isChecked())
     mode |= 0x20;

   if(aCCheckBox->isChecked())
     mode |= 0x40;

   palette.setColor(aInput->foregroundRole(), Qt::black);

   if((mode == 0x31) || (mode == 0x32) || (mode == 0x34) || (mode == 0x51) || (mode == 0x52) ||
      (mode == 0x54) || (mode == 0x61) || (mode == 0x62) || (mode == 0x64) || (mode == 0x07) ||
      (mode == 0x43) || (mode == 0x25) || (mode == 0x16) || (mode == 0x13) || (mode == 0x15) ||
      (mode == 0x23) || (mode == 0x26) || (mode == 0x45) || (mode == 0x46))
   {
      computeButton->setEnabled(true);
      if(!(mode & 0x01))
        aCheckBox->setEnabled(false);
      else
      {
         aInput->setReadOnly(false);
         aInput->setPalette(palette);
      };

      if(!(mode & 0x02))
        bCheckBox->setEnabled(false);
      else
      {
         bInput->setReadOnly(false);
         bInput->setPalette(palette);
      };

      if(!(mode & 0x04))
        cCheckBox->setEnabled(false);
      else
      {
         cInput->setReadOnly(false);
         cInput->setPalette(palette);
      };


      if(!(mode & 0x10))
        aACheckBox->setEnabled(false);
      else
      {
         if(!rightTriangleCheckBox->isChecked())
         {
            aAInput->setReadOnly(false);
            aAInput->setPalette(palette);
         };
      };

      if(!(mode & 0x20))
        aBCheckBox->setEnabled(false);
      else
      {
         aBInput->setReadOnly(false);
         aBInput->setPalette(palette);
      };

      if(!(mode & 0x40))
        aCCheckBox->setEnabled(false);
      else
      {
         aCInput->setReadOnly(false);
         aCInput->setPalette(palette);
      };
   }
   else
   {
      aCheckBox->setEnabled(true);
      aInput->setReadOnly(true);
      bCheckBox->setEnabled(true);
      bInput->setReadOnly(true);
      cCheckBox->setEnabled(true);
      cInput->setReadOnly(true);

      if(!rightTriangleCheckBox->isChecked())
      {
         aACheckBox->setEnabled(true);
         aAInput->setReadOnly(true);
      };
      aBCheckBox->setEnabled(true);
      aBInput->setReadOnly(true);
      aCCheckBox->setEnabled(true);
      aCInput->setReadOnly(true);

      palette.setColor(aInput->foregroundRole(), Qt::blue);
      aInput->setPalette(palette);
      bInput->setPalette(palette);
      cInput->setPalette(palette);
      aAInput->setPalette(palette);
      aBInput->setPalette(palette);
      aCInput->setPalette(palette);

      computeButton->setEnabled(false);

   };
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

BHCTab::BHCTab( QWidget * parent) : QWidget(parent)
{

   setupUi(this);

   QValidator *xCenterInputValid = new QDoubleValidator(-9999, 9999, 3, this);
   xCenterInput->setValidator(xCenterInputValid);
   xCenterInput->installEventFilter(this);
   QValidator *yCenterInputValid = new QDoubleValidator(-9999, 9999, 3, this);
   yCenterInput->setValidator(yCenterInputValid);
   yCenterInput->installEventFilter(this);
   QValidator *diaInputValid = new QDoubleValidator(1, 9999, 3, this);
   diaInput->setValidator(diaInputValid);
   diaInput->installEventFilter(this);
   QValidator *holesInputValid = new QIntValidator(1, 360, this);
   holesInput->setValidator(holesInputValid);
   QValidator *angleStartInputValid = new QDoubleValidator(0, 360, 3, this);
   angleStartInput->setValidator(angleStartInputValid);
   angleStartInput->installEventFilter(this);
   QValidator *angleBeetwenInputValid = new QDoubleValidator(0, 360, 3, this);
   angleBeetwenInput->setValidator(angleBeetwenInputValid);
   angleBeetwenInput->installEventFilter(this);

   connect(roateInput, SIGNAL(valueChanged(int)), SLOT(inputChk()));
   connect(mirrorX, SIGNAL(toggled(bool)), SLOT(inputChk()));
   connect(mirrorY, SIGNAL(toggled(bool)), SLOT(inputChk()));

   contextMenu = new QMenu(this);
   commentActGroup = new QActionGroup(this);


   QAction *copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
   copyAct->setShortcut(QKeySequence::Copy);
   copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
   connect(copyAct, SIGNAL(triggered()), this, SLOT(copySelection()));
   resultTable->addAction(copyAct);

   QAction *selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"), this);
   selAllAct->setShortcut(QKeySequence::SelectAll);
   selAllAct->setStatusTip(tr("Select all"));
   connect(selAllAct, SIGNAL(triggered()), this, SLOT(sellAll()));
   resultTable->addAction(selAllAct);

   contextMenu->addAction(copyAct);
   contextMenu->addSeparator();
   contextMenu->addAction(selAllAct);
   contextMenu->addSeparator();
   addCommentsId = contextMenu->addAction(tr("Add ; comments"));
   addCommentsId->setCheckable(true);
   addCommentsId->setActionGroup(commentActGroup);
   addCommentsParaId = contextMenu->addAction(tr("Add () comments"));
   addCommentsParaId->setCheckable(true);
   addCommentsParaId->setActionGroup(commentActGroup);

   resultTable->setContextMenuPolicy(Qt::CustomContextMenu);

   resultTable->setHorizontalHeaderLabels(QStringList()<<"X"<<"Y");


   connect(resultTable, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuReq(const QPoint &)));
   resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
;

   connect(xCenterInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
   connect(yCenterInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
   connect(diaInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
   connect(holesInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
   connect(angleStartInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));
   connect(angleBeetwenInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));

}

//**************************************************************************************************
//
//**************************************************************************************************

BHCTab::~BHCTab()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

bool BHCTab::eventFilter(QObject *obj, QEvent *ev)
{
    if(qobject_cast<QLineEdit *>(obj))
    {
        if(ev->type() == QEvent::KeyPress)
        {
            QKeyEvent *k = (QKeyEvent*) ev;

            if(QLocale().decimalPoint() == '.')
                if(k->key() == Qt::Key_Comma)
                {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, ".", false, 1));
                    return true;
                };

            if(QLocale().decimalPoint() == ',')
                if(k->key() == Qt::Key_Period)
                {
                    QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier, ",", false, 1));
                    return true;
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

void BHCTab::inputChanged()
{
   QPalette palette;

   palette.setColor(xCenterInput->foregroundRole(), Qt::red);

   if(xCenterInput->hasAcceptableInput())
     xCenterInput->setPalette(QPalette());
   else
     xCenterInput->setPalette(palette);

   if(yCenterInput->hasAcceptableInput())
     yCenterInput->setPalette(QPalette());
   else
     yCenterInput->setPalette(palette);

   if(diaInput->hasAcceptableInput())
     diaInput->setPalette(QPalette());
   else
     diaInput->setPalette(palette);

   if(holesInput->hasAcceptableInput())
     holesInput->setPalette(QPalette());
   else
     holesInput->setPalette(palette);

   if(angleStartInput->hasAcceptableInput())
     angleStartInput->setPalette(QPalette());
   else
     angleStartInput->setPalette(palette);

   if(angleBeetwenInput->hasAcceptableInput())
     angleBeetwenInput->setPalette(QPalette());
   else
     angleBeetwenInput->setPalette(palette);

}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCTab::contextMenuReq(const QPoint &pos)
{
   Q_UNUSED(pos);
   contextMenu->popup(QCursor::pos());
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCTab::sellAll()
{
   resultTable->selectAll();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCTab::copySelection()
{
    int i;
    QString selText, tmp;
    QTableWidgetItem *it;

    selText = "";

    for(i = 0; i < resultTable->rowCount(); i++)
    {

        if(resultTable->item(i, 0)->isSelected() || resultTable->item(i, 1)->isSelected())
        {
            it = resultTable->item(i, 0);
            selText += "X" + it->text();
            it = resultTable->item(i, 1);
            selText += " Y" + it->text();

            if(addCommentsParaId->isChecked())
            {
                it = resultTable->verticalHeaderItem(i);
                tmp = it->text();
                tmp.remove(")");
                tmp.replace("(", "- ");
                tmp = tmp.simplified();
                selText += " (" + tmp + ")\n";
            }
            else
                if(addCommentsId->isChecked())
                {
                    it = resultTable->verticalHeaderItem(i);
                    tmp = it->text();
                    tmp.remove(")");
                    tmp.replace("(", "- ");
                    tmp = tmp.simplified();
                    selText += " ;" + tmp + "\n";
                }
                else
                {
                    selText += "\n";
                };
        };

    };

    selText.remove(selText.length()-1, 1);
    selText = removeZeros(selText);

    if(addCommentsParaId->isChecked())
        selText.prepend(QString(tr("(DIAMETER: %1, NO. OF HOLES: %2, START ANGLE: %3)\n"))
                        .arg(diaInput->text()).arg(holesInput->text()).arg(angleStartInput->text()));

    if(addCommentsId->isChecked())
        selText.prepend(QString(tr(";DIAMETER: %1, NO. OF HOLES: %2, START ANGLE: %3\n"))
                        .arg(diaInput->text()).arg(holesInput->text()).arg(angleStartInput->text()));

    QClipboard *clipBoard = QApplication::clipboard();
    clipBoard->setText(selText, QClipboard::Clipboard);
    if(clipBoard->supportsSelection())
        clipBoard->setText(selText, QClipboard::Selection);


}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCTab::inputChk()
{
   emit commonChk();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

BHCDraw::BHCDraw(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    setWindowTitle(tr("Bolt circle - preview"));
    setAttribute(Qt::WA_DeleteOnClose);
    //sizeGripEnabled(true);

    setToolTip(tr("Click to close"));

    setBackgroundRole(QPalette::Shadow);
}

//**************************************************************************************************
//
//**************************************************************************************************

BHCDraw::~BHCDraw()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::mousePressEvent(QMouseEvent *event)
 {
   if(event->button() == Qt::LeftButton)
   {
      close();
   };
 }

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::init(int w, int h)
{
    scale = 1;

    resize(w, h);
    setMaximumSize(width(), height());
    setMinimumSize(width(), height());

    pm = new QPixmap(width(), height());
    pm->fill(Qt::black);
    drawLines();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::setScale(double sc)
{
    scale = sc;
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::paintEvent(QPaintEvent *)
{
    //bitBlt( this, 0, 0, pm );
    if(pm->isNull())
      return;

    QPainter painter(this);
    painter.drawPixmap(0, 0, *pm);
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::focusOutEvent(QFocusEvent *)
{
    //close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::drawHole(qreal ang, qreal dia, qreal xcenter, qreal ycenter, bool first, bool last, QColor color)
{
    qreal x, y, x1, y1, sca, d;

    QPainter *paint = new QPainter(pm);
    QFont font = paint->font();
    font.setPointSize(10);
    paint->setFont(font);
    QFontMetrics fm = paint->fontMetrics();

    //paint->setRenderHint(QPainter::Antialiasing);
    paint->save();
    int c = qMin(geometry().width(), geometry().height());
    c = c + (fm.lineSpacing() * 8);

    paint->setWindow(-(c / 2), -(c / 2), c, c);
    QRect v = paint->viewport();
    c = qMin(v.width(), v.height());

    paint->setViewport(v.left() + (v.width() - c) / 2, v.top() + (v.height() - c) / 2, c, c);

    sca = ((c - 20) / 2) / (scale / 2);

    paint->scale(sca, sca);

    d = 12 / sca;
    x = xcenter + (dia + (d)) * cos((M_PI/180) * ang);
    y = ycenter + (dia + (d)) * sin((M_PI/180) * ang);

    x1 = xcenter + (dia - (d)) * cos((M_PI/180) * ang);
    y1 = ycenter + (dia - (d)) * sin((M_PI/180) * ang);


    paint->setPen(QPen(Qt::gray, 0, Qt::DotLine));
    paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));


    if(last)
    {
       QBrush brush(Qt::green, Qt::SolidPattern);
       brush.setColor(color);
       paint->setBrush(brush);
    }
    else
      paint->setBrush(Qt::NoBrush);

    if(first)
    {
        // circle center point
        paint->setPen(QPen(Qt::gray, 0, Qt::DashDotLine));
        paint->drawLine(QPointF(xcenter + v.x() / 2, -(ycenter - d)), QPointF(xcenter + v.x() / 2, -(ycenter + d)));
        paint->drawLine(QPointF(xcenter - d, -(ycenter + v.y() / 2)), QPointF(xcenter + d, -(ycenter + v.y() / 2)));

        //circle radius line
        x = xcenter + 0 * cos((M_PI/180) * ang);
        y = ycenter + (0) * sin((M_PI/180) * ang);
        x1 = xcenter + (dia - (d)) * cos((M_PI/180) * ang);
        y1 = ycenter + (dia - (d)) * sin((M_PI/180) * ang);
        paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

        //diameter circle
        paint->setPen(QPen(Qt::gray, 0, Qt::DotLine));
        paint->setBrush(Qt::NoBrush);
        paint->drawEllipse(QPointF(xcenter + v.x() / 2, - ycenter -v.y() / 2), dia, dia);

        paint->setPen(QPen(color.darker(65), 0, Qt::SolidLine));
    }
    else
      paint->setPen(QPen(color, 0, Qt::SolidLine));


    x = xcenter + dia * cos((M_PI/180) * ang);
    y = ycenter + dia * sin((M_PI/180) * ang);

    d = 8 / sca;

    paint->drawEllipse(QPointF(x, -y), d, d);


//    v = paint->viewport();
//    c = qMin(v.width(), v.height());

    paint->restore();
    paint->end();

}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::drawLines(qreal dia, qreal ang, QColor cl)
{
    qreal x, y, x1, y1;

    dia = dia / 2;

    QPainter *paint = new QPainter(pm);
    QFont font = paint->font();
    font.setPointSize(10);
    paint->setFont(font);
    QFontMetrics fm = paint->fontMetrics();

    //paint->setRenderHint(QPainter::Antialiasing);
    paint->save();

    int d = qMin(geometry().width(), geometry().height());
    d = d + (fm.lineSpacing() * 6);
    paint->setWindow(-(d / 2), -(d / 2), d, d);
    QRect v = paint->viewport();
    d = qMin( v.width(), v.height() );
    paint->setViewport(v.left() + (v.width() - d) / 2, v.top() + (v.height() - d) / 2, d, d);
    //paint->scale(vp, vp);

    //paint->setRasterOp(Qt::OrROP);
    paint->setPen(QPen(cl, 0, Qt::DotLine));

    x = (dia + 4) * cos((M_PI/180) * ang);
    y = (dia + 4) * sin((M_PI/180) * ang);
    x1 = (dia + 4) * cos((M_PI/180) * (ang + 180));
    y1 = (dia + 4) * sin((M_PI/180) * (ang + 180));
    paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

    x = (dia + 4) * cos((M_PI/180) * (ang + 90));
    y = (dia + 4) * sin((M_PI/180) * (ang + 90));
    x1 = (dia + 4) * cos((M_PI/180) * (ang + 270));
    y1 = (dia + 4) * sin((M_PI/180) * (ang + 270));
    paint->drawLine(QPointF(x, -y), QPointF(x1, -y1));

    paint->restore();
    paint->end();

}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::clear()
{
   pm->fill(Qt::black);
   drawLines();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::printText(int x, int y, int line, const QString &text, QColor color)
{
   QPainter *paint = new QPainter(pm);
   QFont font = paint->font();
   font.setPointSize(10);
   paint->setFont(font);
   QFontMetrics fm = paint->fontMetrics();

   //paint->setRenderHint(QPainter::Antialiasing);
   paint->save();
   paint->setPen(QPen(color, 0, Qt::SolidLine));
   paint->drawText(x, y + (fm.lineSpacing() * line), text);
   paint->restore();
   paint->end();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDraw::drawLines()
{
    QPainter *paint = new QPainter(pm);
    QFont font = paint->font();
    font.setPointSize(10);
    paint->setFont(font);
    QFontMetrics fm = paint->fontMetrics();

    paint->save();

    int d = qMin(geometry().width(), geometry().height());
    d = d + (fm.lineSpacing() * 6);
    paint->setWindow(-(d / 2), -(d / 2), d, d);
    QRect v = paint->viewport();
    d = qMin(v.width(), v.height());
    paint->setViewport(v.left() + (v.width() - d) / 2, v.top() + (v.height() - d) / 2, d, d);
    //paint->scale(vp, vp);

    paint->setPen(QPen(Qt::gray, 0, Qt::DashDotLine));

    v = paint->viewport();
    paint->drawLine(QPointF(0, (v.height() / 2) - 5), QPointF(0, 5 - (v.height() / 2)));
    paint->drawLine(QPointF((v.width() / 2) - 5, 0), QPointF(5 - (v.width() / 2), 0));

    paint->restore();
    paint->end();

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

BHCDialog::BHCDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
   setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Bolt circle"));
   //setFocusPolicy(QWidget::StrongFocus);

   drawing = NULL;

   parentHeight = parent->height() - 10;
   parentWidth = parent->width() - 15;

   tabBar = new QTabWidget(this);
   pageLayout->addWidget(tabBar);


   BHCTab *page1 = new BHCTab(this);
   BHCTab *page2 = new BHCTab(this);
   BHCTab *page3 = new BHCTab(this);
   BHCTab *page4 = new BHCTab(this);

   tabBar->addTab(page1, tr("Circle 1 - green"));
   tabBar->addTab(page2, tr("Circle 2 - blue"));
   tabBar->addTab(page3, tr("Circle 3 - red"));
   tabBar->addTab(page4, tr("Circle 4 - yellow"));


   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("BHC");

   for(int tabId = 0; tabId < tabBar->count(); tabId++)
   {
      BHCTab *tab = (BHCTab*)tabBar->widget(tabId);
      if(tab == NULL)
        continue;

      settings.beginGroup(QString("TAB%1").arg(tabId));

      tab->xCenterInput->setText(settings.value("XCenter", "0").toString());
      tab->yCenterInput->setText(settings.value("YCenter", "0").toString());
      tab->diaInput->setText(settings.value("Dia", "0").toString());
      tab->angleStartInput->setText(settings.value("StartAngle", "0").toString());
      tab->angleBeetwenInput->setText(settings.value("AngleBeetwen", "0").toString());
      tab->holesInput->setText(settings.value("NoOfHoles", "0").toString());
      tab->roateInput->setValue(settings.value("Rotate", 0).toInt());

      tab->mirrorX->setChecked(settings.value("MirrorX", false).toBool());
      tab->mirrorY->setChecked(settings.value("MirrorY", false).toBool());
      tab->all->setChecked(settings.value("Common", true).toBool());

      settings.endGroup();
   };

   settings.endGroup();

   connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
   connect(closeButton, SIGNAL(clicked()), SLOT(closeButtonClicked()));
   connect(clearAllButton, SIGNAL(clicked()), SLOT(clearAll()));
   connect(page1, SIGNAL(commonChk()), SLOT(comChk()));
   connect(page2, SIGNAL(commonChk()), SLOT(comChk()));
   connect(page3, SIGNAL(commonChk()), SLOT(comChk()));
   connect(page4, SIGNAL(commonChk()), SLOT(comChk()));

   adjustSize();
}

//**************************************************************************************************
//
//**************************************************************************************************

BHCDialog::~BHCDialog()
{

    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("BHC");

    for(int tabId = 0; tabId < tabBar->count(); tabId++)
    {
       BHCTab *tab = (BHCTab*)tabBar->widget(tabId);
       if(tab == NULL)
         continue;

       settings.beginGroup(QString("TAB%1").arg(tabId));

       settings.setValue("XCenter", tab->xCenterInput->text());
       settings.setValue("YCenter", tab->yCenterInput->text());
       settings.setValue("Dia", tab->diaInput->text());
       settings.setValue("StartAngle", tab->angleStartInput->text());
       settings.setValue("AngleBeetwen", tab->angleBeetwenInput->text());
       settings.setValue("NoOfHoles", tab->holesInput->text());
       settings.setValue("Rotate", tab->roateInput->value());

       settings.setValue("MirrorX", tab->mirrorX->isChecked());
       settings.setValue("MirrorY", tab->mirrorY->isChecked());
       settings.setValue("Common", tab->all->isChecked());

       settings.endGroup();
    };

    settings.endGroup();

}

//**************************************************************************************************
//
//**************************************************************************************************

//void BHCDialog::windowActivationChange(bool oldActive)
//{
//    Q_UNUSED(oldActive);
//    drawing = (BHCDraw *) findChild<BHCDraw *>();
//    if(drawing > 0)
//      if(!this->isActiveWindow())
//         drawing->close();
//}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDialog::comChk()
{
   BHCTab *tab;
   int tabId, roat, activTab;
   bool mirX, mirY;

   tab = (BHCTab*)tabBar->currentWidget();
   activTab = tabBar->currentIndex();

   if(tab->all->isChecked())
   {
      roat = tab->roateInput->value();
      mirX = tab->mirrorX->isChecked();
      mirY = tab->mirrorY->isChecked();

      for(tabId = 0; tabId < tabBar->count(); tabId++)
      {
         tab = (BHCTab*)tabBar->widget(tabId);
         if(tab == NULL)
           continue;

         if(tabId == activTab)
           continue;

         if(tab->all->isChecked())
         {
            tab->roateInput->setValue(roat);
            tab->mirrorX->setChecked(mirX);
            tab->mirrorY->setChecked(mirY);
         };
      };
   };

   computeButtonClicked();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDialog::clearAll()
{
    for(int tabId = 0; tabId < tabBar->count(); tabId++)
    {
       BHCTab *tab = (BHCTab*)tabBar->widget(tabId);
       if(tab == NULL)
         continue;

       tab->xCenterInput->setText("0");
       tab->yCenterInput->setText("0");
       tab->diaInput->setText("0");
       tab->angleStartInput->setText("0");
       tab->angleBeetwenInput->setText("0");
       tab->holesInput->setText("0");
       tab->roateInput->setValue(0);

       tab->resultTable->setRowCount(0);
       tab->resultTable->clearContents();

    };

    if(drawing > NULL)
        drawing->close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDialog::closeButtonClicked()
{
   close();

}

//**************************************************************************************************
//
//**************************************************************************************************

void BHCDialog::computeButtonClicked()
{
   BHCTab *tab;
   QColor col;
   int tabId, i, textPosY, textPosX, dir;
   bool ok;
   int holeCount;
   qreal dia, firstAngle, angleBeetwen, roate, x, y, ang, xCenter, yCenter;
   double maxDia;


   maxDia = 0;
   for(tabId = 0; tabId <= tabBar->count(); tabId++)
   {
      tab = (BHCTab*)tabBar->widget(tabId);
      if(tab == NULL)
        continue;

      maxDia = qMax(maxDia, (QLocale().toDouble(tab->diaInput->text(), &ok)+ 2*abs(QLocale().toDouble(tab->xCenterInput->text(), &ok))));
//      maxDia = qMax(maxDia, (QLocale().toDouble(tab->diaInput->text(), &ok)- 2*(QLocale().toDouble(tab->xCenterInput->text(), &ok))));
      maxDia = qMax(maxDia, (QLocale().toDouble(tab->diaInput->text(), &ok)+ 2*abs(QLocale().toDouble(tab->yCenterInput->text(), &ok))));
//      maxDia = qMax(maxDia, (QLocale().toDouble(tab->diaInput->text(), &ok)- 2*(QLocale().toDouble(tab->yCenterInput->text(), &ok))));
   };


   for(tabId = 0; tabId < tabBar->count(); tabId++)
   {
      tab = (BHCTab*)tabBar->widget(tabId);
      if(tab == NULL)
        continue;

      if(!tab->xCenterInput->hasAcceptableInput())
      {
         tab->xCenterInput->setText("0");
         xCenter = 0;
      }
      else
        xCenter = QLocale().toDouble(tab->xCenterInput->text(), &ok);

      if(!tab->yCenterInput->hasAcceptableInput())
      {
         tab->yCenterInput->setText("0");
         yCenter = 0;
      }
      else
        yCenter = QLocale().toDouble(tab->yCenterInput->text(), &ok);

      if(!tab->diaInput->hasAcceptableInput())
      {
         tab->diaInput->setText("0");
         tab->resultTable->setRowCount(0);
         continue;
      }
      else
        dia = QLocale().toDouble(tab->diaInput->text(), &ok);

      if(tab->angleStartInput->hasAcceptableInput())
        firstAngle = QLocale().toDouble(tab->angleStartInput->text(), &ok);
      else
      {
         tab->angleStartInput->setText("0");
         firstAngle = 0;
      };

      if(tab->angleBeetwenInput->hasAcceptableInput())
        angleBeetwen = QLocale().toDouble(tab->angleBeetwenInput->text(), &ok);
      else
      {
         tab->angleBeetwenInput->setText("0");
         angleBeetwen = 0;
      };

      if(tab->holesInput->hasAcceptableInput())
        holeCount = tab->holesInput->text().toInt(&ok);
      else
      {
         tab->holesInput->setText("0");
         holeCount = 0;
      };


      roate = tab->roateInput->text().toInt(&ok);

      if((holeCount == 0) && (angleBeetwen == 0))
      {
         tab->resultTable->setRowCount(0);
         continue;
      };

      if(dia == 0)
          continue;

      roate = roate / 10;

      if(holeCount <= 0)
        holeCount = 360/angleBeetwen;

      if(angleBeetwen <= 0)
        angleBeetwen = double(360/double(holeCount));


      firstAngle += roate;

      tab->resultTable->setRowCount(holeCount);

      dia = dia / 2;

      if(tab->mirrorY->isChecked())
         xCenter = -xCenter;

      if(tab->mirrorX->isChecked())
         yCenter = -yCenter;

      for(i = 0; i < holeCount; i++)
      {
         ang = firstAngle + (angleBeetwen * i);

         if(tab->mirrorY->isChecked())
            ang = 180 - ang;

         if(tab->mirrorX->isChecked())
            ang = 360 - ang;

         dir = ang / 360;

         if(ang >= 360)
         {
            ang = ang - (dir * 360);
         };

         if(ang < 0)
         {
            ang = ang + 360;
         };

         x = xCenter + (dia * cos((M_PI/180) * ang));
         y = yCenter + (dia * sin((M_PI/180) * ang));

         QTableWidgetItem *xItem = new QTableWidgetItem(removeZeros(QString("%1").arg(x, 0, 'f', 3)));
         //xItem->setFlags(Qt::ItemIsEnabled);
         xItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

         QTableWidgetItem *yItem = new QTableWidgetItem(removeZeros(QString("%1").arg(y, 0, 'f', 3)));
         yItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
         //yItem->setFlags(Qt::ItemIsEnabled);


         QTableWidgetItem *hdr = new QTableWidgetItem(removeZeros(QString("%1 - %2 ").arg(i+1).arg(ang, 0, 'f', 3)));
         tab->resultTable->setVerticalHeaderItem(i, hdr);
         tab->resultTable->setItem(i, 0, xItem);
         tab->resultTable->setItem(i, 1, yItem);
      };
   };



   drawing = (BHCDraw *) findChild<BHCDraw *>();
   if(!drawing)
   {
      drawing = new BHCDraw(this);
      drawing->init(parentWidth - this->width(), parentHeight - this->y());
      drawing->move((this->x()) - drawing->width(), this->y());
   };

   drawing->setScale(maxDia);

   drawing->show();
   drawing->setUpdatesEnabled(false);
   drawing->clear();



   for(tabId = 0; tabId <= tabBar->count(); tabId++)
   {
      tab = (BHCTab*)tabBar->widget(tabId);
      if(tab == 0)
        continue;

      if(!tab->xCenterInput->hasAcceptableInput())
      {
         tab->xCenterInput->setText("0");
         xCenter = 0;
      }
      else
        xCenter = QLocale().toDouble(tab->xCenterInput->text(), &ok);

      if(!tab->yCenterInput->hasAcceptableInput())
      {
         tab->yCenterInput->setText("0");
         yCenter = 0;
      }
      else
        yCenter = QLocale().toDouble(tab->yCenterInput->text(), &ok);

      if(!tab->diaInput->hasAcceptableInput())
      {
         tab->diaInput->setText("0");
         tab->resultTable->setRowCount(0);
         continue;
      }
      else
        dia = QLocale().toDouble(tab->diaInput->text(), &ok);

      if(tab->angleStartInput->hasAcceptableInput())
        firstAngle = QLocale().toDouble(tab->angleStartInput->text(), &ok);
      else
      {
         tab->angleStartInput->setText("0");
         firstAngle = 0;
      };

      if(tab->angleBeetwenInput->hasAcceptableInput())
        angleBeetwen = QLocale().toDouble(tab->angleBeetwenInput->text(), &ok);
      else
      {
         tab->angleBeetwenInput->setText("0");
         angleBeetwen = 0;
      };

      if(tab->holesInput->hasAcceptableInput())
        holeCount = tab->holesInput->text().toInt(&ok);
      else
      {
         tab->holesInput->setText("0");
         holeCount = 0;
      };


      roate = tab->roateInput->text().toInt(&ok);

      if((holeCount == 0) && (angleBeetwen == 0))
      {
         tab->resultTable->setRowCount(0);
         continue;
      }

      roate = roate / 10;

      if(holeCount <= 0)
        holeCount = 360/angleBeetwen;

      if(angleBeetwen <= 0)
        angleBeetwen = double(360/double(holeCount));


      switch(tabId)
      {
         case 1:  col = Qt::blue;
                  textPosX = 8;
                  textPosY = drawing->height();
                  dir = 5;
                  break;
         case 2:  col = Qt::red;
                  textPosX = (drawing->width()/2) + drawing->width()/4;
                  textPosY = 0;
                  dir = -1;
                  break;
         case 3:  col = Qt::yellow;
                  textPosX = (drawing->width()/2) + drawing->width()/4;
                  textPosY = drawing->height();
                  dir = 5;
                  break;
         default: col = Qt::green;
                  textPosX = 8;
                  textPosY = 0;
                  dir = -1;

      };

      drawing->printText(textPosX, textPosY, 0-dir, QString(tr("Diameter : %1")).arg(dia), col);
      drawing->printText(textPosX, textPosY, 1-dir, QString(tr("Number of holes : %1")).arg(holeCount), col);
      drawing->printText(textPosX, textPosY, 2-dir, QString(tr("Angle of first hole : %1")).arg(firstAngle), col);
      drawing->printText(textPosX, textPosY, 3-dir, QString(tr("Angle beetwen holes : %1")).arg(angleBeetwen), col);
      drawing->printText(textPosX, textPosY, 4-dir, QString(tr("Center position : X%1 Y%2")).arg(xCenter).arg(yCenter), col);
      firstAngle += roate;

      tab->resultTable->setRowCount(holeCount);

      dia = dia / 2;

      if(tab->mirrorY->isChecked())
         xCenter = -xCenter;

      if(tab->mirrorX->isChecked())
         yCenter = -yCenter;

      for(i = 0; i < holeCount; i++)
      {
         ang = firstAngle + (angleBeetwen * i);

         if(tab->mirrorY->isChecked())
            ang = 180 - ang;

         if(tab->mirrorX->isChecked())
            ang = 360 - ang;

         dir = ang / 360;

         if(ang >= 360)
         {
            ang = ang - (dir * 360);
         };

         if(ang < 0)
         {
            ang = ang + 360;
         };

         drawing->drawHole(ang, dia, xCenter, yCenter, (i == 0), (i == (holeCount-1)), col);

      };

   };
   drawing->setUpdatesEnabled(true);
   drawing->update();

   //adjustSize();
   //setFocus();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

ChamferDialog::ChamferDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{

   setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose);
   setWindowTitle(tr("Chamfer"));


   connect(angCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   QValidator *angInputValid = new QDoubleValidator( 0.1, 90, 3, this);
   angInput->setValidator(angInputValid);
   connect(zlCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   QValidator *zlInputValid = new QDoubleValidator( 0.1, 1000, 3, this);
   zlInput->setValidator(zlInputValid);
   connect(dlCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   QValidator *dlInputValid = new QDoubleValidator( 0.1, 1000, 3, this);
   dlInput->setValidator(dlInputValid);
   connect(xoCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   QValidator *xoInputValid = new QDoubleValidator( 0, 2000, 3, this);
   xoInput->setValidator(xoInputValid);
   connect(xdCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
   QValidator *xdInputValid = new QDoubleValidator( 0.001, 2000, 3, this);
   xdInput->setValidator(xdInputValid);


   connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
   connect(closeButton, SIGNAL(clicked()), SLOT(close()));


   //inputChanged();

   //setMaximumSize(width(), height());

   connect(angInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(zlInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(dlInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(xdInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
   connect(xoInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));

   checkBoxToggled();
}

//**************************************************************************************************
//
//**************************************************************************************************

ChamferDialog::~ChamferDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void ChamferDialog::computeButtonClicked()
{
   double ZL, XL, X1, X2, ang;
   bool ok;


   if(angCheckBox->isChecked() && zlCheckBox->isChecked())
   {
      if(!zlInput->hasAcceptableInput() || !angInput->hasAcceptableInput())
        return;

      ang = QLocale().toDouble(angInput->text(), &ok);
      ZL = QLocale().toDouble(zlInput->text(), &ok);

      XL = tan((M_PI/180) * ang) * (ZL * 2);
      dlInput->setText(QString("%1").arg(XL, 0, 'f', 3));

      if(xoCheckBox->isChecked() && xoInput->hasAcceptableInput())
      {
         X1 = QLocale().toDouble(xoInput->text(), &ok);
         X2 = X1 + XL;
         xdInput->setText(QString("%1").arg(X2, 0, 'f', 3));

      };

      if(xdCheckBox->isChecked() && xdInput->hasAcceptableInput())
      {
         X2 = QLocale().toDouble(xdInput->text(), &ok);
         X1 = X2 - XL;
         xoInput->setText(QString("%1").arg(X1, 0, 'f', 3));

      };
      return;
   };


   if(angCheckBox->isChecked() && dlCheckBox->isChecked())
   {
      if(!dlInput->hasAcceptableInput() || !angInput->hasAcceptableInput())
        return;

      ang = QLocale().toDouble(angInput->text(), &ok);
      XL = QLocale().toDouble(dlInput->text(), &ok);

      ZL = (XL / 2) / tan((M_PI/180) * ang) ;
      zlInput->setText(QString("%1").arg(ZL, 0, 'f', 3));


      return;
   };

   if(angCheckBox->isChecked() && xoCheckBox->isChecked() && xdCheckBox->isChecked())
   {
      if(!xdInput->hasAcceptableInput() || !xoInput->hasAcceptableInput() || !angInput->hasAcceptableInput())
        return;

      ang = QLocale().toDouble(angInput->text(), &ok);
      X1 = QLocale().toDouble(xoInput->text(), &ok);
      X2 = QLocale().toDouble(xdInput->text(), &ok);
      XL = X2 - X1;
      dlInput->setText(QString("%1").arg(XL, 0, 'f', 3));
      ZL = (XL / 2) / tan((M_PI/180) * ang) ;
      zlInput->setText(QString("%1").arg(ZL, 0, 'f', 3));


      return;
   };

   if(zlCheckBox->isChecked() && dlCheckBox->isChecked())
   {
      if(!zlInput->hasAcceptableInput() || !dlInput->hasAcceptableInput())
        return;

      XL = QLocale().toDouble(dlInput->text(), &ok);
      ZL = QLocale().toDouble(zlInput->text(), &ok);
      ang = (atan((XL / 2) / ZL)) / (M_PI/180);

      angInput->setText(QString("%1").arg(ang, 0, 'f', 3));

      return;
   };


}

//**************************************************************************************************
//
//**************************************************************************************************

void ChamferDialog::checkBoxToggled()
{
   bool ena;


   ena = (angCheckBox->isChecked() && zlCheckBox->isChecked());
   dlCheckBox->setEnabled(!ena && (!xdCheckBox->isChecked() || !xoCheckBox->isChecked()));

   ena = (angCheckBox->isChecked() && dlCheckBox->isChecked());
   zlCheckBox->setEnabled(!ena && (!xdCheckBox->isChecked() || !xoCheckBox->isChecked()));

   //zlCheckBox->setEnabled(!xdCheckBox->isChecked() || !xoCheckBox->isChecked());
   //dlCheckBox->setEnabled(!xdCheckBox->isChecked() || !xoCheckBox->isChecked());

   xoCheckBox->setEnabled((!xdCheckBox->isChecked()) || !zlCheckBox->isChecked());
   xdCheckBox->setEnabled((!xoCheckBox->isChecked()) || !zlCheckBox->isChecked());

   if(dlCheckBox->isChecked())
   {
      xoCheckBox->setEnabled(false);
      xdCheckBox->setEnabled(false);
   };

   angCheckBox->setEnabled(!zlCheckBox->isChecked() || !dlCheckBox->isChecked());


   if(!angCheckBox->isEnabled())
   {
     angCheckBox->setChecked(false);
     angInput->setText("0");
   };
   if(!zlCheckBox->isEnabled())
   {
     zlCheckBox->setChecked(false);
     zlInput->setText("0");
   };
   if(!dlCheckBox->isEnabled())
   {
     dlCheckBox->setChecked(false);
     dlInput->setText("0");
   };
   if(!xdCheckBox->isEnabled())
   {
     xdCheckBox->setChecked(false);
     xdInput->setText("0");
   };
   if(!xoCheckBox->isEnabled())
   {
     xoCheckBox->setChecked(false);
     xoInput->setText("0");
   };

   angInput->setReadOnly(!angCheckBox->isChecked());
   zlInput->setReadOnly(!zlCheckBox->isChecked());
   dlInput->setReadOnly(!dlCheckBox->isChecked());
   xdInput->setReadOnly(!xdCheckBox->isChecked());
   xoInput->setReadOnly(!xoCheckBox->isChecked());

   computeButton->setEnabled(false);

   inputChanged();

}

//**************************************************************************************************
//
//**************************************************************************************************

void ChamferDialog::inputChanged()
{
   QPalette palette;
   palette.setColor(angInput->foregroundRole(), Qt::red);

   if(angInput->hasAcceptableInput())
     angInput->setPalette(QPalette());
   else
     angInput->setPalette(palette);

   if(zlInput->hasAcceptableInput())
     zlInput->setPalette(QPalette());
   else
     zlInput->setPalette(palette);

   if(dlInput->hasAcceptableInput())
     dlInput->setPalette(QPalette());
   else
     dlInput->setPalette(palette);

   if(xdInput->hasAcceptableInput())
     xdInput->setPalette(QPalette());
   else
     xdInput->setPalette(palette);

   if(xoInput->hasAcceptableInput())
     xoInput->setPalette(QPalette());
   else
     xoInput->setPalette(palette);


   if(angCheckBox->isChecked() && zlCheckBox->isChecked())
     if(zlInput->hasAcceptableInput() && angInput->hasAcceptableInput())
     {
        computeButton->setEnabled(true);
        return;
     };


   if(angCheckBox->isChecked() && dlCheckBox->isChecked())
     if(dlInput->hasAcceptableInput() && angInput->hasAcceptableInput())
     {
        computeButton->setEnabled(true);
        return;
     };

   if(angCheckBox->isChecked() && xoCheckBox->isChecked() && xdCheckBox->isChecked())
     if(xdInput->hasAcceptableInput() && xoInput->hasAcceptableInput() && angInput->hasAcceptableInput())
     {
        computeButton->setEnabled(true);
        return;
     };

   if(zlCheckBox->isChecked() && dlCheckBox->isChecked())
     if(zlInput->hasAcceptableInput() && dlInput->hasAcceptableInput())
     {
        computeButton->setEnabled(true);
        return;
     };

   computeButton->setEnabled(false);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

I2MProgDialog::I2MProgDialog(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{

   setupUi(this);
   setWindowTitle(tr("Convert program inch to metric"));


   connect(mInput, SIGNAL(textChanged(const QString&)), this, SLOT(inputChanged()));


   connect(okButton, SIGNAL(clicked()), SLOT(accept()));
   connect(closeButton, SIGNAL(clicked()), SLOT(close()));


   //setMaximumSize(width(), height());

   setFocusProxy(mInput);
   inputChanged();

}

//**************************************************************************************************
//
//**************************************************************************************************

I2MProgDialog::~I2MProgDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void I2MProgDialog::inputChanged()
{
   okButton->setEnabled(mInput->hasAcceptableInput());
}

//**************************************************************************************************
//
//**************************************************************************************************

void I2MProgDialog::okButtonClicked()
{
   accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void I2MProgDialog::getState(QString &txt, int &x, bool &in)
{
   txt = mInput->text();
   txt.remove(' ');
   x = precInput->value();
   in = inchCheckBox->isChecked();
}

//**************************************************************************************************
//
//**************************************************************************************************

void I2MProgDialog::setState(const QString &txt, int x, bool in)
{
   mInput->setText(txt);
   mInput->selectAll();
   inchCheckBox->setChecked(in);
   mmCheckBox->setChecked(!in);
   precInput->setValue(x);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=========================================================================================

=========================================================================================
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

SetupDialog::SetupDialog( QWidget* parent, const _editor_properites* prop, Qt::WindowFlags f) : QDialog(parent, f)
{

   QPalette palette;


   setupUi(this);
   setWindowTitle(tr("EdytorNC configuration"));

   editProp = *prop;

   readOnlyModeCheckBox->setChecked(editProp.defaultReadOnly);

   fontLabel->setText(QString(tr("Current font : <b>\"%1\", %2 pt.<\b>")
                      .arg(editProp.fontName).arg(editProp.fontSize)));
   fontLabel->setFont(QFont(editProp.fontName, editProp.fontSize));

   connect(changeFontButton, SIGNAL(clicked()), SLOT(changeFont()));
   connect(browseButton, SIGNAL(clicked()), SLOT(browseButtonClicked()));

   colorButtons = new QButtonGroup(this);
   connect(colorButtons, SIGNAL(buttonClicked(QAbstractButton *)), SLOT(changeColor(QAbstractButton *)));


   palette.setColor(backgroundColorButton->foregroundRole(), prop->hColors.backgroundColor);
   backgroundColorButton->setPalette(palette);
   backgroundColorButton->setAutoFillBackground(true);
   colorButtons->addButton(backgroundColorButton);


   QColor color = prop->hColors.backgroundColor;

   palette.setColor(QPalette::Button, color);
   palette.setColor(QPalette::Light, color);
   palette.setColor(QPalette::Dark, color);
   palette.setColor(QPalette::Shadow, color);
   palette.setColor(QPalette::Midlight, color);
   palette.setColor(QPalette::Base, color);


   palette.setColor(fontColorButton->foregroundRole(), prop->hColors.defaultColor);
   fontColorButton->setPalette(palette);
   fontColorButton->setAutoFillBackground(true);
   fontColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(fontColorButton);

   palette.setColor(commentColorButton->foregroundRole(), prop->hColors.commentColor);
   commentColorButton->setPalette(palette);
   commentColorButton->setAutoFillBackground(true);
   commentColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(commentColorButton);

   palette.setColor(gColorButton->foregroundRole(), prop->hColors.gColor);
   gColorButton->setPalette(palette);
   gColorButton->setAutoFillBackground(true);
   gColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(gColorButton);

   palette.setColor(mColorButton->foregroundRole(), prop->hColors.mColor);
   mColorButton->setPalette(palette);
   mColorButton->setAutoFillBackground(true);
   mColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(mColorButton);

   palette.setColor(nColorButton->foregroundRole(), prop->hColors.nColor);
   nColorButton->setPalette(palette);
   nColorButton->setAutoFillBackground(true);
   nColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(nColorButton);

   palette.setColor(lColorButton->foregroundRole(), prop->hColors.lColor);
   lColorButton->setPalette(palette);
   lColorButton->setAutoFillBackground(true);
   lColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(lColorButton);

   palette.setColor(fsColorButton->foregroundRole(), prop->hColors.fsColor);
   fsColorButton->setPalette(palette);
   fsColorButton->setAutoFillBackground(true);
   fsColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(fsColorButton);

   palette.setColor(dhtColorButton->foregroundRole(), prop->hColors.dhtColor);
   dhtColorButton->setPalette(palette);
   dhtColorButton->setAutoFillBackground(true);
   dhtColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(dhtColorButton);

   palette.setColor(rColorButton->foregroundRole(), prop->hColors.rColor);
   rColorButton->setPalette(palette);
   rColorButton->setAutoFillBackground(true);
   rColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(rColorButton);

   palette.setColor(macroColorButton->foregroundRole(), prop->hColors.macroColor);
   macroColorButton->setPalette(palette);
   macroColorButton->setAutoFillBackground(true);
   macroColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(macroColorButton);

   palette.setColor(keyWordColorButton->foregroundRole(), prop->hColors.keyWordColor);
   keyWordColorButton->setPalette(palette);
   keyWordColorButton->setAutoFillBackground(true);
   keyWordColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(keyWordColorButton);

   palette.setColor(progNameColorButton->foregroundRole(), prop->hColors.progNameColor);
   progNameColorButton->setPalette(palette);
   progNameColorButton->setAutoFillBackground(true);
   progNameColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(progNameColorButton);

   palette.setColor(operatorColorButton->foregroundRole(), prop->hColors.operatorColor);
   operatorColorButton->setPalette(palette);
   operatorColorButton->setAutoFillBackground(true);
   operatorColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(operatorColorButton);

   palette.setColor(zColorButton->foregroundRole(), prop->hColors.zColor);
   zColorButton->setPalette(palette);
   zColorButton->setAutoFillBackground(true);
   zColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(zColorButton);

   palette.setColor(aColorButton->foregroundRole(), prop->hColors.aColor);
   aColorButton->setPalette(palette);
   aColorButton->setAutoFillBackground(true);
   aColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(aColorButton);

   palette.setColor(bColorButton->foregroundRole(), prop->hColors.bColor);
   bColorButton->setPalette(palette);
   bColorButton->setAutoFillBackground(true);
   bColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(bColorButton);

   palette.setColor(underlineColorButton->foregroundRole(), prop->underlineColor);
   underlineColorButton->setPalette(palette);
   underlineColorButton->setAutoFillBackground(true);
   underlineColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(underlineColorButton);

   palette.setColor(curLineColorButton->foregroundRole(), prop->lineColor);
   curLineColorButton->setPalette(palette);
   curLineColorButton->setAutoFillBackground(true);
   curLineColorButton->setBackgroundRole(QPalette::Base);
   colorButtons->addButton(curLineColorButton);

   capsLockCheckBox->setChecked(editProp.intCapsLock);
   syntaxHCheckBox->setChecked(editProp.syntaxH);
   underlineCheckBox->setChecked(editProp.underlineChanges);


   tabbedModecheckBox->setChecked(editProp.windowMode & TABBED_MODE);
   fileNameCheckBox->setChecked(editProp.windowMode & SHOW_FILENAME);
   filePathCheckBox->setChecked(editProp.windowMode & SHOW_FILEPATH);
   titleCheckBox->setChecked(editProp.windowMode & SHOW_PROGTITLE);

   calcLineEdit->setText(editProp.calcBinary);
   clearUndocheckBox->setChecked(editProp.clearUndoHistory);
   clearUnderlinecheckBox->setChecked(editProp.clearUnderlineHistory);
   editorToolTipsCheckBox->setChecked(editProp.editorToolTips);
   startEmptyCheckBox->setChecked(editProp.startEmpty);

   QStringListIterator extIterator(editProp.extensions);
   while (extIterator.hasNext())
       lstExtensions->addItem(extIterator.next().toLocal8Bit().constData());

   edtSaveExtension->setText(editProp.saveExtension);
   edtSaveDirectory->setText(editProp.saveDirectory);

   if(editProp.guessFileNameByProgNum)
       progNumCheckBox->setChecked(true);
   else
       firstCommCheckBox->setChecked(true);

   highlightModeComboBox->addItem(tr("AUTO"), MODE_AUTO);
   highlightModeComboBox->addItem(tr("FANUC"), MODE_FANUC);
   highlightModeComboBox->addItem(tr("HEIDENHAIN DIALOG"), MODE_HEIDENHAIN);
   highlightModeComboBox->addItem(tr("HEIDENHAIN ISO"), MODE_HEIDENHAIN_ISO);
   highlightModeComboBox->addItem(tr("OKUMA OSP"), MODE_OKUMA);
   highlightModeComboBox->addItem(tr("PHILIPS"), MODE_PHILIPS);
   highlightModeComboBox->addItem(tr("SINUMERIK OLD"), MODE_SINUMERIK);
   highlightModeComboBox->addItem(tr("SINUMERIK NEW"), MODE_SINUMERIK_840);
   highlightModeComboBox->addItem(tr("LinuxCNC"), MODE_LINUXCNC);
   highlightModeComboBox->addItem(tr("TOOLTIPS"), MODE_TOOLTIPS);

   int id = highlightModeComboBox->findData(editProp.defaultHighlightMode);
   highlightModeComboBox->setCurrentIndex(id);

   QRegExp rx("(\\*\\.)[A-Z0-9]{1,3}");
   rx.setCaseSensitivity(Qt::CaseInsensitive);
   QValidator *edtExtensionValid = new QRegExpValidator(rx, this);
   edtExtension->setValidator(edtExtensionValid);

   connect(defaultButton, SIGNAL(clicked()), SLOT(setDefaultProp()));
   connect(okButton, SIGNAL(clicked()), SLOT(accept()));
   connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
}

//**************************************************************************************************
//
//**************************************************************************************************

SetupDialog::~SetupDialog()
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont( editProp.fontName, editProp.fontSize ), this);
    if(ok)
    {
       editProp.fontName = font.family();
       editProp.fontSize = font.pointSize();
       fontLabel->setText(QString(tr("Current font : <b>\"%1\", %2 pt.<\b>")
                          .arg(editProp.fontName).arg(editProp.fontSize)));
       fontLabel->setFont(QFont(editProp.fontName, editProp.fontSize));
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::browseButtonClicked()
{

#ifdef Q_OS_WIN32
   QString filter = tr("Executables (*.exe)");
#else
   QString filter = tr("All files (*)");
#endif

   QString fileName = QFileDialog::getOpenFileName(
                         this,
                         tr("Select calculator executable"),
                         calcLineEdit->text(),
                         filter);

   QFileInfo file(fileName);

   if((file.exists()) && (file.isReadable()))
   {
       calcLineEdit->setText(QDir::toNativeSeparators(file.canonicalFilePath()));
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

_editor_properites SetupDialog::getSettings()
{
   QPalette palette;
   int r, g, b;
   bool ok;

   r = 0;
   if(tabbedModecheckBox->isChecked())
     r |= TABBED_MODE;
   if(fileNameCheckBox->isChecked())
     r |= SHOW_FILENAME;
   if(filePathCheckBox->isChecked())
     r |= SHOW_FILEPATH;
   if(titleCheckBox->isChecked())
     r |= SHOW_PROGTITLE;

   editProp.windowMode = r;
   editProp.intCapsLock = capsLockCheckBox->isChecked();
   editProp.syntaxH = syntaxHCheckBox->isChecked();
   editProp.underlineChanges = underlineCheckBox->isChecked();
   editProp.calcBinary = calcLineEdit->text();
   editProp.clearUndoHistory = clearUndocheckBox->isChecked();
   editProp.clearUnderlineHistory = clearUnderlinecheckBox->isChecked();
   editProp.editorToolTips = editorToolTipsCheckBox->isChecked();
   editProp.defaultReadOnly = readOnlyModeCheckBox->isChecked();
   editProp.startEmpty = startEmptyCheckBox->isChecked();

   int id = highlightModeComboBox->currentIndex();
   if(id >= 0)
   {
      editProp.defaultHighlightMode = highlightModeComboBox->itemData(id).toInt(&ok);
   };

   palette = backgroundColorButton->palette();
   palette.color(backgroundColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.backgroundColor = (r << 16) + (g << 8) + b;

   palette = fontColorButton->palette();
   palette.color(fontColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.defaultColor = (r << 16) + (g << 8) + b;

   palette = commentColorButton->palette();
   palette.color(commentColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.commentColor = (r << 16) + (g << 8) + b;

   palette = gColorButton->palette();
   palette.color(gColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.gColor = (r << 16) + (g << 8) + b;

   palette = mColorButton->palette();
   palette.color(mColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.mColor = (r << 16) + (g << 8) + b;

   palette = nColorButton->palette();
   palette.color(nColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.nColor = (r << 16) + (g << 8) + b;

   palette = lColorButton->palette();
   palette.color(lColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.lColor = (r << 16) + (g << 8) + b;

   palette = fsColorButton->palette();
   palette.color(fsColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.fsColor = (r << 16) + (g << 8) + b;

   palette = dhtColorButton->palette();
   palette.color(dhtColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.dhtColor = (r << 16) + (g << 8) + b;

   palette = rColorButton->palette();
   palette.color(rColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.rColor = (r << 16) + (g << 8) + b;

   palette = macroColorButton->palette();
   palette.color(macroColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.macroColor = (r << 16) + (g << 8) + b;

   palette = keyWordColorButton->palette();
   palette.color(keyWordColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.keyWordColor = (r << 16) + (g << 8) + b;

   palette = progNameColorButton->palette();
   palette.color(progNameColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.progNameColor = (r << 16) + (g << 8) + b;

   palette = operatorColorButton->palette();
   palette.color(operatorColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.operatorColor = (r << 16) + (g << 8) + b;

   palette = zColorButton->palette();
   palette.color(zColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.zColor = (r << 16) + (g << 8) + b;

   palette = aColorButton->palette();
   palette.color(aColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.aColor = (r << 16) + (g << 8) + b;

   palette = bColorButton->palette();
   palette.color(bColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.hColors.bColor = (r << 16) + (g << 8) + b;

   palette = underlineColorButton->palette();
   palette.color(underlineColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.underlineColor = (r << 16) + (g << 8) + b;

   palette = curLineColorButton->palette();
   palette.color(curLineColorButton->foregroundRole()).getRgb(&r, &g, &b);
   editProp.lineColor = (r << 16) + (g << 8) + b;

   editProp.extensions.clear();
   for(int row = 0; row < lstExtensions->count(); row++) {
       QListWidgetItem *item = lstExtensions->item(row);
       editProp.extensions.append(item->text());
   }

   editProp.saveExtension = edtSaveExtension->text();
   editProp.saveDirectory = edtSaveDirectory->text();

   editProp.guessFileNameByProgNum = progNumCheckBox->isChecked();

   return(editProp);
}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::changeColor(QAbstractButton *button)
{
   QPalette palette;

   palette = button->palette();

   QColor color = QColorDialog::getColor(palette.color(button->foregroundRole()), this);
   if(color.isValid())
   {
      palette.setColor(button->foregroundRole(), color);
      button->setPalette(palette);
   };


   palette = backgroundColorButton->palette();
   color = palette.color(backgroundColorButton->foregroundRole());

   palette = fontColorButton->palette();
   palette.setColor(fontColorButton->backgroundRole(), color);
   fontColorButton->setPalette(palette);

   palette = commentColorButton->palette();
   palette.setColor(commentColorButton->backgroundRole(), color);
   commentColorButton->setPalette(palette);

   palette = commentColorButton->palette();
   palette.setColor(commentColorButton->backgroundRole(), color);
   commentColorButton->setPalette(palette);

   palette = gColorButton->palette();
   palette.setColor(gColorButton->backgroundRole(), color);
   gColorButton->setPalette(palette);

   palette = mColorButton->palette();
   palette.setColor(mColorButton->backgroundRole(), color);
   mColorButton->setPalette(palette);

   palette = nColorButton->palette();
   palette.setColor(nColorButton->backgroundRole(), color);
   nColorButton->setPalette(palette);

   palette = lColorButton->palette();
   palette.setColor(lColorButton->backgroundRole(), color);
   lColorButton->setPalette(palette);

   palette = fsColorButton->palette();
   palette.setColor(fsColorButton->backgroundRole(), color);
   fsColorButton->setPalette(palette);

   palette = dhtColorButton->palette();
   palette.setColor(dhtColorButton->backgroundRole(), color);
   dhtColorButton->setPalette(palette);

   palette = rColorButton->palette();
   palette.setColor(rColorButton->backgroundRole(), color);
   rColorButton->setPalette(palette);

   palette = macroColorButton->palette();
   palette.setColor(macroColorButton->backgroundRole(), color);
   macroColorButton->setPalette(palette);

   palette = keyWordColorButton->palette();
   palette.setColor(keyWordColorButton->backgroundRole(), color);
   keyWordColorButton->setPalette(palette);

   palette = progNameColorButton->palette();
   palette.setColor(progNameColorButton->backgroundRole(), color);
   progNameColorButton->setPalette(palette);

   palette = operatorColorButton->palette();
   palette.setColor(operatorColorButton->backgroundRole(), color);
   operatorColorButton->setPalette(palette);

   palette = zColorButton->palette();
   palette.setColor(zColorButton->backgroundRole(), color);
   zColorButton->setPalette(palette);

   palette = aColorButton->palette();
   palette.setColor(aColorButton->backgroundRole(), color);
   aColorButton->setPalette(palette);

   palette = bColorButton->palette();
   palette.setColor(bColorButton->backgroundRole(), color);
   bColorButton->setPalette(palette);

   palette = underlineColorButton->palette();
   palette.setColor(underlineColorButton->backgroundRole(), color);
   underlineColorButton->setPalette(palette);

   palette = curLineColorButton->palette();
   palette.setColor(curLineColorButton->backgroundRole(), color);
   curLineColorButton->setPalette(palette);

}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::setDefaultProp()
{
   QPalette palette;

   palette.setColor(commentColorButton->foregroundRole(), 0xde0020);
   commentColorButton->setPalette(palette);

   palette.setColor(gColorButton->foregroundRole(), 0x1600ee);
   gColorButton->setPalette(palette);

   palette.setColor(mColorButton->foregroundRole(), 0x80007d);
   mColorButton->setPalette(palette);

   palette.setColor(nColorButton->foregroundRole(), Qt::darkGray);
   nColorButton->setPalette(palette);

   palette.setColor(lColorButton->foregroundRole(), 0x535b5f);
   lColorButton->setPalette(palette);

   palette.setColor(fsColorButton->foregroundRole(), 0x516600);
   fsColorButton->setPalette(palette);

   palette.setColor(dhtColorButton->foregroundRole(), 0x660033);
   dhtColorButton->setPalette(palette);

   palette.setColor(rColorButton->foregroundRole(), 0x24576f);
   rColorButton->setPalette(palette);

   palette.setColor(macroColorButton->foregroundRole(), 0x000080);
   macroColorButton->setPalette(palette);

   palette.setColor(keyWordColorButton->foregroundRole(), 0x1d8000);
   keyWordColorButton->setPalette(palette);

   palette.setColor(progNameColorButton->foregroundRole(), Qt::black);
   progNameColorButton->setPalette(palette);

   palette.setColor(operatorColorButton->foregroundRole(), 0x9a2200);
   operatorColorButton->setPalette(palette);

   palette.setColor(zColorButton->foregroundRole(), 0x000080);
   zColorButton->setPalette(palette);

   palette.setColor(aColorButton->foregroundRole(), Qt::black);
   aColorButton->setPalette(palette);

   palette.setColor(bColorButton->foregroundRole(), Qt::black);
   bColorButton->setPalette(palette);

   palette.setColor(underlineColorButton->foregroundRole(), Qt::green);
   underlineColorButton->setPalette(palette);

   palette.setColor(curLineColorButton->foregroundRole(), 0xFEFFB6);
   curLineColorButton->setPalette(palette);

   palette.setColor(fontColorButton->foregroundRole(), Qt::black);
   fontColorButton->setPalette(palette);

   palette.setColor(backgroundColorButton->foregroundRole(), 0xFFFFFF);
   backgroundColorButton->setPalette(palette);


   syntaxHCheckBox->setChecked(true);
   capsLockCheckBox->setChecked(true);
   underlineCheckBox->setChecked(true);
   tabbedModecheckBox->setChecked(false);
   fileNameCheckBox->setChecked(true);
   filePathCheckBox->setChecked(false);
   titleCheckBox->setChecked(false);
   editorToolTipsCheckBox->setChecked(true);
   readOnlyModeCheckBox->setChecked(false);
   editProp.fontName = "Courier";
   editProp.fontSize = 12;

   editProp.defaultHighlightMode = MODE_AUTO;
   int id = highlightModeComboBox->findData(editProp.defaultHighlightMode);
   highlightModeComboBox->setCurrentIndex(id);

   clearUndocheckBox->setChecked(false);
   clearUnderlinecheckBox->setChecked(true);
   startEmptyCheckBox->setChecked(false);

#ifdef Q_OS_LINUX
   editProp.calcBinary = "kcalc";
#endif

#ifdef Q_OS_WIN32
   editProp.calcBinary = "calc.exe";
#endif

   calcLineEdit->setText(editProp.calcBinary);

   fontLabel->setText(QString(tr("Current font : <b>\"%1\", %2 pt.<\b>")
                      .arg(editProp.fontName).arg(editProp.fontSize)));
   fontLabel->setFont(QFont(editProp.fontName, editProp.fontSize));

   lstExtensions->clear();
   lstExtensions->addItem("*.nc");
   lstExtensions->addItem("*.cnc");
   //lstExtensions->addItem("*.anc");

   progNumCheckBox->setChecked(true);

   edtSaveExtension->setText("*.nc");
}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::on_btnAddExtension_clicked()
{
    QString ext = edtExtension->text();
    ext.remove(" ");
    ext.simplified();
    if((ext == "") || (ext == "*."))
        return;
    lstExtensions->addItem(ext);
    edtExtension->setText("*.");
}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::on_btnDeleteExtension_clicked()
{
    qDeleteAll(lstExtensions->selectedItems());
}

//**************************************************************************************************
//
//**************************************************************************************************

void SetupDialog::on_btnBrowseDirectory_clicked()
{
    QString dir = edtSaveDirectory->text();
    if(dir.isEmpty())
        dir = QDir::homePath();

    dir = QFileDialog::getExistingDirectory(
                this,
                tr("Select default save directory"),
                dir);
    if(!dir.isEmpty())
        edtSaveDirectory->setText(dir);
}

//**************************************************************************************************
//
//**************************************************************************************************
