#include "splinesinterpolation.h"

SplinesInterpolation::SplinesInterpolation()
{

}

SplinesInterpolation::~SplinesInterpolation()
{

}

QVector<QPointF> SplinesInterpolation::interpolate(QVector<QPointF> points, double step, QVector<QVector<double> > &coefficients)
{
    if (points.isEmpty()) {
        return QVector<QPointF>();
    }

    int pointsCount = points.count();
    std::sort(points.begin(), points.end(), [] (QPointF p1, QPointF p2) { return p1.x() < p2.x(); });

    coefficients = getCoefficientsMatrix(points);

    // Approximate function values calculation
    QVector<QPointF> resultPoints;
    double currentX = points[0].x();
    while (currentX < points[pointsCount - 1].x()) {
        int index;
        double currentY;
        for (index = 1; index < pointsCount - 1; index++) {
            if (currentX < points[index].x()) {
                break;
            }
        }
        currentY = coefficients[index - 1][0] + coefficients[index - 1][1] * (currentX - points[index].x()) +
                coefficients[index - 1][2] / 2 * (currentX - points[index].x()) * (currentX - points[index].x()) +
                coefficients[index - 1][3] / 6 * (currentX - points[index].x()) *
                (currentX - points[index].x()) * (currentX - points[index].x());
        resultPoints.append(QPointF(currentX, currentY));
        currentX += step;
    }
    if (resultPoints.last().x() != points.last().x()) {
        resultPoints += points.last();
    }
    return resultPoints;
}

double SplinesInterpolation::getValueAtPoint(QVector<QPointF> points, double point)
{
    if (points.isEmpty()) {
        return 0;
    }
    int pointsCount = points.count();
    std::sort(points.begin(), points.end(), [] (QPointF p1, QPointF p2) { return p1.x() < p2.x(); });

    QVector<QVector<double> > coefficients = getCoefficientsMatrix(points);
    int index;
    if (point <= points.first().x()) {
        index = 1;
    } else if (point >= points.last().x()) {
        index = pointsCount - 1;
    } else {
        for (int i = 1; i < pointsCount; i++) {
            if (points[i - 1].x() <= point && point <= points[i].x()) {
                index = i;
                break;
            }
        }
    }
    return coefficients[index - 1][0] + coefficients[index - 1][1] * (point - points[index].x()) +
            coefficients[index - 1][2] / 2 * (point - points[index].x()) * (point - points[index].x()) +
            coefficients[index - 1][3] / 6 * (point - points[index].x()) *
            (point - points[index].x()) * (point - points[index].x());
}

QVector<QVector<double> > SplinesInterpolation::getCoefficientsMatrix(QVector<QPointF> points)
{
    int pointsCount = points.count();
    std::sort(points.begin(), points.end(), [] (QPointF p1, QPointF p2) { return p1.x() < p2.x(); });

    QVector<double> h(pointsCount - 1);
    for (int i = 0; i < pointsCount - 1; i++) {
        h[i] = points[i + 1].x() - points[i].x();
    }

    // 3-diag matrix for splines' coefficients calculation
    QVector<double> f(pointsCount);
    QVector<QVector<double> > matrix(pointsCount, QVector<double>(pointsCount, 0));
    f[0] = 0;
    f[pointsCount - 1] = 0;
    matrix[0][0] = 1;
    matrix[pointsCount - 1][pointsCount - 1] = 1;
    for (int i = 1; i < pointsCount - 1; i++) {
        f[i] = 6 * ((points[i + 1].y() - points[i].y()) / h[i] - (points[i].y() - points[i - 1].y()) / h[i - 1]);
        matrix[i][i - 1] = h[i - 1];
        matrix[i][i] = 2 * (h[i - 1] + h[i]);
        matrix[i][i + 1] = h[i];
    }

    // 3-diag matrix solving
    QVector<double> c(pointsCount, 0);
    QVector<double> d(pointsCount, 0);
    c[0] = matrix[0][1] / matrix[0][0];
    c[pointsCount - 1] = 0;
    for (int i = 1; i < pointsCount - 1; i++) {
        c[i] = matrix[i][i + 1] / (matrix[i][i] - c[i - 1] * matrix[i][i - 1]);
    }
    d[0] = f[0] / matrix[0][0];
    for (int i = 1; i < pointsCount; i++) {
        d[i] = (f[i] - d[i - 1] * matrix[i][i - 1]) / (matrix[i][i] - c[i - 1] * matrix[i][i - 1]);
    }

    // Splines coefficients calculation
    QVector<QVector<double> > coefficients = QVector<QVector<double> >(pointsCount - 1, QVector<double>(4, 0));
    coefficients[pointsCount - 2][2] = d[pointsCount - 1];
    for (int i = pointsCount - 3; i >= 0; i--) {
        coefficients[i][2] = d[i + 1] - c[i + 1] * coefficients[i + 1][2];
    }
    coefficients[0][3] = coefficients[0][2] / h[0];
    for (int i = 1; i < pointsCount - 1; i++) {
        coefficients[i][3] = (coefficients[i][2] - coefficients[i - 1][2]) / h[i - 1];
    }
    for (int i = 0; i < pointsCount - 1; i++) {
        coefficients[i][1] = (h[i] / 2) * coefficients[i][2] - ((h[i] * h[i]) / 6) * coefficients[i][3] +
                (points[i + 1].y() - points[i].y()) / h[i];
    }
    for (int i = 0; i < pointsCount - 1; i++) {
        coefficients[i][0] = points[i + 1].y();
    }
    return coefficients;
}
