
I2MProgDialog::I2MProgDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle(tr("Convert program inch to metric"));

    connect(mInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(closeButton, SIGNAL(clicked()), SLOT(close()));

    //setMaximumSize(width(), height());

    setFocusProxy(mInput);
    inputChanged();
}

I2MProgDialog::~I2MProgDialog()
{
}

void I2MProgDialog::inputChanged()
{
    okButton->setEnabled(mInput->hasAcceptableInput());
}

void I2MProgDialog::okButtonClicked()
{
    accept();
}

void I2MProgDialog::getState(QString &txt, int &x, bool &in)
{
    txt = mInput->text();
    txt.remove(' ');
    x = precInput->value();
    in = inchCheckBox->isChecked();
}

void I2MProgDialog::setState(const QString &txt, int x, bool in)
{
    mInput->setText(txt);
    mInput->selectAll();
    inchCheckBox->setChecked(in);
    mmCheckBox->setChecked(!in);
    precInput->setValue(x);
}
