
/**
 * @brief The BHCDraw class
 */
class BHCDraw : public QWidget
{
    Q_OBJECT

public:
    BHCDraw(QWidget *parent = 0,
            Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::Tool);
    ~BHCDraw();

public slots:
    void clear();
    void printText(int x, int y, int line, const QString &text, QColor color);
    void drawHole(qreal ang, qreal dia, qreal xcenter, qreal ycenter, bool first = false,
                  bool last = false, QColor color = Qt::green);
    void init(int w, int h);
    void drawLines(qreal dia, qreal ang, QColor cl);
    void setScale(double sc);

private slots:
    void drawLines();

protected:
    void paintEvent(QPaintEvent *);
    void focusOutEvent(QFocusEvent *);
    void mousePressEvent(QMouseEvent *event);

private:
    double scale;
    QPixmap *pm;
};
