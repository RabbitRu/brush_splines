#ifndef ASPLINE_H
#define ASPLINE_H

#endif // ASPLINE_H

#include "tabletcanvas.h"

void computeXlimits(QVector<mPoint>* points,int s, int e, qreal *llimit, qreal *ulimit);

void calcspline(QVector<mPoint>* points, QVector<mPoint>* outp, int d);
