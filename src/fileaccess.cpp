/***************************************************************************
 *   Copyright (C) 2003-2011 by Joachim Eibl                               *
 *   joachim.eibl at gmx.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "fileaccess.h"
#include "optiondialog.h"
#include "common.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qeventloop.h>
#include <QProcess>
#include <QProgressBar>

#include <vector>
#include <cstdlib>
#include <iostream>
//#include <klocale.h>
//#include <ktemporaryfile.h>
//#include <kio/global.h>
//#include <kmessagebox.h>
//#include <kio/jobuidelegate.h>
//#include <kio/copyjob.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <sys/utime.h>
#include <io.h>
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>          // Needed for creating symbolic links via symlink().
#include <utime.h>
#endif


ProgressDialog* g_pProgressDialog=0;


FileAccess::FileAccess( const QString& name, bool bWantToWrite )
{
   setFile( name, bWantToWrite );
}

FileAccess::FileAccess()
{
   m_bValidData = false;
   m_size = 0;
   m_creationTime = QDateTime();
   m_accessTime = QDateTime();
   m_modificationTime = QDateTime();
   m_bReadable = false;
   m_bWritable = false;
   m_bExecutable = false;
   m_bLocal = false;
   m_bHidden = false;
   m_bExists = false;
   m_bFile = false;
   m_bDir  = false;
   m_bSymLink = false;
}

FileAccess::~FileAccess()
{
   if( !m_localCopy.isEmpty() )
   {
      removeTempFile( m_localCopy );
   }
}

void FileAccess::setFile( const QString& name, bool bWantToWrite )
{
   m_url = QUrl( name );
   m_bValidData = false;

   m_size = 0;
   m_creationTime = QDateTime();
   m_accessTime = QDateTime();
   m_modificationTime = QDateTime();
   m_bReadable = false;
   m_bWritable = false;
   m_bExecutable = false;
   m_bHidden = false;
   m_bExists = false;
   m_bFile = false;
   m_bDir  = false;
   m_bSymLink = false;
   m_linkTarget = "";
   m_fileType = -1;
   m_bLocal = true;

   // Note: Checking if the filename-string is empty is necessary for Win95/98/ME.
   //       The isFile() / isDir() queries would cause the program to crash.
   //       (This is a Win95-bug which has been corrected only in WinNT/2000/XP.)
   if ( !name.isEmpty() )
   {
      // FileAccess tries to detect if the given name is an URL or a local file.
      // This is a problem if the filename looks like an URL (i.e. contains a colon ':').
      // e.g. "file:f.txt" is a valid filename.
      // Most of the time it is sufficient to check if the file exists locally.
      // 2 Problems remain:
      //   1. When the local file exists and the remote location is wanted nevertheless. (unlikely)
      //   2. When the local file doesn't exist and should be written to.

      bool bExistsLocal = QDir().exists(name);
      if (m_url.isRelative() || !m_url.isValid() || bExistsLocal ) // assuming that invalid means relative
      {
         QString localName = name;
         if ( !bExistsLocal && name.left(5).toLower()=="file:" )
         {
            localName = m_url.path(); // I want the path without preceding "file:"
         }
         QFileInfo fi( localName );
#if defined(Q_WS_WIN)
         // On some windows machines in a network this takes very long.
         // and it's not so important anyway.
         m_bReadable    = true;
         m_bWritable    = true; // in certain situations this might become a problem though
         m_bExecutable  = false;
#else
         m_bReadable    = fi.isReadable();
         m_bWritable    = fi.isWritable();
         m_bExecutable  = fi.isExecutable();
#endif
         m_creationTime = fi.created();
         m_bHidden    = fi.isHidden();
         m_modificationTime = fi.lastModified();
         m_accessTime = fi.lastRead();
         m_size       = fi.size();
         m_bSymLink   = fi.isSymLink();
         m_bFile      = fi.isFile();
         m_bDir       = fi.isDir();
         m_bExists    = fi.exists();
         m_name       = fi.fileName();
         m_path       = fi.filePath();
         m_absoluteFilePath= fi.absoluteFilePath();
         if ( m_bSymLink ) m_linkTarget = fi.readLink();
         m_bLocal = true;
         m_bValidData = true;
         if ( ! m_url.isValid() )
         {
            m_url.setPath( m_absoluteFilePath );
         }

         if ( !m_bExists  && m_absoluteFilePath.contains("@@") )
         {
            // Try reading a clearcase file
            m_localCopy = FileAccess::tempFileName();
            QString cmd = "cleartool get -to \"" + m_localCopy + "\"  \"" + m_absoluteFilePath + "\"";
            QProcess process;
            process.start( cmd );
            process.waitForFinished(-1);
            //::system( cmd.local8Bit() );

            QFileInfo fi( m_localCopy );
#if defined(Q_WS_WIN)
            m_bReadable    = true;//fi.isReadable();
            m_bWritable    = true;//fi.isWritable();
            m_bExecutable  = false;//fi.isExecutable();
#else
            m_bReadable    = fi.isReadable();
            m_bWritable    = fi.isWritable();
            m_bExecutable  = fi.isExecutable();
#endif
            m_creationTime = fi.created();
            m_bHidden    = fi.isHidden();
            m_modificationTime = fi.lastModified();
            m_accessTime = fi.lastRead();
            m_size       = fi.size();
            m_bSymLink   = fi.isSymLink();
            m_bFile      = fi.isFile();
            m_bDir       = fi.isDir();
            m_bExists    = fi.exists();
         }
      }
      else
      {
         m_absoluteFilePath = name;
         //m_name   = m_url.fileName();
         m_bLocal = false;

         FileAccessJobHandler jh( this ); // A friend, which writes to the parameters of this class!
         jh.stat(2/*all details*/, bWantToWrite); // returns bSuccess, ignored

         m_path = name;
         m_bValidData = true; // After running stat() the variables are initialised
                              // and valid even if the file doesn't exist and the stat
                              // query failed.
      }
   }
}

void FileAccess::addPath( const QString& txt )
{
//   if ( m_url.isValid() )
//   {
//      m_url.addPath( txt );
//      setFile( m_url.url() );  // reinitialise
//   }
//   else
//   {
//      QString slash = (txt.isEmpty() || txt[0]=='/') ? "" : "/";
//      setFile( absoluteFilePath() + slash + txt );
//   }
}

/*     Filetype:
       S_IFMT     0170000   bitmask for the file type bitfields
       S_IFSOCK   0140000   socket
       S_IFLNK    0120000   symbolic link
       S_IFREG    0100000   regular file
       S_IFBLK    0060000   block device
       S_IFDIR    0040000   directory
       S_IFCHR    0020000   character device
       S_IFIFO    0010000   fifo
       S_ISUID    0004000   set UID bit
       S_ISGID    0002000   set GID bit (see below)
       S_ISVTX    0001000   sticky bit (see below)

       Access:
       S_IRWXU    00700     mask for file owner permissions
       S_IRUSR    00400     owner has read permission
       S_IWUSR    00200     owner has write permission
       S_IXUSR    00100     owner has execute permission
       S_IRWXG    00070     mask for group permissions
       S_IRGRP    00040     group has read permission
       S_IWGRP    00020     group has write permission
       S_IXGRP    00010     group has execute permission
       S_IRWXO    00007     mask for permissions for others (not in group)
       S_IROTH    00004     others have read permission
       S_IWOTH    00002     others have write permisson
       S_IXOTH    00001     others have execute permission
*/

