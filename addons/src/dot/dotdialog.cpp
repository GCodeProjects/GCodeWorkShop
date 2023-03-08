
DotDialog::DotDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);

    setWindowTitle(tr("Insert dots"));

    connect(mInput, SIGNAL(textChanged(const QString &)), this,
            SLOT(inputChanged(const QString &)));
    connect(mCheckAtEnd, SIGNAL(clicked()), this, SLOT(atEndClicked()));
    connect(mCheckDivide, SIGNAL(clicked()), this, SLOT(divideClicked()));
    connect(mSpinBox, SIGNAL(valueChanged(int)), this, SLOT(spinBoxVal(int)));
    connect(okButton, SIGNAL(clicked()), SLOT(okButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(close()));

    setMaximumSize(width(), height());
    okButton->setEnabled(mInput->hasAcceptableInput());
}

DotDialog::~DotDialog()
{
}

void DotDialog::okButtonClicked()
{
    accept();
}

void DotDialog::getState(QString &txt, bool &at_end, bool &div, int &x)
{
    txt = mInput->text();
    txt.remove(' ');
    at_end = mCheckAtEnd->isChecked();
    div = mCheckDivide->isChecked();
    x = mSpinBox->value();
}

void DotDialog::setState(const QString &txt, bool at_end, bool div, int x)
{
    mInput->setText(txt);
    mInput->selectAll();

    mCheckAtEnd->setChecked(at_end);
    mCheckDivide->setChecked(div);
    mSpinBox->setEnabled(mCheckDivide->isChecked());
    mSpinBox->setValue(x);
}

void DotDialog::inputChanged(const QString &text)
{
    Q_UNUSED(text);
    okButton->setEnabled(mInput->hasAcceptableInput());
}

void DotDialog::atEndClicked()
{
    mCheckDivide->setChecked(!mCheckAtEnd->isChecked());
    mSpinBox->setEnabled(mCheckDivide->isChecked());
}

void DotDialog::divideClicked()
{
    mCheckAtEnd->setChecked(!mCheckDivide->isChecked());
    mSpinBox->setEnabled(mCheckDivide->isChecked());
}

void DotDialog::spinBoxVal(int val)
{
    if (val == 99) {
        mSpinBox->setValue(10);
    } else if (val == 999) {
        mSpinBox->setValue(10);
    } else if (val == 9999) {
        mSpinBox->setValue(100);
    }

    if (val == 11) {
        mSpinBox->setValue(100);
    } else if (val == 101) {
        mSpinBox->setValue(1000);
    } else if (val == 1001) {
        mSpinBox->setValue(10000);
    }
}
