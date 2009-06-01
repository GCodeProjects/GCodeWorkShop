/***************************************************************************
 *   Copyright (C) 2009 by Artur Kozioł                                    *
 *   artkoz@poczta.onet.pl                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

 /***************************************************************************
                           customfdialog.h  -  description
                              -------------------
     begin                : Fri Nov 30 2001
     copyright            : (C) 2001 by Franz Schmid
     email                : Franz.Schmid@altmuehlnet.de
  ***************************************************************************/
 
 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/
 
#ifndef CUSTOMFDIALOG_H
#define CUSTOMFDIALOG_H
 
#include <QDialog>
#include <QFileIconProvider>
#include <QLabel>
#include <QFileDialog>
#include <QTextEdit>

#include "highlighter.h"
#include "commoninc.h"


class QPushButton;
class QComboBox;
class QCheckBox;
class QHBoxLayout;
class QVBoxLayout;
class QFrame;
class ScFileWidget;
class QIcon;

 
class ImIconProvider : public QFileIconProvider
{
public:
    ImIconProvider();
    ~ImIconProvider() {};
    QIcon icon(const QFileInfo &fi) const;
    QIcon ncpm;
    QIcon cncpm;
    QIcon minpm;
    QIcon ancpm;
    QIcon txtpm;
};

//**************************************************************************************************
//
//**************************************************************************************************

class ScFileWidget : public QFileDialog
{
    Q_OBJECT
 
public:
    ScFileWidget(QWidget * parent); 
    QString selectedFile();

};

//**************************************************************************************************
//
//**************************************************************************************************

class FDialogPreview : public QTextEdit
{
    Q_OBJECT
public:
    FDialogPreview(QWidget *pa);
    ~FDialogPreview() {};
    void updtPix();
    void GenPreview(QString name);

public slots:
    void setHColors(const _h_colors colors);

private:
    Highlighter *highlighter;

};

//**************************************************************************************************
//
//**************************************************************************************************

typedef enum {
    fdNone = 0,
    fdShowPreview = 1, // display text/image previewer
    fdExistingFiles = 2, // When set, set to the QFileDialog::ExistingFile mode when true, QFileDialog::AnyFile otherwise
    fdCompressFile = 4, // Show and handle the "Compress" behaviour and CheckBox (true)
    fdSave = 8, // Show and handle text codecs and ComboBox (true)
    fdDirectoriesOnly = 16 // Show only directories
} fdFlags;

//**************************************************************************************************
//
//**************************************************************************************************

class CustomFDialog : public QDialog
{
    Q_OBJECT
public:
    CustomFDialog(QWidget *parent, int flags = fdExistingFiles);
    ~CustomFDialog();

    void setExtension(QString e);
    QString extension();

    QCheckBox* SaveZip;
    QCheckBox* WithFonts;
    QCheckBox* WithProfiles;
    QFrame* Layout;
    QFrame* LayoutC;
    QComboBox *TxCodeM;
    QLabel *TxCodeT;
    ScFileWidget *fileDialog;
    FDialogPreview *pw;
    QCheckBox *showPreview;
    QPushButton* OKButton;
    QPushButton* CancelB;
    void setSelection(QString );
    QString selectedFile();
    void addWidgets(QWidget *widgets);


private slots:
    void fileClicked(const QString &path);
    void togglePreview();
    void resizeEvent(QResizeEvent *event);

public slots:
    void setNameFilters(const QStringList filters);
    bool restoreState(const QByteArray & state);
    void selectNameFilter(const QString & filter);
    QByteArray saveState() const;
    QString selectedNameFilter() const;
    QStringList selectedFiles() const;
    void setHighlightColors(const _h_colors colors);
    void selectFile(const QString &filename);
    void setConfirmOverwrite(bool enabled);


protected:
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout1C;
    QString ext;
    QString extZip;
    int optionFlags;
    bool previewIsShown;

private:
    QStringList fileFilters;
    QString fileName;



};

#endif
