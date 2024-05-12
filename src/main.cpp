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

#include <QtGlobal> // Q_INIT_RESOURCE
#include <QObject>
#include <QString>

#include <QtSingleApplication> // QtSingleApplication
#include <utils/medium.h>      // Medium

#include "edytornc.h" // EdytorNc


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

	Medium &medium = Medium::instance();
	medium.addTranslationUnit("kdiff3");
	medium.updateTranslation();
	EdytorNc *mw = EdytorNc::instance();

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
