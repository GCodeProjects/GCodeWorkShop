/***************************************************************************
 *   Copyright (C) 2006-2010 by Artur Kozioł                               *
 *   artkoz@poczta.onet.pl                                                 *
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

#include <QtGui>

#include "findinf.h"


#define MAXLISTS        16


FindInFiles::FindInFiles(QWidget *parent): QDialog(parent)
{
    setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Find Files"));

    connect(browseButton, SIGNAL(clicked()), SLOT(browse()));
    connect(findButton, SIGNAL(clicked()), SLOT(find()));
    connect(closeButton, SIGNAL(clicked()), SLOT(closeDialog()));
    connect(hideButton, SIGNAL(clicked()), SLOT(close()));

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

void FindInFiles::closeDialog()
{
    setAttribute(Qt::WA_DeleteOnClose);
    close();
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), directoryComboBox->currentText());
    if(!directory.isEmpty())
    {
       directoryComboBox->addItem(directory);
       directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::find()
{
    QStringList files;

    filesTable->setRowCount(0);
    preview->clear();

    QString fileName = fileComboBox->currentText();
    QString text = textComboBox->currentText();
    QString path = directoryComboBox->currentText();

    findButton->setEnabled(FALSE);
    closeButton->setEnabled(FALSE);
    hideButton->setEnabled(FALSE);
    QApplication::setOverrideCursor(Qt::BusyCursor);
    qApp->processEvents();

    QDir directory = QDir(path);

    if(fileName.isEmpty())
      fileName = "*";
    files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    if(!text.isEmpty())
      files = findFiles(directory, files, text);

    findButton->setEnabled(TRUE);
    closeButton->setEnabled(TRUE);
    hideButton->setEnabled(TRUE);
    QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

QStringList FindInFiles::findFiles(const QDir &directory, const QStringList &files,
                                   const QString &text)
{
    int pos;
    QRegExp exp;
    QString comment_tx;
    qint64 size;
    bool textFounded, word, notFound;
    QString line;
    QStringList foundFiles;

    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));
    qApp->processEvents();

    exp.setCaseSensitivity(Qt::CaseInsensitive);
    exp.setPattern("\\([^\\n\\r]*\\)|;[^\\n\\r]*");

    notFound = true;
    pos = 0;
    for(int i = 0; i < files.size(); ++i)
    {
        progressDialog.setLabelText(tr("Searching file number %1 of %2")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();
        progressDialog.setValue(i);

        if(progressDialog.wasCanceled())
            break;

        QFile file(directory.absoluteFilePath(files[i]));

        if(file.open(QIODevice::ReadOnly))
        {  
            QTextStream in(&file);

            textFounded = false;
            word = false;
            line = in.readAll();

            if(text == "*") //files containing anything
            {
               textFounded = true;
            }
            else
            {
               pos = line.indexOf(text, 0, Qt::CaseInsensitive);
               textFounded = (pos >= 0);
            };


            if(textFounded && wholeWordsCheckBox->isChecked())
            {
               if(pos > 0)
                 if(line[pos - 1].isLetterOrNumber())
                   word = true;
                 pos = pos + text.size();
                 if(pos < line.size())
                   if(line[pos].isLetterOrNumber())
                     word = true;
            };

            if((textFounded && (!wholeWordsCheckBox->isChecked())) ||
              (textFounded && (wholeWordsCheckBox->isChecked() && !word)))
            {
               notFound = false;
               textFounded = false;
               word = false;
               size = file.size();

               pos = 0;
               while((pos = line.indexOf(exp, pos)) >= 0)
               {
                  comment_tx = line.mid(pos, exp.matchedLength());
                  pos += exp.matchedLength();
                  if(!comment_tx.contains(";$"))
                  {
                     comment_tx.remove('(');
                     comment_tx.remove(')');
                     comment_tx.remove(';');
                     break;
                  };
               };

               QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
               fileNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

               QTableWidgetItem *infoNameItem = new QTableWidgetItem(comment_tx);
               infoNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

               QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
               sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

               int row = filesTable->rowCount();
               filesTable->insertRow(row);
               filesTable->setItem(row, 0, fileNameItem);
               filesTable->setItem(row, 1, infoNameItem);
               filesTable->setItem(row, 2, sizeItem);
            };
            file.close();
        };   
    };

    if(notFound)
    {
       QTableWidgetItem *fileNameItem = new QTableWidgetItem(tr("No"));
       fileNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
       QTableWidgetItem *infoNameItem = new QTableWidgetItem(tr("files"));
       infoNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
       QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("found."));
       sizeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
       filesTable->insertRow(0);
       filesTable->setItem(0, 0, fileNameItem);
       filesTable->setItem(0, 1, infoNameItem);
       filesTable->setItem(0, 2, sizeItem);
    };

    filesTable->resizeColumnsToContents();
    filesTable->resizeRowsToContents();
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
   QString item;

   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("FindFileDialog");

   settings.setValue("WholeWords", wholeWordsCheckBox->isChecked());

   list.clear();
   list.append(directoryComboBox->currentText());
   for(int i = 0; i <= directoryComboBox->count(); i++)
   {
      item = directoryComboBox->itemText(i);
      if(!item.isEmpty())
        if(!list.contains(item))
          list.append(item);
   };
   
   while(list.size() > MAXLISTS)
   {
      list.removeLast();
   };
   settings.setValue("Dirs", list);
   settings.setValue("SelectedDir", directoryComboBox->currentText());

   list.clear();
   list.append(fileComboBox->currentText());
   for(int i = 0; i <= fileComboBox->count(); i++)
   {
      item = fileComboBox->itemText(i);
      if(!item.isEmpty())
        if(!list.contains(item))
          list.append(item);
   };

   while(list.size() > MAXLISTS)
   {
      list.removeLast();
   };
   settings.setValue("Filters", list);
   settings.setValue("SelectedFilter", fileComboBox->currentText());
   
   list.clear();
   list.append(textComboBox->currentText());
   for(int i = 0; i <= textComboBox->count(); i++)
   {
      item = textComboBox->itemText(i);
      if(!item.isEmpty())
        if(!list.contains(item, Qt::CaseInsensitive))
          list.append(item);
   };

   while(list.size() > MAXLISTS)
   {
      list.removeLast();
   }; 
   settings.setValue("Texts", list);
   settings.setValue("SelectedText", textComboBox->currentText());

   settings.endGroup();

   event->accept();
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::readSettings()
{
   QStringList list;
   QString item;
   int i;
   

   textComboBox->clear(); 
   directoryComboBox->clear(); 
   fileComboBox->clear(); 

   QSettings settings("EdytorNC", "EdytorNC");
   settings.beginGroup("FindFileDialog");

   wholeWordsCheckBox->setChecked(settings.value("WholeWords", FALSE).toBool());

   list = settings.value("Dirs", QStringList(QDir::homePath())).toStringList();
   list.removeDuplicates();
   list.sort();
   directoryComboBox->addItems(list);
   item = settings.value("SelectedDir", QString(QDir::homePath())).toString();
   i = directoryComboBox->findText(item);
   directoryComboBox->setCurrentIndex(i);

   list = settings.value("Filters", "*.nc").toStringList();
   list.removeDuplicates();
   list.sort();
   fileComboBox->addItems(list);
   item = settings.value("SelectedFilter", QString("*.nc")).toString();
   i = fileComboBox->findText(item);
   fileComboBox->setCurrentIndex(i);

   list = settings.value("Texts", QStringList()).toStringList();
   list.removeDuplicates();
   list.sort();
   textComboBox->addItems(list);
   item = settings.value("SelectedText", QString("*")).toString();
   i = textComboBox->findText(item, Qt::MatchExactly);
   textComboBox->setCurrentIndex(i);

   settings.endGroup();
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::filesTableClicked(int x, int y)
{
   Q_UNUSED(y);
   
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
      Q_UNUSED(y);

      QTableWidgetItem *item = filesTable->item(x, 0);

      QApplication::setOverrideCursor(Qt::BusyCursor);

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

         if((!textComboBox->currentText().isEmpty()) && !(textComboBox->currentText() == "*"))
         {
            highlightFindText(textComboBox->currentText(), (wholeWordsCheckBox->isChecked() ? QTextDocument::FindWholeWords : QTextDocument::FindFlags(0)));
         };
      };

      QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void FindInFiles::setHighlightColors(const _h_colors colors)
{
   highlighter = new Highlighter(preview->document());
   highlighter->setHColors(colors, preview->font());
}

//**************************************************************************************************
//
//************************************************************************************************** 
 
void FindInFiles::setDir(const QString dir)
{
   directoryComboBox->addItem(dir);
   directoryComboBox->setCurrentIndex(directoryComboBox->findText(dir));
}

//**************************************************************************************************
//
//**************************************************************************************************

void FindInFiles::highlightFindText(QString searchString, QTextDocument::FindFlags options)
 {
    findTextExtraSelections.clear();
    QColor lineColor = QColor(Qt::yellow).lighter(155);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = preview->document();
    QTextCursor cursor = preview->textCursor();
    cursor.setPosition(0);

    do
    {
       cursor = doc->find(searchString, cursor, options);
       if(!cursor.isNull())
       {
          selection.cursor = cursor;
          
          QTextCharFormat format = cursor.charFormat();
          //format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
          //format.setUnderlineColor(Qt::green);
          format.setFontPointSize(16);
          qApp->processEvents();
          cursor.mergeCharFormat(format);

          findTextExtraSelections.append(selection);

       };
    } while(!cursor.isNull());

    preview->setExtraSelections(findTextExtraSelections);

 }

//**************************************************************************************************
//
//**************************************************************************************************

