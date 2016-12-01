#include <QtWidgets>
#include <math.h>

#include "tabletcanvas.h"

/*TabletCanvas::TabletCanvas()
    : QWidget(Q_NULLPTR)
    , m_alphaChannelValuator(PressureValuator)
    , m_colorSaturationValuator(NoValuator)
    , m_lineWidthValuator(PressureValuator)
    , m_color(Qt::red)
    , m_brush(m_color)
    , m_pen(m_brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , m_deviceDown(false)
{
    m_Timer.start();
    initPixmap();
    setAutoFillBackground(true);
}*/

TabletCanvas::TabletCanvas(QWidget*& a)
    : QWidget(a)
    , m_transparencyValuator(FixedValuator)
    , m_widthValuator(PressureValuator)
    , m_colorValuator(NoValuator)
    , m_color(Qt::red)
    , m_brushWidth(25)
    , m_brushTrans(1)
    , m_brush(m_color)
    , m_pen(m_brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , m_deviceDown(false)
{
    m_Timer.start();
    initPixmap();
    setAutoFillBackground(true);
}


TabletCanvas::~TabletCanvas()
{
    if(recordFile.isOpen())recordFile.close();
}

bool TabletCanvas::newImage(){
    initPixmap();
    m_pixmap.fill((Qt::white));
    update();
    return true;
}

void TabletCanvas::initPixmap()
{
    int w=width(),h=height();
    if(m_pixmap.width()>w)
        w=m_pixmap.width();
    if(m_pixmap.height()>h)
        h=m_pixmap.height();
    QPixmap newPixmap = QPixmap(w, h);
    newPixmap.fill(Qt::white);
    QPainter painter(&newPixmap);
    if (!m_pixmap.isNull())
        painter.drawPixmap(0, 0, m_pixmap);
    painter.end();
    m_pixmap = newPixmap;
}

bool TabletCanvas::saveImage(const QString &file)
{
    return m_pixmap.save(file);
}

bool TabletCanvas::loadImage(const QString &file)
{
    bool success = m_pixmap.load(file);
    if (success) {
        update();
        return true;
    }
    return false;
}


bool TabletCanvas::recordStroke(const QString &file){
    recordStrokes=true;
    recordFile.setFileName(file);
    recordFile.resize(0);
    recordFile.open(QIODevice::WriteOnly);
    if(!recordFile.isOpen())return false;
    QTextStream(&recordFile)<<this->width()<<" "<<this->height()<<"\n";
    return true;
}

bool TabletCanvas::stoprecord(){
    if(recordFile.isOpen()){
        recordFile.close();
        return true;
    }
    return false;
}

bool TabletCanvas::playStroke(const QString &file){
    qint32 w,h,x,y;
    qreal p;
    QVector<mPoint> inpoints;
    recordFile.setFileName(file);
    recordFile.open(QIODevice::ReadOnly);
    if(!recordFile.isOpen())return false;
    QTextStream inputStream(&recordFile);
    if(!inputStream.atEnd())inputStream>>w>>h;
    else return false;
    inpoints.clear();
    while(!inputStream.atEnd()){
        inputStream>>x>>y>>p;
        inpoints.push_back(mPoint(x,y,p));
    }
    if(inpoints.last().pressure == 0
            && inpoints.last().pos.x()==0
            && inpoints.last().pos.y()==0) inpoints.pop_back();

    recordFile.close();

    window()->resize(w,h);
    resize(w,h);
    QPainter painter(&m_pixmap);

    QTime execTime;
    execTime.start();

    int i;
    points.clear();
    if(inpoints.length()>1){points.push_back(inpoints[0]);points.push_back(inpoints[0]);}

    switch(inpoints.length()){
    case 0:
        break;
    case 1:
        updateBrush(inpoints[0].pressure);
        painter.drawPoint(inpoints[0].pos);
        update();
        break;
    default:
        for(i=1;i<inpoints.length();i++){
            updateBrush(inpoints[i].pressure);
            drawStroke(painter,inpoints[i]);
            update();
            points.pop_front();
            points.push_back(inpoints[i]);
        }
    }

    qDebug() << "\nDrawning time:" << execTime.elapsed() << "\n";
    return true;
}

void TabletCanvas::resizeEvent(QResizeEvent *)
{
    initPixmap();
}


int prevTime = 0;
int counter=0,allt=0;
#include <QDebug>
void TabletCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}

