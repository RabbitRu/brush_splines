#ifndef TESTFUNCS_H
#define TESTFUNCS_H
#include <QPoint>
#include <QtMath>
#include <QVector>
#include "mpoint.h"

enum funcVal{Sqrt,Sin,ArchSpiral,Cardiode,Deltoid,Trifolium,Ranunculoid};

enum stepVal{Lin, Geom, Arifm};

void gtestArray(QVector<mPoint>* arr, funcVal func, stepVal step, long stepmod, qreal a, qreal b);

QPoint archspiral(qreal a, qreal b, qreal t);

QPoint cardioide(qreal a, qreal b, qreal t);

QPoint deltoid(qreal a, qreal b, qreal t);

QPoint trifolium(qreal a, qreal b, qreal t);

QPoint ranunculoid(qreal a, qreal b, qreal t);

#endif // TESTFUNCS_H