//#ifdef KREPLACEMENTS_H
//void FileAccess::setUdsEntry( const KIO::UDSEntry& ){}  // not needed if KDE is not available
//#else
//void FileAccess::setUdsEntry( const KIO::UDSEntry& e )
//{
//   long acc = 0;
//   long fileType = 0;
//   QList< uint > fields = e.listFields();
//   for( QList< uint >::ConstIterator ei=fields.constBegin(); ei!=fields.constEnd(); ++ei )
//   {
//      uint f = *ei;
//      switch( f )
//      {
//         case KIO::UDSEntry::UDS_SIZE :              m_size   = e.numberValue(f);   break;
//         case KIO::UDSEntry::UDS_USER :              m_user   = e.stringValue(f);    break;
//         case KIO::UDSEntry::UDS_GROUP :             m_group  = e.stringValue(f);    break;
//         case KIO::UDSEntry::UDS_NAME :              m_path   = e.stringValue(f);    break;  // During listDir the relative path is given here.
//         case KIO::UDSEntry::UDS_MODIFICATION_TIME : m_modificationTime.setTime_t( e.numberValue(f) ); break;
//         case KIO::UDSEntry::UDS_ACCESS_TIME :       m_accessTime.setTime_t( e.numberValue(f) ); break;
//         case KIO::UDSEntry::UDS_CREATION_TIME :     m_creationTime.setTime_t( e.numberValue(f) ); break;
//         case KIO::UDSEntry::UDS_LINK_DEST :         m_linkTarget       = e.stringValue(f); break;
//         case KIO::UDSEntry::UDS_ACCESS :
//         {
//            acc = e.numberValue(f);
//            m_bReadable   = (acc & S_IRUSR)!=0;
//            m_bWritable   = (acc & S_IWUSR)!=0;
//            m_bExecutable = (acc & S_IXUSR)!=0;
//            break;
//         }
//         case KIO::UDSEntry::UDS_FILE_TYPE :
//         {
//            fileType = e.numberValue(f);
//            m_bDir     = ( fileType & S_IFMT ) == S_IFDIR;
//            m_bFile    = ( fileType & S_IFMT ) == S_IFREG;
//            m_bSymLink = ( fileType & S_IFMT ) == S_IFLNK;
//            m_bExists  = fileType != 0;
//            m_fileType = fileType;
//            break;
//         }
//
//         case KIO::UDSEntry::UDS_URL :               // m_url = KUrl( e.stringValue(f) );
//                                           break;
//         case KIO::UDSEntry::UDS_MIME_TYPE :         break;
//         case KIO::UDSEntry::UDS_GUESSED_MIME_TYPE : break;
//         case KIO::UDSEntry::UDS_XML_PROPERTIES :    break;
//         default: break;
//      }
//   }
//
//   m_bExists = acc!=0 || fileType!=0;
//
//   m_bLocal = false;
//   m_bValidData = true;
//   m_bSymLink = !m_linkTarget.isEmpty();
//   if ( m_name.isEmpty() )
//   {
//      int pos = m_path.lastIndexOf('/') + 1;
//      m_name = m_path.mid( pos );
//   }
//   m_bHidden = m_name[0]=='.';
//}
//#endif


bool FileAccess::isValid() const       {   return m_bValidData;  }
bool FileAccess::isFile() const        {   return m_bFile;       }
bool FileAccess::isDir() const         {   return m_bDir;        }
bool FileAccess::isSymLink() const     {   return m_bSymLink;    }
bool FileAccess::exists() const        {   return m_bExists;     }
long FileAccess::size() const          {   return m_size;        }
QUrl FileAccess::url() const           {   return m_url;         }
bool FileAccess::isLocal() const       {   return m_bLocal;      }
bool FileAccess::isReadable() const    {   return m_bReadable;   }
bool FileAccess::isWritable() const    {   return m_bWritable;   }
bool FileAccess::isExecutable() const  {   return m_bExecutable; }
bool FileAccess::isHidden() const      {   return m_bHidden;     }
QString FileAccess::readLink() const   {   return m_linkTarget;  }
QString FileAccess::absoluteFilePath() const{   return m_absoluteFilePath; }  // Full abs path
QString FileAccess::fileName() const   {   return m_name;        }  // Just the name-part of the path, without parent directories
QString FileAccess::filePath() const   {   return m_path;        }  // The path-string that was used during construction
//QString FileAccess::prettyAbsPath() const { return isLocal() ? m_absoluteFilePath : m_url.prettyUrl(); }

QDateTime FileAccess::created() const
{
   return ( m_creationTime.isValid() ?  m_creationTime : m_modificationTime );
}

QDateTime FileAccess::lastModified() const
{
   return m_modificationTime;
}

QDateTime FileAccess::lastRead() const
{
   return ( m_accessTime.isValid() ?  m_accessTime : m_modificationTime );
}

static bool interruptableReadFile( QFile& f, void* pDestBuffer, unsigned long maxLength )
{
   ProgressProxy pp;
   const unsigned long maxChunkSize = 100000;
   unsigned long i=0;
   while( i<maxLength )
   {
      unsigned long nextLength = min2( maxLength-i, maxChunkSize );
      unsigned long reallyRead = f.read( (char*)pDestBuffer+i, nextLength );
      if ( reallyRead != nextLength )
      {
         return false;
      }
      i+=reallyRead;

      pp.setCurrent( double(i)/maxLength );
      if ( pp.wasCancelled() ) return false;
   }
   return true;
}

bool FileAccess::readFile( void* pDestBuffer, unsigned long maxLength )
{
   if ( !m_localCopy.isEmpty() )
   {
      QFile f( m_localCopy );
      if ( f.open( QIODevice::ReadOnly ) )
         return interruptableReadFile(f, pDestBuffer, maxLength);// maxLength == f.read( (char*)pDestBuffer, maxLength );
   }
   else if (m_bLocal)
   {
      QFile f( filePath() );

      if ( f.open( QIODevice::ReadOnly ) )
         return interruptableReadFile(f, pDestBuffer, maxLength); //maxLength == f.read( (char*)pDestBuffer, maxLength );
   }
   else
   {
      FileAccessJobHandler jh( this );
      //return jh.get( pDestBuffer, maxLength );
   }
   return false;
}

bool FileAccess::writeFile( const void* pSrcBuffer, unsigned long length )
{
   ProgressProxy pp;
   if (m_bLocal)
   {
      QFile f( filePath() );
      if ( f.open( QIODevice::WriteOnly ) )
      {
         const unsigned long maxChunkSize = 100000;
         unsigned long i=0;
         while( i<length )
         {
            unsigned long nextLength = min2( length-i, maxChunkSize );
            unsigned long reallyWritten = f.write( (char*)pSrcBuffer+i, nextLength );
            if ( reallyWritten != nextLength )
            {
               return false;
            }
            i+=reallyWritten;

            pp.setCurrent( double(i)/length );
            if ( pp.wasCancelled() ) return false;
         }
         f.close();
#ifndef _WIN32
         if ( isExecutable() )  // value is true if the old file was executable
         {
            // Preserve attributes
            f.setPermissions(f.permissions() | QFile::ExeUser);
            //struct stat srcFileStatus;
            //int statResult = ::stat( filePath().toLocal8Bit().constData(), &srcFileStatus );
            //if (statResult==0)
            //{
            //   ::chmod ( filePath().toLocal8Bit().constData(), srcFileStatus.st_mode | S_IXUSR );
            //}
         }
#endif

         return true;
      }
   }
   else
   {
      FileAccessJobHandler jh( this );
      //return jh.put( pSrcBuffer, length, true /*overwrite*/ );
   }
   return false;
}

bool FileAccess::copyFile( const QString& dest )
{
   FileAccessJobHandler jh( this );
   return jh.copyFile( dest );   // Handles local and remote copying.
}

bool FileAccess::rename( const QString& dest )
{
   FileAccessJobHandler jh( this );
   return jh.rename( dest );
}

bool FileAccess::removeFile()
{
   if ( isLocal() )
   {
      return QDir().remove( absoluteFilePath() );
   }
   else
   {
      FileAccessJobHandler jh( this );
      return jh.removeFile( absoluteFilePath() );
   }
}

bool FileAccess::removeFile( const QString& name ) // static
{
   return FileAccess(name).removeFile();
}

bool FileAccess::listDir( t_DirectoryList* pDirList, bool bRecursive, bool bFindHidden,
   const QString& filePattern, const QString& fileAntiPattern, const QString& dirAntiPattern,
   bool bFollowDirLinks, bool bUseCvsIgnore )
{
   FileAccessJobHandler jh( this );
   return jh.listDir( pDirList, bRecursive, bFindHidden, filePattern, fileAntiPattern,
                      dirAntiPattern, bFollowDirLinks, bUseCvsIgnore );
}

