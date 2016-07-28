#include "aitkeninterpolation.h"

AitkenInterpolation::AitkenInterpolation()
{

}

AitkenInterpolation::~AitkenInterpolation()
{

}

QVector<QPointF> AitkenInterpolation::interpolate(QVector<QPointF> points, double step)
{
    if (points.isEmpty()) {
        return QVector<QPointF>();
    }

    std::sort(points.begin(), points.end(), [] (QPointF p1, QPointF p2) { return p1.x() < p2.x(); });

    QVector<QPointF> resultPoints;

    double currentX = points[0].x();
    while (currentX <= points.last().x()) {
        resultPoints += QPointF(currentX, aitkenInterpolatedValueAtPoint(currentX, 0, points.count() - 1, points));
        currentX += step;
    }
    if (resultPoints.last().x() != points.last().x()) {
        resultPoints += points.last();
    }
    return resultPoints;
}

double AitkenInterpolation::getValueAtPoint(QVector<QPointF> points, double point)
{
    if (points.isEmpty()) {
        return 0;
    }
    return aitkenInterpolatedValueAtPoint(point, 0, points.count() - 1, points);
}

double AitkenInterpolation::aitkenInterpolatedValueAtPoint(double x, int rangeBegin, int rangeEnd, QVector<QPointF> points)
{
    int pointsCount = points.count();
    int rangeSize = rangeEnd - rangeBegin;

    if (rangeSize < 0 || rangeBegin > pointsCount - 1 || rangeEnd > pointsCount - 1) {
        qDebug() << "Wrong bounds!";
        return 0;
    } else if (rangeSize == 0) {
        return points[rangeBegin].y();
    } else {
        return (x - points[rangeEnd].x()) * aitkenInterpolatedValueAtPoint(x, rangeBegin, rangeEnd - 1, points) / (points[rangeBegin].x() - points[rangeEnd].x()) +
               (x - points[rangeBegin].x()) * aitkenInterpolatedValueAtPoint(x, rangeBegin + 1, rangeEnd, points) / (points[rangeEnd].x() - points[rangeBegin].x());
    }
}
