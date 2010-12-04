/*
 *   kdiff3 - Text Diff And Merge Tool
 *   Copyright (C) 2002-2009  Joachim Eibl, joachim.eibl at gmx.de
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit> 
#include <QToolTip>
#include <QRadioButton>
#include <QGroupBox>
#include <QTextCodec>

#include <QSettings>
#include <QLocale>
#include <QGridLayout>
#include <QPixmap>
#include <QFrame>
#include <QVBoxLayout>

#include <map>

#include "optiondialog.h"
#include "diff.h"


#include <iostream>



#define KDIFF3_CONFIG_GROUP "KDiff3 Options"

static QString s_historyEntryStartRegExpToolTip;
static QString s_historyEntryStartSortKeyOrderToolTip;
static QString s_autoMergeRegExpToolTip;
static QString s_historyStartRegExpToolTip;

void OptionDialog::addOptionItem(OptionItem* p)
{
   m_optionItemList.push_back(p);
}

class OptionItem
{
public:
   OptionItem( OptionDialog* pOptionDialog, const QString& saveName )
   {
      assert(pOptionDialog!=0);
      pOptionDialog->addOptionItem( this );
      m_saveName = saveName;
   }
   virtual ~OptionItem(){}
   virtual void setToDefault()=0;
   virtual void setToCurrent()=0;
   virtual void apply()=0;
   virtual void write(ValueMap*)=0;
   virtual void read(ValueMap*)=0;
   QString getSaveName(){return m_saveName;}
protected:
   QString m_saveName;
};

class OptionCheckBox : public QCheckBox, public OptionItem
{
public:
   OptionCheckBox( QString text, bool bDefaultVal, const QString& saveName, bool* pbVar,
                   QWidget* pParent, OptionDialog* pOD )
   : QCheckBox( text, pParent ), OptionItem( pOD, saveName )
   {
      m_pbVar = pbVar;
      m_bDefaultVal = bDefaultVal;
   }
   void setToDefault(){ setChecked( m_bDefaultVal );      }
   void setToCurrent(){ setChecked( *m_pbVar );           }
   void apply()       { *m_pbVar = isChecked();                              }
   void write(ValueMap* config){ config->writeEntry(m_saveName, *m_pbVar );   }
   void read (ValueMap* config){ *m_pbVar = config->readBoolEntry( m_saveName, *m_pbVar ); }
private:
   OptionCheckBox( const OptionCheckBox& ); // private copy constructor without implementation
   bool* m_pbVar;
   bool m_bDefaultVal;
};

class OptionRadioButton : public QRadioButton, public OptionItem
{
public:
   OptionRadioButton( QString text, bool bDefaultVal, const QString& saveName, bool* pbVar,
                   QWidget* pParent, OptionDialog* pOD )
   : QRadioButton( text, pParent ), OptionItem( pOD, saveName )
   {
      m_pbVar = pbVar;
      m_bDefaultVal = bDefaultVal;
   }
   void setToDefault(){ setChecked( m_bDefaultVal );      }
   void setToCurrent(){ setChecked( *m_pbVar );           }
   void apply()       { *m_pbVar = isChecked();                              }
   void write(ValueMap* config){ config->writeEntry(m_saveName, *m_pbVar );   }
   void read (ValueMap* config){ *m_pbVar = config->readBoolEntry( m_saveName, *m_pbVar ); }
private:
   OptionRadioButton( const OptionRadioButton& ); // private copy constructor without implementation
   bool* m_pbVar;
   bool m_bDefaultVal;
};


template<class T>
class OptionT : public OptionItem
{
public:
   OptionT( const T& defaultVal, const QString& saveName, T* pVar, OptionDialog* pOD )
   : OptionItem( pOD, saveName )
   {
      m_pVar = pVar;
      *m_pVar = defaultVal;
   }
   OptionT( const QString& saveName, T* pVar, OptionDialog* pOD )
   : OptionItem( pOD, saveName )
   {
      m_pVar = pVar;
   }
   void setToDefault(){}
   void setToCurrent(){}
   void apply()       {}
   void write(ValueMap* vm){ writeEntry( vm, m_saveName, *m_pVar ); }
   void read (ValueMap* vm){ *m_pVar = vm->readEntry ( m_saveName, *m_pVar ); }
private:
   OptionT( const OptionT& ); // private copy constructor without implementation
   T* m_pVar;
};

template <class T> void writeEntry(ValueMap* vm, const QString& saveName, const T& v ) {   vm->writeEntry( saveName, v ); }


typedef OptionT<bool> OptionToggleAction;
typedef OptionT<int>  OptionNum;
typedef OptionT<QPoint> OptionPoint;
typedef OptionT<QSize> OptionSize;
typedef OptionT<QStringList> OptionStringList;

class OptionFontChooser : public QFontDialog, public OptionItem
{
public:
   OptionFontChooser( const QFont& defaultVal, const QString& saveName, QFont* pbVar, QWidget* pParent, OptionDialog* pOD ) :
       QFontDialog( pParent ),
       OptionItem( pOD, saveName )
   {
      m_pbVar = pbVar;
      *m_pbVar = defaultVal;
      m_default = defaultVal;
   }
   void setToDefault(){ setFont(m_default); }
   void setToCurrent(){ setFont(*m_pbVar); }
   void apply()       { *m_pbVar = font();}
   void write(ValueMap* config){ config->writeEntry(m_saveName, *m_pbVar );   }
   void read (ValueMap* config){ *m_pbVar = config->readFontEntry( m_saveName, m_pbVar ); }
private:
   OptionFontChooser( const OptionToggleAction& ); // private copy constructor without implementation
   QFont* m_pbVar;
   QFont m_default;
};

class OptionColorButton : public QColorDialog, public OptionItem
{
public:
   OptionColorButton( QColor defaultVal, const QString& saveName, QColor* pVar, QWidget* pParent, OptionDialog* pOD )
   : QColorDialog( defaultVal, pParent ), OptionItem( pOD, saveName )
   {
      m_pVar = pVar;
      m_defaultVal = defaultVal;
   }
   //void setToDefault(){ setColor( m_defaultVal );      }
   //void setToCurrent(){ setColor( *m_pVar );           }
   void apply()       { *m_pVar = selectedColor();                              }
   void write(ValueMap* config){ config->writeEntry(m_saveName, *m_pVar );   }
   void read (ValueMap* config){ *m_pVar = config->readColorEntry( m_saveName, m_pVar ); }
private:
   OptionColorButton( const OptionColorButton& ); // private copy constructor without implementation
   QColor* m_pVar;
   QColor m_defaultVal;
};

class OptionLineEdit : public QComboBox, public OptionItem
{
public:
   OptionLineEdit( const QString& defaultVal, const QString& saveName, QString* pVar,
                   QWidget* pParent, OptionDialog* pOD )
   : QComboBox( pParent ), OptionItem( pOD, saveName )
   {
      setMinimumWidth(50);
      setEditable(true);
      m_pVar = pVar;
      m_defaultVal = defaultVal;
      m_list.push_back(defaultVal);
      insertText();
   }
   void setToDefault(){ setEditText( m_defaultVal );   }
   void setToCurrent(){ setEditText( *m_pVar );        }
   void apply()       { *m_pVar = currentText(); insertText();            }
   void write(ValueMap* config){ config->writeEntry( m_saveName, m_list, '|' );      }
   void read (ValueMap* config){ 
      m_list = config->readListEntry( m_saveName, QStringList(m_defaultVal), '|' ); 
      if ( !m_list.empty() ) *m_pVar = m_list.front();
      clear();
      insertItems(0,m_list);
   }
private:
   void insertText()
   {  // Check if the text exists. If yes remove it and push it in as first element
      QString current = currentText();
      m_list.removeAll( current );
      m_list.push_front( current );
      clear();
      if ( m_list.size()>10 ) 
         m_list.erase( m_list.begin()+10, m_list.end() );
      insertItems(0,m_list);
   }
   OptionLineEdit( const OptionLineEdit& ); // private copy constructor without implementation
   QString* m_pVar;
   QString m_defaultVal;
   QStringList m_list;
};

#if defined QT_NO_VALIDATOR
#error No validator
#endif
class OptionIntEdit : public QLineEdit, public OptionItem
{
public:
   OptionIntEdit( int defaultVal, const QString& saveName, int* pVar, int rangeMin, int rangeMax,
                   QWidget* pParent, OptionDialog* pOD )
   : QLineEdit( pParent ), OptionItem( pOD, saveName )
   {
      m_pVar = pVar;
      m_defaultVal = defaultVal;
      QIntValidator* v = new QIntValidator(this);
      v->setRange( rangeMin, rangeMax );
      setValidator( v );
   }
   void setToDefault(){ QString s;  s.setNum(m_defaultVal); setText( s );  }
   void setToCurrent(){ QString s;  s.setNum(*m_pVar);      setText( s );  }
   void apply()       { const QIntValidator* v=static_cast<const QIntValidator*>(validator());
                        *m_pVar = minMaxLimiter( text().toInt(), v->bottom(), v->top());
                        setText( QString::number(*m_pVar) );  }
   void write(ValueMap* config){ config->writeEntry(m_saveName, *m_pVar );   }
   void read (ValueMap* config){ *m_pVar = config->readNumEntry( m_saveName, *m_pVar ); }
private:
   OptionIntEdit( const OptionIntEdit& ); // private copy constructor without implementation
   int* m_pVar;
   int m_defaultVal;
};

class OptionComboBox : public QComboBox, public OptionItem
{
public:
   OptionComboBox( int defaultVal, const QString& saveName, int* pVarNum,
                   QWidget* pParent, OptionDialog* pOD )
   : QComboBox( pParent ), OptionItem( pOD, saveName )
   {
      setMinimumWidth(50);
      m_pVarNum = pVarNum;
      m_pVarStr = 0;
      m_defaultVal = defaultVal;
      setEditable(false);
   }
   OptionComboBox( int defaultVal, const QString& saveName, QString* pVarStr,
                   QWidget* pParent, OptionDialog* pOD )
   : QComboBox( pParent ), OptionItem( pOD, saveName )
   {
      m_pVarNum = 0;
      m_pVarStr = pVarStr;
      m_defaultVal = defaultVal;
      setEditable(false);
   }
   void setToDefault()
   { 
      setCurrentIndex( m_defaultVal ); 
      if (m_pVarStr!=0){ *m_pVarStr=currentText(); } 
   }
   void setToCurrent()
   { 
      if (m_pVarNum!=0) setCurrentIndex( *m_pVarNum );
      else              setText( *m_pVarStr );
   }
   void apply()
   { 
      if (m_pVarNum!=0){ *m_pVarNum = currentIndex(); }
      else             { *m_pVarStr = currentText(); }
   }
   void write(ValueMap* config)
   { 
      if (m_pVarStr!=0) config->writeEntry(m_saveName, *m_pVarStr );
      else              config->writeEntry(m_saveName, *m_pVarNum );   
   }
   void read (ValueMap* config)
   {
      if (m_pVarStr!=0)  setText( config->readEntry( m_saveName, currentText() ) );
      else               *m_pVarNum = config->readNumEntry( m_saveName, *m_pVarNum ); 
   }
private:
   OptionComboBox( const OptionIntEdit& ); // private copy constructor without implementation
   int* m_pVarNum;
   QString* m_pVarStr;
   int m_defaultVal;
   
   void setText(const QString& s)
   {
      // Find the string in the combobox-list, don't change the value if nothing fits.
      for( int i=0; i<count(); ++i )
      {
         if ( itemText(i)==s )
         {
            if (m_pVarNum!=0) *m_pVarNum = i;
            if (m_pVarStr!=0) *m_pVarStr = s;
            setCurrentIndex(i);
            return;
         }
      }
   }
};

class OptionEncodingComboBox : public QComboBox, public OptionItem
{
   std::vector<QTextCodec*> m_codecVec;
   QTextCodec** m_ppVarCodec;
public:
   OptionEncodingComboBox( const QString& saveName, QTextCodec** ppVarCodec,
                   QWidget* pParent, OptionDialog* pOD )
   : QComboBox( pParent ), OptionItem( pOD, saveName )
   {
      m_ppVarCodec = ppVarCodec;
      insertCodec( tr("Unicode, 8 bit"),  QTextCodec::codecForName("UTF-8") );
      insertCodec( tr("Unicode"), QTextCodec::codecForName("iso-10646-UCS-2") );
      insertCodec( tr("Latin1"), QTextCodec::codecForName("iso 8859-1") );

      // First sort codec names:
      std::map<QString, QTextCodec*> names;
      QList<int> mibs = QTextCodec::availableMibs();
      foreach(int i, mibs)
      {
         QTextCodec* c = QTextCodec::codecForMib(i);
         if ( c!=0 )
            names[QString(c->name()).toUpper()]=c;
      }

      std::map<QString, QTextCodec*>::iterator it;
      for(it=names.begin();it!=names.end();++it)
      {
         insertCodec( "", it->second );
      }

      this->setToolTip( tr(
         "Change this if non-ASCII characters are not displayed correctly."
         ));
   }
   void insertCodec( const QString& visibleCodecName, QTextCodec* c )
   {
      if (c!=0)
      {
         for( unsigned int i=0; i<m_codecVec.size(); ++i )
         {
            if ( c==m_codecVec[i] )
               return;  // don't insert any codec twice
         }
         addItem( visibleCodecName.isEmpty() ? QString(c->name()) : visibleCodecName+" ("+c->name()+")", (int)m_codecVec.size() );
         m_codecVec.push_back( c );
      }
   }
   void setToDefault()
   {
      QString defaultName = QTextCodec::codecForLocale()->name();
      for(int i=0;i<count();++i)
      {
         if (defaultName==itemText(i) &&
             m_codecVec[i]==QTextCodec::codecForLocale())
         {
            setCurrentIndex(i);
            if (m_ppVarCodec!=0){ *m_ppVarCodec=m_codecVec[i]; }
            return;
         }
      }

      setCurrentIndex( 0 );
      if (m_ppVarCodec!=0){ *m_ppVarCodec=m_codecVec[0]; }
   }
   void setToCurrent()
   {
      if (m_ppVarCodec!=0)
      {
         for(unsigned int i=0; i<m_codecVec.size(); ++i)
         {
            if ( *m_ppVarCodec==m_codecVec[i] )
            {
               setCurrentIndex( i );
               break;
            }
         }
      }
   }
   void apply()
   {
      if (m_ppVarCodec!=0){ *m_ppVarCodec = m_codecVec[ currentIndex() ]; }
   }
   void write(ValueMap* config)
   {
      if (m_ppVarCodec!=0) config->writeEntry(m_saveName, QString((*m_ppVarCodec)->name()) );
   }
   void read (ValueMap* config)
   {
      QString codecName = config->readEntry( m_saveName, QString(m_codecVec[ currentIndex() ]->name()) );
      for(unsigned int i=0; i<m_codecVec.size(); ++i)
      {
         if ( codecName == m_codecVec[i]->name() )
         {
            setCurrentIndex( i );
            if (m_ppVarCodec!=0) *m_ppVarCodec = m_codecVec[i];
            break;
         }
      }
   }
};


OptionDialog::OptionDialog(QWidget *parent) : QTabWidget( parent )
{

   setupFontPage();
   setupColorPage();
   setupOtherOptions();

}

OptionDialog::~OptionDialog( void )
{
}

void OptionDialog::setupOtherOptions()
{

   m_bShowWhiteSpaceCharacters = false;

   m_bShowWhiteSpace = false;

   m_bShowLineNumbers = false;

}

void OptionDialog::setupFontPage( void )
{

    m_font =
#ifdef _WIN32
      QFont("Courier New", 12);
#else
      QFont("Courier", 12);
#endif

}


void OptionDialog::setupColorPage( void )
{

   m_fgColor = Qt::black;

   m_bgColor = Qt::white;


   m_diffBgColor = QColor(qRgb(224,224,224));

   m_colorA = QColor(qRgb(0,0,200));

   m_colorB = QColor(qRgb(0,150,0));

   m_colorC = QColor(qRgb(150,0,150));

   m_colorForConflict = Qt::red;

   m_currentRangeBgColor = QColor(qRgb(220,220,100));

   m_currentRangeDiffBgColor = QColor(qRgb(255,255,150));

   m_manualHelpRangeColor = QColor(qRgb(0xff,0xd0,0x80));

}


void OptionDialog::setupEditPage( void )
{
   m_tabSize = 8;

}


void OptionDialog::setupDiffPage( void )
{

}

void OptionDialog::setupMergePage( void )
{

}

void OptionDialog::setupDirectoryMergePage( void )
{

}
/*
static void insertCodecs(OptionComboBox* p)
{
   std::multimap<QString,QString> m;  // Using the multimap for case-insensitive sorting.
   int i;
   for(i=0;;++i)
   {
      QTextCodec* pCodec = QTextCodec::codecForIndex ( i );
      if ( pCodec != 0 )  m.insert( std::make_pair( QString(pCodec->mimeName()).toUpper(), pCodec->mimeName()) );
      else                break;
   }
   
   p->insertItem( tr("Auto"), 0 );
   std::multimap<QString,QString>::iterator mi;
   for(mi=m.begin(), i=0; mi!=m.end(); ++mi, ++i)
      p->insertItem(mi->second, i+1);
}
*/

