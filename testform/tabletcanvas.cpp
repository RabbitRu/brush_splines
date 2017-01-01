#include <QtWidgets>

#include "tabletcanvas.h"
#include "aspline.h"

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
    , m_algo(BasicVal)
    , m_transparencyValuator(FixedValuator)
    , m_widthValuator(PressureValuator)
    , m_colorValuator(NoValuator)
    , m_sval(Lin)
    , m_color(Qt::red)
    , m_brushWidth(25)
    , m_brushTrans(1)
    , m_repAmmount(1)
    , m_a(100)
    , m_b(100)
    , m_stepmod(100)
    , m_brush(m_color)
    , m_pen(m_brush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    , m_deviceDown(false)
{
    m_Timer.start();
    initPixmap();
    setAutoFillBackground(true);
}

bool kek;

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
    recordStrokes=false;
    if(recordFile.isOpen()){
        recordFile.close();
        return true;
    }
    return false;
}

QPoint TabletCanvas::playStroke(const QString &file){
    qint32 w,h,x,y;
    qreal p;
    QVector<mPoint> inpoints;
    recordFile.setFileName(file);
    recordFile.open(QIODevice::ReadOnly);
    if(!recordFile.isOpen())return QPoint(-1,0);
    QTextStream inputStream(&recordFile);
    if(!inputStream.atEnd())inputStream>>w>>h;
    else return QPoint(-1,0);
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
    inpoints.push_back(inpoints.last());
    int i, j;
    for(j=0;j<m_repAmmount;j++){
        kek=false;
        points.clear();
        if(inpoints.length()>1){points.push_back(inpoints[0]);points.push_back(inpoints[0]);}

        switch(inpoints.length()){
        case 0:
            break;
        case 1:
            updateBrush(inpoints[0].pressure);
            painter.setPen(m_pen);
            m_drawLine(painter,points.last(),inpoints[0]);
            update();
            break;
        default:
            for(i=1;i<inpoints.length();i++){
                updateBrush(inpoints[i].pressure);
                drawStroke(painter,inpoints[i]);
                points.pop_front();
                points.push_back(inpoints[i]);
            }
        }
        if(kek==false)
            if(m_algo==SplineVal)drawStroke(painter,inpoints.last());
    }
    QPoint ret(execTime.elapsed(),m_repAmmount);
    //qDebug() << "\nDrawning time:" << execTime.elapsed() << "\n";
    return ret;
}

void TabletCanvas::drawTestFunc(funcVal a, QString file){
    QVector<mPoint> arr;
    int i, p = 10;
    bool d = false;
    recordFile.setFileName(file);
    recordFile.resize(0);
    recordFile.open(QIODevice::WriteOnly);
    if(!recordFile.isOpen())return;

    gtestArray(&arr,a,m_sval,m_stepmod,m_a,m_b);
    for(i=0;i<arr.size();i++){
        arr[i].pos.setX(arr[i].pos.x()+width()/2);arr[i].pos.setY(arr[i].pos.y()+height()/2);
    }

    QTextStream(&recordFile)<<this->width()<<" "<<this->height()<<"\n";
    for(i=0;i<arr.size();i++){
        QTextStream(&recordFile)<<arr[i].pos.x() <<" ";
        QTextStream(&recordFile)<<arr[i].pos.y()<<" ";
        QTextStream(&recordFile)<<(0.1*((int)(arr[i].pressure)%10))<<"\n";
        if(p==10)d=false;
        if(p==0)d=true;
        if(d)p++;
        else p--;

    }
    arr.clear();
    recordFile.close();
    update();
}

