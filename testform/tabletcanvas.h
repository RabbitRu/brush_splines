#ifndef TABLETCANVAS_H
#define TABLETCANVAS_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QTabletEvent>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QTime>

#include "mpoint.h"
#include "testfuncs.h"


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
    void drawtests();
    bool saveImage(const QString &file);
    bool loadImage(const QString &file);
    double compareImages(QImage &im1, QImage &im2);
    bool recordStroke(const QString &file);
    bool stoprecord();
    QPoint playStroke(const QString &file);
    void drawTestFunc(funcVal a, QString file);
    void testgen();
    void compareResults();

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
    void setAmmount(qreal t)
    {m_repAmmount=t; }
    qreal getWidth()
    {return m_brushWidth; }
    qreal getTrans()
    {return m_brushTrans; }
    qreal getAmmount()
    {return m_repAmmount; }
    void setDebugPoints(bool t)
    {m_debug = t;   }
    void setStepmod(long in)
    { m_stepmod = in;}
    long getStepmod()
    {   return m_stepmod;}
    void setA(qreal in)
    { m_a = in;}
    long getA()
    {   return m_a;}
    void setB(qreal in)
    { m_b = in;}
    long getB()
    {   return m_b;}
    void setSval(stepVal a)
    {   m_sval=a;}
    stepVal getSval()
    {   return m_sval;}

protected:
    void tabletEvent(QTabletEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void initPixmap();
    void paintPixmap(QPainter &painter, QTabletEvent *event);
    void drawStroke(QPainter &painter,const mPoint p);
    void m_drawLine(QPainter &painter, const mPoint p1,const mPoint p2);
    void m_drawPoints(QPainter &painter, const mPoint p1, const mPoint p2);
    void m_drawSpline(QPainter &painter, const mPoint p);
    void drawIdeal(funcVal a);
    //Qt::BrushStyle brushPattern(qreal value);
    void updateBrush(qreal pressure);
    void eraserBrush(qreal p);
    void updateCursor(const QTabletEvent *event);

    AlgoValuator m_algo;
    Valuator m_transparencyValuator;
    Valuator m_widthValuator;
    Valuator m_colorValuator;
    stepVal m_sval;

    QVector<mPoint> points;
    QColor m_color, m_scolor, m_ecolor;
    qreal pressuremode, m_brushWidth, m_brushTrans, m_repAmmount, m_a, m_b;
    long m_stepmod;
    QPixmap m_pixmap;
    QBrush m_brush;
    QPen m_pen;
    QTime m_Timer;
    QFile recordFile;
    bool m_deviceDown, m_debug = false, recordStrokes = false;
};

#endif
