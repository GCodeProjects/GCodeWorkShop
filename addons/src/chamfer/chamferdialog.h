
/**
 * @brief The ChamferDialog class
 */
class ChamferDialog : public QDialog, private Ui::ChamferDialog
{
    Q_OBJECT

public:
    ChamferDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~ChamferDialog();

protected slots:
    void inputChanged();
    void checkBoxToggled();

private slots:
    void computeButtonClicked();
};