QString FileAccess::tempFileName()
{

      QString fileName;
      #ifdef _WIN32
         QString tmpDir = getenv("TEMP");
      #else
         QString tmpDir = "/tmp";
      #endif
      for(int i=0; ;++i)
      {
         // short filenames for WIN98 because for system() the command must not exceed 120 characters.
         #ifdef _WIN32
         if ( QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based ) // Win95, 98, ME
            fileName = tmpDir + "\\" + QString::number(i);
         else
            fileName = tmpDir + "/kdiff3_" + QString::number(_getpid()) + "_" + QString::number(i) +".tmp";
         #else
            fileName = tmpDir + "/kdiff3_" + QString::number(getpid()) + "_" + QString::number(i) +".tmp";
         #endif
         if ( ! FileAccess::exists(fileName) && 
              QFile(fileName).open(QIODevice::WriteOnly) ) // open, truncate and close the file, true if successful
         {
            break;
         }
      }
      return QDir::toNativeSeparators(fileName+".2");
}

bool FileAccess::removeTempFile( const QString& name ) // static
{
   if (name.endsWith(".2"))
      FileAccess(name.left(name.length()-2)).removeFile();
   return FileAccess(name).removeFile();
}


bool FileAccess::makeDir( const QString& dirName )
{
   FileAccessJobHandler fh(0);
   return fh.mkDir( dirName );
}

bool FileAccess::removeDir( const QString& dirName )
{
   FileAccessJobHandler fh(0);
   return fh.rmDir( dirName );
}

#ifdef _WIN32
bool FileAccess::symLink( const QString& /*linkTarget*/, const QString& /*linkLocation*/ )
{
   return false;
}
#else
bool FileAccess::symLink( const QString& linkTarget, const QString& linkLocation )
{
   return 0==::symlink( linkTarget.toLocal8Bit().constData(), linkLocation.toLocal8Bit().constData() );
   //FileAccessJobHandler fh(0);
   //return fh.symLink( linkTarget, linkLocation );
}
#endif

bool FileAccess::exists( const QString& name )
{
   FileAccess fa( name );
   return fa.exists();
}

// If the size couldn't be determined by stat() then the file is copied to a local temp file.
long FileAccess::sizeForReading()
{
   if ( m_size == 0 && !isLocal() )
   {
      // Size couldn't be determined. Copy the file to a local temp place.
      QString localCopy = tempFileName();
      bool bSuccess = copyFile( localCopy );
      if ( bSuccess )
      {
         QFileInfo fi( localCopy );
         m_size = fi.size();
         m_localCopy = localCopy;
         return m_size;
      }
      else
      {
         return 0;
      }
   }
   else
      return m_size;
}

QString FileAccess::getStatusText()
{
   return m_statusText;
}

QString FileAccess::cleanPath( const QString& path ) // static
{
   QUrl url(path);
   if(!url.isValid() )
   {
      return QDir().cleanPath( path );
   }
   else
   {
      return path;
   }
}

bool FileAccess::createBackup( const QString& bakExtension )
{
   if ( exists() )
   {
      // First rename the existing file to the bak-file. If a bak-file file exists, delete that.
      QString bakName = absoluteFilePath() + bakExtension;
      FileAccess bakFile( bakName, true /*bWantToWrite*/ );
      if ( bakFile.exists() )
      {
         bool bSuccess = bakFile.removeFile();
         if ( !bSuccess )
         {
            //m_statusText = tr("While trying to make a backup, deleting an older backup failed. \nFilename: ") + bakName;
            return false;
         }
      }
      bool bSuccess = rename( bakName );
      if (!bSuccess)
      {
         //m_statusText = tr("While trying to make a backup, renaming failed. \nFilenames: ") +
               absoluteFilePath() + " -> " + bakName;
         return false;
      }
   }
   return true;
}

FileAccessJobHandler::FileAccessJobHandler( FileAccess* pFileAccess )
{
   m_pFileAccess = pFileAccess;
   m_bSuccess = false;
}

bool FileAccessJobHandler::stat( int detail, bool bWantToWrite )
{
//   m_bSuccess = false;
//   m_pFileAccess->m_statusText = QString();
//   KIO::StatJob* pStatJob = KIO::stat( m_pFileAccess->m_url,
//         bWantToWrite ? KIO::StatJob::DestinationSide : KIO::StatJob::SourceSide,
//         detail, KIO::HideProgressInfo );
//
//   connect( pStatJob, SIGNAL(result(KJob*)), this, SLOT(slotStatResult(KJob*)));
//
//   g_pProgressDialog->enterEventLoop( pStatJob, i18n("Getting file status: %1",m_pFileAccess->prettyAbsPath()) );
//
//   return m_bSuccess;
    return true;
}



bool FileAccessJobHandler::mkDir( const QString& dirName )
{
//   KUrl dirURL = KUrl( dirName );
//   if ( dirName.isEmpty() )
//      return false;
//   else if ( dirURL.isLocalFile() || dirURL.isRelative() )
//   {
//      return QDir().mkdir( dirURL.path() );
//   }
//   else
//   {
//      m_bSuccess = false;
//      KIO::SimpleJob* pJob = KIO::mkdir( dirURL );
//      connect( pJob, SIGNAL(result(KJob*)), this, SLOT(slotSimpleJobResult(KJob*)));
//
//      g_pProgressDialog->enterEventLoop( pJob, i18n("Making directory: %1", dirName) );
//      return m_bSuccess;
//   }
    return true;
}

bool FileAccessJobHandler::rmDir( const QString& dirName )
{
//   KUrl dirURL = KUrl( dirName );
//   if ( dirName.isEmpty() )
//      return false;
//   else if ( dirURL.isLocalFile() )
//   {
//      return QDir().rmdir( dirURL.path() );
//   }
//   else
//   {
//      m_bSuccess = false;
//      KIO::SimpleJob* pJob = KIO::rmdir( dirURL );
//      connect( pJob, SIGNAL(result(KJob*)), this, SLOT(slotSimpleJobResult(KJob*)));
//
//      g_pProgressDialog->enterEventLoop(pJob, i18n("Removing directory: %1",dirName));
//      return m_bSuccess;
//   }
    return true;
}

bool FileAccessJobHandler::removeFile( const QString& fileName )
{
//   if ( fileName.isEmpty() )
//      return false;
//   else
//   {
//      m_bSuccess = false;
//      KIO::SimpleJob* pJob = KIO::file_delete( fileName, KIO::HideProgressInfo );
//      connect( pJob, SIGNAL(result(KJob*)), this, SLOT(slotSimpleJobResult(KJob*)));
//
//      g_pProgressDialog->enterEventLoop( pJob, i18n("Removing file: %1",fileName) );
//      return m_bSuccess;
//   }
    return true;
}

bool FileAccessJobHandler::symLink( const QString& linkTarget, const QString& linkLocation )
{
//   if ( linkTarget.isEmpty() || linkLocation.isEmpty() )
//      return false;
//   else
//   {
//      m_bSuccess = false;
//      KIO::CopyJob* pJob = KIO::link( linkTarget, linkLocation, false );
//      connect( pJob, SIGNAL(result(KJob*)), this, SLOT(slotSimpleJobResult(KJob*)));
//
//      g_pProgressDialog->enterEventLoop( pJob,
//         i18n("Creating symbolic link: %1 -> %2",linkLocation,linkTarget) );
//      return m_bSuccess;
//   }
    return true;
}

bool FileAccessJobHandler::rename( const QString& dest )
{
//   if ( dest.isEmpty() )
//      return false;
//
//   KUrl kurl( dest );
//   if ( !kurl.isValid() )
//      kurl = KUrl( QDir().absoluteFilePath(dest) ); // assuming that invalid means relative
//
//   if ( m_pFileAccess->isLocal() && kurl.isLocalFile() )
//   {
//      return QDir().rename( m_pFileAccess->absoluteFilePath(), kurl.path() );
//   }
//   else
//   {
//      int permissions=-1;
//      m_bSuccess = false;
//      KIO::FileCopyJob* pJob = KIO::file_move( m_pFileAccess->m_url, kurl, permissions, KIO::HideProgressInfo );
//      connect( pJob, SIGNAL(result(KJob*)), this, SLOT(slotSimpleJobResult(KJob*)));
//      connect( pJob, SIGNAL(percent(KJob*,unsigned long)), this, SLOT(slotPercent(KJob*, unsigned long)));
//
//      g_pProgressDialog->enterEventLoop( pJob,
//         i18n("Renaming file: %1 -> %2",m_pFileAccess->prettyAbsPath(),dest) );
//      return m_bSuccess;
//   }
    return true;
}




