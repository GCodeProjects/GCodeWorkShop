/***************************************************************************
 *   Copyright (C) 2009 by Artur Kozioł                                    *
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

 /***************************************************************************
                           customfdialog.cpp  -  description
                              -------------------
     begin                : Fri Nov 30 2001
     copyright            : (C) 2001 by Franz Schmid
     email                : Franz.Schmid@altmuehlnet.de
  ***************************************************************************/
 
 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/
 
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextStream>

#include "customfiledialog.h"

 
extern QString DocDir;
 
ImIconProvider::ImIconProvider() : QFileIconProvider()
 {
    txtpm = QIcon(":/images/txtfile.png");
    cncpm = QIcon(":/images/ncfile.png");
    ncpm = QIcon(":/images/ncfile.png");
    ancpm = QIcon(":/images/ncfile.png");
    minpm = QIcon(":/images/ncfile.png");
}
 
//**************************************************************************************************
//
//**************************************************************************************************

QIcon ImIconProvider::icon(const QFileInfo &fi) const
{
    QString ext = fi.suffix().toLower();
    if (ext.isEmpty())
        return QFileIconProvider::icon(fi);


        ext = fi.completeSuffix().toLower();
        if (ext.endsWith("nc", Qt::CaseInsensitive))
            return ncpm;
        else if (ext.endsWith("cnc", Qt::CaseInsensitive))
            return cncpm;
        else if (ext.endsWith("min", Qt::CaseInsensitive))
            return minpm;
        else if (ext.endsWith("txt", Qt::CaseInsensitive))
            return txtpm;
        else if (ext.endsWith("anc", Qt::CaseInsensitive))
            return ancpm;
        else
            return QFileIconProvider::icon(fi);

    return QIcon();
}
 
//**************************************************************************************************
//
//**************************************************************************************************

FDialogPreview::FDialogPreview(QWidget *pa) : QTextEdit(pa)
{

    setWordWrapMode(QTextOption::NoWrap);
    setAcceptRichText(FALSE);
    setReadOnly(TRUE);
    highlighter = 0;
    setFont(QFont("Courier", 10, QFont::Normal));
    setFixedSize(QSize( 300, 200 ));

}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void FDialogPreview::setHColors(const _h_colors colors)
{
   if(highlighter <= 0)
     highlighter = new Highlighter(document(), colors);
   if(highlighter > 0)
     highlighter->rehighlight();
}

//**************************************************************************************************
//
//**************************************************************************************************

 void FDialogPreview::updtPix()
 {
     QPixmap pm;
     QRect inside = contentsRect();
     pm = QPixmap(inside.width(), inside.height());
     pm.fill(Qt::white);
     //setPixmap(pm);
 }

//**************************************************************************************************
//
//**************************************************************************************************
 
 void FDialogPreview::GenPreview(QString name)
 {

    QFileInfo fi = QFileInfo(name);
    if (fi.isDir())
         return;

     QFile file(name);
     if(file.open(QFile::ReadOnly | QFile::Text)) 
     {
        QTextStream in(&file);
        setPlainText(in.readAll());
     };

 }

//**************************************************************************************************
//
//**************************************************************************************************

