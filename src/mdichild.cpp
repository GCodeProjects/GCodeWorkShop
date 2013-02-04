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


#include "mdichild.h"
#include "edytornc.h"




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
    curFile = tr("program%1.nc").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    curFileInfo = curFile;

    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

//**************************************************************************************************
//  Load template file
//**************************************************************************************************

void MdiChild::newFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("EdytorNC"),
                             tr("Cannot read file \"%1\".\n %2")
                             .arg(fileName)
                             .arg(file.errorString()));
    }
    else
    {
        QTextStream in(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString tex = in.readAll();
        textEdit->setPlainText(tex);
        file.close();
        QApplication::restoreOverrideCursor();
    };

    newFile();
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
   QString fileName, filters, saveExt;



#ifdef Q_OS_LINUX

   QString extText = tr("CNC programs files %1 (%1);;");

#endif

#ifdef Q_OS_WIN32

   QString extText = tr("CNC programs files (%1);;");

#endif

#ifdef Q_OS_MACX

   QString extText = tr("CNC programs files %1 (%1);;");

#endif


   filters = extText.arg(mdiWindowProperites.saveExtension);

   foreach(const QString ext, mdiWindowProperites.extensions)
   {
      saveExt = extText.arg(ext);
      if(ext != mdiWindowProperites.saveExtension)
        filters.append(saveExt);
   };

   filters.append(tr("Text files (*.txt);;"
                     "All files (*.* *)"));


   if(isUntitled)
      fileName = guessFileName();
   else
      fileName = curFile;

   QString file = QFileDialog::getSaveFileName(
            this,
            tr("Save file as..."),
            fileName,
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

bool MdiChild::saveFile(const QString &fileName)
{
   int curPos;
   QRegExp exp;
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
      cursor.beginEditBlock();
      cursor.removeSelectedText();
      cursor.insertText(dat.toString("dd.MM.yyyy"));
      cursor.endEditBlock();

      textEdit->setUpdatesEnabled(TRUE);
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
         textEdit->setUpdatesEnabled(FALSE);
         cursor.beginEditBlock();
         cursor.removeSelectedText();
         cursor.insertText(dat.toString("dd.MM.yyyy"));
         cursor.endEditBlock();

         textEdit->setUpdatesEnabled(TRUE);
         textEdit->repaint();
      }
   };

   QTextStream out(&file);

   QString tex = textEdit->toPlainText();
   if(!tex.contains(QLatin1String("\r\n")))
      tex.replace(QLatin1String("\n"), QLatin1String("\r\n"));
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
      if(!(f_tx.mid(0, 2) == QLatin1String(";$")))
      {
         f_tx.remove(QLatin1Char('('));
         f_tx.remove(QLatin1Char(')'));
         f_tx.remove(QLatin1Char(';'));
         break;
      };
      pos += exp.matchedLength();
      pos = text.indexOf(exp, pos);
   };

   if(f_tx.isEmpty())
      curFileInfo = QFileInfo(curFile).fileName();
   else
      curFileInfo = f_tx.simplified();

   updateWindowTitle();
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::updateWindowTitle()
{
   QString title = "";

   if((mdiWindowProperites.windowMode & SHOW_PROGTITLE))
      title = curFileInfo;

   if(!title.isEmpty() && ((mdiWindowProperites.windowMode & SHOW_FILEPATH) || (mdiWindowProperites.windowMode & SHOW_FILENAME)))
      title += " ---> ";

   if((mdiWindowProperites.windowMode & SHOW_FILEPATH))
      title += (QFileInfo(curFile).canonicalPath().isEmpty() ? "" : (QDir::toNativeSeparators(QFileInfo(curFile).canonicalPath() + "/")));

   if((mdiWindowProperites.windowMode & SHOW_FILENAME))
      title += QFileInfo(curFile).fileName();

   if(title.isEmpty())
      title += QFileInfo(curFile).fileName();

   title += "[*]";
   setWindowTitle(title);
}

//**************************************************************************************************
//
//**************************************************************************************************

//QString MdiChild::strippedName(const QString &fullFileName)
//{
//   return QFileInfo(fullFileName).fileName();
//}

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