// Copy local or remote files.
bool FileAccessJobHandler::copyFile( const QString& dest )
{
//   ProgressProxy pp;
//   KUrl destUrl( dest );
//   m_pFileAccess->m_statusText = QString();
//   if ( ! m_pFileAccess->isLocal() || ! destUrl.isLocalFile() ) // if either url is nonlocal
//   {
//      int permissions = (m_pFileAccess->isExecutable()?0111:0)+(m_pFileAccess->isWritable()?0222:0)+(m_pFileAccess->isReadable()?0444:0);
//      m_bSuccess = false;
//      KIO::FileCopyJob* pJob = KIO::file_copy ( m_pFileAccess->m_url, destUrl, permissions, KIO::HideProgressInfo );
//      connect( pJob, SIGNAL(result(KJob*)), this, SLOT(slotSimpleJobResult(KJob*)));
//      connect( pJob, SIGNAL(percent(KJob*,unsigned long)), this, SLOT(slotPercent(KJob*, unsigned long)));
//      g_pProgressDialog->enterEventLoop( pJob,
//         i18n("Copying file: %1 -> %2",m_pFileAccess->prettyAbsPath(),dest) );
//
//      return m_bSuccess;
//      // Note that the KIO-slave preserves the original date, if this is supported.
//   }
//
//   // Both files are local:
//   QString srcName = m_pFileAccess->absoluteFilePath();
//   QString destName = dest;
//   QFile srcFile( srcName );
//   QFile destFile( destName );
//   bool bReadSuccess = srcFile.open( QIODevice::ReadOnly );
//   if ( bReadSuccess == false )
//   {
//      m_pFileAccess->m_statusText = i18n("Error during file copy operation: Opening file for reading failed. Filename: %1",srcName);
//      return false;
//   }
//   bool bWriteSuccess = destFile.open( QIODevice::WriteOnly );
//   if ( bWriteSuccess == false )
//   {
//      m_pFileAccess->m_statusText = i18n("Error during file copy operation: Opening file for writing failed. Filename: %1",destName);
//      return false;
//   }
//
//#if QT_VERSION==230
//   typedef long Q_LONG;
//#endif
//   std::vector<char> buffer(100000);
//   qint64 bufSize = buffer.size();
//   qint64 srcSize = srcFile.size();
//   while ( srcSize > 0 && !pp.wasCancelled() )
//   {
//      qint64 readSize = srcFile.read( &buffer[0], min2( srcSize, bufSize ) );
//      if ( readSize==-1 || readSize==0 )
//      {
//         m_pFileAccess->m_statusText = i18n("Error during file copy operation: Reading failed. Filename: %1",srcName);
//         return false;
//      }
//      srcSize -= readSize;
//      while ( readSize > 0 )
//      {
//         qint64 writeSize = destFile.write( &buffer[0], readSize );
//         if ( writeSize==-1 || writeSize==0 )
//         {
//            m_pFileAccess->m_statusText = i18n("Error during file copy operation: Writing failed. Filename: %1",destName);
//            return false;
//         }
//         readSize -= writeSize;
//      }
//      destFile.flush();
//      pp.setCurrent( (double)(srcFile.size()-srcSize)/srcFile.size(), false );
//   }
//   srcFile.close();
//   destFile.close();
//
//   // Update the times of the destFile
//#ifdef _WIN32
//   struct _stat srcFileStatus;
//   int statResult = ::_stat( srcName.toLocal8Bit().constData(), &srcFileStatus );
//   if (statResult==0)
//   {
//      _utimbuf destTimes;
//      destTimes.actime = srcFileStatus.st_atime;/* time of last access */
//      destTimes.modtime = srcFileStatus.st_mtime;/* time of last modification */
//
//      _utime ( destName.toLocal8Bit().constData(), &destTimes );
//      _chmod ( destName.toLocal8Bit().constData(), srcFileStatus.st_mode );
//   }
//#else
//   struct stat srcFileStatus;
//   int statResult = ::stat( srcName.toLocal8Bit().constData(), &srcFileStatus );
//   if (statResult==0)
//   {
//      utimbuf destTimes;
//      destTimes.actime = srcFileStatus.st_atime;/* time of last access */
//      destTimes.modtime = srcFileStatus.st_mtime;/* time of last modification */
//
//      utime ( destName.toLocal8Bit().constData(), &destTimes );
//      chmod ( destName.toLocal8Bit().constData(), srcFileStatus.st_mode );
//   }
//#endif
   return true;
}

bool wildcardMultiMatch( const QString& wildcard, const QString& testString, bool bCaseSensitive )
{
   QStringList sl = wildcard.split( ";" );

   for ( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it )
   {
      QRegExp pattern( *it, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, QRegExp::Wildcard );
      if ( pattern.exactMatch( testString ) )
         return true;
   }

   return false;
}


// class CvsIgnoreList from Cervisia cvsdir.cpp
//    Copyright (C) 1999-2002 Bernd Gehrmann <bernd at mail.berlios.de>
// with elements from class StringMatcher
//    Copyright (c) 2003 Andr�Woebeking <Woebbeking at web.de>
// Modifications for KDiff3 by Joachim Eibl
class CvsIgnoreList
{
public:
    CvsIgnoreList(){}
    void init(FileAccess& dir, bool bUseLocalCvsIgnore );
    bool matches(const QString& fileName, bool bCaseSensitive ) const;

private:
    void addEntriesFromString(const QString& str);
    void addEntriesFromFile(const QString& name);
    void addEntry(const QString& entry);

    QStringList m_exactPatterns;
    QStringList m_startPatterns;
    QStringList m_endPatterns;
    QStringList m_generalPatterns;
};


void CvsIgnoreList::init( FileAccess& dir, bool bUseLocalCvsIgnore )
{
   static const char *ignorestr = ". .. core RCSLOG tags TAGS RCS SCCS .make.state "
           ".nse_depinfo #* .#* cvslog.* ,* CVS CVS.adm .del-* *.a *.olb *.o *.obj "
           "*.so *.Z *~ *.old *.elc *.ln *.bak *.BAK *.orig *.rej *.exe _$* *$";

   addEntriesFromString(QString::fromLatin1(ignorestr));
   addEntriesFromFile(QDir::homePath() + "/.cvsignore");
   addEntriesFromString(QString::fromLocal8Bit(::getenv("CVSIGNORE")));

   if (bUseLocalCvsIgnore)
   {
      FileAccess file(dir);
      file.addPath( ".cvsignore" );
      int size = file.exists() ? file.sizeForReading() : 0;
      if ( size>0 )
      {
         char* buf=new char[size];
         if (buf!=0)
         {
            file.readFile( buf, size );
            int pos1 = 0;
            for ( int pos = 0; pos<=size; ++pos )
            {
               if( pos==size || buf[pos]==' ' || buf[pos]=='\t' || buf[pos]=='\n' || buf[pos]=='\r' )
               {
                  if (pos>pos1)
                  {
                     addEntry( QString::fromLatin1( &buf[pos1], pos-pos1 ) );
                  }
                  ++pos1;
               }
            }
            delete buf;
         }
      }
   }
}


void CvsIgnoreList::addEntriesFromString(const QString& str)
{
    int posLast(0);
    int pos;
    while ((pos = str.indexOf(' ', posLast)) >= 0)
    {
        if (pos > posLast)
            addEntry(str.mid(posLast, pos - posLast));
        posLast = pos + 1;
    }

    if (posLast < static_cast<int>(str.length()))
        addEntry(str.mid(posLast));
}


void CvsIgnoreList::addEntriesFromFile(const QString &name)
{
    QFile file(name);

    if( file.open(QIODevice::ReadOnly) )
    {
        QTextStream stream(&file);
        while( !stream.atEnd() )
        {
            addEntriesFromString(stream.readLine());
        }
    }
}

