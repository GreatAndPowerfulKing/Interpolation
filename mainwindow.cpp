#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treeWidget->setItemDelegate(new SpinBoxDelegate);

    // Plot menu actions
    QAction *fitAction = new QAction("Fit", this);
    QAction *getValueAtPointAction = new QAction("Get value at point", this);
    QAction *replotAction = new QAction("Replot", this);
    QAction *saveAsCsvAction = new QAction("Save as .csv", this);
    QAction *saveAsPngAction = new QAction("Save as .png", this);

    plotMenu = new QMenu();
    plotMenu->addAction(fitAction);
    plotMenu->addAction(getValueAtPointAction);
    plotMenu->addSeparator();
    plotMenu->addAction(replotAction);
    plotMenu->addSeparator();
    plotMenu->addAction(saveAsCsvAction);
    plotMenu->addAction(saveAsPngAction);
    ui->plotMenuButton->setMenu(plotMenu);

    // Data menu actions
    QAction *clearAction = new QAction("Clear", this);
    QAction *loadAction = new QAction("Load from .csv", this);
    QAction *saveAction = new QAction("Save as .csv", this);

    dataMenu = new QMenu();
    dataMenu->addAction(clearAction);
    dataMenu->addSeparator();
    dataMenu->addAction(loadAction);
    dataMenu->addAction(saveAction);
    ui->dataMenuButton->setMenu(dataMenu);

    // Plot initialization
    ui->plotWidget->xAxis->setLabel("x");
    ui->plotWidget->yAxis->setLabel("y");
    ui->plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plotWidget->addGraph();
    ui->plotWidget->addGraph();
    ui->plotWidget->addGraph();
    ui->plotWidget->addGraph(); // Points
    ui->plotWidget->graph(1)->setPen(QPen(QColor(255, 0, 0)));
    ui->plotWidget->graph(2)->setPen(QPen(QColor(0, 255, 0)));
    ui->plotWidget->graph(3)->setPen(QPen(QColor(0, 0, 0)));

    // CheckBoxes checking
    ui->realCheckBox->setChecked(true);
    ui->splinesCheckBox->setChecked(true);
    ui->aitkenCheckBox->setChecked(true);
    ui->dotsCheckBox->setChecked(true);

    // Data's connects
    connect(clearAction, &QAction::triggered, [this] () {
        ui->treeWidget->clear();
        clearPlot();
    });
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadFromCsv);
    connect(saveAction, &QAction::triggered, [this] () {
        saveRealPointsAsCsv();
    });

    // Plot's connects
    connect(fitAction, &QAction::triggered, [this] () {
        ui->plotWidget->rescaleAxes();
        ui->plotWidget->replot();
    });
    connect(getValueAtPointAction, &QAction::triggered, [this] () {
        QVector<QPointF> realPoints;
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            realPoints.append(QPointF(item->data(0, 0).toDouble(), item->data(1, 0).toDouble()));
        }
        GetValueAtPointDialog d;
        d.prepareForValueGetting(realPoints);
        d.exec();
    });
    connect(replotAction, &QAction::triggered, [this] () {
        replot();
    });
    connect(saveAsPngAction, &QAction::triggered, [this] () {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                        saveFileName,
                                                        tr("Images (*.png)"));
        if (fileName.isEmpty()) {
            return;
        }
        QImage image(ui->plotWidget->size(), QImage::Format_ARGB32);
        QPainter painter(&image);
        ui->plotWidget->render(&painter);
        image.save(fileName);
    });
    connect(saveAsCsvAction, &QAction::triggered, this, &MainWindow::saveIntepolatedValuesAsCsv);
}

MainWindow::~MainWindow()
{
    plotMenu->clear();
    dataMenu->clear();

    delete plotMenu;
    delete dataMenu;

    delete ui;
}

void MainWindow::loadFromCsv()
{    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    openFileName,
                                                    tr("Text files (*.csv)"));
    QFileInfo fileInfo(fileName);
    openFileName = fileInfo.absoluteFilePath();
    saveFileName = fileInfo.absolutePath();

    if (fileName.isEmpty()) {
        return;
    }

    ui->treeWidget->clear();
    clearPlot();

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            QStringList values = line.split(",");
            if (values.count() != 2) {
                QMessageBox::warning(this, "Something gone wrong...", "Bad file structure!");
                ui->treeWidget->clear();
                return;
            }
            values[0] = values[0].trimmed();
            values[1] = values[1].trimmed();
            bool ok;
            values[0].toDouble(&ok);
            values[1].toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, "Something gone wrong...", "Bad file structure!");
                ui->treeWidget->clear();
                return;
            }
            QTreeWidgetItem *item = new QTreeWidgetItem(values);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->treeWidget->addTopLevelItem(item);
        }
        file.close();
        replot();
    } else {
        QMessageBox::warning(this, "Something gone wrong...", file.errorString());
    }
}

