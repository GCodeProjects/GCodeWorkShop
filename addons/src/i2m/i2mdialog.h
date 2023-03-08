
/**
 * @brief The I2MDialog class
 */
class I2MDialog : public QDialog, private Ui::I2MDialog
{
    Q_OBJECT

public:
    I2MDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~I2MDialog();

protected slots:
    void inputChanged();
    void checkBoxToggled();
};