void CvsIgnoreList::addEntry(const QString& pattern)
{
   if (pattern != QString("!"))
   {
      if (pattern.isEmpty())    return;

      // The general match is general but slow.
      // Special tests for '*' and '?' at the beginning or end of a pattern
      // allow fast checks.

      // Count number of '*' and '?'
      unsigned int nofMetaCharacters = 0;

      const QChar* pos;
      pos = pattern.unicode();
      const QChar* posEnd;
      posEnd=pos + pattern.length();
      while (pos < posEnd)
      {
         if( *pos==QChar('*') || *pos==QChar('?') )  ++nofMetaCharacters;
         ++pos;
      }

      if ( nofMetaCharacters==0 )
      {
         m_exactPatterns.append(pattern);
      }
      else if ( nofMetaCharacters==1 )
      {
         if ( pattern.at(0) == QChar('*') )
         {
            m_endPatterns.append( pattern.right( pattern.length() - 1) );
         }
         else if (pattern.at(pattern.length() - 1) == QChar('*'))
         {
            m_startPatterns.append( pattern.left( pattern.length() - 1) );
         }
         else
         {
            m_generalPatterns.append(pattern.toLocal8Bit());
         }
      }
      else
      {
         m_generalPatterns.append(pattern.toLocal8Bit());
      }
   }
   else
   {
      m_exactPatterns.clear();
      m_startPatterns.clear();
      m_endPatterns.clear();
      m_generalPatterns.clear();
   }
}

bool CvsIgnoreList::matches(const QString& text, bool bCaseSensitive ) const
{
    if ( m_exactPatterns.indexOf(text) >=0 )
    {
        return true;
    }

    QStringList::ConstIterator it;
    QStringList::ConstIterator itEnd;
    for ( it=m_startPatterns.begin(), itEnd=m_startPatterns.end(); it != itEnd; ++it)
    {
        if (text.startsWith(*it))
        {
            return true;
        }
    }

    for ( it = m_endPatterns.begin(), itEnd=m_endPatterns.end(); it != itEnd; ++it)
    {
        if (text.mid( text.length() - (*it).length() )==*it)  //(text.endsWith(*it))
        {
            return true;
        }
    }

    /*
    for (QValueList<QCString>::const_iterator it(m_generalPatterns.begin()),
                                              itEnd(m_generalPatterns.end());
         it != itEnd; ++it)
    {
        if (::fnmatch(*it, text.local8Bit(), FNM_PATHNAME) == 0)
        {
            return true;
        }
    }
    */


   for ( it = m_generalPatterns.begin(); it != m_generalPatterns.end(); ++it )
   {
      QRegExp pattern( *it, bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, QRegExp::Wildcard );
      if ( pattern.exactMatch( text ) )
         return true;
   }

   return false;
}

static QString nicePath( const QFileInfo& fi )
{
   QString fp = fi.filePath();
   if ( fp.length()>2 && fp[0] == '.' && fp[1] == '/' )
   {
      return fp.mid(2);
   }
   return fp;
}

static bool cvsIgnoreExists( t_DirectoryList* pDirList )
{
   t_DirectoryList::iterator i;
   for( i = pDirList->begin(); i!=pDirList->end(); ++i )
   {
      if ( i->fileName()==".cvsignore" )
         return true;
   }
   return false;
}

//bool FileAccessJobHandler::listDir( t_DirectoryList* pDirList, bool bRecursive, bool bFindHidden, const QString& filePattern,
//   const QString& fileAntiPattern, const QString& dirAntiPattern, bool bFollowDirLinks, bool bUseCvsIgnore )
//{
//   ProgressProxy pp;
//   m_pDirList = pDirList;
//   m_pDirList->clear();
//   m_bFindHidden = bFindHidden;
//   m_bRecursive = bRecursive;
//   m_bFollowDirLinks = bFollowDirLinks;  // Only relevant if bRecursive==true.
//   m_fileAntiPattern = fileAntiPattern;
//   m_filePattern = filePattern;
//   m_dirAntiPattern = dirAntiPattern;

//   if ( pp.wasCancelled() )
//      return true; // Cancelled is not an error.

//   pp.setInformation( tr("Reading directory: ") + m_pFileAccess->absoluteFilePath(), 0, false );

//   if( m_pFileAccess->isLocal() )
//   {
//      QString currentPath = QDir::currentPath();
//      m_bSuccess = QDir::setCurrent( m_pFileAccess->absoluteFilePath() );
//      if ( m_bSuccess )
//      {
//#ifndef _WIN32
//         m_bSuccess = true;
//         QDir dir( "." );

//         dir.setSorting( QDir::Name | QDir::DirsFirst );
//         dir.setFilter( QDir::Files | QDir::Dirs | /* from KDE3 QDir::TypeMaskDirs | */ QDir::Hidden );

//         QFileInfoList fiList = dir.entryInfoList();
//         if ( fiList.isEmpty() )
//         {
//            // No Permission to read directory or other error.
//            m_bSuccess = false;
//         }
//         else
//         {
//            foreach ( QFileInfo fi, fiList )       // for each file...
//            {
//               if ( fi.fileName() == "." ||  fi.fileName()==".." )
//                  continue;

//               pDirList->push_back( FileAccess( nicePath(fi) ) );
//            }
//         }
//#else
//         QString pattern ="*.*";
//         WIN32_FIND_DATA findData;
//         WIN32_FIND_DATAA& findDataA=*(WIN32_FIND_DATAA*)&findData;  // Needed for Win95

//         Qt::HANDLE searchHandle = QT_WA_INLINE(
//                 FindFirstFile( (TCHAR*)pattern.utf16(), &findData ),
//                 FindFirstFileA( pattern.toLocal8Bit(), &findDataA )
//              );

//         if ( searchHandle != INVALID_HANDLE_VALUE )
//         {
//            QString absPath = m_pFileAccess->absoluteFilePath();
//            QString relPath = m_pFileAccess->filePath();
//            bool bFirst=true;
//            while( ! pp.wasCancelled() )
//            {
//               if (!bFirst)
//               {
//                  if ( ! QT_WA_INLINE(
//                            FindNextFile(searchHandle,&findData),
//                            FindNextFileA(searchHandle,&findDataA)) )
//                     break;
//               }
//               bFirst = false;
//               FileAccess fa;
//               fa.m_size = findData.nFileSizeLow ;//+ findData.nFileSizeHigh;

//               FILETIME ft;
//               SYSTEMTIME t;
//               FileTimeToLocalFileTime( &findData.ftLastWriteTime, &ft ); FileTimeToSystemTime(&ft,&t);
//               fa.m_modificationTime = QDateTime( QDate(t.wYear, t.wMonth, t.wDay), QTime(t.wHour, t.wMinute, t.wSecond) );
//               FileTimeToLocalFileTime( &findData.ftLastAccessTime, &ft ); FileTimeToSystemTime(&ft,&t);
//               fa.m_accessTime       = QDateTime( QDate(t.wYear, t.wMonth, t.wDay), QTime(t.wHour, t.wMinute, t.wSecond) );
//               FileTimeToLocalFileTime( &findData.ftCreationTime, &ft ); FileTimeToSystemTime(&ft,&t);
//               fa.m_creationTime     = QDateTime( QDate(t.wYear, t.wMonth, t.wDay), QTime(t.wHour, t.wMinute, t.wSecond) );

//               int  a = findData.dwFileAttributes;
//               fa.m_bWritable   = ( a & FILE_ATTRIBUTE_READONLY) == 0;
//               fa.m_bDir        = ( a & FILE_ATTRIBUTE_DIRECTORY ) != 0;
//               fa.m_bFile       = !fa.m_bDir;
//               fa.m_bHidden     = ( a & FILE_ATTRIBUTE_HIDDEN) != 0;

//               fa.m_bExecutable = false; // Useless on windows
//               fa.m_bExists     = true;
//               fa.m_bReadable   = true;
//               fa.m_bLocal      = true;
//               fa.m_bValidData  = true;
//               fa.m_bSymLink    = false;
//               fa.m_fileType    = 0;

//               fa.m_name = QT_WA_INLINE(
//                  QString::fromUtf16((const ushort*)findData.cFileName),
//                  QString::fromLocal8Bit(findDataA.cFileName)
//                  );

