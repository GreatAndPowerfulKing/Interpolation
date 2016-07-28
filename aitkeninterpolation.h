#ifndef AITKENINTERPOLATION_H
#define AITKENINTERPOLATION_H

#include <QVector>
#include <QPointF>
#include <cmath>
#include <QDebug>
#include <QDateTime>
#include "interpolation.h"

class AitkenInterpolation : public Interpolation
{
public:
    AitkenInterpolation();
    ~AitkenInterpolation();

public:
    static QVector<QPointF> interpolate(QVector<QPointF> points, double step);
    static double getValueAtPoint(QVector<QPointF> points, double point);

private:
    static double aitkenInterpolatedValueAtPoint(double x, int rangeBegin, int rangeEnd, QVector<QPointF> points);
};

#endif // AITKENINTERPOLATION_H