QString MdiChild::fileName()
{
   return QFileInfo(curFile).fileName();
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
   updateWindowTitle();
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
   int pos, count, lineCount, matchedLength;
   long int i, num, it;
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
         lineCount = textEdit->document()->lineCount() - 1;
         for(i = 0; i < lineCount; i++)
         {
            line = tx.section(QLatin1Char('\n'), i, i);

            i_tx = QString("%1").arg(num, prec);
            i_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
            i_tx += "  ";

            exp.setPattern("^[0-9]{1,9}\\s\\s");
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
            matchedLength = exp.matchedLength();
            f_tx = tx.mid(pos, matchedLength);

            //qDebug() << f_tx;

            if(((f_tx.contains(QLatin1Char('(')) == 0) && (f_tx.contains(QLatin1Char('\'')) == 0) && (f_tx.contains(QLatin1Char(';')) == 0)))
            {
               tx.remove(pos, matchedLength);
               num++;
            }
            else
               pos += matchedLength;
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
            matchedLength = exp.matchedLength();
            f_tx = tx.mid(pos, matchedLength);

            //qDebug() << f_tx;

            if(pos > 0)
               if(tx[pos - 1].isLetterOrNumber())
               {
                  pos += matchedLength;
                  continue;
               };

            insertSpace = true;
            if(f_tx.endsWith(QLatin1Char(' ')))
               insertSpace = false;

            if((!f_tx.contains(QLatin1Char(' '))) && (!f_tx.contains(QLatin1Char('\n'))))
            {
               i = matchedLength;
            }
            else
            {
               i = matchedLength - 1;
            };

            if((!(f_tx.contains(QLatin1Char('('))) && (!f_tx.contains(QLatin1Char('\''))) && (!f_tx.contains(QLatin1Char(';')))))
            {
               f_tx.remove(0, 1);
               f_tx.remove(QLatin1Char(' '));
               if(!f_tx.isEmpty())
                  it = f_tx.toInt(&ok);
               else
                  it = 0;
               if(((it >= from) || (renumMarked && it == 0)) && (it < to))
               {
                  f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                  f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                  if(insertSpace)
                     f_tx.append(QLatin1String(" "));
                  tx.replace(pos, i, f_tx);
                  num += inc;
                  count++;
               };
            };
            pos += matchedLength;
         };
         break;
      };

      if(mode == 2) //renumber all
      {
         num = startAt;
         exp.setPattern("[N]{1,1}[0-9]+[\\s]{0,}|\\([^\\n\\r]*\\)|\'[^\\n\\r]*\'|;[^\\n\\r]*");
         lineCount = textEdit->document()->lineCount();
         for(i = 0; i < lineCount; i++)
         {
            line = tx.section(QLatin1Char('\n'), i, i);

            //qDebug() << line;

            pos = 0;
            while(1)
            {
               if(line.isEmpty())
               {
                  if(!renumEmpty)
                     break;
                  f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                  f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                  num += inc;
                  count++;
                  line.insert(0, f_tx);
                  break;
               };

               if(((pos = line.indexOf(exp, pos)) >= 0) && (line.at(0) != QLatin1Char('$')))
               {
                  i_tx = line.mid(pos, exp.matchedLength());

                  if((!(i_tx.contains(QLatin1Char('('))) && !(i_tx.contains(QLatin1Char('\''))) && (!i_tx.contains(QLatin1Char(';')))))
                  {
                     f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                     f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                     num += inc;
                     count++;
                     f_tx.append(QLatin1String(" "));
                     line.replace(i_tx, f_tx);
                     break;
                  }
                  else
                     if(renumComm)
                        break;
               }

               if((line.at(0) == QLatin1Char('N')) && (!line.at(1).isLetter()))
               {
                  f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                  f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                  num += inc;
                  count++;
                  f_tx.append(QLatin1String(" "));
                  line.replace(0, 1, f_tx);
                  break;
               };

               if(((line.at(0) != QLatin1Char('%')) && (line.at(0) != QLatin1Char(':')) && (line.at(0) != QLatin1Char('O')) && (line.at(0) != QLatin1Char('$'))))
               {
                  f_tx = QString(QLatin1String("N%1")).arg(num, prec);
                  f_tx.replace(QLatin1Char(' '), QLatin1Char('0'));
                  num += inc;
                  count++;
                  f_tx.append(QLatin1String(" "));
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
      if(tx.at(i) == QLatin1Char('('))
         do
      {
         i++;
         if(i > tx.length())
            break;
      }while(!((tx.at(i) == QLatin1Char(')')) || (tx.at(i) == QLatin1Char('\n'))));

      if(tx.at(i) == QLatin1Char('\''))
         do
      {
         i++;
         if(i > tx.length())
            break;
      }while(!((tx.at(i) == QLatin1Char('\'')) || (tx.at(i) == QLatin1Char('\n'))));

      if(tx.at(i) == QLatin1Char(';'))
         do
      {
         i++;
         if(i > tx.length())
            break;
      }while(!((tx.at(i) == QLatin1Char('\n'))));

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
// Deletes empty lines
//**************************************************************************************************

void MdiChild::doRemoveEmptyLines()
{
   int i;
   QString tx, line, newTx;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   tx = textEdit->toPlainText();

   for(i = 0; i < (textEdit->document()->lineCount() - 1); i++)
   {
      line = tx.section(QLatin1Char('\n'), i, i);
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
// Deletes text that matches the regular expression
//**************************************************************************************************

void MdiChild::doRemoveTextByRegExp(QStringList exp)
{
    QString tx;

    if(exp.isEmpty())
        return;

    QApplication::setOverrideCursor(Qt::BusyCursor);
    tx = textEdit->toPlainText();


    foreach(QString expTx, exp)
    {

        if(expTx.contains('$'))
            if(!expTx.contains("\\$"))
                expTx.replace('$', "\\n");

        tx.remove(QRegExp(expTx));

    };


    textEdit->selectAll();
    textEdit->insertPlainText(tx);
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(0);
    textEdit->setTextCursor(cursor);
    QApplication::restoreOverrideCursor();
}

//**************************************************************************************************
// Add empty line after each block
//**************************************************************************************************

void MdiChild::doInsertEmptyLines()
{
   QString tx;

   QApplication::setOverrideCursor(Qt::BusyCursor);
   tx = textEdit->toPlainText();

   if(tx.contains(QLatin1String("\r\n")))
      tx.replace(QLatin1String("\r\n"), QLatin1String("\r\n\r\n"));
   else
      tx.replace(QLatin1String("\n"), QLatin1String("\n\n"));


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

         if(tx.at(pos) == QLatin1Char('('))
         {
            if((tx.at(pos - 1) != QLatin1Char(' ')) && (tx.at(pos - 1) != QLatin1Char('\n')))
            {
               tx.insert(pos, QLatin1Char(' '));
               pos++;
            };
            do
            {
               pos++;
               if(pos > tx.length())
                  break;
            }while(!((tx.at(pos) == QLatin1Char(')')) || (tx.at(pos) == QLatin1Char('\n'))));
            break;
         };

         if(tx.at(pos) == QLatin1Char(';'))
         {
            if((tx.at(pos - 1) != QLatin1Char(' ')) && (tx.at(pos - 1) != QLatin1Char('\n')))
            {
               tx.insert(pos, QLatin1Char(' '));
               pos++;
            };
            do
            {
               pos++;
               if(pos > tx.length())
                  break;
            }while(!((tx.at(pos) == QLatin1Char('\n'))));
            break;
         };

         if(tx.at(pos) == QLatin1Char('\''))
         {
            do
            {
               pos++;
               if(pos > tx.length())
                  break;
            }while(!((tx.at(pos) == QLatin1Char('\'')) || (tx.at(pos) == QLatin1Char('\n'))));
            break;
         };

         if((tx.at(pos) == QLatin1Char('#')))
         {
            if(tx.at(pos-1).isDigit())
            {
               tx.insert(pos, QLatin1Char(' '));
               pos++;
            };
            break;
         };

         if((tx.at(pos - 1) != QLatin1Char(' ')) && (tx.at(pos - 1) != QLatin1Char('\n')))
         {
            tx.insert(pos, QLatin1Char(' '));
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

      if(((f_tx.contains(QLatin1Char('(')) == 0) && (f_tx.contains(QLatin1Char('\'')) == 0) && (f_tx.contains(QLatin1Char(';')) == 0)))
      {
         if(mdiWindowProperites.dotAfter && (f_tx.contains(QLatin1Char('.')) == 0))
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

         if((mdiWindowProperites.atEnd && (f_tx.contains(QLatin1Char('.')) == 0)))
         {
            tx.insert(pos + exp.matchedLength() - 1, QLatin1Char('.'));
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
      if((str->at(pos + exp.matchedLength() - 1) == '0') && str->at(pos-1) != QLatin1Char(';'))
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
      case MODE_OKUMA            : group = QLatin1String("OKUMA");
         break;
      case MODE_FANUC            : group = QLatin1String("FANUC");
         break;
      case MODE_SINUMERIK_840    : group = QLatin1String("SINUMERIK_840");
         break;
      case MODE_PHILIPS          :
      case MODE_SINUMERIK        : group = QLatin1String("SINUMERIK");
         break;
      case MODE_HEIDENHAIN       : group = QLatin1String("HEIDENHAIN");
         break;
      case MODE_HEIDENHAIN_ISO   : group = QLatin1String("HEIDENHAIN_ISO");
         break;
      case MODE_LINUXCNC         : group = QLatin1String("LinuxCNC");
         break;
      case MODE_TOOLTIPS         : group = QLatin1String("TOOLTIP");
         break;
      default                    : event->accept();
         return true;

      };

      key = "";

      QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

      cursor = textEdit->cursorForPosition(helpEvent->pos());
      cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 2);  //fix cursor position

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

         }while((key.length() > 0 ? ((key.at(key.length() - 1).isLetter()) || (key.at(key.length() - 1) == QLatin1Char('.'))): false) && !key.isEmpty() && !cursor.atBlockEnd());

         cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

         if(key.length() < 3)
         {
            cursor = textEdit->cursorForPosition(helpEvent->pos());
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 2);  //fix cursor position

            do
            {
               cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
               key = cursor.selectedText();

            }while(!((key.at(0) == QLatin1Char('#')) || key.at(0).isLetter()) && !key.isEmpty() && !cursor.atStart());

            cursor.clearSelection();
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            do
            {
               cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
               key = cursor.selectedText();
            }while((key.at(key.length() - 1).isDigit() || (key.at(key.length() - 1) ==  QLatin1Char('.'))) && !key.isEmpty() && !cursor.atEnd());

            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

         };

         key = cursor.selectedText();

      }
      else
      {

         if(cursor.atEnd())
            return true;

         cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);

         cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
         key = cursor.selectedText();

         if(key.at(0) != QLatin1Char('@'))
         {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            key = "";
         }
         else
           cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

         cursor.movePosition(QTextCursor::EndOfWord,  QTextCursor::KeepAnchor);

         key = key + cursor.selectedText();

      };


      key = key.simplified();


      if(key.length() == 2)
      {
         if((key.at(0) == QLatin1Char('G')) || (key.at(0) == QLatin1Char('M')))
            if(!key.at(1).isLetter())
               key.insert(1, "0");
      };

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
         key = QLatin1String("<p style='white-space:pre'>");
         if(text.length() > 128)
            key = QLatin1String("<p style='white-space:normal'>");
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

int MdiChild::compileMacro()
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
      QMessageBox::warning(this, tr("EdytorNc - compile macro"), tr("No constant definition .\n{BEGIN}\n...\n{END}\n No macro ?"));
      return -1;
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

      param = param.remove(' ');
      param = param.remove('{');
      param = param.remove('=');

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
            QApplication::restoreOverrideCursor();
            return -1;
         };
      }while((text.at(pos) != '}'));

      val = val.remove(' ');

      i = defEnd;

      while((i = text.indexOf(param, i)) > 0)
      {
         text.replace(i, param.length(), val);
      };
   };

   text.remove(defBegin, (defEnd + 5) - defBegin);

   pos = 0;
   exp.setPattern("\\{[-+*=.,()$/0-9A-Z\\s]*\\b[-+*=.,()$/0-9A-Z\\s]*[}]");

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
      param = param.simplified();
      param = param.remove(QChar(' '));
      param = param.replace(',', '.');

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
            QApplication::restoreOverrideCursor();
            return -1;
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
               QApplication::restoreOverrideCursor();
               return -1;
            };
         };

         val = param;
         val = val.remove('{');
         val = val.remove('}');
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
            QApplication::restoreOverrideCursor();
            return -1;
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
   return 1;
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
   QString openBrace;
   QString closeBrace;
   bool proceed;
   QList<QTextEdit::ExtraSelection> tmpSelections;
   QTextDocument::FindFlags findOptions;


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

   proceed = true;
   findOptions = 0;

   if((brace != QLatin1String("{")) && (brace != QLatin1String("}")) && (brace != QLatin1String("[")) && (brace != QLatin1String("]")) && (brace != QLatin1String("("))
         && (brace != QLatin1String(")")) && (brace != QLatin1String("\""))
         && (((brace != QLatin1String("<")) && (brace != QLatin1String(">")))))
   {
      if((beforeBrace == QLatin1String("{")) || (beforeBrace == QLatin1String("}")) || (beforeBrace == QLatin1String("["))
            || (beforeBrace == QLatin1String("]"))
            || (beforeBrace == QLatin1String("("))
            || (beforeBrace == QLatin1String(")"))
            || (beforeBrace == QLatin1String("\""))
            || (((beforeBrace == QLatin1String("<"))
            || (beforeBrace == QLatin1String(">")))))
      {

         cursor = beforeCursor;
         brace = cursor.selectedText();
         proceed = true;
      }
      else
      {
          proceed = false;

          if(mdiWindowProperites.hColors.highlightMode == MODE_LINUXCNC)
          {
              cursor = textEdit->textCursor();

              cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
              cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
              brace = cursor.selectedText().toUpper();

              findOptions = QTextDocument::FindWholeWords;

              if(brace.length() > 1)
              {
                  if(brace[0] == QLatin1Char('O'))
                  {
                      beforeCursor = cursor;
                      openBrace = brace;
                      closeBrace = brace;
                      proceed = true;
                  }

                  if((brace == QLatin1String("IF")) || (brace == QLatin1String("ENDIF")))
                  {
                      openBrace = QLatin1String("IF");
                      closeBrace = QLatin1String("ENDIF");
                      proceed = true;
                  }

                  if((brace == QLatin1String("SUB")) || (brace == QLatin1String("ENDSUB")))
                  {
                      openBrace = QLatin1String("SUB");
                      closeBrace = QLatin1String("ENDSUB");
                      proceed = true;
                  }

                  if(brace == QLatin1String("WHILE") || (brace == QLatin1String("ENDWHILE")))
                  {
                      openBrace = QLatin1String("WHILE");
                      closeBrace = QLatin1String("ENDWHILE");
                      proceed = true;
                  }
              }
          };


          if(mdiWindowProperites.hColors.highlightMode == MODE_SINUMERIK_840)
          {
              cursor = textEdit->textCursor();

              cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
              cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
              brace = cursor.selectedText().toUpper();

              findOptions = QTextDocument::FindWholeWords;

              if(brace.length() > 1)
              {
                  if((brace == QLatin1String("IF")) || (brace == QLatin1String("ENDIF")))
                  {
                      openBrace = QLatin1String("IF");
                      closeBrace = QLatin1String("ENDIF");
                      proceed = true;
                  }

                  if((brace == QLatin1String("REPEAT")) || (brace == QLatin1String("UNTIL")))
                  {
                      openBrace = QLatin1String("REPEAT");
                      closeBrace = QLatin1String("UNTIL");
                      proceed = true;
                  }

                  if(brace == QLatin1String("WHILE") || (brace == QLatin1String("ENDWHILE")))
                  {
                      openBrace = QLatin1String("WHILE");
                      closeBrace = QLatin1String("ENDWHILE");
                      proceed = true;
                  }
              }
          };

      }
   }

   if(!proceed)
   {
       tmpSelections.append(extraSelections);
       textEdit->setExtraSelections(tmpSelections);
       return;
   }


   QTextCharFormat format;
   format.setForeground(Qt::red);
   format.setFontWeight(QFont::Bold);


   if((brace == QLatin1String("{")) || (brace == QLatin1String("}"))) {
      openBrace = QLatin1String("{");
      closeBrace = QLatin1String("}");
   }

   if((brace == QLatin1String("[")) || (brace == QLatin1String("]"))) {
      openBrace = QLatin1String("[");
      closeBrace = QLatin1String("]");
   }

   if((brace == QLatin1String("(")) || (brace == QLatin1String(")"))) {
      openBrace = QLatin1String("(");
      closeBrace = QLatin1String(")");
   }


   if(mdiWindowProperites.hColors.highlightMode == MODE_LINUXCNC)
   {
       if((brace == QLatin1String("<")) || (brace == QLatin1String(">"))) {
          openBrace = QLatin1String("<");
          closeBrace = QLatin1String(">");
       }
   }

   if((brace == QLatin1String("\"")))
   {
      selection.cursor = cursor;
      extraSelections.append(selection);
      QTextCursor cursor1 = doc->find(QLatin1String("\""), cursor);
      if(!cursor1.isNull() && (cursor1 != cursor))
      {
         selection.cursor = cursor1;
         extraSelections.append(selection);
      }
      else
      {
         QTextCursor cursor2 = doc->find(QLatin1String("\""), cursor, QTextDocument::FindBackward);
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

   if(brace == openBrace)
   {
      QTextCursor cursor1 = doc->find(closeBrace, cursor, findOptions);
      QTextCursor cursor2 = doc->find(openBrace, cursor, findOptions);
      if(cursor2.isNull())
      {
         selection.cursor = cursor;
         extraSelections.append(selection);
         selection.cursor = cursor1;
         extraSelections.append(selection);
      }
      else
      {

         while(cursor1.position() > cursor2.position())
         {
            cursor1 = doc->find(closeBrace, cursor1, findOptions);
            cursor2 = doc->find(openBrace, cursor2, findOptions);
            if(cursor2.isNull())
            {
               break;
            }
         }
         selection.cursor = cursor;
         extraSelections.append(selection);
         selection.cursor = cursor1;
         extraSelections.append(selection);
      }
   } else {
      if(brace == closeBrace)
      {
         QTextCursor cursor1 = doc->find(openBrace, cursor, QTextDocument::FindBackward | findOptions);
         QTextCursor cursor2 = doc->find(closeBrace, cursor, QTextDocument::FindBackward| findOptions);
         if(cursor2.isNull())
         {
            selection.cursor = cursor;
            extraSelections.append(selection);
            selection.cursor = cursor1;
            extraSelections.append(selection);
         }
         else
         {
            while(cursor1.position() < cursor2.position())
            {
               cursor1 = doc->find(openBrace, cursor1, QTextDocument::FindBackward | findOptions);
               cursor2 = doc->find(closeBrace, cursor2, QTextDocument::FindBackward | findOptions);
               if(cursor2.isNull())
               {
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

void MdiChild::highlightFindText(QString searchString, QTextDocument::FindFlags options, bool ignoreComments)
{
   QList<QTextEdit::ExtraSelection> tmpSelections;
   bool inComment, isRegExp, isRegExpMinMax, ok;
   QString cur_line, exp, sval;
   int commentPos, id, cur_line_column;
   int pos;
   QRegExp regExp;
   double min, max;
   Qt::CaseSensitivity caseSensitivity;


   inComment = false;
   isRegExp = false;
   isRegExpMinMax = false;
   max = 0;
   min = 0;


   tmpSelections.clear();
   findTextExtraSelections.clear();
   tmpSelections.append(extraSelections);
   QColor lineColor = QColor(Qt::yellow).lighter(155);
   selection.format.setBackground(lineColor);

   QTextDocument *doc = textEdit->document();
   QTextCursor cursor = textEdit->textCursor();
   cursor.setPosition(0);

   exp = searchString;

   if(options & QTextDocument::FindCaseSensitively)
     caseSensitivity = Qt::CaseSensitive;
   else
     caseSensitivity = Qt::CaseInsensitive;

   if(exp.contains(QRegExp("\\$\\$")))
   {
      exp.remove("$$");
      isRegExp = true;
   }
   else
   {
      pos = 0;
      regExp.setPattern(QString("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"));
      regExp.setCaseSensitivity(caseSensitivity);
      pos = regExp.indexIn(exp, 0);
      if(pos >= 0)
      {
         isRegExp = true;
         isRegExpMinMax = true;
         cur_line = exp.mid(pos, regExp.matchedLength());
         exp.remove(pos, regExp.matchedLength());

         cur_line.remove("$");
         max = cur_line.toDouble(&ok);
         if(!ok)
            max = 0;

         pos = regExp.indexIn(exp, 0);
         if(pos > 0)
         {
            cur_line = exp.mid(pos, regExp.matchedLength());
            exp.remove(pos, regExp.matchedLength());
            cur_line.remove("$");
            min = cur_line.toDouble(&ok);
            if(!ok)
               min = 0;
         };
      };
   };

   cursor.setPosition(0);
   do
   {
      if(isRegExp)
      {
         if(exp.isEmpty())
            return;

         regExp.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(exp));
         regExp.setCaseSensitivity(caseSensitivity);

         cursor = doc->find(regExp, cursor, options);
      }
      else
         cursor = doc->find(searchString, cursor, options);

      if(!cursor.isNull())
      {
         cur_line = cursor.block().text();
         cur_line_column = cursor.columnNumber();

         if(ignoreComments)
         {
            id = getHighligthMode();
            if((id == MODE_SINUMERIK_840) || (id == MODE_HEIDENHAIN_ISO) || (id == MODE_HEIDENHAIN))
               commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
            else
            {
               if((id == MODE_AUTO) || (id == MODE_OKUMA) || (id == MODE_SINUMERIK) || (id == MODE_PHILIPS))
                  commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
               else
               {
                  commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
                  if(commentPos > cur_line_column)
                     commentPos = -1;

                  if(commentPos < 0)
                     commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
               };
            };

            if(commentPos < 0)
               commentPos = cur_line_column + 1;

            inComment = (commentPos < cur_line_column);
         }
         else
            inComment = false;

         if(!inComment)
         {
            if(isRegExpMinMax)
            {
               sval = cursor.selectedText();
               double val = QString(sval).remove(exp, caseSensitivity).toDouble(&ok);

               if((val >= min) && (val <= max))
               {
                  selection.cursor = cursor;
                  findTextExtraSelections.append(selection);
               };
            }
            else
            {
               selection.cursor = cursor;
               findTextExtraSelections.append(selection);
            };
         };

      };
   }while(!cursor.isNull());

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
      mdiWindowProperites.hColors.highlightMode = autoDetectHighligthMode(text.toUpper());
      if(mdiWindowProperites.hColors.highlightMode == MODE_AUTO)
         mdiWindowProperites.hColors.highlightMode = mdiWindowProperites.defaultHighlightMode;
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

bool MdiChild::foundTextMatched(QString pattern, QString text)
{
   bool matched, isRegExp, isRegExpMinMax, ok;
   int pos;
   QString str;
   QRegExp regExp;
   double min, max;

   matched = false;
   isRegExp = false;
   isRegExpMinMax = false;
   max = 0;
   min = 0;


   if(pattern.contains(QRegExp("\\$\\$")))
   {
      pattern.remove("$$");
      isRegExp = true;
   }
   else
   {
      pos = 0;
      regExp.setPattern(QString("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"));
      regExp.setCaseSensitivity(Qt::CaseInsensitive);
      pos = regExp.indexIn(pattern, 0);
      if(pos >= 0)
      {
         isRegExp = true;
         isRegExpMinMax = true;
         str = pattern.mid(pos, regExp.matchedLength());
         pattern.remove(pos, regExp.matchedLength());

         str.remove("$");
         max = str.toDouble(&ok);
         if(!ok)
            max = 0;

         pos = regExp.indexIn(pattern, 0);
         if(pos > 0)
         {
            str = pattern.mid(pos, regExp.matchedLength());
            pattern.remove(pos, regExp.matchedLength());
            str.remove("$");
            min = str.toDouble(&ok);
            if(!ok)
               min = 0;
         };
      };
   };

   if(isRegExp)
   {
      regExp.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(pattern));


      if(text.contains(regExp))
      {
         if(isRegExpMinMax)
         {
            double val = QString(text).remove(pattern, Qt::CaseInsensitive).toDouble(&ok);

            if(((val >= min) && (val <= max)))
            {
               matched = true;
            };
         }
         else
         {
            matched = true;
         };
      };
   }
   else
   {
      matched = (pattern == text);
   };

   return matched;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::findText(const QString &text, QTextDocument::FindFlags options, bool ignoreComments)
{
   bool found, inComment, isRegExp, isRegExpMinMax, ok;
   QTextCursor cursor;
   QString cur_line, exp, sval;
   int cur_line_column;
   int commentPos, id;
   int pos;
   QRegExp regExp;
   double min, max;
   Qt::CaseSensitivity caseSensitivity;


   inComment = false;
   found = false;
   isRegExp = false;
   isRegExpMinMax = false;
   max = 0;
   min = 0;

   if(options & QTextDocument::FindCaseSensitively)
     caseSensitivity = Qt::CaseSensitive;
   else
     caseSensitivity = Qt::CaseInsensitive;

   exp = text;

   if(exp.contains(QRegExp("\\$\\$")))
   {
      exp.remove("$$");
      isRegExp = true;
   }
   else
   {
      pos = 0;
      regExp.setPattern(QString("(\\$)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}"));
      regExp.setCaseSensitivity(caseSensitivity);

      pos = regExp.indexIn(exp, 0);
      if(pos >= 0)
      {
         isRegExp = true;
         isRegExpMinMax = true;
         cur_line = exp.mid(pos, regExp.matchedLength());
         exp.remove(pos, regExp.matchedLength());

         cur_line.remove("$");
         max = cur_line.toDouble(&ok);
         if(!ok)
            max = 0;

         pos = regExp.indexIn(exp, 0);
         if(pos > 0)
         {
            cur_line = exp.mid(pos, regExp.matchedLength());
            exp.remove(pos, regExp.matchedLength());
            cur_line.remove("$");
            min = cur_line.toDouble(&ok);
            if(!ok)
               min = 0;
         };
      };
   };

   textEdit->setUpdatesEnabled(false);

   if(exp.isEmpty())
      return false;


   cursor = textEdit->textCursor();
   do
   {
      if(isRegExp)
      {
         regExp.setPattern(QString("%1[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(exp));
         regExp.setCaseSensitivity(caseSensitivity);

         cursor = textEdit->document()->find(regExp, cursor, options);

         found = !cursor.isNull();
         if(found)
         {
            if(!isRegExpMinMax)
               textEdit->setTextCursor(cursor);
         }
         else
         {
            break;
         }
      }
      else
      {
         found = textEdit->find(exp, options);
         cursor = textEdit->textCursor();
      };

      cur_line = cursor.block().text();
      cur_line_column = cursor.columnNumber();     

      if(found && ignoreComments)
      {
         id = getHighligthMode();
         if((id == MODE_SINUMERIK_840) || (id == MODE_HEIDENHAIN_ISO) || (id == MODE_HEIDENHAIN))
            commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
         else
         {
            if((id == MODE_AUTO) || (id == MODE_OKUMA) || (id == MODE_SINUMERIK) || (id == MODE_PHILIPS))
               commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
            else
            {
               commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
               if(commentPos > cur_line_column)
                  commentPos = -1;

               if(commentPos < 0)
                  commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
            };
         };

         if(commentPos < 0)
            commentPos = cur_line_column + 1;

         inComment = (commentPos < cur_line_column);
      }
      else
         inComment = false;

      if((isRegExpMinMax && found) && !inComment)
      {
         sval = cursor.selectedText();
         double val = QString(sval).remove(exp, caseSensitivity).toDouble(&ok);

         if(((val >= min) && (val <= max)))
         {
            inComment = false;
            textEdit->setTextCursor(cursor);           
         }
         else
            inComment = true;
      };

   }while(inComment);

   textEdit->setUpdatesEnabled(true);
   return found;
}

//**************************************************************************************************
// Tries to guess the file name
//**************************************************************************************************

QString MdiChild::guessFileName()
{
    QTextCursor cursor;
    QString fileName;
    QString text;
    int pos;
    QRegExp expression;

    cursor = textEdit->textCursor();
    text = textEdit->toPlainText();

    if(mdiWindowProperites.guessFileNameByProgNum)
    {

        while(1)
        {
            expression.setPattern(FILENAME_SINU840);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(QLatin1String("%_N_"));
                fileName.remove(QRegExp("_(MPF|SPF|TEA|COM|PLC|DEF|INI)"));
                break;
            };

            expression.setPattern(FILENAME_OSP);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(QLatin1String("$"));
                fileName.remove(QRegExp(".(MIN|SSB|SDF|TOP|LIB|SUB|MSB)[%]{0,1}"));
                break;
            };

            expression.setPattern(FILENAME_SINU);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(QRegExp("%(MPF|SPF|TEA)[\\s]{0,3}"));
                break;
            };

            expression.setPattern(FILENAME_PHIL);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(QRegExp("%PM[\\s]{1,}[N]{1,1}"));
                break;
            };

            expression.setPattern(FILENAME_FANUC);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());

                if(fileName.at(0)!='O')
                    fileName[0]='O';
                if(fileName.at(0)=='O' && fileName.at(1)=='O')
                    fileName.remove(0,1);
                break;
            }

            expression.setPattern(FILENAME_HEID1);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(QLatin1String("%"));
                fileName.remove(QRegExp("\\s"));
                break;
            };

            expression.setPattern(FILENAME_HEID2);
            pos = text.indexOf(expression);
            if(pos >= 0)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(QRegExp("(BEGIN)(\\sPGM\\s)"));
                fileName.remove(QRegExp("(\\sMM|\\sINCH)"));
                break;
            };

            fileName = "";
            break;
        };

    }
    else
    {
        while(true)
        {
            expression.setPattern("(;)[\\w:*=+ -]{4,64}");
            pos = text.indexOf(expression);
            if(pos >= 2)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove(";");
                break;
            };

            expression.setPattern("(\\()[\\w:*=+ -]{4,64}(\\))");
            pos = text.indexOf(expression);
            if(pos >= 2)
            {
                fileName = text.mid(pos, expression.matchedLength());
                fileName.remove("(");
                fileName.remove(")");
                break;
            };

            fileName = "";
            break;

        };

    };


    fileName = fileName.simplified();
    //fileName.append(mdiWindowProperites.saveExtension);
    fileName.prepend(mdiWindowProperites.saveDirectory + "/");

    fileName = QDir::cleanPath(fileName);

    textEdit->setTextCursor(cursor);

    return fileName;
}

