/***************************************************************************
 *   Copyright (C) 2006-2018 by Artur Kozioł                               *
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

#include "qtsingleapplication.h"

#include "edytornc.h"


#define LOCALE_PATH         "/usr/share/edytornc/lang/"

int main(int argc, char *argv[])
{
    bool argProccesed;

    Q_INIT_RESOURCE(application);
    QtSingleApplication app("EdytorNC", argc, argv);

    QString txMessage;

    for (int i = 1; i < argc; ++i) {
        txMessage += argv[i];

        if (i < argc - 1) {
            txMessage += ";";
        }
    }

    if (app.sendMessage(txMessage)) {
        return 0;
    }

    QTranslator
    qtTranslator; // Try to load Qt translations from QLibraryInfo::TranslationsPath if this fails looks for translations in app dir.

    if (!qtTranslator.load("qt_" + QLocale::system().name(),
                           QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qtTranslator.load("qt_" + QLocale::system().name(), app.applicationDirPath());
    }

    app.installTranslator(&qtTranslator);

    QTranslator
    myappTranslator; // Try to load EdytorNC translations from LOCALE_PATH if this fails looks for translations in app dir.

    if (!myappTranslator.load("edytornc_" + QLocale::system().name(), LOCALE_PATH)) {
        myappTranslator.load("edytornc_" + QLocale::system().name(), app.applicationDirPath());
    }

    app.installTranslator(&myappTranslator);

    if (!myappTranslator.load("kdiff3_" + QLocale::system().name(), LOCALE_PATH)) {
        myappTranslator.load("kdiff3_" + QLocale::system().name(), app.applicationDirPath());
    }

    app.installTranslator(&myappTranslator);

    EdytorNc *mw = new EdytorNc();

    argProccesed = false;

    if (argc >= 3) {
        if (QString(argv[1]).trimmed() == "-diff") {
            mw->diffTwoFiles(QString(argv[2]).trimmed(), QString(argv[3]).trimmed());
            argProccesed = true;
        }
    }

    if (!argProccesed) {
        for (int i = 1; i < argc; ++i) {
            //qDebug() << argc << argv[i] << i;
            mw->openFile(argv[i]);
        }
    }

    mw->show();

    app.setActivationWindow(mw, false);

    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                     mw, SLOT(messReceived(const QString &)));

    QObject::connect(mw, SIGNAL(needToShow()), &app, SLOT(activateWindow()));

    return app.exec();
}
