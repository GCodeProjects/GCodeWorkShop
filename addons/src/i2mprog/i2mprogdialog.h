
/**
 * @brief The I2MProgDialog class
 */
class I2MProgDialog : public QDialog, private Ui::I2MProgDialog
{
    Q_OBJECT

public:
    I2MProgDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~I2MProgDialog();

public slots:
    void getState(QString &txt, int &x, bool &in);
    void setState(const QString &txt, int x, bool in);

private slots:
    void inputChanged();
    void okButtonClicked();
};
