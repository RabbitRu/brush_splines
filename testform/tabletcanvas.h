#ifndef TABLETCANVAS_H
#define TABLETCANVAS_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QTabletEvent>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QTime>

typedef struct mPoint {
    QPoint pos;
    qreal pressure;
    mPoint(){pressure=0;pos=QPoint(0,0);}
    mPoint(const QPoint& p, qreal r){
        pos=p;
        pressure=r;
    }
    mPoint(qreal x, qreal y, qreal r){
        pos=QPoint(x,y);
        pressure=r;
    }
} mPoint;

class TabletCanvas : public QWidget
{
    Q_OBJECT

public:
    enum Valuator {NoValuator, FixedValuator, PressureValuator};
    Q_ENUM(Valuator)
    enum AlgoValuator { BasicVal, BrushVal, SplineVal};
    Q_ENUM(AlgoValuator)

    //TabletCanvas();
    TabletCanvas(QWidget*&);
    ~TabletCanvas();

    bool newImage();
    bool saveImage(const QString &file);
    bool loadImage(const QString &file);
    bool recordStroke(const QString &file);
    bool stoprecord();
    bool playStroke(const QString &file);
    void setAlgo(AlgoValuator type)
    { m_algo=type; }
    void setTransparencyType(Valuator type)
    { m_transparencyValuator = type; }
    void setTransitionType(Valuator type)
    { m_colorValuator = type; }
    void setWidthType(Valuator type)
    { m_widthValuator = type; }
    void setColor(const QColor &c)
    { if (c.isValid()) m_color = c; }
    void setEndingColor(const QColor &c)
    { if (c.isValid()) m_ecolor = c; }
    void setStartingColor(const QColor &c)
    { if (c.isValid()) m_scolor = c; }
    QColor color() const
    { return m_color; }
    void setTabletDevice(QTabletEvent *event)
    { updateCursor(event); }
    void setWidth(qreal w)
    {m_brushWidth=w; }
    void setTrans(qreal t)
    {m_brushTrans=t; }

protected:
    void tabletEvent(QTabletEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void initPixmap();
    void paintPixmap(QPainter &painter, QTabletEvent *event);
    void drawStroke(QPainter &painter,const mPoint p);
    void mydrawPoint(QPainter &painter, const mPoint p);
    //Qt::BrushStyle brushPattern(qreal value);
    void updateBrush(qreal pressure);
    void eraserBrush(qreal p);
    void updateCursor(const QTabletEvent *event);

    AlgoValuator m_algo;

    Valuator m_transparencyValuator;
    Valuator m_widthValuator;
    Valuator m_colorValuator;

    QVector<mPoint> points;
    QColor m_color, m_scolor, m_ecolor;
    qreal m_brushWidth, m_brushTrans;
    QPixmap m_pixmap;
    QBrush m_brush;
    QPen m_pen;
    QTime m_Timer;
    bool m_deviceDown;
    bool recordStrokes = false;
    QFile recordFile;
};

#endif
