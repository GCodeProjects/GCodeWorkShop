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
/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "findinf.h"


#define MAXLISTS        10


FindInFiles::FindInFiles(QWidget *parent): QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    browseButton = createButton(tr("&Browse..."), SLOT(browse()));
    browseButton->setDefault(FALSE);
    findButton = createButton(tr("&Find"), SLOT(find()));
    findButton->setDefault(TRUE);
    closeButton = createButton(tr("&Close"), SLOT(close()));
    closeButton->setDefault(FALSE);

    fileComboBox = createComboBox();
    fileComboBox->setDuplicatesEnabled(FALSE);
    textComboBox = createComboBox();
    textComboBox->setDuplicatesEnabled(FALSE);
    directoryComboBox = createComboBox();
    directoryComboBox->setDuplicatesEnabled(FALSE);

    fileLabel = new QLabel(tr("Named:"));
    textLabel = new QLabel(tr("Containing text:"));
    directoryLabel = new QLabel(tr("In directory:"));
    filesFoundLabel = new QLabel;

    createFilesTable();
    
    preview = new QTextEdit;
    preview->setReadOnly(TRUE);
    preview->setWordWrapMode(QTextOption::NoWrap);
    preview->setFont(QFont("Courier", 10, QFont::Normal));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(findButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(closeButton);

    QHBoxLayout *viewLayout = new QHBoxLayout;
    viewLayout->addWidget(filesTable);
    viewLayout->addWidget(preview);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(textLabel, 0, 0);
    mainLayout->addWidget(textComboBox, 0, 1, 1, 1);
    mainLayout->addWidget(fileLabel, 1, 0);
    mainLayout->addWidget(fileComboBox, 1, 1, 1, 1);
    mainLayout->addWidget(directoryLabel, 2, 0);
    mainLayout->addWidget(directoryComboBox, 2, 1);
    mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addLayout(viewLayout, 3, 0, 1, 3);
    //mainLayout->addWidget(filesTable, 3, 0, 1, 2);
    //mainLayout->addWidget(preview, 3, 2, 1, 3); 
    mainLayout->addWidget(filesFoundLabel, 4, 0);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("Find Files"));
    resize(parent->width() * 0.7, parent->height() * 0.5);

    readSettings();
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), directoryComboBox->currentText());
    if (!directory.isEmpty()) 
    {
       directoryComboBox->addItem(directory);
       directoryComboBox->setCurrentIndex(directoryComboBox->count()-1);
    }
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::find()
{
    filesTable->setRowCount(0);

    QString fileName = fileComboBox->currentText();
    QString text = textComboBox->currentText();
    QString path = directoryComboBox->currentText();

    QDir directory = QDir(path);
    QStringList files;
    if(fileName.isEmpty())
        fileName = "*";
    files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    if(!text.isEmpty())
      files = findFiles(directory, files, text);
    showFiles(directory, files);
}

//**************************************************************************************************
//
//**************************************************************************************************

QStringList FindInFiles::findFiles(const QDir &directory, const QStringList &files,
                              const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));


    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();


        if (progressDialog.wasCanceled())
            break;

        QFile file(directory.absoluteFilePath(files[i]));

        if (file.open(QIODevice::ReadOnly))
        {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if(line.contains(text, Qt::CaseInsensitive))
                {
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
    return foundFiles;
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(Qt::ItemIsEnabled);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()));
}

//**************************************************************************************************
//
//**************************************************************************************************

QPushButton *FindInFiles::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

//**************************************************************************************************
//
//**************************************************************************************************

QComboBox *FindInFiles::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::createFilesTable()
{
    filesTable = new QTableWidget(0, 2);
    QStringList labels;
    labels << tr("File Name") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
    connect(filesTable, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(filesTableClicked(QTableWidgetItem *)));
    connect(filesTable, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(filePreview(QTableWidgetItem *)));

}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::closeEvent(QCloseEvent *event)
{
   QStringList list;

   QSettings settings("Trolltech", "EdytorNC");
   settings.beginGroup("FindDialog" );


   list.clear();
   for(int i = 0; i < directoryComboBox->count(); i++)
   {
      if(!directoryComboBox->itemText(i).isEmpty())
        list.prepend(directoryComboBox->itemText(i));
   };
   
   while(list.size() > MAXLISTS)
   {
      list.removeLast();
   };
   settings.setValue("Dirs", list);

   list.clear();
   for(int i = 0; i < fileComboBox->count(); i++)
   {
      if(!fileComboBox->itemText(i).isEmpty())
        list.prepend(fileComboBox->itemText(i));
   };

   while(list.size() > MAXLISTS)
   {
      list.removeLast();
   };
   settings.setValue("Filters", list);
   
   list.clear();
   for(int i = 0; i < textComboBox->count(); i++)
   {
      if(!textComboBox->itemText(i).isEmpty())
        list.prepend(textComboBox->itemText(i));
   };

   while(list.size() > MAXLISTS)
   {
      list.removeLast();
   }; 
   settings.setValue("Texts", list);

   settings.endGroup();

   event->accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::readSettings()
{
   QStringList list;
   

   textComboBox->clear(); 
   directoryComboBox->clear(); 
   fileComboBox->clear(); 

   QSettings settings("Trolltech", "EdytorNC");
   settings.beginGroup("FindDialog" );

   list = settings.value("Dirs", QStringList(QDir::homePath())).toStringList();
   directoryComboBox->addItems(list);

   list = settings.value("Filters", "*.nc").toStringList();
   fileComboBox->addItems(list);

   list = settings.value("Texts", QStringList()).toStringList();
   textComboBox->addItems(list);

   settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::filesTableClicked(QTableWidgetItem *item)
{
   if(item->column() == 0)
   {
      QString dir = directoryComboBox->currentText();
      if(!dir.endsWith("/"))
         dir = dir + "/";
      emit fileClicket(dir + item->text());
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::filePreview(QTableWidgetItem *item)
{  
   QTextCharFormat format;
   QTextCursor crs;

   //format.setFontUnderline(TRUE);
   
   if(item->column() == 0)
   {
      QString dir = directoryComboBox->currentText();
      if(!dir.endsWith("/"))
         dir = dir + "/";
      QFile file(dir + item->text());
      if(file.open(QFile::ReadOnly | QFile::Text)) 
      {
         QTextStream in(&file);
         crs = preview->textCursor();
         //format = crs.charFormat();
         crs.setCharFormat(format);
         preview->setPlainText(in.readAll());
         crs.setPosition(0);
         preview->setTextCursor(crs);
           
         if(!textComboBox->currentText().isEmpty())
         {
            do
            {  
               crs = preview->document()->find(textComboBox->currentText(), crs, 0);
               if(!crs.isNull())
               {
                  format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                  format.setFontPointSize(16);
                  //format.setFontWeight(QFont::Bold);  
                  crs.setCharFormat(format);
                  preview->setTextCursor(crs);
               };
            }while(!crs.isNull());
         };
         crs.setPosition(0);
         preview->setTextCursor(crs); 
      };

    
   };

}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void FindInFiles::setHighlightColors(const _h_colors colors)
{
   highlighter = new Highlighter(preview->document(), colors);
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void FindInFiles::setDir(const QString dir)
{
   directoryComboBox->addItem(dir);
   directoryComboBox->setCurrentIndex(directoryComboBox->count()-1);
}

//**************************************************************************************************
//
//**************************************************************************************************

