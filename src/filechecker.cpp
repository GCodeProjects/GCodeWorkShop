/***************************************************************************
 *   Copyright (C) 2006-2015 by Artur Kozioł                               *
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


#include "filechecker.h"
#include "ui_filechecker.h"



FileChecker::FileChecker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileChecker)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    extensions.clear();

    connect(ui->fileTableWidget, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(filesTableCurrentCellChanged(int, int, int, int)));
    //connect(ui->fileTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(filesTableClicked(int, int)));

    okBtn = NULL;
    noBtn = NULL;
    prevRow = -1;

    showMaximized();

    createDiff();

    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("FileCheck");
    splitterState = settings.value("VSplitterState", QByteArray()).toByteArray();
    ui->vSplitter->restoreState(splitterState);
    settings.endGroup();

    ui->fileTableWidget->setAutoScroll(false);
}

//**************************************************************************************************
//
//**************************************************************************************************

FileChecker::~FileChecker()
{
    QSettings settings("EdytorNC", "EdytorNC");
    settings.beginGroup("FileCheck");
    settings.setValue("VSplitterState", ui->vSplitter->saveState());
    settings.endGroup();

    disconnect(ui->fileTableWidget, 0, 0, 0);
    if(diffApp)
        delete(diffApp);

    delete ui;
}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::setData(const QString startDir, QStringList readPaths, QStringList fileFilter)
{
    extensions.clear();
    extensions = fileFilter;
    readPathList.clear();
    readPathList = readPaths;
    savePath = startDir;

    ui->savePathLabel->setText(QDir::toNativeSeparators(savePath));

    disconnect(ui->readPathComboBox, 0, 0, 0);
    ui->readPathComboBox->clear();
    ui->readPathComboBox->insertItems(0, readPathList);
    //ui->readPathComboBox->setCurrentIndex(0);
    connect(ui->readPathComboBox, SIGNAL(currentIndexChanged(QString)), SLOT(readPathComboBoxChanged(QString)));
}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::findFiles()
{
    findFiles(savePath, extensions);
}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::findFiles(const QString startDir, QStringList fileFilter)
{
    int pos;
    QRegExp exp;
    QString comment_tx;
    qint64 size;
    QString line;
    QStringList files;

    QStringList labels;
    labels << tr("File Name") << tr("Info") << tr("Accept") << tr("Delete") << tr("Status") << tr("Size") << tr("Modification date");
    ui->fileTableWidget->setColumnCount(labels.size());
    ui->fileTableWidget->setHorizontalHeaderLabels(labels);


    exp.setCaseSensitivity(Qt::CaseInsensitive);
    exp.setPattern("\\([^\\n\\r]*\\)|;[^\\n\\r]*");

    pos = 0;


    QDir directory = QDir(startDir);

    files = directory.entryList(fileFilter, QDir::Files | QDir::NoSymLinks | QDir::Readable);

    //   progressDialog->setLabelText(tr("Searching in folder: \"%1\"").arg(QDir::toNativeSeparators(directory.absolutePath())));

    ui->fileTableWidget->setSortingEnabled(false);
    for(int i = 0; i < files.size(); ++i)
    {
        //      progressDialog->setRange(0, files.size());
        //      progressDialog->setValue(i);
        //      qApp->processEvents();

        //      if(progressDialog->wasCanceled())
        //         break;

        QFile file(directory.absoluteFilePath(files[i]));

        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);

            line = in.readAll();



            size = file.size();
            comment_tx.clear();
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

            //qDebug() << files[i] << size;

            QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
            fileNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            QTableWidgetItem *infoNameItem = new QTableWidgetItem(comment_tx);
            infoNameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
            sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QTableWidgetItem *dateItem = new QTableWidgetItem();
            dateItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            dateItem->setData(Qt::DisplayRole, QFileInfo(file).lastModified());


            int row = ui->fileTableWidget->rowCount();
            ui->fileTableWidget->insertRow(row);
            ui->fileTableWidget->setItem(row, 0, fileNameItem);
            ui->fileTableWidget->setItem(row, 1, infoNameItem);
            ui->fileTableWidget->setItem(row, 5, sizeItem);
            ui->fileTableWidget->setItem(row, 6, dateItem);

            file.close();



            QString path2 = ui->readPathComboBox->currentText();
            if(!path2.endsWith("/"))
                path2 = path2 + "/";

            path2 = path2 + files[i];

            comment_tx = "";
            if(QFile::exists(path2))
            {
                if(diffApp)
                {
                    //qDebug() << "DIFF" << path2 << directory.absoluteFilePath(files[i]);

                    //splitterState = ui->vSplitter->saveState();
                    diffApp->close();
                    //ui->vSplitter->restoreState(splitterState);
                    if(diffApp->completeInit(path2, directory.absoluteFilePath(files[i])))
                        comment_tx = tr("Equal");
                    else
                        comment_tx = tr("Changed");

                };
            }
            else
            {
                comment_tx = tr("New");
            };


            QTableWidgetItem *statusItem = new QTableWidgetItem(comment_tx);
            statusItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ui->fileTableWidget->setItem(row, 4, statusItem);


//            QLabel *lbl = new QLabel;
//            lbl->setAlignment(Qt::AlignCenter);
//            QPixmap pix(":/images/cancel.png");
//            QPixmap resPix = pix.scaled(17,17, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//            lbl->setPixmap(resPix);


//            ui->fileTableWidget->setCellWidget(row, 4, lbl);

        };
    };
    ui->fileTableWidget->setSortingEnabled(true);

    ui->fileTableWidget->resizeRowsToContents();
    ui->fileTableWidget->resizeColumnsToContents();

}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::createDiff()
{
    if(!diffApp)
    {
        diffApp = new KDiff3App(ui->vSplitter, "Diff", extensions);
        QList<int> sizes;
        sizes.clear();
        sizes.append(80);
        sizes.append(ui->vSplitter->height());
        ui->vSplitter->setSizes(sizes);
        diffApp->hide();
    }
    else
    {
        diffApp->show();
    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::acceptFile()
{
    int row = ui->fileTableWidget->currentRow();

    QTableWidgetItem *item = ui->fileTableWidget->item(row, 0);
    if(item)
    {
        QString dir = ui->savePathLabel->text();
        if(!dir.endsWith("/"))
            dir = dir + "/";

        QString path1 = dir + item->text();


        dir = ui->readPathComboBox->currentText();
        if(!dir.endsWith("/"))
            dir = dir + "/";

        QString path2 = dir + item->text();

        if(QFile::exists(path2))
            QFile::remove(path2);

        if(QFile::rename(path1, path2))
        {
            QPixmap pix(":/images/ok.png");
            QPixmap resPix = pix.scaled(17,17, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QLabel *lbl = new QLabel;
            lbl->setPixmap(resPix);
            lbl->setAlignment(Qt::AlignCenter);
            ui->fileTableWidget->setCellWidget(row, 2, lbl);
            ui->fileTableWidget->setCellWidget(row, 3, new QLabel);

            prevRow = -1;
        };
    };

}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::deleteFile()
{
    int row = ui->fileTableWidget->currentRow();

    QTableWidgetItem *item = ui->fileTableWidget->item(row, 0);
    if(item)
    {
        QString dir = ui->savePathLabel->text();
        if(!dir.endsWith("/"))
            dir = dir + "/";

        QString path1 = dir + item->text();

        if(QFile::exists(path1))
            if(QFile::remove(path1))
            {
                QPixmap pix(":/images/cancel.png");
                QPixmap resPix = pix.scaled(17,17, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                QLabel *lbl = new QLabel;
                lbl->setPixmap(resPix);
                lbl->setAlignment(Qt::AlignCenter);
                ui->fileTableWidget->setCellWidget(row, 2, new QLabel);
                ui->fileTableWidget->setCellWidget(row, 3, lbl);

                prevRow = -1;
            };
    };

}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::filesTableClicked(int row, int col)
{
//    Q_UNUSED(col);

//    QTableWidgetItem *item = ui->fileTableWidget->item(row, 0);
//    if(item)
//    {

//        QString path1 = ui->savePathLabel->text();
//        if(!path1.endsWith("/"))
//            path1 = path1 + "/";

//        QString path2 = ui->readPathComboBox->currentText();
//        if(!path2.endsWith("/"))
//            path2 = path2 + "/";

//        path1 = path1 + item->text();
//        path2 = path2 + item->text();



//        if(path1 == path2)
//            return;

//        if((prevRow >= 0) && (prevRow < ui->fileTableWidget->rowCount()))
//        {
//            ui->fileTableWidget->setCellWidget(prevRow, 2, new QLabel);
//            ui->fileTableWidget->setCellWidget(prevRow, 3, new QLabel);
//        };


//        if(QFile::exists(path1))
//        {
//            prevRow = row;

//            okBtn = new QToolButton();
//            okBtn->setIcon(QIcon(":/images/ok.png"));
//            okBtn->setToolTip(tr("Accept received file and move it to selected search path"));
//            connect(okBtn, SIGNAL(clicked()), this, SLOT(acceptFile()));

//            noBtn = new QToolButton();
//            noBtn->setIcon(QIcon(":/images/cancel.png"));
//            noBtn->setToolTip(tr("Delete received file"));
//            connect(noBtn, SIGNAL(clicked()), this, SLOT(deleteFile()));

//            ui->fileTableWidget->setCellWidget(row, 2, okBtn);
//            ui->fileTableWidget->setCellWidget(row, 3, noBtn);


//            createDiff();

//            if(diffApp)
//            {
//                splitterState = ui->vSplitter->saveState();
//                diffApp->close();
//                ui->vSplitter->restoreState(splitterState);
//                diffApp->completeInit(path2, path1);
//            };
//        }
//        else
//            prevRow = -1;
//    };
}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::readPathComboBoxChanged(QString text)
{
    Q_UNUSED(text);

    findFiles(ui->savePathLabel->text(), extensions);
}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::preliminaryDiff(QString file1, QString file2)
{

}

//**************************************************************************************************
//
//**************************************************************************************************

void FileChecker::filesTableCurrentCellChanged(int row, int col, int pRow, int pCol)
{
    Q_UNUSED(col);
    Q_UNUSED(pCol);



    QTableWidgetItem *item = ui->fileTableWidget->item(row, 0);
    if(item)
    {

        QString path1 = ui->savePathLabel->text();
        if(!path1.endsWith("/"))
            path1 = path1 + "/";

        QString path2 = ui->readPathComboBox->currentText();
        if(!path2.endsWith("/"))
            path2 = path2 + "/";

        path1 = path1 + item->text();
        path2 = path2 + item->text();



        if(path1 == path2)
            return;

        setUpdatesEnabled(false);
        if((pRow >= 0) && (pRow < ui->fileTableWidget->rowCount()))
        {
            ui->fileTableWidget->setCellWidget(pRow, 2, new QLabel);
            ui->fileTableWidget->setCellWidget(pRow, 3, new QLabel);
        };


        if(QFile::exists(path1))
        {
            okBtn = new QToolButton();
            okBtn->setIcon(QIcon(":/images/ok.png"));
            okBtn->setToolTip(tr("Accept received file and move it to selected search path"));
            connect(okBtn, SIGNAL(clicked()), this, SLOT(acceptFile()));

            noBtn = new QToolButton();
            noBtn->setIcon(QIcon(":/images/cancel.png"));
            noBtn->setToolTip(tr("Delete received file"));
            connect(noBtn, SIGNAL(clicked()), this, SLOT(deleteFile()));

            ui->fileTableWidget->setCellWidget(row, 2, okBtn);
            ui->fileTableWidget->setCellWidget(row, 3, noBtn);


            createDiff();

            if(diffApp)
            {
                //diffApp->setUpdatesEnabled(false);
//                splitterState = ui->vSplitter->saveState();
//                diffApp->close();
//                ui->vSplitter->restoreState(splitterState);
                diffApp->completeInit(path2, path1);
                //diffApp->setUpdatesEnabled(true);

                ui->fileTableWidget->setFocus();
            };
        };
        setUpdatesEnabled(true);
    };

}

//**************************************************************************************************
//
//**************************************************************************************************