// UTF8-Codec that saves a BOM
// UTF8-Codec that saves a BOM
class Utf8BOMCodec : public QTextCodec
{
   public:
   QByteArray name () const { return "UTF-8-BOM"; }
   int mibEnum () const { return 2123; }
   QByteArray convertFromUnicode ( const QChar * input, int number, ConverterState * pState ) const
   {
      QByteArray r;
      if ( pState && pState->state_data[0]==0)
      {
        r += "\xEF\xBB\xBF";
        pState->state_data[0]=1;
      }

      r += QString::fromRawData( input, number ).toUtf8();
      return r;
   }
   QString convertToUnicode ( const char * p, int len, ConverterState* ) const
   {
      return QString::fromUtf8 ( p, len );
   }

   class UTF8BOMEncoder : public QTextEncoder
   {
      bool bBOMAdded;
   public:
      UTF8BOMEncoder(const QTextCodec* pTC):QTextEncoder(pTC)  {  bBOMAdded=false;  }
      QByteArray fromUnicode(const QString& uc, int& lenInOut )
      {
         QByteArray r;
         if (!bBOMAdded)
         {
            r += "\xEF\xBB\xBF";
            bBOMAdded=true;
         }
         r += uc.toUtf8();
         lenInOut = r.length();
         return r;
      }
   };
   QTextEncoder* makeEncoder() const
   {
      return new UTF8BOMEncoder(this);
   }