void TabletCanvas::tabletEvent(QTabletEvent *event)//Here everything starts
{
    int elap=m_Timer.elapsed();
    //    qDebug() << elap - prevTime;
    counter++;
    allt+=elap-prevTime;
    prevTime=elap;
    //    qDebug()<< "Averagetime between events" << allt/counter << "Events counter:" << counter ;


    qDebug()<<event->pos().x()<<" "<<event->pos().y();
    switch (event->type()) {
    case QEvent::TabletPress:
        if (!m_deviceDown) {
            m_deviceDown = true;
            points.push_back(mPoint(event->pos(),event->pressure()));
            //lastPoint.rotation = event->rotation();
        }
        break;
    case QEvent::TabletMove:
        //if (event->device() == QTabletEvent::RotationStylus)
        //  updateCursor(event);
        if (m_deviceDown) {
            QPainter painter(&m_pixmap);
            updateCursor(event);
            if(event->pointerType()==QTabletEvent::Eraser){
                eraserBrush(event->pressure());
                painter.setPen(m_pen);
                painter.drawLine(points.last().pos, event->pos());
            }
            else {
                updateBrush(event->pressure());
                paintPixmap(painter, event);
            }

            points.push_back(mPoint(event->pos(),event->pressure()));
            if(points.length()>2)points.pop_front();
        }
        break;
    case QEvent::TabletRelease:
        if (m_deviceDown){// && event->buttons() == Qt::NoButton)
            m_deviceDown = false;
            points.clear();
        }
        break;
    default:
        break;
    }
    event->accept();
    update();
}

void TabletCanvas::paintPixmap(QPainter &painter, QTabletEvent *event)
{
    //painter.setRenderHint(QPainter::Antialiasing);// ///////////////////////////////////////

    switch (event->device()) {
    case QTabletEvent::Stylus:
        if(recordStrokes){
            QTextStream(&recordFile)<<event->pos().x() <<" ";
            QTextStream(&recordFile)<<event->pos().y()<<" ";
            QTextStream(&recordFile)<<event->pressure()<<"\n";

        }
        drawStroke(painter,mPoint(event->pos(),event->pressure()));
        break;
    default:
        break;
    }
}

#include "aspline.cpp"

void swapxy(QVector<mPoint>* arr){
    int i,x;
    for(i=0;i<arr->length();i++){
        x=(*arr)[i].pos.x();
        (*arr)[i].pos.setX((*arr)[i].pos.y());
        (*arr)[i].pos.setY(x);
    }
}

void TabletCanvas::drawStroke(QPainter &painter,const mPoint p){
    eraserBrush(points.last().pressure*0.99);
    painter.setPen(m_pen);
    painter.drawPoint(points.last().pos);

    switch(m_algo){
    case BasicVal:
        //eraserBrush(prevPoint.pressure);
        //painter.drawPoint(prevPoint.pos);

        updateBrush(p.pressure);
        painter.setPen(m_pen);
        painter.drawLine(points.last().pos, p.pos);
        break;
    case BrushVal:
        mydrawPoint(painter, p);
        break;
    case SplineVal:
        QVector<mPoint> outp, inp;
        bool swapa=false, swapb=false;
        inp.push_back(points[0]);

        if(points.length()>1){
            if(points[1].pos!=points[0].pos)
                inp.push_back(points[1]);
            else
                inp[0].pressure=qMax(points[0].pressure,points[1].pressure);
        }

        if(inp.last().pos!=p.pos)inp.push_back(p);
        else inp.last().pressure=qMax(inp.last().pressure,p.pressure);

        if(inp.length()>2 &&( inp[0].pos.x()==inp[1].pos.x()||inp[0].pos.x()==inp[2].pos.x()||inp[1].pos.x()==inp[2].pos.x())){
            swapxy(&inp);
            swapa=true;
            qDebug()<<"swapA";
        }

        if(inp.length()>2 && (inp[0].pos.x()>inp[1].pos.x() || inp[1].pos.x()>inp[2].pos.x())){
            mPoint temp=inp[0];
            inp[0]=inp[2];inp[2]=temp;
            if(inp[1].pos.x()>inp[2].pos.x() || inp[0].pos.x()>inp[1].pos.x()){
                if(swapa){
                    qDebug()<<"ERRORRRRRRRRRRRRRRRRRRRR";
                    return;
                }
                swapxy(&inp);
                swapb=true;
                qDebug()<<"swapB";
                if(inp[0].pos.x()>inp[1].pos.x() || inp[1].pos.x()>inp[2].pos.x()){
                    temp=inp[0];
                    inp[0]=inp[2];inp[2]=temp;
                }
            }
        }
        if(inp.length()>2 &&
                ((inp[0].pos.x()==inp[1].pos.x()||inp[0].pos.x()==inp[2].pos.x()||inp[1].pos.x()==inp[2].pos.x())
                 ||(inp[0].pos.x()>inp[1].pos.x() || inp[1].pos.x()>inp[2].pos.x()))){
            if(swapa||swapb)swapxy(&inp);
            painter.drawLine(inp[0].pos,inp[1].pos);
            painter.drawLine(inp[2].pos,inp[1].pos);
            return;
        }
        calcspline(&inp,&outp,50);
        if(swapa||swapb){
            swapxy(&outp);
        }
        int i=0;
        for(i=1;i<outp.length();i++){
            updateBrush(outp[i-1].pressure);
            painter.setPen(m_pen);
            painter.drawLine(outp[i-1].pos,outp[i].pos);
            update();
        }
        break;
    }
}

