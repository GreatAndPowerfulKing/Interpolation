#ifndef SPLINESINTERPOLATION_H
#define SPLINESINTERPOLATION_H

#include <QVector>
#include <QPointF>
#include <cmath>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include "interpolation.h"

class SplinesInterpolation  : public Interpolation
{
public:
    SplinesInterpolation();
    ~SplinesInterpolation();

public:
    static QVector<QPointF> interpolate(QVector<QPointF> points, double step, QVector<QVector<double> > &coefficients);
    static double getValueAtPoint(QVector<QPointF> points, double point);

private:
    static QVector<QVector<double> > getCoefficientsMatrix(QVector<QPointF> points);
};

#endif // SPLINESINTERPOLATION_H