   class UTF8BOMDecoder : public QTextDecoder
   {
      QTextDecoder *m_pDecoder;
   public:
      UTF8BOMDecoder(const QTextCodec* pTC) : QTextDecoder(pTC)  {  m_pDecoder = QTextCodec::codecForName("UTF-8")->makeDecoder();  }
      ~UTF8BOMDecoder() {
         delete m_pDecoder;
      }
      QString toUnicode( const char* p, int len)
      {
         return m_pDecoder->toUnicode( p, len );
      }
   };
   QTextDecoder* makeDecoder() const
   {
      return new UTF8BOMDecoder(this);
   }
};

void OptionDialog::setupRegionalPage( void )
{

}

//#ifdef _WIN32
//#include "ccInstHelper.cpp"
//#endif

void OptionDialog::setupIntegrationPage( void )
{

}

void OptionDialog::slotEncodingChanged()
{
   if ( m_pSameEncoding->isChecked() )
   {
      m_pEncodingBComboBox->setEnabled( false );
      m_pEncodingBComboBox->setCurrentIndex( m_pEncodingAComboBox->currentIndex() );
      m_pEncodingCComboBox->setEnabled( false );
      m_pEncodingCComboBox->setCurrentIndex( m_pEncodingAComboBox->currentIndex() );
      m_pEncodingOutComboBox->setEnabled( false );
      m_pEncodingOutComboBox->setCurrentIndex( m_pEncodingAComboBox->currentIndex() );
      m_pEncodingPPComboBox->setEnabled( false );
      m_pEncodingPPComboBox->setCurrentIndex( m_pEncodingAComboBox->currentIndex() );
      m_pAutoDetectUnicodeB->setEnabled( false );
      m_pAutoDetectUnicodeB->setCheckState( m_pAutoDetectUnicodeA->checkState() );
      m_pAutoDetectUnicodeC->setEnabled( false );
      m_pAutoDetectUnicodeC->setCheckState( m_pAutoDetectUnicodeA->checkState() );
      m_pAutoSelectOutEncoding->setEnabled( false );
      m_pAutoSelectOutEncoding->setCheckState( m_pAutoDetectUnicodeA->checkState() );
   }
   else
   {
      m_pEncodingBComboBox->setEnabled( true );
      m_pEncodingCComboBox->setEnabled( true );
      m_pEncodingOutComboBox->setEnabled( true );
      m_pEncodingPPComboBox->setEnabled( true );
      m_pAutoDetectUnicodeB->setEnabled( true );
      m_pAutoDetectUnicodeC->setEnabled( true );
      m_pAutoSelectOutEncoding->setEnabled( true );
      m_pEncodingOutComboBox->setEnabled( m_pAutoSelectOutEncoding->checkState()==Qt::Unchecked );
   }
}

