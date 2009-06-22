/***************************************************************************
 *   Copyright (C) 2006-2009 by Artur Kozioł                               *
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
    highlighter = 0;
    textEdit->setAcceptRichText(FALSE);
    setFocusProxy(textEdit);

    marginWidget->setAutoFillBackground(TRUE);
    marginWidget->setBackgroundRole(QPalette::Base);
    textEdit->installEventFilter(this);
    setWindowIcon(QIcon(":/images/ncfile.png"));
}

//**************************************************************************************************
//
//**************************************************************************************************

MdiChild::~MdiChild()
{
    if(highlighter > 0)
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
                            tr("Cannot read file %1:\n%2.")
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
    setFocus();
    if(isUntitled) 
    {
       return saveAs();
    } 
    else 
    {
       return saveFile(curFile);
    }
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::saveAs()
{

#ifdef Q_OS_LINUX
    QString filters = tr("CNC programs files *.nc (*.nc);;"
                         "CNC programs files *.nc *.min *.anc *.cnc (*.nc *.min *.anc *.cnc);;"
                         "Text files *.txt (*.txt);; All files (*.* *)");
#endif

#ifdef Q_OS_WIN32
    QString filters = tr("CNC programs files (*.nc);;"
                         "CNC programs files (*.nc *.min *.anc *.cnc);;"
                         "Text files (*.txt);; All files (*.* *)");
#endif

    QString file = QFileDialog::getSaveFileName(
                         this,
                         tr("Save file as..."),
                         curFile,
                         filters, &saveFileFilter, QFileDialog::DontConfirmOverwrite);

       
    if((QFile(file).exists()))
    {

       QMessageBox msgBox;
       msgBox.setText(tr("<b>File \"%1\" exists.</b>").arg(curFile));
       msgBox.setInformativeText("Do you want overwrite it ?");
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
          switch( QMessageBox::warning(this, tr("Warning"), tr("File : %1 exists. Overwrite ?").arg(fileName),
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
                            tr("Cannot write file %1:\n%2.")
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
   
 
    setWindowTitle(QString("%2 ----> %1 [*]").arg(curFile).arg(f_tx.simplified()));

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
   mdiWindowProperites = opt;
   textEdit->setReadOnly(mdiWindowProperites.readOnly);
   setFont(QFont(mdiWindowProperites.fontName, mdiWindowProperites.fontSize, QFont::Normal));
   connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
   if(mdiWindowProperites.syntaxH)
   {
      if(highlighter <= 0)
        highlighter = new Highlighter(textEdit->document());
      if(highlighter > 0)
      {
         highlighter->setHColors(mdiWindowProperites.hColors, QFont(mdiWindowProperites.fontName, mdiWindowProperites.fontSize, QFont::Normal));
         highlighter->rehighlight();
      };
   }
   else
   {
      if(highlighter > 0)
        delete(highlighter);
      highlighter = 0;
   };
    
   QTextCursor cursor = textEdit->textCursor();
   cursor.setPosition(mdiWindowProperites.cursorPos);
   textEdit->setTextCursor(cursor);
   textEdit->ensureCursorVisible();

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
             QTextCursor cr = textEdit->textCursor(); //Underline changes
             QTextCharFormat format = cr.charFormat();

             if((k->text()[0].isPrint()) && !(k->text()[0].isSpace()))
             {
                format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
                format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
                cr.mergeCharFormat(format);
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
      // pass the event on to the parent class
      return textEdit->eventFilter(obj, ev);
   };
}

//**************************************************************************************************
//
//**************************************************************************************************

int MdiChild::doRenumber(int &mode, int &startAt, int &from, int &prec, int &inc, bool &renumEmpty, bool &renumComm)
{
   int pos, i, num, it, count;
   QString tx, f_tx, line, i_tx, new_tx;
   QRegExp exp;
   bool ok, selection;
   QTextCursor cursor;

   exp.setCaseSensitivity (Qt::CaseInsensitive);

   cursor = textEdit->textCursor();

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

      if(mode == 1) //renumber all
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

            if((!f_tx.contains(' ')) && (!f_tx.contains('\n')))
              i = exp.matchedLength();
            else
              i = exp.matchedLength() - 1;

            if((!(f_tx.contains('(')) && (!f_tx.contains('\'')) && (!f_tx.contains(';'))))
            {
                f_tx.remove(0, 1);
                f_tx.remove(' ');
                if(!f_tx.isEmpty())
                  it = f_tx.toInt(&ok);
                else
                  it = 0;
                if((it >= from))
                {
                   f_tx = QString("N%1").arg(num, prec);
                   f_tx.replace(' ', '0');
                   tx.replace(pos, i, f_tx);
                   num += inc;
                   count++;
                };
            };
            pos += exp.matchedLength();
         };
         break;
      };

      if(mode == 2)
      {
         num = startAt;
         for(i = 0; i < (textEdit->document()->lineCount() - 1); i++)
         {
            line = tx.section('\n', i, i);
            exp.setPattern("[N]{1,1}[0-9]+|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
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
                  line.replace(0, 1, f_tx);
                  break;
               };

               if(((line.at(0) != '%') && (line.at(0) != ':') && (line.at(0) != 'O') && (line.at(0) != '$')))
               {
                  f_tx = QString("N%1").arg(num, prec);
                  f_tx.replace(' ', '0');
                  num += inc;
                  count++;
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
   return count;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doRemoveSpace()
{
   int i;
   QString tx;

   tx = textEdit->toPlainText();

   for(i = 0; i <              tx.length(); i++)
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
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doInsertSpace()
{
   int pos;
   QString tx;
   QRegExp exp;

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
            if((tx.at(pos-1) != ' ') && (tx.at(pos-1) != '\n'))
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
            if((tx.at(pos-1) != ' ') && (tx.at(pos-1) != '\n'))
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

         if((tx.at(pos-1) != ' ') && (tx.at(pos-1) != '\n'))
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

 void MdiChild::highlightCurrentLine()
 {
     QList<QTextEdit::ExtraSelection> extraSelections;

     if (!textEdit->isReadOnly())
     {
         QTextEdit::ExtraSelection selection;

         selection.format.setBackground(QColor(mdiWindowProperites.lineColor));
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);
         selection.cursor = textEdit->textCursor();
         selection.cursor.clearSelection();
         extraSelections.append(selection);
     }

     textEdit->setExtraSelections(extraSelections);
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

  emit message( QString(tr("Converted : %1 numbers.")).arg(count), 6000 );
  cleanUp(&tx);
  textEdit->selectAll();
  textEdit->insertPlainText(tx);
  QTextCursor cursor = textEdit->textCursor();
  cursor.setPosition(0);
  textEdit->setTextCursor(cursor);
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::resizeEvent(QResizeEvent *event)
{
   Q_UNUSED(event);
   textEdit->ensureCursorVisible();
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
      str->replace(j, pos - j, QString( "%1" ).arg( result, 0, 'f', 3 ));

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
   QString param, text, val;
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
      QMessageBox::warning( this, tr("EdytorNc - compile macro"), tr("No constant definition .\n{BEGIN}\n...\n{END}"));
      return;
   };

   //defEnd -= exp.matchedLength();


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
         error = processBrc(&param);
         if(error < 0)
         {
            macroShowError(error);
            return;
         };

         error = compute(&param);
         if(error < 0)
         {
            macroShowError(error);
            return;
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
      text.remove(defBegin, (defEnd + exp.matchedLength()) - defBegin);
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
         text.remove(defBegin, (defEnd + exp.matchedLength()) - defBegin);
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
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::macroShowError(int error)
{
   QString errText;

   if(error < 0)
   {
      switch(error)
      {
         case ERR_NO_BRAC       : errText = tr("No ( or ) !");
                                  break;
         case ERR_NO_PARAM      : errText = tr("Function parameter not found ! \n Check +-*/.");
                                  break;
         case ERR_CONVERT       : errText = tr("Wrong number !");
                                  break;
         case ERR_UNKNOWN_FUNC  : errText = tr("Unknown math function !");
                                  break;
         case ERR_DOUBLE_DOT    : errText = tr("Decimal point or minus writed two times !");
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


//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************


//**************************************************************************************************
//
//**************************************************************************************************

