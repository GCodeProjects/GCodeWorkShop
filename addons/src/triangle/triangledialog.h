
/**
 * @brief The TriangleDialog class
 */
class TriangleDialog : public QDialog, private Ui::TriangleDialog
{
    Q_OBJECT

public:
    TriangleDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~TriangleDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void inputChanged();
    void checkBoxToggled();
    void rightTriangleCheckBoxToggled();

private slots:
    void computeButtonClicked();
    int option1();
    int option2();
    int option3();
    void option4();

private:
    uint mode;
    QPixmap *pic1;
    QPixmap *pic2;
    QDoubleValidator *aAInputValid;
    QDoubleValidator *aBInputValid;
    QDoubleValidator *aCInputValid;
};