void OptionDialog::setupKeysPage( void )
{

}

void OptionDialog::slotOk( void )
{
   slotApply();

   // My system returns variable width fonts even though I
   // disabled this. Even QFont::fixedPitch() doesn't work.
   QFontMetrics fm(m_font);
   if ( fm.width('W')!=fm.width('i') )
   {
//      int result = QMessageBox::warningYesNo(this, tr(
//         "You selected a variable width font.\n\n"
//         "Because this program doesn't handle variable width fonts\n"
//         "correctly, you might experience problems while editing.\n\n"
//         "Do you want to continue or do you want to select another font."),
//         tr("Incompatible Font"),
//         KGuiItem( tr("Continue at Own Risk") ),
//         KGuiItem( tr("Select Another Font")) );
//      if (result==KMessageBox::No)
//         return;
   }

   //accept();
}


/** Copy the values from the widgets to the public variables.*/
void OptionDialog::slotApply( void )
{
   std::list<OptionItem*>::iterator i;
   for(i=m_optionItemList.begin(); i!=m_optionItemList.end(); ++i)
   {
      (*i)->apply();
   }

   emit applyDone();

#ifdef _WIN32
   QString locale = m_language;
   if ( locale == "Auto" || locale.isEmpty() )
      locale = QLocale::system().name().left(2);
   int spacePos = locale.indexOf(' ');
   if (spacePos>0) locale = locale.left(spacePos);
   QSettings settings("HKEY_CURRENT_USER\\Software\\KDiff3\\diff-ext", QSettings::NativeFormat);
   settings.setValue( "Language", locale );
#endif
}