//               fa.m_path = fa.m_name;
//               fa.m_absoluteFilePath = absPath + "/" + fa.m_name;
//               fa.m_url.setPath( fa.m_absoluteFilePath );
//               if ( fa.m_name!="." && fa.m_name!=".." )
//                  pDirList->push_back( fa );
//            }
//            FindClose( searchHandle );
//         }
//         else
//         {
//            QDir::setCurrent( currentPath ); // restore current path
//            return false;
//         }
//#endif
//      }
//      QDir::setCurrent( currentPath ); // restore current path
//   }
//   else
//   {
////      KIO::ListJob* pListJob=0;
////      pListJob = KIO::listDir( m_pFileAccess->m_url, KIO::HideProgressInfo, true /*bFindHidden*/ );
////
////      m_bSuccess = false;
////      if ( pListJob!=0 )
////      {
////         connect( pListJob, SIGNAL( entries( KIO::Job*, const KIO::UDSEntryList& ) ),
////                  this,     SLOT( slotListDirProcessNewEntries( KIO::Job*, const KIO::UDSEntryList& )) );
////         connect( pListJob, SIGNAL( result( KJob* )),
////                  this,     SLOT( slotSimpleJobResult(KJob*) ) );
////
////         connect( pListJob, SIGNAL( infoMessage(KJob*, const QString&)),
////                  this,     SLOT( slotListDirInfoMessage(KJob*, const QString&) ));
////
////         // This line makes the transfer via fish unreliable.:-(
////         //connect( pListJob, SIGNAL(percent(KJob*,unsigned long)), this, SLOT(slotPercent(KJob*, unsigned long)));
////
////         g_pProgressDialog->enterEventLoop( pListJob,
////            i18n("Listing directory: %1",m_pFileAccess->prettyAbsPath()) );
////      }
//   }

//   CvsIgnoreList cvsIgnoreList;
//   if ( bUseCvsIgnore )
//   {
//      cvsIgnoreList.init( *m_pFileAccess, cvsIgnoreExists(pDirList) );
//   }
//#ifdef _WIN32
//   bool bCaseSensitive = false;
//#else
//   bool bCaseSensitive = true;
//#endif

//   // Now remove all entries that don't match:
//   t_DirectoryList::iterator i;
//   for( i = pDirList->begin(); i!=pDirList->end();  )
//   {
//      t_DirectoryList::iterator i2=i;
//      ++i2;
//      QString fn = i->fileName();
//      if (  (!bFindHidden && i->isHidden() )
//            ||
//            (i->isFile() &&
//               ( !wildcardMultiMatch( filePattern, i->fileName(), bCaseSensitive ) ||
//                 wildcardMultiMatch( fileAntiPattern, i->fileName(), bCaseSensitive ) ) )
//            ||
//            (i->isDir() && wildcardMultiMatch( dirAntiPattern, i->fileName(), bCaseSensitive ) )
//            ||
//            cvsIgnoreList.matches( i->fileName(), bCaseSensitive )
//         )
//      {
//         // Remove it
//         pDirList->erase( i );
//         i = i2;
//      }
//      else
//      {
//         ++i;
//      }
//   }

//   if ( bRecursive )
//   {
//      t_DirectoryList subDirsList;

//      t_DirectoryList::iterator i;
//      for( i = m_pDirList->begin(); i!=m_pDirList->end(); ++i )
//      {
//         if  ( i->isDir() && (!i->isSymLink() || m_bFollowDirLinks))
//         {
//            t_DirectoryList dirList;
//            i->listDir( &dirList, bRecursive, bFindHidden,
//               filePattern, fileAntiPattern, dirAntiPattern, bFollowDirLinks, bUseCvsIgnore );

//            t_DirectoryList::iterator j;
//            for( j = dirList.begin(); j!=dirList.end(); ++j )
//            {
//               j->m_path = i->fileName() + "/" + j->m_path;
//            }

//            // append data onto the main list
//            subDirsList.splice( subDirsList.end(), dirList );
//         }
//      }

//      m_pDirList->splice( m_pDirList->end(), subDirsList );
//   }

//   return m_bSuccess;
//}

