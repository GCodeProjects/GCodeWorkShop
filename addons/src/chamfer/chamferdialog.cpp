/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of EdytorNC.
 *
 *  EdytorNC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Enable the M_PI constant in MSVC
// see https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/math-constant
#define _USE_MATH_DEFINES

#include <cmath>    // for tan, M_PI, atan

// IWYU pragma: no_forward_declare QValidator
// IWYU pragma: no_forward_declare QWidget
#include <QCheckBox>        // for QCheckBox
#include <QDoubleValidator> // for QDoubleValidator
#include <QLineEdit>        // for QLineEdit
#include <QLocale>          // for QLocale
#include <QPalette>         // for QPalette
#include <QPushButton>      // for QPushButton
#include <QValidator>       // for QValidator
#include <QWidget>          // for QWidget

#include "chamferdialog.h"


ChamferDialog::ChamferDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Chamfer"));

    connect(angCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    QValidator *angInputValid = new QDoubleValidator(0.1, 90, 3, this);
    angInput->setValidator(angInputValid);
    connect(zlCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    QValidator *zlInputValid = new QDoubleValidator(0.1, 1000, 3, this);
    zlInput->setValidator(zlInputValid);
    connect(dlCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    QValidator *dlInputValid = new QDoubleValidator(0.1, 1000, 3, this);
    dlInput->setValidator(dlInputValid);
    connect(xoCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    QValidator *xoInputValid = new QDoubleValidator(0, 2000, 3, this);
    xoInput->setValidator(xoInputValid);
    connect(xdCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    QValidator *xdInputValid = new QDoubleValidator(0.001, 2000, 3, this);
    xdInput->setValidator(xdInputValid);

    connect(computeButton, SIGNAL(clicked()), SLOT(computeButtonClicked()));
    connect(closeButton, SIGNAL(clicked()), SLOT(close()));

    //inputChanged();
    //setMaximumSize(width(), height());

    connect(angInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(zlInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(dlInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(xdInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(xoInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));

    checkBoxToggled();
}

ChamferDialog::~ChamferDialog()
{
}

void ChamferDialog::computeButtonClicked()
{
    double ZL, XL, X1, X2, ang;
    bool ok;

    if (angCheckBox->isChecked() && zlCheckBox->isChecked()) {
        if (!zlInput->hasAcceptableInput() || !angInput->hasAcceptableInput()) {
            return;
        }

        ang = QLocale().toDouble(angInput->text(), &ok);
        ZL = QLocale().toDouble(zlInput->text(), &ok);

        XL = tan((M_PI / 180) * ang) * (ZL * 2);
        dlInput->setText(QString("%1").arg(XL, 0, 'f', 3));

        if (xoCheckBox->isChecked() && xoInput->hasAcceptableInput()) {
            X1 = QLocale().toDouble(xoInput->text(), &ok);
            X2 = X1 + XL;
            xdInput->setText(QString("%1").arg(X2, 0, 'f', 3));

        }

        if (xdCheckBox->isChecked() && xdInput->hasAcceptableInput()) {
            X2 = QLocale().toDouble(xdInput->text(), &ok);
            X1 = X2 - XL;
            xoInput->setText(QString("%1").arg(X1, 0, 'f', 3));

        }

        return;
    }

    if (angCheckBox->isChecked() && dlCheckBox->isChecked()) {
        if (!dlInput->hasAcceptableInput() || !angInput->hasAcceptableInput()) {
            return;
        }

        ang = QLocale().toDouble(angInput->text(), &ok);
        XL = QLocale().toDouble(dlInput->text(), &ok);

        ZL = (XL / 2) / tan((M_PI / 180) * ang) ;
        zlInput->setText(QString("%1").arg(ZL, 0, 'f', 3));

        return;
    }

    if (angCheckBox->isChecked() && xoCheckBox->isChecked() && xdCheckBox->isChecked()) {
        if (!xdInput->hasAcceptableInput() || !xoInput->hasAcceptableInput()
                || !angInput->hasAcceptableInput()) {
            return;
        }

        ang = QLocale().toDouble(angInput->text(), &ok);
        X1 = QLocale().toDouble(xoInput->text(), &ok);
        X2 = QLocale().toDouble(xdInput->text(), &ok);
        XL = X2 - X1;
        dlInput->setText(QString("%1").arg(XL, 0, 'f', 3));
        ZL = (XL / 2) / tan((M_PI / 180) * ang) ;
        zlInput->setText(QString("%1").arg(ZL, 0, 'f', 3));

        return;
    }

    if (zlCheckBox->isChecked() && dlCheckBox->isChecked()) {
        if (!zlInput->hasAcceptableInput() || !dlInput->hasAcceptableInput()) {
            return;
        }

        XL = QLocale().toDouble(dlInput->text(), &ok);
        ZL = QLocale().toDouble(zlInput->text(), &ok);
        ang = (atan((XL / 2) / ZL)) / (M_PI / 180);

        angInput->setText(QString("%1").arg(ang, 0, 'f', 3));

        return;
    }
}

void ChamferDialog::checkBoxToggled()
{
    bool ena;

    ena = (angCheckBox->isChecked() && zlCheckBox->isChecked());
    dlCheckBox->setEnabled(!ena && (!xdCheckBox->isChecked() || !xoCheckBox->isChecked()));

    ena = (angCheckBox->isChecked() && dlCheckBox->isChecked());
    zlCheckBox->setEnabled(!ena && (!xdCheckBox->isChecked() || !xoCheckBox->isChecked()));

    //zlCheckBox->setEnabled(!xdCheckBox->isChecked() || !xoCheckBox->isChecked());
    //dlCheckBox->setEnabled(!xdCheckBox->isChecked() || !xoCheckBox->isChecked());

    xoCheckBox->setEnabled((!xdCheckBox->isChecked()) || !zlCheckBox->isChecked());
    xdCheckBox->setEnabled((!xoCheckBox->isChecked()) || !zlCheckBox->isChecked());

    if (dlCheckBox->isChecked()) {
        xoCheckBox->setEnabled(false);
        xdCheckBox->setEnabled(false);
    }

    angCheckBox->setEnabled(!zlCheckBox->isChecked() || !dlCheckBox->isChecked());

    if (!angCheckBox->isEnabled()) {
        angCheckBox->setChecked(false);
        angInput->setText("0");
    }

    if (!zlCheckBox->isEnabled()) {
        zlCheckBox->setChecked(false);
        zlInput->setText("0");
    }

    if (!dlCheckBox->isEnabled()) {
        dlCheckBox->setChecked(false);
        dlInput->setText("0");
    }

    if (!xdCheckBox->isEnabled()) {
        xdCheckBox->setChecked(false);
        xdInput->setText("0");
    }

    if (!xoCheckBox->isEnabled()) {
        xoCheckBox->setChecked(false);
        xoInput->setText("0");
    }

    angInput->setReadOnly(!angCheckBox->isChecked());
    zlInput->setReadOnly(!zlCheckBox->isChecked());
    dlInput->setReadOnly(!dlCheckBox->isChecked());
    xdInput->setReadOnly(!xdCheckBox->isChecked());
    xoInput->setReadOnly(!xoCheckBox->isChecked());

    computeButton->setEnabled(false);

    inputChanged();
}

void ChamferDialog::inputChanged()
{
    QPalette palette;
    palette.setColor(angInput->foregroundRole(), Qt::red);

    if (angInput->hasAcceptableInput()) {
        angInput->setPalette(QPalette());
    } else {
        angInput->setPalette(palette);
    }

    if (zlInput->hasAcceptableInput()) {
        zlInput->setPalette(QPalette());
    } else {
        zlInput->setPalette(palette);
    }

    if (dlInput->hasAcceptableInput()) {
        dlInput->setPalette(QPalette());
    } else {
        dlInput->setPalette(palette);
    }

    if (xdInput->hasAcceptableInput()) {
        xdInput->setPalette(QPalette());
    } else {
        xdInput->setPalette(palette);
    }

    if (xoInput->hasAcceptableInput()) {
        xoInput->setPalette(QPalette());
    } else {
        xoInput->setPalette(palette);
    }

    if (angCheckBox->isChecked() && zlCheckBox->isChecked())
        if (zlInput->hasAcceptableInput() && angInput->hasAcceptableInput()) {
            computeButton->setEnabled(true);
            return;
        }

    if (angCheckBox->isChecked() && dlCheckBox->isChecked())
        if (dlInput->hasAcceptableInput() && angInput->hasAcceptableInput()) {
            computeButton->setEnabled(true);
            return;
        }

    if (angCheckBox->isChecked() && xoCheckBox->isChecked() && xdCheckBox->isChecked())
        if (xdInput->hasAcceptableInput() && xoInput->hasAcceptableInput()
                && angInput->hasAcceptableInput()) {
            computeButton->setEnabled(true);
            return;
        }

    if (zlCheckBox->isChecked() && dlCheckBox->isChecked())
        if (zlInput->hasAcceptableInput() && dlInput->hasAcceptableInput()) {
            computeButton->setEnabled(true);
            return;
        }

    computeButton->setEnabled(false);
}