//**************************************************************************************************
// Split file
//**************************************************************************************************

QStringList MdiChild::splitFile()
{
   int progBegin, progEnd;
   QStringList progs, exp;
   QList<int> progBegins;
   int index;
   QString tx;

   QString text = textEdit->toPlainText();

   exp << FILENAME_SINU840
       << FILENAME_OSP
       << FILENAME_FANUC
       << FILENAME_SINU
       << FILENAME_HEID1
       << FILENAME_HEID2
       << FILENAME_PHIL ;

   index = 0;
   foreach(const QString expTx, exp)
   {
      QRegExp expression(expTx);
      do
      {
         index = text.indexOf(expression, index);
         if(index >= 0)
         {
            progBegins.append(index);
            index += expression.matchedLength();
         }
         else
            index = 0;

      }while(index > 0);
   };
   qSort(progBegins.begin(), progBegins.end());

   QList<int>::const_iterator it = progBegins.constBegin();
   while(it != progBegins.constEnd())
   {
      progBegin = *it;
      it++;
      if(it != progBegins.constEnd())
         progEnd = *it;
      else
         progEnd = text.size();

      tx = text.mid(progBegin, progEnd - progBegin);
      if(!tx.isEmpty())
      {
         progs.append(tx);
      };
   };

   return progs;
}

