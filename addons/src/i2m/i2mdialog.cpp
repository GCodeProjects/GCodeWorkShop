
I2MDialog::I2MDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Inch to metric"));

    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("Inch2mm");
    inchCheckBox->setChecked(settings.value("Inch", true).toBool());
    mmCheckBox->setChecked(!settings.value("Inch", true).toBool());
    settings.endGroup();

    inputChanged();
    checkBoxToggled();

    //setMaximumSize(width(), height());

    connect(inchInput, SIGNAL(textChanged(const QString &)), SLOT(inputChanged()));
    connect(inchCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(mmCheckBox, SIGNAL(toggled(bool)), SLOT(checkBoxToggled()));
    connect(closePushButton, SIGNAL(clicked()), SLOT(close()));

    setFocusProxy(inchInput);
}

I2MDialog::~I2MDialog()
{
    QSettings &settings = *Medium::instance().settings();
    settings.beginGroup("Inch2mm");
    settings.setValue("Inch", inchCheckBox->isChecked());
    settings.endGroup();
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
