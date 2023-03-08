
/**
 * @brief The BHCTab class
 */
class BHCTab : public QWidget, public Ui::BHCTab
{
    Q_OBJECT

public:
    BHCTab(QWidget *parent = 0);
    ~BHCTab();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void contextMenuReq(const QPoint &pos);
    void inputChk();

signals:
    void commonChk();

private slots:
    void copySelection();
    void sellAll();
    void inputChanged();

private:
    QMenu *contextMenu;
    QAbstractItemModel *model;
    QAction *addCommentsId;
    QAction *addCommentsParaId;
    QActionGroup *commentActGroup;
};
