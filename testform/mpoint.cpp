#include "mpoint.h"

mPoint::mPoint(){
    pressure=0;
    pos=QPoint(0,0);
}
mPoint::mPoint(const QPoint& p, qreal r){
    pos=p;
    pressure=r;
}
mPoint::mPoint(qreal x, qreal y, qreal r){
    pos=QPoint(x,y);
    pressure=r;
}
