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



#include "mdichild.h"




//**************************************************************************************************
//
//**************************************************************************************************

MdiChild::MdiChild(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    textEdit->setWordWrapMode(QTextOption::NoWrap);
    highlighter = NULL;
    setFocusProxy(textEdit);

    marginWidget->setAutoFillBackground(TRUE);
    marginWidget->setBackgroundRole(QPalette::Base);
    textEdit->installEventFilter(this);
    setWindowIcon(QIcon(":/images/ncfile.png"));
    splitterH->setBackgroundRole(QPalette::Base);
    //splitterV->setBackgroundRole(QPalette::Base);


    //textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    //setContextMenuPolicy(Qt::CustomContextMenu);
}

//**************************************************************************************************
//
//**************************************************************************************************

MdiChild::~MdiChild()
{
   if(highlighter != NULL)
      delete highlighter;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.nc").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
       QMessageBox::warning(this, tr("EdytorNC"),
                            tr("Cannot read file \"%1\".\n %2")
                            .arg(fileName)
                            .arg(file.errorString()));
       return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString tex = in.readAll();
    textEdit->setPlainText(tex);
    file.close();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName, tex);
    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

    return true;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::save()
{
   bool result;
   setFocus();
   if(isUntitled)
   {
      result = saveAs();
   }
   else
   {
      result = saveFile(curFile);
   };

   if(result)
   {
      if(mdiWindowProperites.clearUndoHistory)
      {
         textEdit->setUndoRedoEnabled(false);  //clear undo/redo history
         textEdit->setUndoRedoEnabled(true);
      };

      if(mdiWindowProperites.clearUnderlineHistory)
      {
         
         QTextCursor cursorPos = textEdit->textCursor();
         textEdit->blockSignals(true);
         textEdit->selectAll();
         if(mdiWindowProperites.underlineChanges)
         {
            QTextCursor cr = textEdit->textCursor(); // Clear underline
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
            cr.setCharFormat(format);

            textEdit->setTextCursor(cr);
         };
         textEdit->setTextCursor(cursorPos);

         textEdit->document()->setModified(false);
         documentWasModified();
         textEdit->blockSignals(false);
      };
   };

   detectHighligthMode();
   return result;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::saveAs()
{

#ifdef Q_OS_LINUX
    QString filters = tr("CNC programs files *.nc (*.nc);;"
                         "CNC programs files *.anc (*.anc);;"
                         "CNC programs files *.min (*.min);;"
                         "CNC programs files *.cnc (*.cnc);;"
                         "Text files *.txt (*.txt);;"
                         "All files (*.* *)");
#endif

#ifdef Q_OS_WIN32
    QString filters = tr("CNC programs files (*.nc);;"
                         "CNC programs files (*.anc);;"
                         "CNC programs files (*.min);;"
                         "CNC programs files (*.cnc);;"
                         "Text files (*.txt);;"
                         "All files (*.* *)");
#endif

    QString file = QFileDialog::getSaveFileName(
                         this,
                         tr("Save file as..."),
                         curFile,
                         filters, &saveFileFilter, QFileDialog::DontConfirmOverwrite);

       
    if((QFile(file).exists()))
    {

       QMessageBox msgBox;
       msgBox.setText(tr("<b>File \"%1\" exists.</b>").arg(file));
       msgBox.setInformativeText(tr("Do you want overwrite it ?"));
       msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
       msgBox.setDefaultButton(QMessageBox::Discard);
       msgBox.setIcon(QMessageBox::Warning);
       int ret = msgBox.exec();
       switch (ret)
       {
          case QMessageBox::Save    : break;
          case QMessageBox::Discard : return false;
                                      break;
          default                   : return false;
                                      break;
       } ;

    };

    if(file.isNull())
      return false;
    else
      return saveFile(file);

}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::saveAsWithPreview()
{

    CustomFDialog *fileDialog;
    QStringList filters;
    QFileInfo file;
    QString fileName;


    fileDialog = new CustomFDialog(this, fdShowPreview);

    filters << tr("CNC programs files (*.nc)")
            << tr("CNC programs files (*.min)")
            << tr("CNC programs files (*.anc)")
            << tr("CNC programs files (*.cnc)")
            << tr("Text files (*.txt)")
            << tr("All files (*.* *)");

    fileDialog->setNameFilters(filters);
    fileDialog->setHighlightColors(mdiWindowProperites.hColors);

    fileDialog->restoreState(saveDialogState);

    if(!saveFileFilter.isEmpty())
      fileDialog->selectNameFilter(saveFileFilter);

    fileDialog->selectFile(curFile);
    fileDialog->setConfirmOverwrite(FALSE);

    if(fileDialog->exec() == QDialog::Accepted)
    {
       saveDialogState = fileDialog->saveState();
       saveFileFilter = fileDialog->selectedNameFilter();
       fileName = fileDialog->selectedFile();

       if((QFile(fileName).exists()))
       {
          switch(QMessageBox::warning(this, tr("Warning"), tr("File : %1 exists. Overwrite ?").arg(fileName),
                                       tr("Yes"), tr("No")))
          {
             case 0:  break;
             case 1:  return false;
          }
       }
       return saveFile(fileName);
    };
    return false;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::saveFile(const QString &fileName)
{
    int curPos;
    QRegExp exp;
    QString f_tx;
    QTextCursor cursor;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
       QMessageBox::warning(this, tr("EdytorNC"),
                            tr("Cannot write file \"%1\".\n %2")
                            .arg(fileName)
                            .arg(file.errorString()));
       return false;
    };

    QApplication::setOverrideCursor(Qt::WaitCursor);
    curPos = textEdit->textCursor().position();
   
    QDate dat = QDate::currentDate();


    exp.setPattern("(DATA|DATE)[:\\s]*[\\d][\\d](\\.|-)[\\d][\\d](\\.|-)[\\d][\\d][\\d][\\d]");
    cursor = textEdit->textCursor();
    cursor.setPosition(0);
    //setTextCursor(cursor);

    cursor = textEdit->document()->find(exp, cursor);
    if(!cursor.isNull())
    {
       textEdit->setUpdatesEnabled(FALSE);
       cursor.removeSelectedText();
       cursor.insertText(dat.toString("dd.MM.yyyy"));

       textEdit->setUpdatesEnabled( TRUE );
       textEdit->repaint();
    }
    else
    {
       exp.setPattern("[\\d][\\d](\\.|-)[\\d][\\d](\\.|-)[\\d][\\d][\\d][\\d]");
       cursor = textEdit->textCursor();
       cursor.setPosition(0);
       cursor = textEdit->document()->find(exp, cursor);
       if(!cursor.isNull())
       {
          textEdit->setUpdatesEnabled( FALSE );
          cursor.removeSelectedText();
          cursor.insertText(dat.toString("dd.MM.yyyy"));

          textEdit->setUpdatesEnabled( TRUE );
          textEdit->repaint();
       }

    };

    QTextStream out(&file);

    QString tex = textEdit->toPlainText();
    if(!tex.contains("\r\n"))
      tex.replace("\n", "\r\n");
    out << tex;
    file.close();
    QApplication::restoreOverrideCursor();
    
    cursor = textEdit->textCursor();
    cursor.setPosition(curPos);
    textEdit->setTextCursor(cursor);
    
    setCurrentFile(fileName, tex);
    return true;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::closeEvent(QCloseEvent *event)
{
   if(maybeSave())
   {
      event->accept();
   }
   else
   {
      event->ignore();
   }
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::documentWasModified()
{
   setWindowModified(textEdit->document()->isModified());
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::maybeSave()
{
   if(textEdit->document()->isModified())
   {
      QMessageBox msgBox;
      msgBox.setText(tr("<b>File: \"%1\"\n has been modified.</b>").arg(curFile));
      msgBox.setInformativeText(tr("Do you want to save your changes ?"));
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Save);
      msgBox.setIcon(QMessageBox::Warning);
      int ret = msgBox.exec();
      switch (ret)
      {
         case QMessageBox::Save    : return save();
                                     break;
         case QMessageBox::Discard : textEdit->document()->setModified(FALSE);
                                     return TRUE;
                                     break;
         case QMessageBox::Cancel  : return FALSE;
                                     break;
         default                   : return TRUE;
                                     break;
      } ;
   };
   return TRUE;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::setCurrentFile(const QString &fileName, const QString &text)
{

    int pos;
    QRegExp exp;
    QString f_tx;


    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    exp.setPattern("\\([^\\n\\r]*\\)|;[^\\n\\r]*"); //find first comment and set it in window tilte
    pos = 0;
    pos = text.indexOf(exp, pos);
    while(pos != -1)
    {
       f_tx = text.mid(pos, exp.matchedLength());
       if(!(f_tx.mid(0, 2) == ";$"))
       {
          f_tx.remove('(');
          f_tx.remove(')');
          f_tx.remove(';');
          break;
       };
       pos += exp.matchedLength();
       pos = text.indexOf(exp, pos);

    };

    if(!f_tx.isEmpty())
       curFileInfo = f_tx.simplified();
    else
       curFileInfo = "";

    setWindowTitle(QString("%2 ---> %1 [*]").arg(curFile).arg(curFileInfo));

}

//**************************************************************************************************
//
//**************************************************************************************************

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//**************************************************************************************************
//
//**************************************************************************************************

QString MdiChild::filePath()
{
    return QFileInfo(curFile).absolutePath();
}

//**************************************************************************************************
//
//**************************************************************************************************

_editor_properites MdiChild::getMdiWindowProperites()
{
   mdiWindowProperites.isRedo = textEdit->document()->isRedoAvailable();
   mdiWindowProperites.isUndo = textEdit->document()->isUndoAvailable();
   mdiWindowProperites.ins = textEdit->overwriteMode();
   mdiWindowProperites.modified = textEdit->document()->isModified();
   mdiWindowProperites.readOnly = textEdit->isReadOnly();
   mdiWindowProperites.isSel = textEdit->textCursor().hasSelection();
   mdiWindowProperites.cursorPos = textEdit->textCursor().position(); //textCursor().blockNumber();
   mdiWindowProperites.geometry = parentWidget()->saveGeometry();

   mdiWindowProperites.fileName = curFile;
   return(mdiWindowProperites);
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::setMdiWindowProperites(_editor_properites opt)
{
   disconnect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
   
   mdiWindowProperites = opt;
   textEdit->setReadOnly(mdiWindowProperites.readOnly);
   setFont(QFont(mdiWindowProperites.fontName, mdiWindowProperites.fontSize, QFont::Normal));

   if(mdiWindowProperites.syntaxH)
   {
      if(highlighter == NULL)
        highlighter = new Highlighter(textEdit->document());

      if(highlighter != NULL)
         detectHighligthMode();
   }
   else
   {
      if(highlighter != NULL)
        delete(highlighter);
      highlighter = NULL;
   };
    
   QTextCursor cursor = textEdit->textCursor();
   cursor.setPosition(mdiWindowProperites.cursorPos);
   textEdit->setTextCursor(cursor);
   textEdit->centerCursor();
   connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::eventFilter(QObject *obj, QEvent *ev)
{
   if((obj == textEdit) && !(textEdit->isReadOnly()))
   {
       if( ev->type() == QEvent::KeyPress )
       {
          QKeyEvent *k = (QKeyEvent*) ev;

          if(k->key() == Qt::Key_Insert)
             textEdit->setOverwriteMode(!textEdit->overwriteMode());

          if(mdiWindowProperites.underlineChanges)
          {
             if((k->text()[0].isPrint()) && !(k->text()[0].isSpace()))
             {
                QTextCursor cr = textEdit->textCursor(); //Underline changes
                QTextCharFormat format = cr.charFormat();
                format.setUnderlineStyle(QTextCharFormat::DotLine);
                format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
                cr.setCharFormat(format);
                textEdit->setTextCursor(cr);
             };
          };

          if(k->key() == Qt::Key_Comma) //Keypad comma should always prints period
          {
             if((k->modifiers() == Qt::KeypadModifier) || (k->nativeScanCode() == 0x53)) // !!! Qt::KeypadModifier - Not working for keypad comma !!!
             {
                QApplication::sendEvent(textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, ".", FALSE, 1));
                return true;
             };

          };

          if(mdiWindowProperites.intCapsLock)
          {
             if(k->text()[0].isLower() && (k->modifiers() == Qt::NoModifier))
             {
                QApplication::sendEvent(textEdit, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::NoModifier, k->text().toUpper(), FALSE, 1));
                return true;

             };

             if(k->text()[0].isUpper() && (k->modifiers() == Qt::ShiftModifier))
             {
                QApplication::sendEvent(textEdit, new QKeyEvent(QEvent::KeyPress, k->key(), Qt::ShiftModifier, k->text().toLower(), FALSE, 1));
                return true;
             };
          };
       };

       return FALSE;
   }
   else
   {
      return textEdit->eventFilter(obj, ev);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

int MdiChild::doRenumber(int &mode, int &startAt, int &from, int &prec, int &inc, int &to, bool &renumEmpty, bool &renumComm, bool &renumMarked)
{
   int pos, i, num, it, count;
   QString tx, f_tx, line, i_tx, new_tx;
   QRegExp exp;
   bool ok, selection, insertSpace;
   QTextCursor cursor;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   exp.setCaseSensitivity (Qt::CaseInsensitive);

   cursor = textEdit->textCursor();
   QTextCharFormat format = cursor.charFormat();
   format.setUnderlineStyle(QTextCharFormat::NoUnderline);
   cursor.mergeCharFormat(format);

   if(cursor.hasSelection())
   {
      tx = textEdit->textCursor().selectedText();
      tx.replace(QChar::ParagraphSeparator, "\n");
      selection = true;
   }
   else
   {
      tx = textEdit->toPlainText();
      selection = false;
   };

   count = 0;
   while(1)
   {
      if(mode == 4) //renumber lines without N
      {
         num = startAt;
         for(i = 0; i < (textEdit->document()->lineCount() - 1); i++)
         {
            line = tx.section('\n', i, i);

            i_tx = QString("%1").arg(num, prec);
            i_tx.replace(' ', '0');
            i_tx += "  ";

            exp.setPattern("^[0-9]{1,4}\\s\\s");
            pos = line.indexOf(exp, 0);
            if(pos >= 0)
            {
               line.replace(pos, exp.matchedLength(), i_tx);
               num += inc;
               count++;
            }
            else
            {
               if(renumEmpty)
               {
                  line.insert(0, i_tx);
                  num += inc;
                  count++;
               };
            };
            new_tx += line + '\n';
         };
         tx = new_tx;
         break;
      };


      if(mode == 3) //remove all
      {
         pos = 0;
         num = 0;
         exp.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
         while((pos = tx.indexOf(exp, pos)) >= 0)
         {
            f_tx = tx.mid(pos, exp.matchedLength());
            if(((f_tx.contains('(') == 0) && (f_tx.contains('\'') == 0) && (f_tx.contains(';') == 0)))
            {
               tx.remove(pos, exp.matchedLength());
               num++;
            }
            else
              pos += exp.matchedLength();
         };
         break;
      };

      if(mode == 1) //renumber all with N
      {
         pos = 0;
         num = startAt;
         exp.setPattern("[N]{1,1}[0-9\\s]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
         while((pos = tx.indexOf(exp, pos)) >= 0)
         {
            f_tx = tx.mid(pos, exp.matchedLength());

            if(pos > 0)
              if(tx[pos - 1].isLetterOrNumber())
               {
                  pos += exp.matchedLength();
                  continue;
               };

            insertSpace = true;
            if(f_tx.endsWith(' '))
               insertSpace = false;

            if((!f_tx.contains(' ')) && (!f_tx.contains('\n')))
            {
               i = exp.matchedLength();
            }
            else
            {
               i = exp.matchedLength() - 1;
            };

            if((!(f_tx.contains('(')) && (!f_tx.contains('\'')) && (!f_tx.contains(';'))))
            {
                f_tx.remove(0, 1);
                f_tx.remove(' ');
                if(!f_tx.isEmpty())
                  it = f_tx.toInt(&ok);
                else
                  it = 0;
                if(((it >= from) || (renumMarked && it == 0)) && (it < to))
                {
                   f_tx = QString("N%1").arg(num, prec);
                   f_tx.replace(' ', '0');
                   if(insertSpace)
                     f_tx.append(" ");
                   tx.replace(pos, i, f_tx);
                   num += inc;
                   count++;
                };
            };
            pos += exp.matchedLength();
         };
         break;
      };

      if(mode == 2) //renumber all
      {
         num = startAt;
         for(i = 0; i < (textEdit->document()->lineCount()); i++)
         {
            line = tx.section('\n', i, i);
            exp.setPattern("[N]{1,1}[0-9]+[\\s]{0,}|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
            pos = 0;
            while(1)
            {
               if(line.isEmpty())
               {
                  if(!renumEmpty)
                    break;
                  f_tx = QString("N%1").arg(num, prec);
                  f_tx.replace(' ', '0');
                  num += inc;
                  count++;
                  line.insert(0, f_tx);
                  break;
               };

               if(((pos = line.indexOf(exp, pos)) >= 0) && (line.at(0) != '$'))
               {
                  i_tx = line.mid(pos, exp.matchedLength());

                  if((!(i_tx.contains('(')) && !(i_tx.contains('\'')) && (!i_tx.contains(';'))))
                  {
                     f_tx = QString("N%1").arg(num, prec);
                     f_tx.replace(' ', '0');
                     num += inc;
                     count++;
                     f_tx.append(" ");
                     line.replace(i_tx, f_tx);
                     break;
                  }
                  else
                    if(renumComm)
                      break;
               }

               if((line.at(0) == 'N') && (!line.at(1).isLetter()))
               {
                  f_tx = QString("N%1").arg(num, prec);
                  f_tx.replace(' ', '0');
                  num += inc;
                  count++;
                  f_tx.append(" ");
                  line.replace(0, 1, f_tx);
                  break;
               };

               if(((line.at(0) != '%') && (line.at(0) != ':') && (line.at(0) != 'O') && (line.at(0) != '$')))
               {
                  f_tx = QString("N%1").arg(num, prec);
                  f_tx.replace(' ', '0');
                  num += inc;
                  count++;
                  f_tx.append(" ");
                  line.insert(0, f_tx);
                  break;
               };
               break;
            };
            new_tx += line + '\n';
         };
         tx = new_tx;
         break;
      };
      break;
   };


   if(selection)
   {
      textEdit->textCursor().insertText(tx);
   }
   else
   {
      textEdit->selectAll();
      textEdit->insertPlainText(tx);
      cursor.setPosition(0);
   };

   textEdit->setTextCursor(cursor);
   QApplication::restoreOverrideCursor();
   return count;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doRemoveSpace()
{
   int i;
   QString tx;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   tx = textEdit->toPlainText();

   for(i = 0; i < tx.length(); i++)
   {
      if(tx.at(i) == '(')
        do
        {
           i++;
           if(i > tx.length())
             break;
        }while(!((tx.at(i) == ')') || (tx.at(i) == '\n')));

      if(tx.at(i) == '\'')
        do
        {
           i++;
           if(i > tx.length())
             break;
        }while(!((tx.at(i) == '\'') || (tx.at(i) == '\n')));

      if(tx.at(i) == ';')
        do
        {
           i++;
           if(i > tx.length())
             break;
        }while(!((tx.at(i) == '\n')));

      if(tx.at(i) == ' ')
      {
          tx.remove(i, 1);
          i--;
      };
   };

   textEdit->selectAll();
   textEdit->insertPlainText(tx);
   QTextCursor cursor = textEdit->textCursor();
   cursor.setPosition(0);
   textEdit->setTextCursor(cursor);
   QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doRemoveEmptyLines()
{
   int i;
   QString tx, line, newTx;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   tx = textEdit->toPlainText();

   for(i = 0; i < (textEdit->document()->lineCount() - 1); i++)
   {
      line = tx.section('\n', i, i);
      line.simplified();
      if(!line.isEmpty())
       newTx.append(line + '\n');
   };

   textEdit->selectAll();
   textEdit->insertPlainText(newTx);
   QTextCursor cursor = textEdit->textCursor();
   cursor.setPosition(0);
   textEdit->setTextCursor(cursor);
   QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doInsertSpace()
{
   int pos;
   QString tx;
   QRegExp exp;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   exp.setCaseSensitivity (Qt::CaseInsensitive);

   exp.setPattern("[A-Z]+|[#@;:(\\']");

   tx = textEdit->toPlainText();
   pos = 1;
   while((pos = tx.indexOf(exp, pos)) > 0)
   {

      while(1)
      {

         if(tx.at(pos) == '(')
         {
            if((tx.at(pos - 1) != ' ') && (tx.at(pos - 1) != '\n'))
            {
               tx.insert(pos, ' ');
               pos++;
            };
            do
            {
               pos++;
               if(pos > tx.length())
                 break;
            }while(!((tx.at(pos) == ')') || (tx.at(pos) == '\n')));
            break;
         };

         if(tx.at(pos) == ';')
         {
            if((tx.at(pos - 1) != ' ') && (tx.at(pos - 1) != '\n'))
            {
               tx.insert(pos, ' ');
               pos++;
            };
            do
            {
               pos++;
               if(pos > tx.length())
                 break;
            }while(!((tx.at(pos) == '\n')));
            break;
         };

         if(tx.at(pos) == '\'')
         {
            do
            {
               pos++;
               if(pos > tx.length())
                 break;
            }while(!((tx.at(pos) == '\'') || (tx.at(pos) == '\n')));
            break;
         };

         if((tx.at(pos) == '#'))
         {
            if(tx.at(pos-1).isDigit())
            {
               tx.insert(pos, ' ');
               pos++;
            };
            break;
         };

         if((tx.at(pos - 1) != ' ') && (tx.at(pos - 1) != '\n'))
         {
            tx.insert(pos, ' ');
            pos++;
            break;
         };

         break;
      };
      pos +=  exp.matchedLength();
   };

   textEdit->selectAll();
   textEdit->insertPlainText(tx);
   QTextCursor cursor = textEdit->textCursor();
   cursor.setPosition(0);
   textEdit->setTextCursor(cursor);
   QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doInsertDot()
{
   int pos, count;
   QString tx, f_tx;
   QRegExp exp;
   double it;
   bool ok;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   exp.setCaseSensitivity(Qt::CaseInsensitive);

   exp.setPattern(QString("[%1]{1,1}[-.+0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$").arg(mdiWindowProperites.dotAdr));

   count = 0;
   tx = textEdit->toPlainText();
   pos = 0;
   while((pos = tx.indexOf(exp, pos)) >= 0)
   {
      f_tx = tx.mid(pos, exp.matchedLength());
      pos++;

      if(((f_tx.contains('(') == 0) && (f_tx.contains('\'') == 0) && (f_tx.contains(';') == 0)))
      {
         if(mdiWindowProperites.dotAfter && (f_tx.contains('.') == 0))
         {
            f_tx.remove(0, 1);

            //f_tx.remove('+');
            it = f_tx.toDouble(&ok);
            if(ok)
            {
               it = it / mdiWindowProperites.dotAftrerCount;
               tx.replace(pos, exp.matchedLength() - 1, QString("%1").arg(it, 0, 'f', 3));
               count++;
            };
         };

         if((mdiWindowProperites.atEnd && (f_tx.contains('.') == 0)))
         {
            tx.insert(pos + exp.matchedLength() - 1, '.');
            pos++;
            count++;
         };
     }
     else
       pos += (exp.matchedLength());
  };

  emit message( QString(tr("Inserted : %1 dots.")).arg(count), 6000 );
  cleanUp(&tx);
  textEdit->selectAll();
  textEdit->insertPlainText(tx);
  QTextCursor cursor = textEdit->textCursor();
  cursor.setPosition(0);
  textEdit->setTextCursor(cursor);
  QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::cleanUp(QString *str)  //remove not needed zeros
{

   QRegExp exp;
   int pos;


   pos = 1;
   exp.setPattern("[\\d]+[.][-+.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$");

   while((pos = str->indexOf(exp, pos)) > 0)
   {
      if((str->at(pos + exp.matchedLength() - 1) == '0') && str->at(pos-1) != ';')
        str->remove(pos + exp.matchedLength() - 1, 1);
      else
        pos += exp.matchedLength();
   };

}

//**************************************************************************************************
//
//**************************************************************************************************

 void MdiChild::doI2M()
{
   int pos, count;
   QString tx, f_tx;
   QRegExp exp;
   double it;
   bool ok;

   QApplication::setOverrideCursor(Qt::BusyCursor);

   exp.setCaseSensitivity(Qt::CaseInsensitive);
   exp.setPattern(QString("[%1]{1,1}[-.0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*$").arg(mdiWindowProperites.i2mAdr));

   count = 0;
   tx = textEdit->toPlainText();
   pos = 0;
   while((pos = tx.indexOf(exp, pos)) >= 0)
   {
      f_tx = tx.mid(pos, exp.matchedLength());
      pos += exp.matchedLength();

      if(((f_tx.contains('(') == 0) && (f_tx.contains('\'') == 0) && (f_tx.contains(';') == 0)))
      {
            f_tx.remove(0, 1);

            it = f_tx.toDouble(&ok);
            if(ok)
            {
               if(it != 0)
               {
                  if(!mdiWindowProperites.inch)
                    it = it / 25.4;
                  else
                    it = it * 25.4;
                  tx.replace(pos - (exp.matchedLength() - 1), exp.matchedLength() - 1, QString("%1").arg(it, 0, 'f', mdiWindowProperites.i2mprec));
                  pos++;
                  count++;
               };
            };
     };
  };

  emit message(QString(tr("Converted : %1 numbers.")).arg(count), 6000);
  cleanUp(&tx);
  textEdit->selectAll();
  textEdit->insertPlainText(tx);
  QTextCursor cursor = textEdit->textCursor();
  cursor.setPosition(0);
  textEdit->setTextCursor(cursor);
  QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::event(QEvent *event)
{
   QString group, key, text;
   QTextCursor cursor;
   QString fileName;

   if((event->type() == QEvent::ToolTip) && mdiWindowProperites.editorToolTips)
   {

      switch(mdiWindowProperites.hColors.highlightMode)
      {
         case MODE_OKUMA            : group = "OKUMA";
                                      break;
         case MODE_FANUC            : group = "FANUC";
                                      break;
         case MODE_SINUMERIK_840    : group = "SINUMERIK_840";
                                      break;
         case MODE_PHILIPS          :
         case MODE_SINUMERIK        : group = "SINUMERIK";
                                      break;
         case MODE_HEIDENHAIN       : group = "HEIDENHAIN";
                                      break;
         case MODE_HEIDENHAIN_ISO   : group = "HEIDENHAIN_ISO";
                                      break;
         default                    : event->accept();
                                      return true;

      };



      QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

      cursor = textEdit->cursorForPosition(helpEvent->pos());

      if(mdiWindowProperites.hColors.highlightMode == MODE_FANUC)
      {
         do
         {
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            key = cursor.selectedText();

         }while(key.at(0).isLetter() && !key.isEmpty() && !cursor.atStart());

         cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
         do
         {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            key = cursor.selectedText();

         }while(key.at(key.length() - 1).isLetter() && !key.isEmpty() && !cursor.atBlockEnd());

         cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

         if(key.length() < 3)
         {
            cursor = textEdit->cursorForPosition(helpEvent->pos());
            do
            {
               cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
               key = cursor.selectedText();

            }while(!((key.at(0) == '#') || key.at(0).isLetter()) && !key.isEmpty() && !cursor.atStart());

            cursor.clearSelection();
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            do
            {
               cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
               key = cursor.selectedText();

            }while(key.at(key.length() - 1).isDigit() && !key.isEmpty() && !cursor.atEnd());

            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
         };

      }
      else
      {

         cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);
         cursor.movePosition(QTextCursor::EndOfWord,  QTextCursor::KeepAnchor);
      };

      key = cursor.selectedText();

      if(key.length() == 2)
      {
         if((key.at(0) == 'G') || (key.at(0) == 'M'))
            if(!key.at(1).isLetter())
               key.insert(1, "0");
      };

      qDebug() << "Full key: " << key;

      fileName = QFileInfo(curFile).canonicalPath() + "/" + "cnc_tips.txt";
      if(QFile::exists(fileName))
      {
         QSettings settings(fileName, QSettings::IniFormat);
         settings.beginGroup(group);
         text = settings.value(key, "").toString();
         settings.endGroup();
      };

      if(text.isEmpty() || text.isNull())
      {
         QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "EdytorNC", "EdytorNC");
         QString config_dir = QFileInfo(cfg.fileName()).absolutePath() + "/";

         fileName = config_dir + "cnc_tips_" + QLocale::system().name() + ".txt";

         if(QFile::exists(fileName))
         {
            QSettings settings(fileName, QSettings::IniFormat);
            settings.beginGroup(group);
            text = settings.value(key, "").toString();
            settings.endGroup();
         }
         else
         {
            event->accept();
            return true;
         };
      };

      if(!text.isEmpty())
      {
         key = "<p style='white-space:pre'>";
         if(text.length() > 128)
            key = "<p style='white-space:normal'>";
         QToolTip::showText(helpEvent->globalPos(), key + text, this, QRect());
      }
      else
      {
         QToolTip::hideText();
         event->ignore();
      };

      return true;
    };
    return QWidget::event(event);
 }

//**************************************************************************************************
//
//**************************************************************************************************

int MdiChild::compute(QString *str)
{

   QRegExp exp;
   QString val1, val2, partmp;
   QString oper;
   int pos, i, j, err;
   double result = 0;
   bool ok, ok1, dot, minus;

   pos = 0;
   exp.setPattern("[$A-Z]+");

   while((pos = str->indexOf(exp, pos)) > 0)
   {
      j = pos;
      oper = str->mid(pos, exp.matchedLength());
      pos += exp.matchedLength();
      val1 = "";
      dot = false;
      minus = false;
      while((str->at(pos) == '-') || (str->at(pos) == '.') || (str->at(pos).isDigit()))
      {
         if(str->at(pos) == '.')
         {
            if((dot))
              return(ERR_DOUBLE_DOT);
            dot = true;
         };

         if(str->at(pos) == '-')
         {
            if(minus || dot)
            {
               break;
            };
            minus = true;
         };

         val1 += str->at(pos);
         pos++;
      };

      //pos--;
      if(val1.isEmpty())
        return(ERR_NO_PARAM);

      err = processBrc(&val1);
      if(err < 0)
        return(err);

      if(val1.isEmpty())
        val1 = "0";

      result = val1.toDouble(&ok);
      if(!ok)
        return(ERR_CONVERT);

      while(1)
      {
         if(oper == "SIN")
         {
            result = sin((M_PI/180) * result);
            break;
         };

         if(oper == "COS")
         {
            result = cos((M_PI/180) * result);
            break;
         };

         if(oper == "TAN")
         {
            result = tan((M_PI/180) * result);
            break;
         };

         if(oper == "SQRT")
         {
            result = sqrt(result);
            break;
         };

         if(oper == "SQR")
         {
            result = pow(result, 2);
            break;
         };

         if(oper == "ABS")
         {
            result = abs(result);
            break;
         };

         if(oper == "TRUNC")
         {
            result = trunc(result);
            break;
         };

         if(oper == "PI")
         {
            result = M_PI;
            break;
         };

         return(ERR_UNKNOWN_FUNC);
      };

      partmp.number(result, 'g', 3);
      str->replace(j, pos - j, QString("%1").arg(result, 0, 'f', 3));

   };

   pos = 0;
   exp.setPattern("[/*]{1,1}");

   while((pos = str->indexOf(exp, pos)) > 0)
   {
      oper = str->mid(pos, 1);

      val2 = "";
      dot = false;
      minus = false;

      for(i = pos+1; i <= str->length(); i++)
      {
         if((str->at(i) == '.'))
         {
            if((dot))
              return(ERR_DOUBLE_DOT);
            dot = true;
         };

         if((str->at(i) == '-'))
         {
            if(minus || dot)
            {
               break;
            };
            minus = true;
         };

         if(!((str->at(i).isDigit() || (str->at(i) == '.') || (str->at(i) == '-'))))
           break;
         val2 += str->at(i);
      };
      i--;


      val1 = "";

      dot = false;
      minus = false;

      for(j = pos-1; j >= 0; j--)
      {
         if((str->at(j) == '.'))
         {
            if((dot))
            {
               return(ERR_DOUBLE_DOT);
            };
            dot = true;
         };

         if((str->at(j) == '-'))
         {
            if((minus))
            {
               break;
            };
            minus = true;
         };

         if(!((str->at(j).isDigit() || (str->at(j) == '.') || (str->at(j) == '-'))))
           break;
         val1.prepend(str->at(j));
      };
      j++;

      if(val1.isEmpty())
        return(ERR_NO_PARAM);  // val1 = "0";

      if(val2.isEmpty())
        return(ERR_NO_PARAM);  // val2 = "0";


      while(1)
      {
         if(oper.at(0) == '*')
         {
            result = val1.toDouble(&ok) * val2.toDouble(&ok1);
            break;
         };

         if(oper.at(0) == '/')
         {
            result = val1.toDouble(&ok) / val2.toDouble(&ok1);
            break;
         };
         break;

      };
      if(!ok || !ok1)
        return(ERR_CONVERT);

      pos++;

      partmp.number(result, 'g', 3);
      str->replace(j, (i-j)+1, QString( "%1" ).arg( result, 3, 'f', 3 ));
   };


   pos = 0;
   exp.setPattern("[+-]{1,1}");

   while((pos = str->indexOf(exp, pos)) > 0)
   {
      oper = str->mid(pos, 1);

      val2 = "";
      dot = false;
      minus = false;

      for(i = pos+1; i <= str->length(); i++)
      {
         if((str->at(i) == '.'))
         {
            if((dot))
              return(ERR_DOUBLE_DOT);
            dot = true;
         };

         if((str->at(i) == '-'))
         {
            if(minus || dot)
            {
               break;
            };
            minus = true;
         };

         if(!((str->at(i).isDigit() || (str->at(i) == '.') || (str->at(i) == '-'))))
           break;
         val2 += str->at(i);
      };
      i--;

      val1 = "";

      dot = false;
      minus = false;

      for(j = pos-1; j >= 0; j--)
      {
         if((str->at(j) == '.'))
         {
            if((dot))
            {
               return(ERR_DOUBLE_DOT);
            };
            dot = true;
         };

         if((str->at(j) == '-'))
         {
            if((minus))
            {
               break;
            };
            minus = true;
         };

         if(!((str->at(j).isDigit() || (str->at(j) == '.') || (str->at(j) == '-'))))
           break;
         val1.prepend(str->at(j));
      };
      j++;


      if(val1.isEmpty())
        val1 = "0";  //return(ERR_NO_PARAM);

      if(val2.isEmpty())
        return(ERR_NO_PARAM); //val2 = "0";

      while(1)
      {
         if(oper.at(0) == '-')
         {
            result = val1.toDouble(&ok) - val2.toDouble(&ok1);
            break;
         };

         result = val1.toDouble(&ok) + val2.toDouble(&ok1);
         break;

      };
      if(!ok || !ok1)
        return(ERR_CONVERT);

      pos++;

      partmp.number(result, 'g', 3);
      str->replace(j, (i-j)+1, QString( "%1" ).arg( result, 0, 'f', 3 ));
   };

   str->remove('(');
   str->remove(')');
   return(0);

}

//**************************************************************************************************
//
//**************************************************************************************************

int MdiChild::processBrc(QString *str)
{

   QRegExp exp;
   QString par, val, partmp;
   int pos, err;


   if(str->contains(')') != str->contains('('))
     return(ERR_NO_BRAC);


   pos = 0;
   exp.setPattern("\\([-+/*.0-9A-Z]*\\b[.]*\\)");

   while((pos = str->indexOf(exp, pos)) > 0)
   {
      par = str->mid(pos, exp.matchedLength());
      partmp = par;
      pos += exp.matchedLength();

      par.remove(' ');

      err = compute(&par);
      if(err < 0)
        return(err);

      str->replace(str->indexOf(partmp, 0), partmp.length(), par);
      par.remove(' ');
      err = processBrc(str);
      if(err < 0)
        return(err);
   };

   err = compute(str);
   return(err);
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::compileMacro()
{
   QRegExp exp;
   int defBegin, defEnd, pos, i, len, error;
   QString param, text, val, paramTmp;
   QTextCursor cursor;
   QString basicCode, basicSubs;
   BasicInterpreter basicInterpreter;



   text = textEdit->toPlainText();
   pos = 0;
   exp.setCaseSensitivity(Qt::CaseInsensitive);
   //exp.setWildcard(FALSE);

   exp.setPattern("\\{BEGIN\\}");
   defBegin = exp.indexIn(text, pos);

   exp.setPattern("\\{END\\}");
   defEnd = exp.indexIn(text, defBegin);

   if((defBegin < 0) || (defEnd <  0))
   {
      QMessageBox::warning( this, tr("EdytorNc - compile macro"), tr("No constant definition .\n{BEGIN}\n...\n{END}\n No macro ?"));
      return;
   };

   //defEnd -= exp.matchedLength();

   QApplication::setOverrideCursor(Qt::BusyCursor);

   pos = defBegin + 7;

   while(pos < defEnd)
   {
      exp.setPattern("\\{\\$[A-Z0-9\\s]*\\b[=\\n\\r]");
      pos = text.indexOf(exp, pos);
      if (pos < 0) break;
      param = "";

      param = text.mid(pos, exp.matchedLength());
      pos +=  exp.matchedLength();

      param.remove(' ');
      param.remove('{');
      param.remove('=');

      val = "";

      do
      {
         val = val + text.at(pos);
         pos++;
         if(text.at(pos) == '\n' || text.at(pos) == '{')
         {
            cursor = textEdit->textCursor();
            cursor.setPosition(0);
            textEdit->setTextCursor(cursor);
            QMessageBox::warning( this, tr("EdytorNC - compile macro"), tr("Param list: no bracket \'}\' !"));
            return;
         };
      }while((text.at(pos) != '}'));

      val.remove(' ');

      i = defEnd;

      while((i = text.indexOf(param, i)) > 0)
      {
         text.replace(i, param.length(), val);
      };
   };

   text.remove(defBegin, (defEnd + 5) - defBegin);

   pos = 0;
   exp.setPattern("\\{[-+*=.()$/0-9A-Z\\s]*\\b[-+*=.()$/0-9A-Z\\s]*[}]");

   while((pos = text.indexOf(exp, 0)) > 0)
   {
      i = pos;
      param = "";
      do
      {
         param = param + text.at(pos);
         pos++;
      }while(text.at(pos) != '}');

      param.insert(param.length(), '}');


      len = param.length();
      param.remove(' ');
      if(!param.isEmpty())
      {
         paramTmp = param;
         error = processBrc(&param);
         if(error < 0)
         {
            cursor = textEdit->textCursor();
            cursor.setPosition(0);
            textEdit->setTextCursor(cursor);
            textEdit->find(paramTmp);
            macroShowError(error, paramTmp);
            return;
         };

         if(!param.isEmpty())
         {
            paramTmp = param;
            error = processBrc(&param);
            if(error < 0)
            {
               cursor = textEdit->textCursor();
               cursor.setPosition(0);
               textEdit->setTextCursor(cursor);
               textEdit->find(paramTmp);
               macroShowError(error, paramTmp);
               return;
            };
         };

         val = param;
         val.remove('{');
         val.remove('}');
         text.replace(i, len, val);

      };
   };

   defBegin = 0;
   exp.setPattern("\\{BEGIN_SUBS\\}");
   defBegin = exp.indexIn(text, defBegin);
   len = exp.matchedLength();

   exp.setPattern("\\{END_SUBS\\}");
   defEnd = exp.indexIn(text, defBegin);

   if((defBegin >= 0) && (defEnd > defBegin))
   {
      basicSubs = text.mid(defBegin + len, defEnd - (defBegin + len));
      text.remove(defBegin, (defEnd + exp.matchedLength()) - defBegin + 1);
   };

   defBegin = 0;
   do
   {
      exp.setPattern("\\{BEGIN_BASIC\\}");
      defBegin = exp.indexIn(text, defBegin);
      len = exp.matchedLength();

      exp.setPattern("\\{END_BASIC\\}");
      defEnd = exp.indexIn(text, defBegin);

      if((defBegin >= 0) && (defEnd > defBegin))
      {
         basicCode = text.mid(defBegin + len, defEnd - (defBegin + len));
         text.remove(defBegin, (defEnd + exp.matchedLength()) - defBegin + 1);
         basicCode.append(basicSubs);

         error = basicInterpreter.interpretBasic(basicCode);
         if(error > 0)
         {
            macroShowBasicError(error);
            return;
         };
         text.insert(defBegin, basicCode);

      };
   }while((defBegin >= 0) && (defEnd > defBegin));



   cleanUp(&text);
   textEdit->selectAll();
   textEdit->insertPlainText(text);
   cursor = textEdit->textCursor();
   cursor.setPosition(0);
   textEdit->setTextCursor(cursor);
   QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::macroShowError(int error, QString tx)
{
   QString errText;

   if(error < 0)
   {
      switch(error)
      {
         case ERR_NO_BRAC       : errText = tr("No ( or ) !");
                                  break;
         case ERR_NO_PARAM      : errText = tr("Function parameter not found ! \n Check +-*/.\n\"%1\"").arg(tx);
                                  break;
         case ERR_CONVERT       : errText = tr("Wrong number !");
                                  break;
         case ERR_UNKNOWN_FUNC  : errText = tr("Unknown math function !\n\"%1\"").arg(tx);
                                  break;
         case ERR_DOUBLE_DOT    : errText = tr("Decimal point or minus writed two times !\n\"%1\"").arg(tx);
                                  break;
         default                : errText = tr("Unknown error !");
      };

      QMessageBox::warning(this, tr("EdytorNc - compile macro"), errText);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::macroShowBasicError(int error)
{
   QString errText;

   switch(error)
   {
      case 0  : errText = tr("Syntax error");
                break;
      case 1  : errText = tr("Unbalanced parentheses");
                break;
      case 2  : errText = tr("No expression present");
                break;
      case 3  : errText = tr("Equals sign expected");
                break;
      case 4  : errText = tr("Not a variable");
                break;
      case 5  : errText = tr("Label table full");
                break;
      case 6  : errText = tr("Duplicate label");
                break;
      case 7  : errText = tr("Undefined label");
                break;
      case 8  : errText = tr("THEN expected");
                break;
      case 9  : errText = tr("TO expected");
                break;
      case 10 : errText = tr("Too many nested FOR loops");
                break;
      case 11 : errText = tr("NEXT without FOR");
                break;
      case 12 : errText = tr("Too many nested GOSUBs");
                break;
      case 13 : errText = tr("RETURN without GOSUBs");
                break;
      default : errText = tr("Unknown error");
   };
   QMessageBox::warning(this, tr("EdytorNc - compile basic"), errText);
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> tmpSelections;


    tmpSelections.clear();
    extraSelections.clear();
    //tmpSelections.append(extraSelections);
    tmpSelections.append(findTextExtraSelections);
    textEdit->setExtraSelections(tmpSelections);

    if(!textEdit->isReadOnly())
    {
       selection.format.setBackground(QColor(mdiWindowProperites.lineColor));
       selection.format.setProperty(QTextFormat::FullWidthSelection, true);
       selection.cursor = textEdit->textCursor();
       selection.cursor.clearSelection();
       extraSelections.append(selection);
    }

    QColor lineColor = QColor(mdiWindowProperites.lineColor).darker(108);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = textEdit->document();
    QTextCursor cursor = textEdit->textCursor();
    QTextCursor beforeCursor = cursor;

    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    QString brace = cursor.selectedText();

    beforeCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    QString beforeBrace = beforeCursor.selectedText();

    if((brace != "{") && (brace != "}") && (brace != "[") && (brace != "]") && (brace != "(")
        && (brace != ")") && (brace != "\"")) {
        if((beforeBrace == "{") || (beforeBrace == "}") || (beforeBrace == "[")
            || (beforeBrace == "]")
            || (beforeBrace == "(")
            || (beforeBrace == ")")
            || (beforeBrace == "\"")) {
            cursor = beforeCursor;
            brace = cursor.selectedText();
        } else
        {
           tmpSelections.append(extraSelections);
           textEdit->setExtraSelections(tmpSelections);
           return;
        }
    }

    QTextCharFormat format;
    format.setForeground(Qt::red);
    format.setFontWeight(QFont::Bold);

    QString openBrace;
    QString closeBrace;

    if((brace == "{") || (brace == "}")) {
        openBrace = "{";
        closeBrace = "}";
    }

    if((brace == "[") || (brace == "]")) {
        openBrace = "[";
        closeBrace = "]";
    }

    if((brace == "(") || (brace == ")")) {
        openBrace = "(";
        closeBrace = ")";
    }

    if((brace == "\""))
    {
       selection.cursor = cursor;
       extraSelections.append(selection);
       QTextCursor cursor1 = doc->find("\"", cursor);
       if(!cursor1.isNull() && (cursor1 != cursor))
       {
          selection.cursor = cursor1;
          extraSelections.append(selection);
       }
       else
       {
          QTextCursor cursor2 = doc->find("\"", cursor, QTextDocument::FindBackward);
          if(!cursor2.isNull())
          {
             selection.cursor = cursor2;
             extraSelections.append(selection);
          };
       }

       tmpSelections.append(extraSelections);
       textEdit->setExtraSelections(tmpSelections);
       return;
    }

    if(brace == openBrace) {
        QTextCursor cursor1 = doc->find(closeBrace, cursor);
        QTextCursor cursor2 = doc->find(openBrace, cursor);
        if(cursor2.isNull()) {
            selection.cursor = cursor;
            extraSelections.append(selection);
            selection.cursor = cursor1;
            extraSelections.append(selection);
        } else {

            while(cursor1.position() > cursor2.position()) {
                cursor1 = doc->find(closeBrace, cursor1);
                cursor2 = doc->find(openBrace, cursor2);
                if(cursor2.isNull()) {
                    break;
                }
            }
            selection.cursor = cursor;
            extraSelections.append(selection);
            selection.cursor = cursor1;
            extraSelections.append(selection);
        }
    } else {
        if(brace == closeBrace) {
            QTextCursor cursor1 = doc->find(openBrace, cursor, QTextDocument::FindBackward);
            QTextCursor cursor2 = doc->find(closeBrace, cursor, QTextDocument::FindBackward);
            if(cursor2.isNull()) {
                selection.cursor = cursor;
                extraSelections.append(selection);
                selection.cursor = cursor1;
                extraSelections.append(selection);
            } else {
                while(cursor1.position() < cursor2.position()) {
                    cursor1 = doc->find(openBrace, cursor1, QTextDocument::FindBackward);
                    cursor2 = doc->find(closeBrace, cursor2, QTextDocument::FindBackward);
                    if(cursor2.isNull()) {
                        break;
                    }
                }
                selection.cursor = cursor;
                extraSelections.append(selection);
                selection.cursor = cursor1;
                extraSelections.append(selection);
            }
        }
    }
    tmpSelections.append(extraSelections);
    textEdit->setExtraSelections(tmpSelections);
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::highlightFindText(QString searchString, QTextDocument::FindFlags options)
 {
    QList<QTextEdit::ExtraSelection> tmpSelections;


    tmpSelections.clear();
    findTextExtraSelections.clear();
    tmpSelections.append(extraSelections);
    QColor lineColor = QColor(Qt::yellow).lighter(155);
    selection.format.setBackground(lineColor);

    QTextDocument *doc = textEdit->document();
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(0);

    do
    {
       cursor = doc->find(searchString, cursor, options);
       if(!cursor.isNull())
       {
          selection.cursor = cursor;
          findTextExtraSelections.append(selection);

       };
    } while(!cursor.isNull());

    tmpSelections.append(findTextExtraSelections);
    textEdit->setExtraSelections(tmpSelections);

 }

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doUndo()
{
   textEdit->undo();
   textEdit->ensureCursorVisible();
   highlightCurrentLine();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doRedo()
{
   textEdit->redo();
   textEdit->ensureCursorVisible();
   highlightCurrentLine();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::detectHighligthMode()
{
   QString text;

   if(!mdiWindowProperites.syntaxH)
      return;

   if(highlighter == NULL)
     return;

   if(mdiWindowProperites.hColors.highlightMode == MODE_AUTO)
   { 
      text = textEdit->toPlainText();
      mdiWindowProperites.hColors.highlightMode = autoDetectHighligthMode(text);
   };

   highlighter->setHColors(mdiWindowProperites.hColors, QFont(mdiWindowProperites.fontName, mdiWindowProperites.fontSize, QFont::Normal));
   highlighter->rehighlight();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::setHighligthMode(int mod)
{
   mdiWindowProperites.hColors.highlightMode = mod;
   detectHighligthMode();
}

//**************************************************************************************************
//
//**************************************************************************************************

int MdiChild::getHighligthMode()
{
   return mdiWindowProperites.hColors.highlightMode;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doDiff()
{

//   setUpdatesEnabled(false);
//
////   if(diffSplitter > 0)
////      return;
////
////   diffSplitter = new QSplitter(Qt::Horizontal, this);
////   diffSplitter->setBackgroundRole(QPalette::Base);
//
//   //splitterV->setEnabled(true);
//
//   diffApp = new KDiff3App(splitterV, "DiffApp");
//
//   diffApp->completeInit(curFile, QFileInfo(curFile).canonicalPath());
//
////   QList<int> list;
////   list << 200 << 200;
////   splitterV->setSizes(list);
////   splitterV->adjustSize();
//
//   //diffSplitter->addWidget(splitter);
//
//   //diffSplitter->show();
//
//
//   //diffApp->resize(width()/2, height());
//   //diffApp->show();
//   //diffApp->adjustSize();
//   //diffApp->resize(800,600);
//   //splitter->adjustSize();
//
//   setUpdatesEnabled(true);
}

//**************************************************************************************************
//
//**************************************************************************************************

QString MdiChild::getCurrentFileInfo()
{
   return curFileInfo;
}

//**************************************************************************************************
//
//**************************************************************************************************

//void MdiChild::getHighligthMode()
//{
//
//}

//**************************************************************************************************
//
//**************************************************************************************************



//**************************************************************************************************
//
//**************************************************************************************************