//**************************************************************************************************
//  insert/remove block skip /
//**************************************************************************************************

void MdiChild::blockSkip()
{
    int idx;

    if(textEdit->textCursor().hasSelection())
    {
        QTextCursor cursor = textEdit->textCursor();

        int start = textEdit->textCursor().selectionStart();
        int end = textEdit->textCursor().selectionEnd();
        if(start < end)  // selection always in same direction
        {
            cursor.setPosition(end, QTextCursor::MoveAnchor);
            cursor.setPosition(start, QTextCursor::KeepAnchor);
        };

        cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(cursor);
        QString selText = cursor.selectedText();

        QStringList list = selText.split(QChar::ParagraphSeparator);
        if(list.isEmpty())
            list.append(selText);

        bool remove = false;

        if(selText[0] == '/')
            remove = true;

        selText.clear();

        foreach(QString txLine, list)
        {
            if(remove)
            {
                if(txLine.length() > 0)
                {
                    idx = txLine.indexOf("/ ");
                    if(idx == 0)
                        txLine.remove(idx, 2);
                    else
                    {
                        idx = txLine.indexOf("/");
                        if(idx == 0)
                            txLine.remove(idx, 1);
                    };
                };
            }
            else
                txLine.prepend("/");

            txLine.append("\n");
            selText.append(txLine);
        };

        selText.remove(selText.length() - 1, 1);

        textEdit->insertPlainText(selText);
        textEdit->setTextCursor(cursor);
    };
}

