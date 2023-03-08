
/**
 * @brief The RenumberDialog class
 */
class RenumberDialog : public QDialog, private Ui::RenumberDialog
{
    Q_OBJECT

public:
    RenumberDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~RenumberDialog();

public slots:
    void getState(int &mode, int &startAt, int &from, int &prec, int &inc, int &to,
                  bool &renumEmpty, bool &renumComm, bool &renumMarked);

private slots:
    void okButtonClicked();
    void atEndClicked();
    void divideClicked();
    void renumClicked();
    void allLinesClicked();
    void removeAllClicked();
    void mRenumHeClicked();
};
