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


//#include <QApplication>
#include "edytornc.h"
#include "qtsingleapplication.h"

int main(int argc, char *argv[])
{   
    Q_INIT_RESOURCE(application);
    QtSingleApplication app("EdytorNC", argc, argv);

    QString txMessage;
    for(int i = 1; i < argc; ++i)
    {
        txMessage += argv[i];
        if(i < argc - 1)
          txMessage += ";";
    };

    if(app.sendMessage(txMessage))
        return 0;

    QTranslator qtTranslator;
    if(!qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
      qtTranslator.load("qt_" + QLocale::system().name(), app.applicationDirPath());
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("edytornc_" + QLocale::system().name(), app.applicationDirPath());
    app.installTranslator(&myappTranslator);

    //app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    edytornc *mw = new edytornc();

    for(int i = 1; i < argc; ++i)
      mw->openFile(argv[i]);

    mw->show();

    app.setActivationWindow(mw, false);

    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                     mw, SLOT(messReceived(const QString&)));

    QObject::connect(mw, SIGNAL(needToShow()), &app, SLOT(activateWindow()));

    return app.exec();

}

