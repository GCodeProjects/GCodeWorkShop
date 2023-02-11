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

#include <QObject>
#include <QString>

#include <QtSingleApplication> // QtSingleApplication
#include <utils/medium.h>      // Medium

#include "commapp.h"      // CommApp


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);
    QtSingleApplication app("CommApp", argc, argv);

    QString txMessage;

    for (int i = 1; i < argc; ++i) {
        txMessage += argv[i];

        if (i < argc - 1) {
            txMessage += ";";
        }
    };

    if (app.sendMessage(txMessage)) {
        return 0;
    }

    Medium &medium = Medium::instance();
    medium.updateTranslation();
    CommApp *mw = new CommApp();
    mw->show();

    app.setActivationWindow(mw, false);
    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                     mw, SLOT(messReceived(const QString &)));

    QObject::connect(mw, SIGNAL(needToShow()), &app, SLOT(activateWindow()));
    return app.exec();
}
