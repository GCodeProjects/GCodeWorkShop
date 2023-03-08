
/**
 * @brief The BHCDialog class
 */
class BHCDialog : public QDialog, private Ui::BHCDialog
{
    Q_OBJECT

public:
    BHCDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog);
    ~BHCDialog();

protected:
    //void windowActivationChange(bool oldActive);

private slots:
    void comChk();
    void computeButtonClicked();
    void closeButtonClicked();
    void clearAll();

private:
    QTabWidget *tabBar;
    BHCDraw *drawing;

    int parentHeight;
    int parentWidth;
};
