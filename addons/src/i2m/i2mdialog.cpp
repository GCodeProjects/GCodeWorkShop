/*
 *  Copyright (C) 2006-2018 by Artur Kozioł, artkoz78@gmail.com
 *  Copyright (C) 2023 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This file is part of GCodeWorkShop.
 *
 *  GCodeWorkShop is free software: you can redistribute it and/or modify
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

#include <QChar>        // for operator==, QChar
#include <QCheckBox>    // for QCheckBox
#include <QLabel>       // for QLabel
#include <QLineEdit>    // for QLineEdit
#include <QPoint>       // for QPoint
#include <QPushButton>  // for QPushButton
#include <QRect>        // for QRect
#include <QSettings>    // for QSettings
#include <QSize>        // for QSize
#include <QString>      // for QString
#include <QVariant>     // for QVariant
#include <Qt>           // for WA_DeleteOnClose

class QWidget;

#include "i2mdialog.h"
#include "i2moptions.h" // for I2MOptions


#define CFG_SECTION  "I2MDialog"
#define CFG_KEY_POS  "Position"
#define CFG_KEY_SIZE "Size"


I2MDialog::I2MDialog(QWidget* parent, QSettings* settings) :
	QDialog(parent)
{
	setupUi(this);

	mSettings = settings;

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Inch to metric"));

	checkBoxToggled();

	connect(inchInput, SIGNAL(textChanged(const QString&)), SLOT(inputChanged()));
	connect(inchCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
	connect(mmCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
	connect(closePushButton, SIGNAL(clicked()), SLOT(accept()));
	connect(this, SIGNAL(finished(int)), SLOT(onFinished(int)));

	setFocusProxy(inchInput);
}

I2MDialog::~I2MDialog()
{
}

void I2MDialog::inputChanged()
{
	double a, b, c;
	bool ok, dot, sl;
	int i;
	QString tx, rs, ls, ms;

	tx = inchInput->text();

	if (tx.isNull() || tx.isEmpty()) {
		return;
	}

	tx.replace(',', '.');
	dot = sl = false;

	for (i = 0; i <= (tx.length() - 1); i++) {
		if ((tx.at(i) == '.')) {
			if (dot || sl) {
				tx.remove(i, 1);
			}

			dot = true;
		}

		if ((tx.at(i) == '/')) {
			if (sl) {
				tx.remove(i, 1);
			}

			sl = true;
		}

		if (!(tx.at(i).isDigit()) && !(tx.at(i) == '.') && !(tx.at(i) == '/')) {
			tx.remove(i, 1);
		}
	}

	if (tx.at(0) == '.') {
		tx.insert(0, '0');
	}

	inchInput->setText(tx);

	i = tx.indexOf('/');

	if (i > 0) {
		rs = tx.mid(i + 1, tx.length() - i);
		ls = tx.left(i);
		i = tx.indexOf('.');

		if (i > 0) {
			ms = ls.left(i);
			ls.remove(0, i + 1);
		} else {
			ms = "0";
		}

		a = rs.toDouble(&ok);
		b = ls.toDouble(&ok);
		c = ms.toDouble(&ok);

		c = c + (b / a);
		tx = QString("%1").arg(c, 0, 'f', 3);
	}

	a = tx.toDouble(&ok);

	if (!ok) {
		mmInput->setText("----");
		return;
	}

	if (inchCheckBox->isChecked()) {
		if (a > 1000) {
			mmInput->setText("----");
			return;
		}

		b = a * 25.4;
		mmInput->setText(QString("%1").arg(b, 0, 'f', 4));

	} else {
		if (a > 25000) {
			mmInput->setText("----");
			return;
		}

		b = a / 25.4;
		mmInput->setText(QString("%1").arg(b, 0, 'f', 4));
	}
}

void I2MDialog::checkBoxToggled()
{
	if (inchCheckBox->isChecked()) {
		inchLabel->setText(tr("Inch"));
		mmLabel->setText(tr("mm"));
	} else {
		inchLabel->setText(tr("mm"));
		mmLabel->setText(tr("Inch"));
	}

	inputChanged();
}

void I2MDialog::setOptions(const I2MOptions& options)
{
	QLineEdit* mm;
	QLineEdit* inch;

	if (options.toInch) {
		mm = mmInput;
		inch = inchInput;
	} else {
		mm = inchInput;
		inch = mmInput;
	}

	mmCheckBox->setChecked(!options.toInch);
	inchCheckBox->setChecked(options.toInch);

	if (options.mm.in) {
		mm->setText(QString::number(options.mm.value));
	} else {
		mm->clear();
	}

	if (options.inch.in) {
		inch->setText(QString::number(options.inch.value));
	} else {
		inch->clear();
	}

	checkBoxToggled();
}

I2MOptions I2MDialog::options()
{
	I2MOptions options;
	QLineEdit* mm;
	QLineEdit* inch;

	options.toInch = inchCheckBox->isChecked();

	if (options.toInch) {
		mm = mmInput;
		inch = inchInput;
	} else {
		mm = inchInput;
		inch = mmInput;
	}

	options.mm.value = mm->text().toDouble(&options.mm.in);
	options.inch.value = inch->text().toDouble(&options.inch.in);

	return options;
}

void I2MDialog::loadSettings()
{
	if (mSettings.isNull()) {
		return;
	}

	mSettings->beginGroup(CFG_SECTION);

	if (mSettings->contains(CFG_KEY_POS)) {
		QPoint pos = mSettings->value(CFG_KEY_POS, geometry().topLeft()).toPoint();
		QSize size = mSettings->value(CFG_KEY_SIZE, geometry().size()).toSize();
		setGeometry(QRect(pos, size));
	}

	I2MOptions opt;
	opt.load(mSettings);

	mSettings->endGroup();

	setOptions(opt);
}

void I2MDialog::saveSettings(bool saveOptions)
{
	if (mSettings.isNull()) {
		return;
	}

	mSettings->beginGroup(CFG_SECTION);

	mSettings->setValue(CFG_KEY_POS, geometry().topLeft());
	mSettings->setValue(CFG_KEY_SIZE, geometry().size());

	if (saveOptions) {
		options().save(mSettings);
	}

	mSettings->endGroup();
}

void I2MDialog::onFinished(int result)
{
	saveSettings(result == QDialog::Accepted);
}