bool FileAccessJobHandler::listDir( t_DirectoryList* pDirList, bool bRecursive, bool bFindHidden, const QString& filePattern,
   const QString& fileAntiPattern, const QString& dirAntiPattern, bool bFollowDirLinks, bool bUseCvsIgnore )
{
   ProgressProxy pp;
   m_pDirList = pDirList;
   m_pDirList->clear();
   m_bFindHidden = bFindHidden;
   m_bRecursive = bRecursive;
   m_bFollowDirLinks = bFollowDirLinks;  // Only relevant if bRecursive==true.
   m_fileAntiPattern = fileAntiPattern;
   m_filePattern = filePattern;
   m_dirAntiPattern = dirAntiPattern;

   if ( pp.wasCancelled() )
      return true; // Cancelled is not an error.

   pp.setInformation( tr("Reading directory: ") + m_pFileAccess->absoluteFilePath(), 0, false );

   if( m_pFileAccess->isLocal() )
   {
      QString currentPath = QDir::currentPath();
      m_bSuccess = QDir::setCurrent( m_pFileAccess->absoluteFilePath() );
      if ( m_bSuccess )
      {
#ifndef _WIN32
         m_bSuccess = true;
         QDir dir( "." );

         dir.setSorting( QDir::Name | QDir::DirsFirst );
         dir.setFilter( QDir::Files | QDir::Dirs | /* from KDE3 QDir::TypeMaskDirs | */ QDir::Hidden | QDir::System );

         QFileInfoList fiList = dir.entryInfoList();
         if ( fiList.isEmpty() )
         {
            // No Permission to read directory or other error.
            m_bSuccess = false;
         }
         else
         {
            foreach ( QFileInfo fi, fiList )       // for each file...
            {
               if ( fi.fileName() == "." ||  fi.fileName()==".." )
                  continue;

//               FileAccess fa;
//               fa.setFile( fi, m_pFileAccess );
//               pDirList->push_back( fa );
               pDirList->push_back( FileAccess( nicePath(fi) ) );
            }
         }
#else
         QString pattern ="*.*";
         WIN32_FIND_DATA findData;

         Qt::HANDLE searchHandle = FindFirstFileW( (const wchar_t*)pattern.utf16(), &findData );

         if ( searchHandle != INVALID_HANDLE_VALUE )
         {
            QString absPath = m_pFileAccess->absoluteFilePath();
            QString relPath = m_pFileAccess->filePath();
            bool bFirst=true;
            while( ! pp.wasCancelled() )
            {
               if (!bFirst)
               {
                  if ( ! FindNextFileW(searchHandle,&findData) )
                     break;
               }
               bFirst = false;
               FileAccess fa;

               fa.m_path = QString::fromUtf16((const ushort*)findData.cFileName);
               if ( fa.m_path!="." && fa.m_path!=".." )
               {
                  fa.m_size = ( qint64( findData.nFileSizeHigh ) << 32 ) + findData.nFileSizeLow;

                  FILETIME ft;
                  SYSTEMTIME t;
                  FileTimeToLocalFileTime( &findData.ftLastWriteTime, &ft ); FileTimeToSystemTime(&ft,&t);
                  fa.m_modificationTime = QDateTime( QDate(t.wYear, t.wMonth, t.wDay), QTime(t.wHour, t.wMinute, t.wSecond) );
                  //FileTimeToLocalFileTime( &findData.ftLastAccessTime, &ft ); FileTimeToSystemTime(&ft,&t);
                  //fa.m_accessTime       = QDateTime( QDate(t.wYear, t.wMonth, t.wDay), QTime(t.wHour, t.wMinute, t.wSecond) );
                  //FileTimeToLocalFileTime( &findData.ftCreationTime, &ft ); FileTimeToSystemTime(&ft,&t);
                  //fa.m_creationTime     = QDateTime( QDate(t.wYear, t.wMonth, t.wDay), QTime(t.wHour, t.wMinute, t.wSecond) );

                  int  a = findData.dwFileAttributes;
                  fa.m_bWritable   = ( a & FILE_ATTRIBUTE_READONLY) == 0;
                  fa.m_bDir        = ( a & FILE_ATTRIBUTE_DIRECTORY ) != 0;
                  fa.m_bFile       = !fa.m_bDir;
                  fa.m_bHidden     = ( a & FILE_ATTRIBUTE_HIDDEN) != 0;

                  //fa.m_bExecutable = false; // Useless on windows
                  fa.m_bExists     = true;
                  //fa.m_bReadable   = true;
                  //fa.m_bLocal      = true;
                  //fa.m_bValidData  = true;
                  fa.m_bSymLink    = false;
                  //fa.m_fileType    = 0;


                  //fa.m_filePath = fa.m_name;
                  //fa.m_absoluteFilePath = absPath + "/" + fa.m_name;
                  //fa.m_url.setPath( fa.m_absoluteFilePath );

                  fa.m_path = fa.m_name;
                  fa.m_absoluteFilePath = absPath + "/" + fa.m_name;
                  fa.m_url.setPath( fa.m_absoluteFilePath );
                  if ( fa.m_name!="." && fa.m_name!=".." )
                      pDirList->push_back( fa );

//                  if ( fa.d() )
//                     fa.m_pData->m_pParent = m_pFileAccess;
//                  else
//                     fa.m_pParent = m_pFileAccess;
//                  pDirList->push_back( fa );
               }
            }
            FindClose( searchHandle );
         }
         else
         {
            QDir::setCurrent( currentPath ); // restore current path
            return false;
         }
#endif
      }
      QDir::setCurrent( currentPath ); // restore current path
   }
   else
   {
//      KIO::ListJob* pListJob=0;
//      pListJob = KIO::listDir( m_pFileAccess->url(), KIO::HideProgressInfo, true /*bFindHidden*/ );

//      m_bSuccess = false;
//      if ( pListJob!=0 )
//      {
//         connect( pListJob, SIGNAL( entries( KIO::Job*, const KIO::UDSEntryList& ) ),
//                  this,     SLOT( slotListDirProcessNewEntries( KIO::Job*, const KIO::UDSEntryList& )) );
//         connect( pListJob, SIGNAL( result( KJob* )),
//                  this,     SLOT( slotSimpleJobResult(KJob*) ) );

//         connect( pListJob, SIGNAL( infoMessage(KJob*, const QString&)),
//                  &pp,      SLOT( slotListDirInfoMessage(KJob*, const QString&) ));

//         // This line makes the transfer via fish unreliable.:-(
//         //connect( pListJob, SIGNAL(percent(KJob*,unsigned long)), &pp, SLOT(slotPercent(KJob*, unsigned long)));

//         ProgressProxy::enterEventLoop( pListJob,
//            i18n("Listing directory: %1",m_pFileAccess->prettyAbsPath()) );
//      }
   }

   CvsIgnoreList cvsIgnoreList;
   if ( bUseCvsIgnore )
   {
      cvsIgnoreList.init( *m_pFileAccess, cvsIgnoreExists(pDirList) );
   }
#if defined(_WIN32) || defined(Q_OS_OS2)
   bool bCaseSensitive = false;
#else
   bool bCaseSensitive = true;
#endif

   // Now remove all entries that don't match:
   t_DirectoryList::iterator i;
   for( i = pDirList->begin(); i!=pDirList->end();  )
   {
      t_DirectoryList::iterator i2=i;
      ++i2;
      QString fn = i->fileName();
      if (  (!bFindHidden && i->isHidden() )
            ||
            (i->isFile() &&
               ( !wildcardMultiMatch( filePattern, fn, bCaseSensitive ) ||
                 wildcardMultiMatch( fileAntiPattern, fn, bCaseSensitive ) ) )
            ||
            (i->isDir() && wildcardMultiMatch( dirAntiPattern, fn, bCaseSensitive ) )
            ||
            cvsIgnoreList.matches( fn, bCaseSensitive )
         )
      {
         // Remove it
         pDirList->erase( i );
         i = i2;
      }
      else
      {
         ++i;
      }
   }

   if ( bRecursive )
   {
      t_DirectoryList subDirsList;

      t_DirectoryList::iterator i;
      for( i = m_pDirList->begin(); i!=m_pDirList->end(); ++i )
      {
         if  ( i->isDir() && (!i->isSymLink() || m_bFollowDirLinks))
         {
            t_DirectoryList dirList;
            i->listDir( &dirList, bRecursive, bFindHidden,
               filePattern, fileAntiPattern, dirAntiPattern, bFollowDirLinks, bUseCvsIgnore );

            t_DirectoryList::iterator j;
            for( j = dirList.begin(); j!=dirList.end(); ++j )
            {
                j->m_path = i->fileName() + "/" + j->m_path;
//               if ( j->parent()==0 )
//                  j->m_filePath = i->fileName() + "/" + j->m_filePath;
            }

            // append data onto the main list
            subDirsList.splice( subDirsList.end(), dirList );
         }
      }

      m_pDirList->splice( m_pDirList->end(), subDirsList );
   }

   return m_bSuccess;
}


//void FileAccessJobHandler::slotListDirProcessNewEntries( KIO::Job*, const KIO::UDSEntryList& l )
//{
//   KUrl parentUrl( m_pFileAccess->m_absoluteFilePath );
//
//   KIO::UDSEntryList::ConstIterator i;
//   for ( i=l.begin(); i!=l.end(); ++i )
//   {
//      const KIO::UDSEntry& e = *i;
//      FileAccess fa;
//      fa.setUdsEntry( e );
//
//      if ( fa.filePath() != "." && fa.filePath() != ".." )
//      {
//         fa.m_url = parentUrl;
//         fa.m_url.addPath( fa.filePath() );
//         fa.m_absoluteFilePath = fa.m_url.url();
//         m_pDirList->push_back( fa );
//      }
//   }
//}

//void FileAccessJobHandler::slotListDirInfoMessage( KJob*, const QString& msg )
//{
//   g_pProgressDialog->setInformation( msg, 0.0 );
//}
//
//void FileAccessJobHandler::slotPercent( KJob*, unsigned long percent )
//{
//   g_pProgressDialog->setCurrent( percent/100.0 );
//}


ProgressDialog::ProgressDialog( QWidget* pParent )
: QDialog( pParent )
{
   setObjectName("ProgressDialog");
   m_bStayHidden = false;
   setModal(true);
   QVBoxLayout* layout = new QVBoxLayout(this);

   m_pInformation = new QLabel( " ", this );
   layout->addWidget( m_pInformation );

   m_pProgressBar = new QProgressBar();
   m_pProgressBar->setRange(0,1000);
   layout->addWidget( m_pProgressBar );

   m_pSubInformation = new QLabel( " ", this);
   layout->addWidget( m_pSubInformation );

   m_pSubProgressBar = new QProgressBar();
   m_pSubProgressBar->setRange(0,1000);
   layout->addWidget( m_pSubProgressBar );

   m_pSlowJobInfo = new QLabel( " ", this);
   layout->addWidget( m_pSlowJobInfo );

   QHBoxLayout* hlayout = new QHBoxLayout();
   layout->addLayout(hlayout);
   hlayout->addStretch(1);
   m_pAbortButton = new QPushButton( tr("&Cancel"), this);
   hlayout->addWidget( m_pAbortButton );
   connect( m_pAbortButton, SIGNAL(clicked()), this, SLOT(slotAbort()) );

   m_progressDelayTimer = 0;
   resize( 400, 100 );
   m_t1.start();
   m_t2.start();
   m_bWasCancelled = false;
   //m_pJob = 0;
}

void ProgressDialog::setStayHidden( bool bStayHidden )
{
   m_bStayHidden = bStayHidden;
}

void ProgressDialog::push()
{
   ProgressLevelData pld;
   if ( !m_progressStack.empty() )
   {
      pld.m_dRangeMax = m_progressStack.back().m_dSubRangeMax;
      pld.m_dRangeMin = m_progressStack.back().m_dSubRangeMin;
   }
   else
   {
      m_bWasCancelled = false;
      m_t1.restart();
      m_t2.restart();
      if ( !m_bStayHidden )
         show();
   }

   m_progressStack.push_back( pld );
}

void ProgressDialog::pop( bool bRedrawUpdate )
{
   if ( !m_progressStack.empty() )
   {
      m_progressStack.pop_back();
      if ( m_progressStack.empty() )
         hide();
      else
         recalc(bRedrawUpdate);
   }
}

