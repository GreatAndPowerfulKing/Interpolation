#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include "spinboxdelegate.h"
#include "getvalueatpointdialog.h"
#include "aitkeninterpolation.h"
#include "splinesinterpolation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_realCheckBox_stateChanged(int arg1);
    void on_splinesCheckBox_stateChanged(int arg1);
    void on_aitkenCheckBox_stateChanged(int arg1);
    void on_addPointButton_clicked();
    void on_removePointButton_clicked();
    void on_dotsCheckBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    QMenu *plotMenu;
    QMenu *dataMenu;

    QString openFileName = QDir::homePath();
    QString saveFileName = QDir::homePath();

private:
    void loadFromCsv();
    void saveRealPointsAsCsv();
    void saveIntepolatedValuesAsCsv();
    void replot(QVector<QPointF> realPoints, QVector<QPointF> splinesPoints, QVector<QPointF> aitkenPoints);
    void replot();
    void updatePlotsVisibility();
    void clearPlot();
};

#endif // MAINWINDOW_H