qreal min(qreal a,qreal b){
    if(a>b)return b;
    return a;
}

#include <math.h>
void TabletCanvas::mydrawPoint(QPainter &painter, const mPoint p){
    mPoint prev=points.last();
    qreal mp = p.pressure - prev.pressure ;
    QPoint ml = QPoint(p.pos.x()-prev.pos.x(),p.pos.y()-prev.pos.y()), tmp;
    qreal l =  sqrt(ml.x()*ml.x()+ml.y()*ml.y()), mmn=min(p.pressure,prev.pressure);
    int dots=(l/4)/mmn;
    for(int i=0; i<=dots;i++){
        updateBrush((prev.pressure+mp*((qreal)i/dots)));
        painter.setPen(m_pen);
        tmp = QPoint(prev.pos.x()+ml.x()*((qreal)i/dots),prev.pos.y()+ml.y()*((qreal)i/dots));
        painter.drawPoint(tmp);
        update();
    }
}

void TabletCanvas::updateBrush(qreal pressure)
{
    switch (m_widthValuator) {
    case PressureValuator:
        m_pen.setWidthF(pressure * m_brushWidth + 1);
        break;
    case FixedValuator:
        m_pen.setWidth(m_brushWidth);
        break;
    default:
        m_pen.setWidthF(1);
    }

    switch(m_transparencyValuator){
    case PressureValuator:
        if(m_algo == BrushVal)m_color.setAlphaF(m_brushTrans*pressure / 4);
        else m_color.setAlphaF(pressure*m_brushTrans);
        break;
    case FixedValuator:
        m_color.setAlphaF(pressure*m_brushTrans);
        break;
    default:
        break;
    }

    switch(m_colorValuator){
    case PressureValuator:
    {
        qreal mr = m_ecolor.red() - m_scolor.red(),mg = m_ecolor.green() - m_scolor.green(),mb = m_ecolor.blue() - m_scolor.blue();
        QColor pcolor = QColor(mr*pressure,mg*pressure,mb*pressure,255);
        m_brush.setColor(pcolor);
        m_pen.setColor(pcolor);
        break;
    }
    default:
        m_brush.setColor(m_color);
        m_pen.setColor(m_color);
        break;
    }

}

void TabletCanvas::eraserBrush(qreal p)
{
    m_brush.setColor(Qt::white);
    m_pen.setColor(Qt::white);
    m_pen.setWidthF(p * m_brushWidth + 1);
}

void TabletCanvas::updateCursor(const QTabletEvent *event)//???
{
    QCursor cursor;
    if (event->type() != QEvent::TabletLeaveProximity) {
        if (event->pointerType() == QTabletEvent::Eraser) {
            cursor = QCursor(QPixmap("/home/ultra/QtProjects/testform/images/cursor-eraser.png"), 0, 0);
        } else {
            switch (event->device()) {
            case QTabletEvent::Stylus:
                cursor = QCursor(QPixmap("/home/ultra/QtProjects/testform/images/cursor-pencil.png"), 0, 0);
                break;
                //case QTabletEvent::Airbrush:
                //    break;
                //case QTabletEvent::RotationStylus:
                //    break;
            default:
                cursor = QCursor(QPixmap("/home/ultra/QtProjects/testform/images/cursor-unknown.png"), 0, 0);
                break;
            }
        }
    }
    setCursor(cursor);
}