void MainWindow::saveRealPointsAsCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File",
                                                    saveFileName,
                                                    "Text files (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            stream << item->data(0, 0).toDouble() << ", " << item->data(1, 0).toDouble() << "\n";
        }
        file.close();
    } else {
        QMessageBox::warning(this, "Something gone wrong...", file.errorString());
    }
}

void MainWindow::saveIntepolatedValuesAsCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File",
                                                    saveFileName,
                                                    "Text files (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "x, y (splines), y (aitken)\n";

        QVector<QPointF> realPoints;
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
            realPoints.append(QPointF(item->data(0, 0).toDouble(), item->data(1, 0).toDouble()));
        }
        QVector<QVector<double> > matrix(0, QVector<double>(0));
        QVector<QPointF> splinesPoints = SplinesInterpolation::interpolate(realPoints, 0.01, matrix);
        QVector<QPointF> aitkenPoints = AitkenInterpolation::interpolate(realPoints, 0.01);

        for (int i = 0; i < splinesPoints.count(); i++) {
            stream << QString("%1").arg(splinesPoints[i].x(), 1, 'f', 2) << ", ";
            stream << QString("%1").arg(splinesPoints[i].y(), 1, 'f', 2) << ", ";
            stream << QString("%1").arg(aitkenPoints[i].y(), 1, 'f', 2) << "\n";
        }

        file.close();
    } else {
        QMessageBox::warning(this, "Something gone wrong...", file.errorString());
    }
}

void MainWindow::replot(QVector<QPointF> realPoints, QVector<QPointF> splinesPoints, QVector<QPointF> aitkenPoints)
{
    clearPlot();

    QVector<double> x;
    QVector<double> y;

    for (QPointF point : realPoints) {
        x.append(point.x());
        y.append(point.y());
    }
    ui->plotWidget->graph(0)->addData(x, y);
    ui->plotWidget->graph(3)->addData(x, y);
    ui->plotWidget->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->plotWidget->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

    x.clear();
    y.clear();

    for (QPointF point : splinesPoints) {
        x.append(point.x());
        y.append(point.y());
    }
    ui->plotWidget->graph(1)->addData(x, y);

    x.clear();
    y.clear();

    for (QPointF point : aitkenPoints) {
        x.append(point.x());
        y.append(point.y());
    }
    ui->plotWidget->graph(2)->addData(x, y);

    ui->plotWidget->rescaleAxes();
    updatePlotsVisibility();
}

void MainWindow::replot()
{
    QVector<QPointF> realPoints;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);

        for (int i = 0; i < realPoints.size(); i++) {
            if (realPoints[i].x() == item->data(0, 0).toDouble()) {
                QMessageBox::warning(this, "Bad input!",
                                     QString("Different function value at point %1").arg(item->data(0, 0).toDouble()));
                clearPlot();
                return;
            }
        }

        realPoints.append(QPointF(item->data(0, 0).toDouble(), item->data(1, 0).toDouble()));
    }

    QVector<QVector<double> > matrix(0, QVector<double>(0));
    QVector<QPointF> splinesPoints = SplinesInterpolation::interpolate(realPoints, 0.01, matrix);

    QVector<QPointF> aitkenPoints = AitkenInterpolation::interpolate(realPoints, 0.01);

    replot(realPoints, splinesPoints, aitkenPoints);
}

void MainWindow::updatePlotsVisibility()
{
    ui->plotWidget->graph(0)->setVisible(ui->realCheckBox->isChecked());
    ui->plotWidget->graph(1)->setVisible(ui->splinesCheckBox->isChecked());
    ui->plotWidget->graph(2)->setVisible(ui->aitkenCheckBox->isChecked());
    ui->plotWidget->graph(3)->setVisible(ui->dotsCheckBox->isChecked());
    ui->plotWidget->replot();
}

void MainWindow::clearPlot()
{
    ui->plotWidget->graph(0)->clearData();
    ui->plotWidget->graph(1)->clearData();
    ui->plotWidget->graph(2)->clearData();
    ui->plotWidget->graph(3)->clearData();
    ui->plotWidget->replot();
}


void MainWindow::on_realCheckBox_stateChanged(int arg1)
{
    updatePlotsVisibility();
}

void MainWindow::on_splinesCheckBox_stateChanged(int arg1)
{
    updatePlotsVisibility();
}

void MainWindow::on_aitkenCheckBox_stateChanged(int arg1)
{
    updatePlotsVisibility();
}

void MainWindow::on_dotsCheckBox_stateChanged(int arg1)
{
    updatePlotsVisibility();
}

void MainWindow::on_addPointButton_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem({"0", "0"});
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->treeWidget->addTopLevelItem(item);
    ui->treeWidget->setCurrentItem(item);
    ui->treeWidget->editItem(item);
}

void MainWindow::on_removePointButton_clicked()
{
    if (!ui->treeWidget->selectedItems().isEmpty()) {
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->selectedItems()[0]));
    }
}
