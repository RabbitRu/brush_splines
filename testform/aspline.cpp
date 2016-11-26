/***************************************************************************
 * aspline.c -- aspline does an Akima spline interpolation.                *
 *                                                                         *
 * spline is (c) David Frey, 1996, 1998, 1999, 2002, 2009                  *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program; if not, write to the Free Software Foundation, *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>

#include "QDebug"
#include "tabletcanvas.h"

void computelimits(QVector<mPoint>* points,int s, int e, double *llimit, double *ulimit)
/* Compute minimum and maximum of the x-values */
{
    int i;

    *llimit=FLT_MAX; *ulimit=-FLT_MAX;
    for (i=s; i < e; i++) {
        if ((*points)[i].pos.x() < *llimit) *llimit=(*points)[i].pos.x();
        if ((*points)[i].pos.x() > *ulimit) *ulimit=(*points)[i].pos.x();
    }
}

void computelimitp(QVector<mPoint>* points,int s, int e, double *llimit, double *ulimit)
/* Compute minimum and maximum of the x-values */
{
    int i;

    *llimit=1; *ulimit=0;
    for (i=s; i < e; i++) {
        if ((*points)[i].pressure < *llimit) *llimit=(*points)[i].pressure;
        if ((*points)[i].pressure > *ulimit) *ulimit=(*points)[i].pressure;
    }
}

qreal max(qreal a,qreal b){
    if(a>b)return a;
    return b;
}

