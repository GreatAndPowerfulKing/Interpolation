#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <QVector>
#include <QPointF>
#include <cmath>
#include <QDebug>

class Interpolation
{
public:
    Interpolation();
    ~Interpolation();

public:
    static QVector<QPointF> interpolate(QVector<QPointF> points, double step, QVector<QVector<double> > &coefficients);
    static double getValueAtPoint(QVector<QPointF> points, double point);


};

#endif // INTERPOLATION_H
