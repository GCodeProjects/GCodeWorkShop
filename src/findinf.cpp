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
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Find Files"));

    connect( browseButton, SIGNAL(clicked()), SLOT(browse()));
    connect( findButton, SIGNAL(clicked()), SLOT(find()));
    connect( closeButton, SIGNAL(clicked()), SLOT(close()));

    createFilesTable();

    preview->setReadOnly(TRUE);
    preview->setWordWrapMode(QTextOption::NoWrap);
    preview->setFont(QFont("Courier", 10, QFont::Normal));

    resize(parent->width() * 0.8, parent->height() * 0.6);

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
       directoryComboBox->setCurrentIndex(directoryComboBox->count() - 1);
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

    int pos;
    QRegExp exp;
    QString f_tx;
    qint64 size;
    bool founded;
    QString line;
    QStringList foundFiles;



    exp.setPattern("\\([^\\n\\r]*\\)|;[^\\n\\r]*"); //find first comment and set it in window tilte

    for (int i = 0; i < files.size(); ++i)
    {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();


        if (progressDialog.wasCanceled())
            break;

        QFile file(directory.absoluteFilePath(files[i]));

        if(file.open(QIODevice::ReadOnly))
        {  
            QTextStream in(&file);
            founded = false;
            while (!in.atEnd())
            {
                if (progressDialog.wasCanceled())
                    break;

                line = in.readLine();
                pos = 0;
                if(!founded)
                {
                   pos = line.indexOf(exp, pos);
                   if(pos >= 0)
                   {
                      founded = true;
                      f_tx = line.mid(pos, exp.matchedLength());
                      if(!(f_tx.mid(0, 2) == ";$"))
                      {
                         f_tx.remove('(');
                         f_tx.remove(')');
                         f_tx.remove(';');

                      };
                   };
                };

                if(line.contains(text, Qt::CaseInsensitive))
                {
                    founded = false;
                    size = file.size();

                    QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
                    fileNameItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                    QTableWidgetItem *infoNameItem = new QTableWidgetItem(f_tx);
                    infoNameItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                    QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
                    sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                    int row = filesTable->rowCount();
                    filesTable->insertRow(row);
                    filesTable->setItem(row, 0, fileNameItem);
                    filesTable->setItem(row, 1, infoNameItem);
                    filesTable->setItem(row, 2, sizeItem);
                    filesTable->resizeColumnsToContents();
                    filesTable->resizeRowsToContents();

                    break;
                }
            };
            file.close();
        };
    }

    //filesTable->adjustSize();
    return foundFiles;
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::createFilesTable()
{
    QStringList labels;
    labels << tr("File Name") << tr("Info") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    connect(filesTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(filesTableClicked(int, int)));
    connect(filesTable, SIGNAL(cellClicked(int, int)), this, SLOT(filePreview(int, int)));
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

void FindInFiles::filesTableClicked(int x, int y)
{
   QTableWidgetItem *item = filesTable->item(x, 0);

   QString dir = directoryComboBox->currentText();
   if(!dir.endsWith("/"))
     dir = dir + "/";
   emit fileClicket(dir + item->text());

}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::filePreview(int x, int y)
{

   QTableWidgetItem *item = filesTable->item(x, 0);


      QString dir = directoryComboBox->currentText();
      if(!dir.endsWith("/"))
         dir = dir + "/";
      QFile file(dir + item->text());
      if(file.open(QIODevice::ReadOnly))
      {
         QTextStream in(&file);
         preview->setPlainText(in.readAll());
         file.close();
         qApp->processEvents();

         if(!textComboBox->currentText().isEmpty())
         {
            QTextCursor cr = preview->textCursor();

            while(!cr.isNull() && !cr.atEnd())
            {
               cr = preview->document()->find(textComboBox->currentText(), cr);

               if(!cr.isNull())
               {

                  QTextCharFormat format = cr.charFormat();
                  format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                  format.setUnderlineColor(Qt::green);
                  format.setFontPointSize(16);
                  cr.mergeCharFormat(format);
                  //preview->setTextCursor(cr);
                  qApp->processEvents();
               };

            };
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