//**************************************************************************************************
//  comments/uncomments selected text using ;
//**************************************************************************************************

void MdiChild::semicomment()
{
    int idx;

    if(textEdit->textCursor().hasSelection())
    {
        QTextCursor cursor = textEdit->textCursor();
        QString selText = cursor.selectedText();

        QStringList list = selText.split(QChar::ParagraphSeparator);
        bool remove = false;

        if(selText[0] == ';')
            remove = true;

        selText.clear();

        foreach(QString txLine, list)
        {
            if(remove)
            {
                if(txLine.length() > 0)
                {
                    idx = txLine.indexOf(";");
                    if(idx >= 0)
                        txLine.remove(idx, 1);
                };
            }
            else
                txLine.prepend(";");

            txLine.append("\n");
            selText.append(txLine);
        };

        selText.remove(selText.length() - 1, 1);

        textEdit->insertPlainText(selText);
        textEdit->setTextCursor(cursor);
    };
}

//**************************************************************************************************
//  comments/uncomments selected text using ()
//**************************************************************************************************

void MdiChild::paracomment()
{
    if(textEdit->textCursor().hasSelection())
    {
        int idx;
        QTextCursor cursor = textEdit->textCursor();
        QString selText = cursor.selectedText();

        QStringList list = selText.split(QChar::ParagraphSeparator);
        bool remove = false;

        if(selText[0] == '(')
            remove = true;

        selText.clear();

        foreach(QString txLine, list)
        {
            if(remove)
            {
                if(txLine.length() > 0)
                {
                    idx = txLine.indexOf("(");
                    if(idx >= 0)
                      txLine.remove(idx, 1);

                    idx = txLine.indexOf(")");
                    if(idx >= 0)
                      txLine.remove(idx, 1);
                };
            }
            else
            {
                txLine.prepend("(");
                txLine.append(")");
            };

            txLine.append("\n");
            selText.append(txLine);
        };

        selText.remove(selText.length() - 1, 1);

        textEdit->insertPlainText(selText);
        textEdit->setTextCursor(cursor);

    };
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::findNext(QString textToFind, QTextDocument::FindFlags options, bool ignoreComments)
{
    bool found = false;
    QTextCursor cursor, cursorOld;

    if(textToFind.isEmpty())
        return false;

    textEdit->blockSignals(true);

    found = findText(textToFind, options, ignoreComments);

    if(!found)
    {
        cursor = textEdit->textCursor();
        cursorOld = cursor;

        if(options & QTextDocument::FindBackward)
        {
            cursor.movePosition(QTextCursor::End);
        }
        else
        {
            cursor.movePosition(QTextCursor::Start);
        };

        textEdit->setTextCursor(cursor);

        found = findText(textToFind, options, ignoreComments);

        if(!found)
        {
            cursorOld.clearSelection();
            textEdit->setTextCursor(cursorOld);
        };

    };

    textEdit->blockSignals(false);
    highlightCurrentLine();

    return found;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::replaceNext(QString textToFind, QString replacedText, QTextDocument::FindFlags options, bool ignoreComments)
{
    QString foundText;
    double val, val1;
    bool ok;
    QRegExp regExp;
    QChar op;


    if(textEdit->isReadOnly())
        return false;

    if(textToFind.isEmpty())
        return false;

    bool found = false;

    textEdit->blockSignals(true);

    if(foundTextMatched(textToFind, textEdit->textCursor().selectedText()))
        found = true;
    else
        found = findNext(textToFind, options, ignoreComments);

    if(found)
    {
        QTextCursor cr = textEdit->textCursor();
        cr.beginEditBlock();
        if(mdiWindowProperites.underlineChanges)
        {
            QTextCharFormat format = cr.charFormat();
            format.setUnderlineStyle(QTextCharFormat::DotLine);
            format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
            cr.setCharFormat(format);
        };


        regExp.setPattern(QString("\\$\\$[\\/*+\\-]{1,1}[-]{0,1}[0-9.]{1,}"));
        if(replacedText.contains(regExp))
        {
            replacedText.remove("$$");
            op = replacedText.at(0);
            replacedText.remove(0, 1);
            val = replacedText.toDouble(&ok);

            foundText = cr.selectedText();
            foundText.remove(QRegExp("[A-Za-z#]{1,}"));
            val1 = foundText.toDouble(&ok);
            replacedText = cr.selectedText();
            replacedText.remove(foundText);

            if(op == '+')
                val = val1 + val;
            if(op == '-')
                val = val1 - val;
            if(op == '*')
                val = val1 * val;
            if(op == '/')
                val = val1 / val;

            if(replacedText == "#" || replacedText == "O" || replacedText == "o" || replacedText == "N" || replacedText == "n")
            {
                replacedText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', 3));
                if(replacedText[replacedText.length() - 1] == '.')
                    replacedText = replacedText.remove((replacedText.length() - 1), 1);
            }
            else
                replacedText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', 3));

        };

        cr.insertText(replacedText);
        cr.endEditBlock();
        textEdit->setTextCursor(cr);

        found = findNext(textToFind, options, ignoreComments);
    };

    textEdit->blockSignals(false);
    highlightCurrentLine();
    highlightFindText(textToFind, options, ignoreComments);

    return found;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::replaceAll(QString textToFind, QString replacedText, QTextDocument::FindFlags options, bool ignoreComments)
{
    bool found = false;

    if(textEdit->isReadOnly())
        return false;

    if(textToFind.isEmpty())
        return false;

    if(textEdit->textCursor().selectedText() == textToFind)
        found = true;
    else
        found = findNext(textToFind, options, ignoreComments);

    QTextCursor startCursor = textEdit->textCursor();

    while(found)
    {
        found = replaceNext(textToFind, replacedText, options, ignoreComments);

        if(startCursor.blockNumber() == textEdit->textCursor().blockNumber())
            break;
        qApp->processEvents();
    };

    return found;
}

//**************************************************************************************************
//
//**************************************************************************************************

bool MdiChild::swapAxes(QString textToFind, QString replacedText, double min, double max,
                        int oper, double modifier, QTextDocument::FindFlags options, bool ignoreComments)
{
    double val, val1;
    QRegExp regExp;
    bool found = false;
    bool ok, inSelection;
    QString newText, foundText;
    bool inComment;
    int commentPos, id;
    QTextDocument *document;
    int cursorStart, cursorEnd;

    cursorStart = 0;
    cursorEnd = 0;
    
    if(textEdit->isReadOnly())
        return false;
    
    if(textToFind.isEmpty())
        return false;
    
    if(options & QTextDocument::FindCaseSensitively)
        regExp.setCaseSensitivity(Qt::CaseSensitive);
    else
        regExp.setCaseSensitivity(Qt::CaseInsensitive);

    
    textEdit->blockSignals(true);
    
    inSelection = textEdit->textCursor().hasSelection();

    if(inSelection)
    {
        cursorStart = textEdit->textCursor().selectionStart();
        cursorEnd = textEdit->textCursor().selectionEnd();
        document = new QTextDocument(textEdit->textCursor().selectedText(), this);
    }
    else
        document = textEdit->document();



    QTextCursor cursor(document);
    cursor.setPosition(0);

    
    do
    {
        if(oper == -1)
        {
            regExp.setPattern(QString("(%1)(?=[-=#<.0-9]{0,1}[0-9]{0,}[.]{0,1}[0-9]{0,})(?![A-Z$ ])").arg(textToFind));
            cursor = document->find(regExp, cursor, options);
        }
        else
        {
            regExp.setPattern(QString("(%1)[-]{0,1}[0-9]{0,}[0-9.]{1,1}[0-9]{0,}").arg(textToFind));
            cursor = document->find(regExp, cursor, options);
        };

        found = !cursor.isNull();



        if(found && ignoreComments)
        {
            QString cur_line = cursor.block().text();
            int cur_line_column = cursor.columnNumber();

            id = getHighligthMode();
            if((id == MODE_SINUMERIK_840) || (id == MODE_HEIDENHAIN_ISO) || (id == MODE_HEIDENHAIN))
                commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
            else
            {
                if((id == MODE_AUTO) || (id == MODE_OKUMA) || (id == MODE_SINUMERIK) || (id == MODE_PHILIPS))
                    commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
                else
                {
                    commentPos  = cur_line.indexOf(QLatin1Char('('), 0);
                    if(commentPos > cur_line_column)
                        commentPos = -1;

                    if(commentPos < 0)
                        commentPos  = cur_line.indexOf(QLatin1Char(';'), 0);
                };
            };

            if(commentPos < 0)
                commentPos = cur_line_column + 1;

            inComment = (commentPos < cur_line_column);
        }
        else
            inComment = false;

        
        if(found && !inComment)
        {
            foundText = cursor.selectedText();
            
            if(min > -999999)
            {
                val = QString(foundText.remove(textToFind, Qt::CaseInsensitive)).toDouble(&ok);
                
                if(!ok)
                    continue;

                if(!((val >= min) && (val <= max)))
                {
                    continue;
                };
            };
            
            if(oper > -1)
            {
                foundText.remove(textToFind);  //QRegExp("[A-Za-z#]{1,}")
                val1 = foundText.toDouble(&ok);

                if((modifier == 0) && oper == 3)  //divide by 0
                    modifier = 1;

                switch(oper)
                {
                case 0 : val = val1 + modifier;
                    break;
                case 1 : val = val1 - modifier;
                    break;
                case 2 : val = val1 * modifier;
                    break;
                case 3 : val = val1 / modifier;
                    break;
                default: val = val1;
                    break;
                };

                newText = replacedText + removeZeros(QString("%1").arg(val, 0, 'f', 3));
            }
            else
            {
                newText = replacedText;
            };

            
            if(mdiWindowProperites.underlineChanges)
            {
                QTextCharFormat format = cursor.charFormat();
                format.setUnderlineStyle(QTextCharFormat::DotLine);
                format.setUnderlineColor(QColor(mdiWindowProperites.underlineColor));
                cursor.mergeCharFormat(format);
            };
            cursor.insertText(newText);
        };
        
    }while(found);
    

    if(inSelection)
    {
        //textEdit->insertPlainText(document->toPlainText());
        cursor = QTextCursor(document);
        cursor.select(QTextCursor::Document);
        textEdit->textCursor().insertFragment(cursor.selection());
        delete(document);

        if(cursorStart < cursorEnd)
            cursorStart = cursorEnd;

        cursorEnd = cursorStart + cursor.selectedText().length();

        cursor = textEdit->textCursor();  //restore selection
        cursor.setPosition(cursorStart, QTextCursor::MoveAnchor);
        cursor.setPosition(cursorEnd, QTextCursor::KeepAnchor);
        textEdit->setTextCursor(cursor);
    };


    textEdit->blockSignals(false);

    return found;
}

//**************************************************************************************************
//
//**************************************************************************************************

void MdiChild::doSwapAxes(QString textToFind, QString replacedText, double min, double max,
                          int oper, double modifier, QTextDocument::FindFlags options, bool ignoreComments)
{
    
    if(textEdit->isReadOnly())
        return;
    
    if(textToFind.isEmpty())
        return;
    
    QTextCursor startCursor = textEdit->textCursor();
    startCursor.beginEditBlock();
    
    if(textToFind != replacedText)
    {
        swapAxes(replacedText, QString(":%1:").arg(replacedText), min, max, -1, modifier, options, ignoreComments);
        swapAxes(textToFind, replacedText, min, max, oper, modifier, options, ignoreComments);
        swapAxes(QString(":%1:").arg(replacedText), textToFind, -999999, -999999, -1, 0, options, ignoreComments);
    }
    else
        swapAxes(textToFind, replacedText, min, max, oper, modifier, options, ignoreComments);
    
    startCursor.movePosition(QTextCursor::StartOfLine);
    startCursor.endEditBlock();
    textEdit->setTextCursor(startCursor);
    
    
}

//**************************************************************************************************
//
//**************************************************************************************************




//**************************************************************************************************
//
//**************************************************************************************************