void calcspline(QVector<mPoint>* points, QVector<mPoint>* outp, int d){
    double llimit=0.0,ulimit=0.0, xstep=1.0, pstep=0.0;
    int n=points->length(), i;


    if (n > 0) { /* we have data to process */
        if (n == 1) { /* but only one point :( */
            outp->push_back(points->last());
        } else if (n == 2) { /* wow, two points! */
            double dx, dy, m;

            dx=(*points)[1].pos.x()-(*points)[0].pos.x();
            dy=(*points)[1].pos.y()-(*points)[0].pos.y();

            /* 3rd step: output the coefficients for the subintervalls i=2..n-4 */
            /* calculate the intermediate values */

            computelimits(points, 0,n,&llimit, &ulimit);
            xstep=(ulimit-llimit)/d;

            pstep=((*points)[1].pressure-(*points)[0].pressure)/d;

            m=dy/dx;
            for (i=0; i <= d; i++)
                outp->push_back(mPoint((*points)[0].pos.x()+i*xstep, (*points)[0].pos.y()+i*m*xstep,(*points)[0].pressure+i*pstep));
        } else { /* now we have something to compute */

            /* Leading extrapolation points,
         actual values will be filled in later */
            //            x=(double *)xrealloc(x, (n+4) * sizeof(double));
            //            y=(double *)xrealloc(y, (n+4) * sizeof(double));
            /* Move x[0] to x[2] */
            //            memmove(&x[2],&x[0], (size_t)(n*sizeof(double)));
            //            memmove(&y[2],&y[0], (size_t)(n*sizeof(double)));
            points->push_front(mPoint());
            points->push_front(mPoint());
            points->push_back(mPoint());
            points->push_back(mPoint());
            n += 4;

            int p;                  /* n=no of points */
            double dx[n], dy[n];           /* x[i+1]-x[i], resp. y[i+1]-y[i] */
            double m[n], t[n];             /* the slopes */
            double C[n], D[n];             /* the coefficients */




            /* calculate coefficients of the spline (Akima interpolation itself)
       * NB: There are some unused locations (dx[0]),
       *     but this faciliates the indexing.
       */

            /* a) Calculate the differences and the slopes m[i]. */

            for (i=2; i < n-3; i++) {
                dx[i]=(*points)[i+1].pos.x() - (*points)[i].pos.x();
                dy[i]=(*points)[i+1].pos.y() - (*points)[i].pos.y();
                m[i] =dy[i]/dx[i]; /* dx != 0, asserted by insertpoint() */
            }

            /* b) interpolate the missing points: */

            (*points)[1].pos.setX((*points)[2].pos.x() + (*points)[3].pos.x() - (*points)[4].pos.x());
            dx[1]=(*points)[2].pos.x() - (*points)[1].pos.x();
            (*points)[1].pos.setY(dx[1]*(m[3] - 2*m[2]) + (*points)[2].pos.y());
            dy[1]=(*points)[2].pos.y() - (*points)[1].pos.y();
            m[1]=dy[1]/dx[1];

            (*points)[0].pos.setX(2*(*points)[2].pos.x() - (*points)[4].pos.x());
            dx[0]=(*points)[1].pos.x() - (*points)[0].pos.x();
            (*points)[0].pos.setY(dx[0]*(m[2] - 2*m[1]) + (*points)[1].pos.y());
            dy[0]=(*points)[1].pos.y() - (*points)[0].pos.y();
            m[0]=dy[0]/dx[0];

            (*points)[n-2].pos.setX((*points)[n-3].pos.x() + (*points)[n-4].pos.x() - (*points)[n-5].pos.x());
            (*points)[n-2].pos.setY((2*m[n-4] - m[n-5])*((*points)[n-2].pos.x() - (*points)[n-3l].pos.x()) + (*points)[n-3].pos.y());

            (*points)[n-1].pos.setX(2*(*points)[n-3].pos.x() - (*points)[n-5].pos.x());
            (*points)[n-1].pos.setY((2*m[n-3] - m[n-4])*((*points)[n-1].pos.x() - (*points)[n-2].pos.x()) + (*points)[n-2].pos.y());

            for (i=n-3; i < n-1; i++) {
                dx[i]=(*points)[i+1].pos.x() - (*points)[i].pos.x();
                dy[i]=(*points)[i+1].pos.y() - (*points)[i].pos.y();
                m[i]=dy[i]/dx[i];
            }

            /* the first x slopes and the last y ones are extrapolated: */

            t[0]=0.0; t[1]=0.0;  /* not relevant */
            for (i=2; i < n-2; i++) {
                double num, den;

                num=fabs(m[i+1] - m[i])*m[i-1] + fabs(m[i-1] - m[i-2])*m[i];
                den=fabs(m[i+1] - m[i]) + fabs(m[i-1] - m[i-2]);

                if (fpclassify(den) != FP_ZERO) t[i]=num/den;
                else                            t[i]=0.0;
            }

            /* c) Allocate the polynom coefficients */
            /* A=y_i        B=t_i */

            for (i=2; i < n-2; i++) {
                /* A[i]=y[i];           B[i]=t[i]; */
                C[i]=(3*m[i] - 2*t[i] - t[i+1])/dx[i];
                D[i]=(t[i] + t[i+1] - 2*m[i])/(dx[i]*dx[i]);
            }

            /* 3rd step: output the coefficients for the subintervalls i=2..n-4 */
            /* calculate the intermediate values */

            computelimits(points,2,n-2,&llimit, &ulimit);
            xstep=(ulimit-llimit)/d;

            p=2;
            int steps, cstep;
            double xv, pv;
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
                    double xd=(xv-(*points)[p-1].pos.x());
                    printf("%g %g %g\n", xv, (*points)[p-1].pos.y() + (t[p-1] + (C[p-1] + D[p-1]*xd)*xd)*xd,((*points)[p-1].pressure+(*points)[p-1].pressure)/2);
                    //qDebug()<<xv<<"   "<<(*points)[p-1].pos.y() + (t[p-1] + (C[p-1] + D[p-1]*xd)*xd)*xd;
                    outp->push_back(mPoint(xv, (*points)[p-1].pos.y() + (t[p-1] + (C[p-1] + D[p-1]*xd)*xd)*xd, (*points)[p-1].pressure+pv*cstep));
                    cstep++;
                }
            }
        }
    }
}