CustomFDialog::CustomFDialog(QWidget *parent, int flags) : QDialog(parent, Qt::Dialog), optionFlags(flags)
{
     
     setModal(true);
     setWindowFlags(Qt::Dialog);
     //setAttribute(Qt::WA_DeleteOnClose);

     vboxLayout = new QVBoxLayout(this);
     vboxLayout->setSpacing(5);
     vboxLayout->setMargin(10);
     hboxLayout = new QHBoxLayout;
     hboxLayout->setSpacing(5);
     hboxLayout->setMargin(0);
     fileDialog = new ScFileWidget(this);
     hboxLayout->addWidget(fileDialog);
     fileDialog->setIconProvider(new ImIconProvider());


     vboxLayout1 = new QVBoxLayout;
     vboxLayout1->setSpacing(0);
     vboxLayout1->setMargin(0);
     vboxLayout1->setContentsMargins(0, 37, 0, 0);
     vboxLayout1->setAlignment( Qt::AlignTop );
     pw = new FDialogPreview( this );
     pw->setMinimumSize(QSize(300, fileDialog->height()));
     pw->setMaximumSize(QSize(300, fileDialog->height()));
     vboxLayout1->addWidget(pw);
     hboxLayout->addLayout(vboxLayout1);
     vboxLayout->addLayout(hboxLayout);
     QHBoxLayout *hboxLayout1 = new QHBoxLayout;
     hboxLayout1->setSpacing(5);
     hboxLayout1->setContentsMargins(9, 0, 0, 0);
     showPreview = new QCheckBox(this);
     showPreview->setText( tr("Show Preview"));
     showPreview->setChecked(true);
     previewIsShown = true;
     hboxLayout1->addWidget(showPreview);
     QSpacerItem *spacerItem = new QSpacerItem(2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum);
     hboxLayout1->addItem(spacerItem);
     if (flags & fdExistingFiles)
     {
        OKButton = new QPushButton( tr("&Open"), this);
        setWindowTitle(tr("Open file..."));
        fileDialog->setFileMode(QFileDialog::ExistingFiles);
     }  
     else
     {
        fileDialog->setFileMode(QFileDialog::AnyFile);
        OKButton = new QPushButton( tr("&Save"), this);
        setWindowTitle(tr("Save file as..."));
        fileDialog->setConfirmOverwrite(TRUE);
     };

     OKButton->setDefault( true );
     hboxLayout1->addWidget( OKButton );
     CancelB = new QPushButton( tr("&Cancel"), this);
     CancelB->setAutoDefault( false );
     hboxLayout1->addWidget( CancelB );
     vboxLayout->addLayout(hboxLayout1);

     if (flags & fdDirectoriesOnly)
     {
         Layout = new QFrame(this);
         Layout1 = new QHBoxLayout(Layout);
         Layout1->setSpacing( 0 );
         Layout1->setContentsMargins(9, 0, 0, 0);
         SaveZip = new QCheckBox( tr( "&Compress File" ), Layout);
         Layout1->addWidget(SaveZip, Qt::AlignLeft);
         QSpacerItem* spacer = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
         Layout1->addItem( spacer );
         vboxLayout->addWidget(Layout);
         LayoutC = new QFrame(this);
         Layout1C = new QHBoxLayout(LayoutC);
         Layout1C->setSpacing( 0 );
         Layout1C->setContentsMargins(9, 0, 0, 0);
         WithFonts = new QCheckBox( tr( "&Include Fonts" ), LayoutC);
         Layout1C->addWidget(WithFonts, Qt::AlignLeft);
         WithProfiles = new QCheckBox( tr( "&Include Color Profiles" ), LayoutC);
         Layout1C->addWidget(WithProfiles, Qt::AlignLeft);
         QSpacerItem* spacer2 = new QSpacerItem( 2, 2, QSizePolicy::Expanding, QSizePolicy::Minimum );
         Layout1C->addItem( spacer2 );
         vboxLayout->addWidget(LayoutC);
         fileDialog->setFileMode(QFileDialog::DirectoryOnly);
         pw->hide();
         showPreview->setVisible(false);
         showPreview->setChecked(false);
         previewIsShown = false;
     }
     else
     {
        bool setter = flags & fdShowPreview;
        showPreview->setChecked(setter);
        previewIsShown = setter;
        pw->setVisible(setter);
     }


     connect(OKButton, SIGNAL(clicked()), this, SLOT(accept()));
     connect(CancelB, SIGNAL(clicked()), this, SLOT(reject()));
     connect(showPreview, SIGNAL(clicked()), this, SLOT(togglePreview()));
     connect(fileDialog, SIGNAL(currentChanged(const QString &)), this, SLOT(fileClicked(const QString &)));
     connect(fileDialog, SIGNAL(filesSelected(const QStringList &)), this, SLOT(accept()));
     connect(fileDialog, SIGNAL(accepted()), this, SLOT(accept()));
     connect(fileDialog, SIGNAL(rejected()), this, SLOT(reject()));
     resize(minimumSizeHint());
     
     setMinimumSize(parent->width()*0.85, height());
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void CustomFDialog::resizeEvent(QResizeEvent *event)
{
   ;
   pw->setMinimumSize(QSize(300, fileDialog->height() - 50));
   pw->setMaximumSize(QSize(300, fileDialog->height() - 50));

   //QWidget::resizeEvent(event);
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void CustomFDialog::setNameFilters(const QStringList filters)
{
   fileDialog->setNameFilters(filters);
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
bool CustomFDialog::restoreState(const QByteArray &state)
{
   return fileDialog->restoreState(state);
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void CustomFDialog::selectNameFilter(const QString &filter)
{
   fileDialog->selectNameFilter(filter);
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
QByteArray CustomFDialog::saveState() const
{
   return fileDialog->saveState();
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
QString CustomFDialog::selectedNameFilter() const
{
   return fileDialog->selectedNameFilter();
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
QStringList CustomFDialog::selectedFiles() const
{
   return fileDialog->selectedFiles();
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void CustomFDialog::setHighlightColors(const _h_colors colors)
{
   pw->setHColors(colors);
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void CustomFDialog::fileClicked(const QString &path)
{
   if(previewIsShown)
     pw->GenPreview(path);
   fileName = path;
}
 
//**************************************************************************************************
//
//**************************************************************************************************

void CustomFDialog::togglePreview()
{
   previewIsShown = !previewIsShown;     
   pw->setVisible(previewIsShown);
}
 
//**************************************************************************************************
//
//**************************************************************************************************

void CustomFDialog::setSelection(QString sel)
{
   fileDialog->selectFile(QFileInfo(sel).fileName());
   if(previewIsShown)
      pw->GenPreview(sel);
}

//**************************************************************************************************
//
//**************************************************************************************************

void CustomFDialog::selectFile(const QString &filename)
{
   fileDialog->selectFile(filename);
} 

//**************************************************************************************************
//
//**************************************************************************************************

void CustomFDialog::setConfirmOverwrite(bool enabled)
{
   fileDialog->setConfirmOverwrite(enabled);
} 

//**************************************************************************************************
//
//**************************************************************************************************

QString CustomFDialog::selectedFile()
{
   QString fn; 

   QStringList sel = fileDialog->selectedFiles();

   if(!sel.isEmpty())
     fn = sel[0];
   else
     fn = fileName;

   if(!fn.isEmpty())
   {
      int pos ;
      QString ext = selectedNameFilter();
      if(!ext.contains("*.*", Qt::CaseInsensitive))
      {
         pos = ext.indexOf('*', 0, Qt::CaseInsensitive);
         ext.remove(0, pos + 1);
         ext.remove((ext.length() - 1), 1);
      }
      else
        ext = ".nc";

      pos = fn.indexOf('.', 0, Qt::CaseInsensitive);
      if(pos > 0)
        fn.replace(pos, (fn.length() - pos), ext);
      else
         fn = fn + ext;
   };

   return fn;
}
 
//**************************************************************************************************
//
//**************************************************************************************************

 void CustomFDialog::addWidgets(QWidget *widgets)
 {
     vboxLayout->addWidget(widgets);
 }
 
//**************************************************************************************************
//
//**************************************************************************************************

 CustomFDialog::~CustomFDialog()
 {
 }
 
//**************************************************************************************************
//
//**************************************************************************************************

 void CustomFDialog::setExtension(QString e)
 {
     ext = e;
 }
 
//**************************************************************************************************
//
//**************************************************************************************************

 QString CustomFDialog::extension()
 {
     return ext;
 }

//**************************************************************************************************
//
//**************************************************************************************************

ScFileWidget::ScFileWidget(QWidget * parent) : QFileDialog(parent, Qt::Widget)
{
   setSizeGripEnabled(false);
   setModal(false);
   setViewMode(QFileDialog::List);
   setWindowFlags(Qt::Widget);
   QList<QPushButton *> b = findChildren<QPushButton *>();
   QListIterator<QPushButton *> i(b);
   while (i.hasNext())
     i.next()->setVisible(false);
   setMinimumSize(QSize(480, 310));
   setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

//**************************************************************************************************
//
//**************************************************************************************************

QString ScFileWidget::selectedFile()
{
   QString fn;   

   QStringList l(selectedFiles());
   if(l.count() == 0)
     return QString();

   fn = l.at(0);
   if(!fn.isEmpty())
   {
      int pos ;
      QString ext = selectedNameFilter();
      if(!ext.contains("*.*", Qt::CaseInsensitive))
      {
         pos = ext.indexOf('*', 0, Qt::CaseInsensitive);
         ext.remove(0, pos + 1);
         ext.remove((ext.length() - 1), 1);
      }
      else
        ext = ".nc";

      pos = fn.indexOf('.', 0, Qt::CaseInsensitive);
      if(pos > 0)
        fn.replace(pos, (fn.length() - pos), ext);
      else
         fn = fn + ext;
   };

   return fn;
}

//**************************************************************************************************
//
//**************************************************************************************************

