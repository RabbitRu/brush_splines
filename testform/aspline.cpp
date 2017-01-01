
#include "aspline.h"

void computeXlimits(QVector<mPoint>* points,int s, int e, qreal *llimit, qreal *ulimit)
{
    int i;

    *llimit=std::numeric_limits<qreal>::max();
    *ulimit=-std::numeric_limits<qreal>::max();
    for (i=s; i < e; i++) {
        if ((*points)[i].pos.x() < *llimit) *llimit=(*points)[i].pos.x();
        if ((*points)[i].pos.x() > *ulimit) *ulimit=(*points)[i].pos.x();
    }
}

void calcspline(QVector<mPoint>* points, QVector<mPoint>* outp, int ammount){
    qreal llimit=0.0,ulimit=0.0, xstep=1.0, pstep=0.0;
    int i,n=points->length(), d=ammount-3;


    if (n > 0) {
        if (n == 1) {
            outp->push_back(points->last());
        } else if (n == 2) { /* linear interpolation */
            qreal dx, dy, m;

            dx=(*points)[1].pos.x()-(*points)[0].pos.x();
            dy=(*points)[1].pos.y()-(*points)[0].pos.y();
            computeXlimits(points, 0,n,&llimit, &ulimit);
            xstep=(ulimit-llimit)/d;
            pstep=((*points)[1].pressure-(*points)[0].pressure)/d;
            m=dy/dx;

            for (i=0; i < d; i++)
                outp->push_back(mPoint((*points)[0].pos.x()+i*xstep, (*points)[0].pos.y()+i*m*xstep,(*points)[0].pressure+i*pstep));
            outp->push_back((*points)[1]);
        } else {
            n += 4;//we will extrapolate 4 points
            int p;                         /* n=no of points */
            qreal dx[n], dy[n];            /* x[i+1]-x[i], resp. y[i+1]-y[i] */
            qreal m[n], t[n];              /* the slopes */
            qreal C[n], D[n];              /* the coefficients */
            int steps, cstep = 1;               //m - razd raznost
            qreal xv, pv = 0,num, den, xd;
            /* calculate coefficients of the spline (Akima interpolation itself)*/
            /* a) Calculate the differences and the slopes m[i]. */
            for (i=0; i < n-5; i++) {
                dx[i+2]=(*points)[i+1].pos.x() - (*points)[i].pos.x();
                dy[i+2]=(*points)[i+1].pos.y() - (*points)[i].pos.y();
                m[i+2] =dy[i+2]/dx[i+2]; /* dx != 0 by defenition */
            }

            /* b) interpolate the missing points: */
            points->push_front(mPoint(
                                   (*points)[0].pos.x() + (*points)[1].pos.x() - (*points)[2].pos.x(),
                    0,//depends on dx
                    0));
            dx[1]=(*points)[1].pos.x() - (*points)[0].pos.x();
            (*points)[0].pos.setY(dx[1]*(m[3] - 2*m[2]) + (*points)[1].pos.y());
            dy[1]=(*points)[1].pos.y() - (*points)[0].pos.y();
            m[1]=dy[1]/dx[1];

            points->push_front(mPoint(
                                   2*(*points)[1].pos.x() - (*points)[3].pos.x(),
                    0,//depends on dx
                    0));
            dx[0]=(*points)[1].pos.x() - (*points)[0].pos.x();
            (*points)[0].pos.setY(dx[0]*(m[2] - 2*m[1]) + (*points)[1].pos.y());
            dy[0]=(*points)[1].pos.y() - (*points)[0].pos.y();
            m[0]=dy[0]/dx[0];

            points->push_back(mPoint(
                                  (*points)[n-3].pos.x() + (*points)[n-4].pos.x() - (*points)[n-5].pos.x(),
                    (2*m[n-4] - m[n-5])*((*points)[n-3].pos.x() + (*points)[n-4].pos.x() - (*points)[n-5].pos.x() - (*points)[n-3].pos.x()) + (*points)[n-3].pos.y(),
                    0));

            points->push_back(mPoint(
                                  2*(*points)[n-3].pos.x() - (*points)[n-5].pos.x(),
                    (2*m[n-3] - m[n-4])*(2*(*points)[n-3].pos.x() - (*points)[n-5].pos.x() - (*points)[n-2].pos.x()) + (*points)[n-2].pos.y(),
                    0));

            for (i=n-3; i < n-1; i++) {
                dx[i]=(*points)[i+1].pos.x() - (*points)[i].pos.x();
                dy[i]=(*points)[i+1].pos.y() - (*points)[i].pos.y();
                m[i]=dy[i]/dx[i];
            }

            /* the first x slopes and the last y ones are extrapolated: */
            t[0]=0.0; t[1]=0.0;
            for (i=2; i < n-2; i++) {
                num=qFabs(m[i+1] - m[i])*m[i-1] + qFabs(m[i-1] - m[i-2])*m[i];
                den=qFabs(m[i+1] - m[i]) + qFabs(m[i-1] - m[i-2]);

                if (den != 0) t[i]=num/den; //fpclassify(den) > FP_ZERO
                else                            t[i]=0.0;
            }//t Это di в методичке или c2

            /* c) Allocate the polynom coefficients */
            /* A=y_i        B=t_i */

            for (i=2; i < n-2; i++) {
                /* A[i]=y[i];           B[i]=t[i]; */
                C[i]=(3*m[i] - 2*t[i] - t[i+1])/dx[i];// c3
                D[i]=(t[i] + t[i+1] - 2*m[i])/(dx[i]*dx[i]);//c4
            }

            /* 3rd step: output the coefficients for the subintervalls i=2..n-4 */
            /* calculate the intermediate values */

            computeXlimits(points,2,n-2,&llimit, &ulimit);
            xstep=(ulimit-llimit)/d;

            p=2;
            for(xv=llimit; xv<ulimit+xstep; xv += xstep) {
                while (xv >= (*points)[p].pos.x()) {
                    outp->push_back((*points)[p]);
                    p++;
                    steps = ((*points)[p].pos.x()-xv)/xstep;
                    pv = ((*points)[p].pressure - (*points)[p-1].pressure)/steps;
                    cstep=1;
                }


                /* skip the next interpolated point if it's too close to the current point */
                if (((xv - (*points)[p-1].pos.x()) > xstep/100.0) &&
                        (((*points)[p].pos.x() - xv) > xstep/100.0)) {
                    xd=(xv-(*points)[p-1].pos.x());
                    outp->push_back(mPoint(xv, (*points)[p-1].pos.y() + (t[p-1] + (C[p-1] + D[p-1]*xd)*xd)*xd, (*points)[p-1].pressure+pv*cstep));
                    cstep++;
                }
            }
        }
    }
}

