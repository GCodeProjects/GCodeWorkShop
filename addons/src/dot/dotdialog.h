
/**
 * @brief The DotDialog class
 */
class DotDialog : public QDialog, private Ui::DotDialog
{
    Q_OBJECT

public:
    DotDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~DotDialog();

public slots:
    void getState(QString &text, bool &at_end, bool &div, int &x);
    void setState(const QString &txt, bool at_end, bool div, int x);

private slots:
    void inputChanged(const QString &text);
    void okButtonClicked();
    void atEndClicked();
    void divideClicked();
    void spinBoxVal(int val);
};