void TabletCanvas::drawIdeal(funcVal a){
    int i,p = 1000;
    bool d;
    QVector<mPoint> arr;
    QPainter painter(&m_pixmap);
    gtestArray(&arr,a,Lin,10*m_stepmod,m_a,m_b);
    for(i=0;i<arr.size();i++){
        arr[i].pos.setX(arr[i].pos.x()+width()/2);arr[i].pos.setY(arr[i].pos.y()+height()/2);
    }
    for(i=0;i<arr.size()-1;i++){
        switch (m_widthValuator) {
        case PressureValuator:
            m_pen.setWidthF((arr[i].pressure-1)/100 * m_brushWidth + 1);
            break;
        case FixedValuator:
            m_pen.setWidth(m_brushWidth);
            break;
        default:
            m_pen.setWidthF(1);
        }

        m_brush.setColor(Qt::white);
        m_pen.setColor(Qt::white);
        painter.setPen(m_pen);
        painter.drawPoint(arr[i].pos);

        updateBrush(arr[i].pressure/100);
        painter.setPen(m_pen);
        painter.drawLine(arr[i].pos, arr[i+1].pos);
        if(p==1000)d=false;
        if(p==0)d=true;
        if(d)p++;
        else p--;
    }
    update();
}


double TabletCanvas::compareImages(QImage &im1, QImage &im2)
{
    QPainter painter(&m_pixmap);
    long long allwhite=0;
    qreal comparsion = 0;
    double t;
    int w=qMin(im1.width(),im2.width());
    int h=qMin(im1.height(),im1.height());

    for(int i=0;i<h;i++){
        QRgb *rgb1=(QRgb*)im1.constScanLine(i);
        QRgb *rgb2=(QRgb*)im2.constScanLine(i);
        QColor c1,c2;
        for(int j=0;j<w;j++){
            c1=QColor(rgb1[j]);c2=QColor(rgb2[j]);
            //65535

            if(rgb1[j]==rgb2[j]&&rgb1[j]==QColor(Qt::white).rgb())allwhite++;
            else {
                t=qFabs(c1.green()-c2.green());//t=qFabs((qFabs(c1.green()-c2.green())+qFabs(c1.blue()-c2.blue())+qFabs(c1.red()-c2.red()))/3);
                comparsion+=t/255;
            }
        }
    }

    return 100 - (comparsion*100.0)/(h*w-allwhite);
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
    qDebug() << elap - prevTime;
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
            kek=false;
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
    painter.setRenderHint(QPainter::Antialiasing);// ///////////////////////////////////////

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

void swapxy(QVector<mPoint>* arr){
    int i,x;
    for(i=0;i<arr->length();i++){
        x=(*arr)[i].pos.x();
        (*arr)[i].pos.setX((*arr)[i].pos.y());
        (*arr)[i].pos.setY(x);
    }
}

void TabletCanvas::drawStroke(QPainter &painter,const mPoint p){
    switch(m_algo){
    case BasicVal:{
        m_drawLine(painter,points.last(),p);
        break;
    }
    case BrushVal:
        m_drawPoints(painter, points.last(), p);
        break;
    case SplineVal:
        m_drawSpline(painter,p);
        break;
    }
    if(m_debug){
        m_pen.setWidthF(10);
        m_brush.setColor(Qt::cyan);
        m_pen.setColor(Qt::cyan);
        painter.setPen(m_pen);
        painter.drawPoint(p.pos);
        painter.drawPoint(points.last().pos);
    }
    update();
}

void TabletCanvas::m_drawLine(QPainter &painter, const mPoint prev, const mPoint p){
    if(m_transparencyValuator==PressureValuator||m_brushTrans<1){
        switch (m_widthValuator) {
        case PressureValuator:
            m_pen.setWidthF(p.pressure * m_brushWidth);
            break;
        case FixedValuator:
            m_pen.setWidth(m_brushWidth);
            break;
        default:
            m_pen.setWidthF(1);
        }

        m_brush.setColor(Qt::white);
        m_pen.setColor(Qt::white);
        painter.setPen(m_pen);
        painter.drawPoint(prev.pos);
    }
    updateBrush(p.pressure);
    painter.setPen(m_pen);
    painter.drawLine(prev.pos, p.pos);
    return;
}

void TabletCanvas::m_drawPoints(QPainter &painter, const mPoint prev, const mPoint p){
    qreal mp = p.pressure - prev.pressure ;
    QPoint ml = QPoint(p.pos.x()-prev.pos.x(),p.pos.y()-prev.pos.y());
    qreal l = qSqrt(ml.x()*ml.x()+ml.y()*ml.y()), mmn,x1,x2,y1,y2;
    int dots;
    if(m_transparencyValuator==PressureValuator||m_brushTrans<1){
        dots=l;
        pressuremode=1;
        pressuremode=m_brushWidth/2;
    }else{
        mmn=qMin(p.pressure,prev.pressure);
        dots=qMin(4*l/(m_brushWidth*mmn),l);

    }
    if(dots<2){
        updateBrush(prev.pressure);
        painter.setPen(m_pen);
        painter.drawPoint(prev.pos);
        updateBrush(p.pressure);
        painter.setPen(m_pen);
        painter.drawPoint(p.pos);
        return;
    }
    x1=0;y1=0;
    x2=x1;y2=y1;
    updateBrush(prev.pressure);
    painter.setPen(m_pen);
    //painter.drawPoint(prev.pos);
    for(int i=1; i<=dots;i++){
        updateBrush((prev.pressure+mp*((qreal)i/dots)));
        painter.setPen(m_pen);
        x2+=(qreal)ml.x()/dots;
        y2+=(qreal)ml.y()/dots;
        if((qFabs(qSqrt(x2*x2+y2*y2)-qSqrt(x1*x1+y1*y1)))>1){
            x1=(int)x2;y1=(int)y2;
            //painter.drawEllipse(prev.pos.x()+x2,prev.pos.y()+y2,m_brushWidth*(prev.pressure+mp*((qreal)i/dots)),m_brushWidth*(prev.pressure+mp*((qreal)i/dots)));
            painter.drawPoint(QPoint(prev.pos.x()+x2,prev.pos.y()+y2));
        }
    }
    /* for(int i=1; i<=dots;i++){
        updateBrush((prev.pressure+mp*((qreal)i/dots)));
        painter.setPen(m_pen);
        tmp = QPoint(prev.pos.x()+ml.x()*((qreal)i/dots),prev.pos.y()+ml.y()*((qreal)i/dots));
        painter.drawPoint(tmp);
        update();
    }*/
}

void TabletCanvas::m_drawSpline(QPainter &painter, mPoint p){
    if(kek){
        kek=false;
        return;
    }else{
        kek=true;
    }
    QVector<mPoint> outp, inp;
    bool swapa=false, notspline=false;
    int i;

    inp.push_back(points[0]);
    if(points.length()>1){
        if(points[1].pos!=points[0].pos)
            inp.push_back(points[1]);
        else
            inp[0].pressure=qMax(points[0].pressure,points[1].pressure);
    }

    if(inp.last().pos!=p.pos)
        inp.push_back(p);
    else
        inp.last().pressure=qMax(inp.last().pressure,p.pressure);

    if(inp.length()<3){
        //m_drawLine(painter,inp[0],inp.last());//
        m_drawPoints(painter,inp[0],inp.last());
        return;
    }

    if( inp[0].pos.x()==inp[1].pos.x()||inp[0].pos.x()==inp[2].pos.x()||inp[1].pos.x()==inp[2].pos.x()){
        swapxy(&inp);
        swapa=true;
        qDebug()<<"swapA";
    }
    if(inp[0].pos.x()>inp[1].pos.x()&&inp[1].pos.x()>inp[2].pos.x()){
        mPoint temp=inp[0];
        inp[0]=inp[2];
        inp[2]=temp;
    }
    if(inp[0].pos.x()>inp[1].pos.x()||inp[1].pos.x()>inp[2].pos.x()){
        if(!swapa){
            swapxy(&inp);
            swapa=true;
            qDebug()<<"swapA";
        }else{
            notspline=true;
        }
    }
    if(swapa){
        if( inp[0].pos.x()==inp[1].pos.x()||inp[0].pos.x()==inp[2].pos.x()||inp[1].pos.x()==inp[2].pos.x()){
            notspline=true;

        }else{
            if(inp[0].pos.x()>inp[1].pos.x()&&inp[1].pos.x()>inp[2].pos.x()){
                mPoint temp=inp[0];
                inp[0]=inp[2];
                inp[2]=temp;
            }
            if(inp[0].pos.x()>inp[1].pos.x()||inp[1].pos.x()>inp[2].pos.x()){
                notspline=true;
            }
        }
    }

    if(notspline){
        if(swapa)swapxy(&inp);
        //m_drawLine(painter,inp[0],inp[1]);//
        m_drawPoints(painter,inp[0],inp[1]);
        //m_drawLine(painter,inp[1],inp[2]);//
        m_drawPoints(painter,inp[1],inp[2]);
        return;
    }
    qreal m_min, m_max;
    m_min=qMin(qMin(inp[0].pos.y(),inp[1].pos.y()),inp[2].pos.y());
    m_max=qMax(qMax(inp[0].pos.y(),inp[1].pos.y()),inp[2].pos.y());
    calcspline(&inp,&outp,25);
    for(i=0;i<outp.length();i++){
        if(outp[i].pos.y()>(m_max+20)||outp[i].pos.y()<(m_min-20)){
            if(swapa)swapxy(&inp);
            //m_drawLine(painter,inp[2],inp[3]);//
            m_drawPoints(painter,inp[2],inp[3]);
            //m_drawLine(painter,inp[3],inp[4]);//
            m_drawPoints(painter,inp[3],inp[4]);
            return;
        }
    }
    if(swapa) swapxy(&outp);

    for(i=1;i<outp.length();i++){
        //m_drawLine(painter,outp[i-1],outp[i]);//
        m_drawPoints(painter,outp[i-1],outp[i]);
    }
    return;
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
        if(m_algo == BrushVal)m_color.setAlphaF((m_brushTrans / pressuremode)*pressure);
        else m_color.setAlphaF(pressure*m_brushTrans);
        break;
    case FixedValuator:
        if(m_algo == BrushVal&&m_brushTrans!=1 )m_color.setAlphaF(m_brushTrans / pressuremode);
        else m_color.setAlphaF(m_brushTrans);
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


void TabletCanvas::testgen(){
    const char* stepVal_names[] = {"Lin", "Geom", "Arifm"};
    const char* funcVal_names[] = {"Sqrt","Sin","ArchSpiral","Cardiode","Deltoid","Trifolium","Ranunculoid"};
    funcVal fv;stepVal sv;
    QString filename, temp ="/home/ultra/tests/";
    QVector<mPoint> arr;
    qreal as[] = {1000,100,100,250,100,500,50};
    qreal bs[] = {10,100,100,250,100,500,50};
    int steps[] = {60,60,60,60,60,60,60};
    int i, j, k, p = 10;
    bool d = false;
    for(i = 0;i<7;i++){
        fv=static_cast<funcVal>(i);
        for(j=0;j<2;j++){
            p=10;
            sv=static_cast<stepVal>(j);
            filename=temp+QString(funcVal_names[i])+QString(stepVal_names[j])+".txt";//QString("_Unideal")+".txt";
            QFile File(filename);
            File.resize(0);

            if(!File.open(QIODevice::WriteOnly))return;

            gtestArray(&arr,fv,sv,steps[i],as[i],bs[i]);
            for(k=0;k<arr.size();k++){
                arr[k].pos.setX(arr[k].pos.x()+width()/2);
                arr[k].pos.setY(arr[k].pos.y()+height()/2);
            }

            QTextStream(&File)<<this->width()<<" "<<this->height()<<"\n";
            for(k=0;k<arr.size();k++){
                QTextStream(&File)<<arr[k].pos.x() <<" ";
                QTextStream(&File)<<arr[k].pos.y()<<" ";
                QTextStream(&File)<<arr[k].pressure<<"\n";//(0.1*((int)(arr[k].pressure)%10))<<"\n";
                if(p==10)d=false;
                if(p==0)d=true;
                if(d)p++;
                else p--;

            }
            if(i>2){
                QTextStream(&File)<<arr[0].pos.x() <<" ";
                QTextStream(&File)<<arr[0].pos.y()<<" ";
                QTextStream(&File)<<arr[0].pressure<<"\n";
            }
            File.close();
            arr.clear();
        }
        filename=temp+QString(funcVal_names[i])+QString("_Ideal")+".txt";
        QFile File(filename);
        File.resize(0);

        if(!File.open(QIODevice::WriteOnly))return;

        gtestArray(&arr,fv,Lin,1000,as[i],bs[i]);
        for(k=0;k<arr.size();k++){
            arr[k].pos.setX(arr[k].pos.x()+width()/2);
            arr[k].pos.setY(arr[k].pos.y()+height()/2);
        }
        p=100;
        QTextStream(&File)<<this->width()<<" "<<this->height()<<"\n";
        for(k=0;k<arr.size();k++){
            QTextStream(&File)<<arr[k].pos.x() <<" ";
            QTextStream(&File)<<arr[k].pos.y()<<" ";
            QTextStream(&File)<<arr[k].pressure<<"\n";//(0.01*((int)(arr[k].pressure)%100))<<"\n";
            if(p==100)d=false;
            if(p==0)d=true;
            if(d)p++;
            else p--;

        }
        File.close();
        arr.clear();
    }
}

void TabletCanvas::drawtests(){
    const char* algoVal_names[] = {"Basic", "Mod","Spline"};
    const char* stepVal_names[] = {"Lin", "Geom","Arifm"};
    const char* funcVal_names[] = {"Sqrt","Sin","ArchSpiral","Cardiode","Deltoid","Trifolium","Ranunculoid"};
    QString filename, in ="/home/ultra/tests/", out = "/home/ultra/results/", temp;
    QPoint t;
    AlgoValuator valsav=m_algo;
    int i, j, k;
    for(k=0;k<3;k++){
        out = "/home/ultra/results/"+QString(algoVal_names[k])+"/";
        m_algo=static_cast<AlgoValuator>(k);
        for(i = 0;i<7;i++){
            m_algo=static_cast<AlgoValuator>(k);
            QFile ttime;
            for(j=0;j<2;j++){
                newImage();
                temp = QString(funcVal_names[i])+QString(stepVal_names[j]);//+QString("_Unideal");
                ttime.setFileName(out+"z"+QString(stepVal_names[j])+"time.txt");

                ttime.open(QIODevice::Append);
                filename=in+temp+".txt";
                t = playStroke(filename);
                filename=out+temp+".png";//+"Time("+QString::number(t.x())+")Repeats("+QString::number(t.y())+")"+".bmp";
                saveImage(filename);
                QTextStream(&ttime)<<t.x()<<"\n";

            }
            m_algo=BasicVal;
            if(k==0){
                newImage();
                filename=in+QString(funcVal_names[i])+QString("_Ideal")+".txt";
                t = playStroke(filename);
                filename="/home/ultra/results/"+QString(funcVal_names[i])+QString("_Ideal")+"Time("+QString::number(t.x())+")Repeats)"+QString::number(t.y())+")"+".bmp";
                saveImage(filename);
            }
        }
    }
    m_algo=valsav;
}

void TabletCanvas::compareResults(){
    const char* algoVal_names[] = {"Basic", "Mod","Spline"};
    QDir dir=QDir("/home/ultra/results/");
    QStringList ideal, drawn[3];
    QImage a,b;
    QFile zres[3];
    int i,j;
    qreal t;
    for(i=0;i<3;i++){
        zres[i].setFileName("/home/ultra/results/"+QString(algoVal_names[i])+"/zres.txt");
        zres[i].resize(0);
        if(!zres[i].open(QIODevice::WriteOnly))return;
        drawn[i]=QDir("/home/ultra/results/"+QString(algoVal_names[i])+"/").entryList();
    }

    QString t1,t2;

    ideal=dir.entryList();
    ideal.removeAt(9);ideal.removeAt(6);ideal.removeAt(3);
    for(i=2;i<16;i++){
        for(j=0;j<3;j++){
            t1=QString("/home/ultra/results/"+QString(algoVal_names[j])+"/"+ drawn[j][i]);
            t2=QString("/home/ultra/results/"+ideal[2+(i-2)/2]);
            a.load(t1);
            b.load(t2);
            t = compareImages(a,b);
            QTextStream(&zres[j])<<t<<"\n";
        }
    }
}
