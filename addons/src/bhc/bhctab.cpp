
BHCTab::BHCTab(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    QValidator *xCenterInputValid = new QDoubleValidator(-9999, 9999, 3, this);
    xCenterInput->setValidator(xCenterInputValid);
    xCenterInput->installEventFilter(this);
    QValidator *yCenterInputValid = new QDoubleValidator(-9999, 9999, 3, this);
    yCenterInput->setValidator(yCenterInputValid);
    yCenterInput->installEventFilter(this);
    QValidator *diaInputValid = new QDoubleValidator(1, 9999, 3, this);
    diaInput->setValidator(diaInputValid);
    diaInput->installEventFilter(this);
    QValidator *holesInputValid = new QIntValidator(1, 360, this);
    holesInput->setValidator(holesInputValid);
    QValidator *angleStartInputValid = new QDoubleValidator(0, 360, 3, this);
    angleStartInput->setValidator(angleStartInputValid);
    angleStartInput->installEventFilter(this);
    QValidator *angleBeetwenInputValid = new QDoubleValidator(0, 360, 3, this);
    angleBeetwenInput->setValidator(angleBeetwenInputValid);
    angleBeetwenInput->installEventFilter(this);

    connect(roateInput, SIGNAL(valueChanged(int)), SLOT(inputChk()));
    connect(mirrorX, SIGNAL(toggled(bool)), SLOT(inputChk()));
    connect(mirrorY, SIGNAL(toggled(bool)), SLOT(inputChk()));

    contextMenu = new QMenu(this);
    commentActGroup = new QActionGroup(this);

    QAction *copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copySelection()));
    resultTable->addAction(copyAct);

    QAction *selAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select all"),
                                     this);
    selAllAct->setShortcut(QKeySequence::SelectAll);
    selAllAct->setStatusTip(tr("Select all"));
    connect(selAllAct, SIGNAL(triggered()), this, SLOT(sellAll()));
    resultTable->addAction(selAllAct);

    contextMenu->addAction(copyAct);
    contextMenu->addSeparator();
    contextMenu->addAction(selAllAct);
    contextMenu->addSeparator();
    addCommentsId = contextMenu->addAction(tr("Add ; comments"));
    addCommentsId->setCheckable(true);
    addCommentsId->setActionGroup(commentActGroup);
    addCommentsParaId = contextMenu->addAction(tr("Add () comments"));
    addCommentsParaId->setCheckable(true);
    addCommentsParaId->setActionGroup(commentActGroup);

    resultTable->setContextMenuPolicy(Qt::CustomContextMenu);

    resultTable->setHorizontalHeaderLabels(QStringList() << "X" << "Y");

    connect(resultTable, SIGNAL(customContextMenuRequested(const QPoint &)), this,
            SLOT(contextMenuReq(const QPoint &)));
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(xCenterInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));
    connect(yCenterInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));
    connect(diaInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));
    connect(holesInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));
    connect(angleStartInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));
    connect(angleBeetwenInput, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged()));
}

BHCTab::~BHCTab()
{
}

bool BHCTab::eventFilter(QObject *obj, QEvent *ev)
{
    if (!qobject_cast<QLineEdit *>(obj)) {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, ev);
    }

    if (ev->type() == QEvent::KeyPress) {
        return false;
    }

    QKeyEvent *k = (QKeyEvent *) ev;

    if (QLocale().decimalPoint() == '.' && k->key() == Qt::Key_Comma) {
        QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier,
                                ".", false, 1));
        return true;
    }

    if (QLocale().decimalPoint() == ',' && k->key() == Qt::Key_Period) {
        QApplication::sendEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier, ",",
                                false, 1));
        return true;
    }

    return false;
}

void BHCTab::inputChanged()
{
    QPalette palette;

    palette.setColor(xCenterInput->foregroundRole(), Qt::red);

    if (xCenterInput->hasAcceptableInput()) {
        xCenterInput->setPalette(QPalette());
    } else {
        xCenterInput->setPalette(palette);
    }

    if (yCenterInput->hasAcceptableInput()) {
        yCenterInput->setPalette(QPalette());
    } else {
        yCenterInput->setPalette(palette);
    }

    if (diaInput->hasAcceptableInput()) {
        diaInput->setPalette(QPalette());
    } else {
        diaInput->setPalette(palette);
    }

    if (holesInput->hasAcceptableInput()) {
        holesInput->setPalette(QPalette());
    } else {
        holesInput->setPalette(palette);
    }

    if (angleStartInput->hasAcceptableInput()) {
        angleStartInput->setPalette(QPalette());
    } else {
        angleStartInput->setPalette(palette);
    }

    if (angleBeetwenInput->hasAcceptableInput()) {
        angleBeetwenInput->setPalette(QPalette());
    } else {
        angleBeetwenInput->setPalette(palette);
    }
}

void BHCTab::contextMenuReq(const QPoint &pos)
{
    Q_UNUSED(pos);
    contextMenu->popup(QCursor::pos());
}

void BHCTab::sellAll()
{
    resultTable->selectAll();
}

void BHCTab::copySelection()
{
    int i;
    QString selText, tmp;
    QTableWidgetItem *it;

    selText = "";

    for (i = 0; i < resultTable->rowCount(); i++) {
        if (resultTable->item(i, 0)->isSelected() || resultTable->item(i, 1)->isSelected()) {
            it = resultTable->item(i, 0);
            selText += "X" + it->text();
            it = resultTable->item(i, 1);
            selText += " Y" + it->text();

            if (addCommentsParaId->isChecked()) {
                it = resultTable->verticalHeaderItem(i);
                tmp = it->text();
                tmp.remove(")");
                tmp.replace("(", "- ");
                tmp = tmp.simplified();
                selText += " (" + tmp + ")\n";
            } else if (addCommentsId->isChecked()) {
                it = resultTable->verticalHeaderItem(i);
                tmp = it->text();
                tmp.remove(")");
                tmp.replace("(", "- ");
                tmp = tmp.simplified();
                selText += " ;" + tmp + "\n";
            } else {
                selText += "\n";
            }
        }
    }

    selText.remove(selText.length() - 1, 1);
    selText = removeZeros(selText);

    if (addCommentsParaId->isChecked())
        selText.prepend(QString(tr("(DIAMETER: %1, NO. OF HOLES: %2, START ANGLE: %3)\n"))
                        .arg(diaInput->text()).arg(holesInput->text()).arg(angleStartInput->text()));

    if (addCommentsId->isChecked())
        selText.prepend(QString(tr(";DIAMETER: %1, NO. OF HOLES: %2, START ANGLE: %3\n"))
                        .arg(diaInput->text()).arg(holesInput->text()).arg(angleStartInput->text()));

    QClipboard *clipBoard = QApplication::clipboard();
    clipBoard->setText(selText, QClipboard::Clipboard);

    if (clipBoard->supportsSelection()) {
        clipBoard->setText(selText, QClipboard::Selection);
    }
}

void BHCTab::inputChk()
{
    emit commonChk();
}
