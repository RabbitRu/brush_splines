#ifndef MPOINT_H
#define MPOINT_H
#include <QPoint>

typedef struct mPoint {
public:
    QPoint pos;
    qreal pressure;
    mPoint();
    mPoint(const QPoint& p, qreal r);
    mPoint(qreal x, qreal y, qreal r);
} mPoint;

#endif // MPOINT_H
