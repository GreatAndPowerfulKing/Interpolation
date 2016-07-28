#ifndef GETVALUEATPOINTDIALOG_H
#define GETVALUEATPOINTDIALOG_H

#include <QDialog>
#include <QDebug>
#include "splinesinterpolation.h"
#include "aitkeninterpolation.h"

namespace Ui {
class GetValueAtPointDialog;
}

class GetValueAtPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GetValueAtPointDialog(QWidget *parent = 0);
    ~GetValueAtPointDialog();

public:
    void prepareForValueGetting(QVector<QPointF> points);

private slots:
    void on_pushButton_clicked();

private:
    Ui::GetValueAtPointDialog *ui;

    QVector<QPointF> points;
};

#endif // GETVALUEATPOINTDIALOG_H
