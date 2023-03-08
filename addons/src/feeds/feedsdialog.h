
/**
 * @brief The FeedsDialog class
 */
class FeedsDialog : public QDialog, private Ui::FeedsDialog
{
    Q_OBJECT

public:
    FeedsDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~FeedsDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void inputChanged();

private slots:
    void computeButtonClicked();
    void computeVcButtonClicked();
    void setDefButton();
    void checkBoxChanged();
};
