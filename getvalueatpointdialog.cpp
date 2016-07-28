#include "getvalueatpointdialog.h"
#include "ui_getvalueatpointdialog.h"

GetValueAtPointDialog::GetValueAtPointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetValueAtPointDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);
}

GetValueAtPointDialog::~GetValueAtPointDialog()
{
    delete ui;
}

void GetValueAtPointDialog::prepareForValueGetting(QVector<QPointF> points)
{
    if (!points.isEmpty()) {
        std::sort(points.begin(), points.end(), [] (QPointF p1, QPointF p2) { return p1.x() < p2.x(); });
        ui->xSpinBox->setMinimum(points.first().x());
        ui->xSpinBox->setMaximum(points.last().x());
        ui->xSpinBox->setValue(points.first().x());
        this->points = points;
    }
}

void GetValueAtPointDialog::on_pushButton_clicked()
{
    double x = ui->xSpinBox->value();
//    qDebug() << AitkenInterpolation::getValueAtPoint(points, x);
    ui->splinesResultLabel->setText(QString::number(SplinesInterpolation::getValueAtPoint(points, x), 'g', 2));
    ui->aitkenResultLabel->setText(QString::number(AitkenInterpolation::getValueAtPoint(points, x), 'g', 2));
}