/** Set the default values in the widgets only, while the
    public variables remain unchanged. */
void OptionDialog::slotDefault()
{
//   int result = KMessageBox::warningContinueCancel(this, tr("This resets all options. Not only those of the current topic.") );
//   if ( result==KMessageBox::Cancel ) return;
//   else resetToDefaults();
}

void OptionDialog::resetToDefaults()
{
   std::list<OptionItem*>::iterator i;
   for(i=m_optionItemList.begin(); i!=m_optionItemList.end(); ++i)
   {
      (*i)->setToDefault();
   }

   slotEncodingChanged();
}

/** Initialise the widgets using the values in the public varibles. */
void OptionDialog::setState()
{
   std::list<OptionItem*>::iterator i;
   for(i=m_optionItemList.begin(); i!=m_optionItemList.end(); ++i)
   {
      (*i)->setToCurrent();
   }

   slotEncodingChanged();
}

class ConfigValueMap : public ValueMap
{

};





QString OptionDialog::parseOptions( const QStringList& optionList )
{
   QString result;
   QStringList::const_iterator i;
   for ( i=optionList.begin(); i!=optionList.end(); ++i )
   {
      QString s = *i;

      int pos = s.indexOf('=');
      if( pos > 0 )                     // seems not to have a tag
      {
         QString key = s.left(pos);
         QString val = s.mid(pos+1);
         std::list<OptionItem*>::iterator j;
         bool bFound = false;
         for(j=m_optionItemList.begin(); j!=m_optionItemList.end(); ++j)
         {
            if ( (*j)->getSaveName()==key )
            {
               ValueMap config;
               config.writeEntry( key, val );  // Write the value as a string and
               (*j)->read(&config);       // use the internal conversion from string to the needed value.
               bFound = true;
               break;
            }
         }
         if ( ! bFound )
         {
            result += "No config item named \"" + key + "\"\n";
         }
      }
      else
      {
         result += "No '=' found in \"" + s + "\"\n";
      }
   }
   return result;
}

QString OptionDialog::calcOptionHelp()
{
   ValueMap config;
   std::list<OptionItem*>::iterator j;
   for(j=m_optionItemList.begin(); j!=m_optionItemList.end(); ++j)
   {
      (*j)->write( &config );
   }
   return config.getAsString();
}

void OptionDialog::slotHistoryMergeRegExpTester()
{

}


