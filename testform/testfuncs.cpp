#include "testfuncs.h"
#include <QtMath>

void gtestArray(QVector<mPoint>* arr, funcVal func, stepVal step, long stepmod, qreal a, qreal b){
    int j;
    double temp;
    qreal i;
    qreal geom=1.2;
    switch(func){
    case Sqrt:
        switch(step){
        case Lin:
            for(i=0;i<stepmod;i++)
                arr->push_back(mPoint(a*i/stepmod,b*qSqrt(a*i/stepmod),i/stepmod));
            break;
        case Arifm:
            i=0;j=1;
            while(i<stepmod){
                arr->push_back(mPoint(a*i/stepmod,b*qSqrt(a*i/stepmod),i/stepmod));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            arr->push_back(mPoint(0,0,0));
            for(i=1;i<stepmod;i*=geom){
                arr->push_back(mPoint(a*i/stepmod,b*qSqrt(a*i/stepmod),i/stepmod));
            }
            arr->push_back(mPoint(a,b*qSqrt(a),1));
        }
        break;
    case Sin:
        switch(step){
        case Lin:
            for(i=-stepmod;i<stepmod;i++)
                arr->push_back(mPoint(a*i*2*M_PI/stepmod,b*qSin(i*2*M_PI/stepmod),qFabs(i)/stepmod));
            break;
        case Arifm:
            i=0;j=1;
            while((i-stepmod)<stepmod){
                arr->push_back(mPoint(a*(i-stepmod)*2*M_PI/stepmod,b*qSin(a*(i-stepmod)*2*M_PI/stepmod),qFabs(i-stepmod)/stepmod));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            for(i=0.01;i<2*stepmod;i*=geom){
                arr->push_back(mPoint(a*(i-stepmod)*2*M_PI/stepmod,b*qSin((i-stepmod)*2*M_PI/stepmod),qFabs(i-stepmod)/stepmod));
            }
            arr->push_back(mPoint(a*2*M_PI,b*qSin(2*M_PI),1));
        }
        break;
    case ArchSpiral:
        switch(step){
        case Lin:
            for(i=0;i<stepmod;i++)
                arr->push_back(mPoint(archspiral(a,b,i*2*M_PI/stepmod),i/stepmod));
            break;
        case Arifm:
            i=0;j=1;
            while(i<stepmod){
                arr->push_back(mPoint(archspiral(a,b,i*2*M_PI/stepmod),i/stepmod));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            arr->push_back(mPoint(0,0,0));
            for(i=1;i<stepmod;i*=geom){
                arr->push_back(mPoint(archspiral(a,b,i*2*M_PI/stepmod),i/stepmod));
            }
            arr->push_back(mPoint(archspiral(a,b,2*M_PI),1));
        }
        break;
    case Cardiode:
        switch(step){
        case Lin:
            for(i=0;i<stepmod;i++)
                arr->push_back(mPoint(cardioide(a,b,i*2*M_PI/stepmod),qFabs(1-2*i/stepmod)));
            break;
        case Arifm:
            i=0;j=1;
            while(i<stepmod){
                arr->push_back(mPoint(cardioide(a,b,i*2*M_PI/stepmod),qFabs(1-2*i/stepmod)));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            arr->push_back(mPoint(cardioide(a,b,0),0));
            for(i=1;i<stepmod;i*=geom){
                arr->push_back(mPoint(cardioide(a,b,i*2*M_PI/stepmod),qFabs(1-2*i/stepmod)));
            }
        }
        break;
    case Deltoid:
        switch(step){
        case Lin:
            for(i=0;i<stepmod;i++)
                arr->push_back(mPoint(deltoid(a,b,i*2*M_PI/stepmod),modf(3*i/stepmod,&temp)));
            break;
        case Arifm:
            i=0;j=1;
            while(i<stepmod){
                arr->push_back(mPoint(deltoid(a,b,i*2*M_PI/stepmod),modf(3*i/stepmod,&temp)));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            arr->push_back(mPoint(deltoid(a,b,0),0));
            for(i=1;i<stepmod;i*=geom){
                arr->push_back(mPoint(deltoid(a,b,i*2*M_PI/stepmod),modf(3*i/stepmod,&temp)));
            }
        }
        break;
    case Trifolium:
        switch(step){
        case Lin:
            for(i=0;i<stepmod;i++)
                arr->push_back(mPoint(trifolium(a,b,i*M_PI/stepmod),modf(0.5*stepmod+3*i/stepmod,&temp)));
            break;
        case Arifm:
            i=0;j=1;
            while(i<stepmod){
                arr->push_back(mPoint(trifolium(a,b,i*M_PI/stepmod),modf(0.5*stepmod+3*i/stepmod,&temp)));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            arr->push_back(mPoint(trifolium(a,b,0),0));
            for(i=1;i<stepmod;i*=geom){
                arr->push_back(mPoint(trifolium(a,b,i*M_PI/stepmod),modf(0.5*stepmod+3*i/stepmod,&temp)));
            }
        }
        break;
    case Ranunculoid:
        switch(step){
        case Lin:
            for(i=0;i<stepmod;i++)
                arr->push_back(mPoint(ranunculoid(a,b,i*2*M_PI/stepmod),modf(5*i/stepmod,&temp)));
            break;
        case Arifm:
            i=0;j=1;
            while(i<stepmod){
                arr->push_back(mPoint(ranunculoid(a,b,i*2*M_PI/stepmod),modf(5*i/stepmod,&temp)));
                i+=j;
                j+=1;
            }
            break;
        case Geom:
            arr->push_back(mPoint(ranunculoid(a,b,0),0));
            for(i=1;i<stepmod;i*=geom){
                arr->push_back(mPoint(ranunculoid(a,b,i*2*M_PI/stepmod),modf(5*i/stepmod,&temp)));
            }
        }
        break;
    }
}

QPoint archspiral(qreal a, qreal b, qreal t)// 2pi circle to inf
{
    return QPoint(a*t*qCos(t),b*t*qSin(t));
}

QPoint cardioide(qreal a, qreal b, qreal t)// 2pi
{
    return QPoint(a*(1-qCos(t))*qCos(t),b*qSin(t)*(1-qCos(t)));
}

QPoint deltoid(qreal a, qreal b, qreal t)// 2pi
{
    return QPoint(a*(2*qCos(t)+qCos(2*t)),b*(2*qSin(t)-qSin(2*t)));
}

QPoint trifolium(qreal a, qreal b, qreal t){//pi
    return QPoint(-a*qCos(t)*qCos(3*t),-b*qSin(t)*qCos(3*t));
}

QPoint ranunculoid(qreal a, qreal b, qreal t){//2pi
    return QPoint(a*(6*qCos(t)-qCos(6*t)),b*(6*qSin(t)-qSin(6*t)));
}