void ProgressDialog::setInformation(const QString& info, double dCurrent, bool bRedrawUpdate )
{
   if ( m_progressStack.empty() )
      return;
   ProgressLevelData& pld = m_progressStack.back();
   pld.m_dCurrent = dCurrent;
   int level = m_progressStack.size();
   if ( level==1 )
   {
      m_pInformation->setText( info );
      m_pSubInformation->setText("");
   }
   else if ( level==2 )
   {
      m_pSubInformation->setText( info );
   }
   recalc(bRedrawUpdate);
}

void ProgressDialog::setInformation(const QString& info, bool bRedrawUpdate )
{
   if ( m_progressStack.empty() )
      return;
   //ProgressLevelData& pld = m_progressStack.back();
   int level = m_progressStack.size();
   if ( level==1 )
   {
      m_pInformation->setText( info );
      m_pSubInformation->setText( "" );
   }
   else if ( level==2 )
   {
      m_pSubInformation->setText( info );
   }
   recalc(bRedrawUpdate);
}

void ProgressDialog::setMaxNofSteps( int maxNofSteps )
{
   if ( m_progressStack.empty() )
      return;
   ProgressLevelData& pld = m_progressStack.back();
   pld.m_maxNofSteps = maxNofSteps;
   pld.m_dCurrent = 0;
}

void ProgressDialog::step( bool bRedrawUpdate )
{
   if ( m_progressStack.empty() )
      return;
   ProgressLevelData& pld = m_progressStack.back();
   pld.m_dCurrent += 1.0/pld.m_maxNofSteps;
   recalc(bRedrawUpdate);
}

void ProgressDialog::setCurrent( double dSubCurrent, bool bRedrawUpdate )
{
   if ( m_progressStack.empty() )
      return;
   ProgressLevelData& pld = m_progressStack.back();
   pld.m_dCurrent = dSubCurrent;
   recalc( bRedrawUpdate );
}

// The progressbar goes from 0 to 1 usually.
// By supplying a subrange transformation the subCurrent-values
// 0 to 1 will be transformed to dMin to dMax instead.
// Requirement: 0 < dMin < dMax < 1
void ProgressDialog::setRangeTransformation( double dMin, double dMax )
{
   if ( m_progressStack.empty() )
      return;
   ProgressLevelData& pld = m_progressStack.back();
   pld.m_dRangeMin = dMin;
   pld.m_dRangeMax = dMax;
   pld.m_dCurrent = 0;
}

void ProgressDialog::setSubRangeTransformation( double dMin, double dMax )
{
   if ( m_progressStack.empty() )
      return;
   ProgressLevelData& pld = m_progressStack.back();
   pld.m_dSubRangeMin = dMin;
   pld.m_dSubRangeMax = dMax;
}

void qt_enter_modal(QWidget*);
void qt_leave_modal(QWidget*);

//void ProgressDialog::enterEventLoop( KJob* pJob, const QString& jobInfo )
//{
//   m_pJob = pJob;
//   m_pSlowJobInfo->setText("");
//   m_currentJobInfo = jobInfo;
//   if ( m_progressDelayTimer )
//      killTimer( m_progressDelayTimer );
//   m_progressDelayTimer = startTimer( 3000 ); /* 3 s delay */
//
//   // instead of using exec() the eventloop is entered and exited often without hiding/showing the window.
//   //qt_enter_modal(this);
//   QEventLoop* pEventLoop = new QEventLoop(this);
//   m_eventLoopStack.push_back( pEventLoop );
//   pEventLoop->exec(); // this function only returns after ProgressDialog::exitEventLoop() is called.
//   delete pEventLoop;
//   m_eventLoopStack.pop_back();
//   //qt_leave_modal(this);
//}

void ProgressDialog::exitEventLoop()
{
   if ( m_progressDelayTimer )
      killTimer( m_progressDelayTimer );
   m_progressDelayTimer = 0;
   //m_pJob = 0;
   if (!m_eventLoopStack.empty())
      m_eventLoopStack.back()->exit();
}

void ProgressDialog::recalc( bool bUpdate )
{
   if ( m_progressDelayTimer )
      killTimer( m_progressDelayTimer );
   m_progressDelayTimer = startTimer( 3000 ); /* 3 s delay */

   int level = m_progressStack.size();
   if( ( bUpdate && level==1) || m_t1.elapsed()>200 )
   {
      if (m_progressStack.empty() )
      {
         m_pProgressBar->setValue( 0 );
         m_pSubProgressBar->setValue( 0 );
      }
      else
      {
         std::list<ProgressLevelData>::iterator i = m_progressStack.begin();
         m_pProgressBar->setValue( int( 1000.0 * ( i->m_dCurrent * (i->m_dRangeMax - i->m_dRangeMin) + i->m_dRangeMin ) ) );
         ++i;
         if ( i!=m_progressStack.end() )
            m_pSubProgressBar->setValue( int( 1000.0 * ( i->m_dCurrent * (i->m_dRangeMax - i->m_dRangeMin) + i->m_dRangeMin ) ) );
         else
            m_pSubProgressBar->setValue( int( 1000.0 * m_progressStack.front().m_dSubRangeMin ) );
      }

      if ( !m_bStayHidden && !isVisible() )
         show();
      qApp->processEvents();
      m_t1.restart();
   }
}


#include <qtimer.h>
void ProgressDialog::show()
{
   if ( m_progressDelayTimer )
      killTimer( m_progressDelayTimer );
   m_progressDelayTimer = 0;
   if ( !isVisible() && (parentWidget()==0 || parentWidget()->isVisible()) )
   {
      QDialog::show();
   }
}

void ProgressDialog::hide()
{
   if ( m_progressDelayTimer )
      killTimer( m_progressDelayTimer );
   m_progressDelayTimer = 0;
   // Calling QDialog::hide() directly doesn't always work. (?)
   QTimer::singleShot( 100, this, SLOT(delayedHide()) );
}

void ProgressDialog::delayedHide()
{
//   if (m_pJob!=0)
//   {
//      m_pJob->kill( KJob::Quietly );
//      m_pJob = 0;
//   }
   QDialog::hide();
   m_pInformation->setText( "" );

   //m_progressStack.clear();

   m_pProgressBar->setValue( 0 );
   m_pSubProgressBar->setValue( 0 );
   m_pSubInformation->setText("");
   m_pSlowJobInfo->setText("");
}

void ProgressDialog::reject()
{
   m_bWasCancelled = true;
   QDialog::reject();
}

void ProgressDialog::slotAbort()
{
   reject();
}

bool ProgressDialog::wasCancelled()
{
   if( m_t2.elapsed()>100 )
   {
      qApp->processEvents();
      m_t2.restart();
   }
   return m_bWasCancelled;
}


void ProgressDialog::timerEvent(QTimerEvent*)
{
   if( !isVisible() )
   {
      show();
   }
   m_pSlowJobInfo->setText( m_currentJobInfo );
}


ProgressProxy::ProgressProxy()
{
   g_pProgressDialog->push();
}

ProgressProxy::~ProgressProxy()
{
   g_pProgressDialog->pop(false);
}

void ProgressProxy::setInformation( const QString& info, bool bRedrawUpdate )
{
   g_pProgressDialog->setInformation( info, bRedrawUpdate );
}

void ProgressProxy::setInformation( const QString& info, double dCurrent, bool bRedrawUpdate )
{
   g_pProgressDialog->setInformation( info, dCurrent, bRedrawUpdate );
}

void ProgressProxy::setCurrent( double dCurrent, bool bRedrawUpdate  )
{
   g_pProgressDialog->setCurrent( dCurrent, bRedrawUpdate );
}

void ProgressProxy::step( bool bRedrawUpdate )
{
   g_pProgressDialog->step( bRedrawUpdate );
}

void ProgressProxy::setMaxNofSteps( int maxNofSteps )
{
   g_pProgressDialog->setMaxNofSteps( maxNofSteps );
}

bool ProgressProxy::wasCancelled()
{
   return g_pProgressDialog->wasCancelled();
}

void ProgressProxy::setRangeTransformation( double dMin, double dMax )
{
   g_pProgressDialog->setRangeTransformation( dMin, dMax );
}

void ProgressProxy::setSubRangeTransformation( double dMin, double dMax )
{
   g_pProgressDialog->setSubRangeTransformation( dMin, dMax );
}





//#include "fileaccess.moc"
