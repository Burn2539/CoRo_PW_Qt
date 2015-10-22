#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mv_controller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    controller = new MV_Controller(this);

    ui->setupUi(this);

    /* MainWindow */
    this->setWindowTitle("CapSense data acquisition");


    /* COMBOBOX: Characteristic selection */
    for (int i = 0; i < controller->getNumServices(); i++) {
        for (int j = 0; j < controller->getNumCharacteristics(i); j++) {
            if ( !controller->isCharacteristicNameEmpty(i, j) )
                ui->characteristicSelection->addItem(controller->getCharacteristicName(i, j),
                                                     qVariantFromValue((void *)controller->getCharacteristicAddress(i, j)));
        }
    }
    if (ui->characteristicSelection->count() > 0)
        controller->setCurrChar( (Characteristic *)ui->characteristicSelection->currentData().value<void *>() );


    /* PUSH BUTTON: Start */
    if (controller->isCurrCharEmpty())
        ui->start->setEnabled(FALSE);


    /* PUSH BUTTON: Stop */
    ui->stop->setEnabled(FALSE);


    /* PUSH BUTTON: Clear */
    if (controller->isCurrCharDataModelEmpty())
        ui->clear->setEnabled(FALSE);


    /* LCD NUMBER: Elapsed time (acquisition) */
    lcdTimer = new QLabel;
    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(setLCD()));

    lcdTimer->setText("00:00.000");

    ui->statusBar->addWidget(lcdTimer);


    /* PROGRESS BARS: Sensors' last values */
    ui->sensor0->setValue(0);
    ui->sensor1->setValue(0);
    ui->sensor2->setValue(0);
    ui->sensor3->setValue(0);
    ui->sensor4->setValue(0);


    /* TABLEVIEW: All sensors' values */
    ui->sensorsTable->setModel( controller->getCurrCharDataModelAddress() );


    /* PLOT: Chart for each sensor */
    ui->plot_sensors->plotLayout()->clear();
    ui->plot_sensors->setAntialiasedElements(QCP::aeAll);

    QVector<QCPAxisRect *> sensor;
    sensor.resize(controller->getNumSensors());
    QVector<QCPGraph *> sensorGraph;
    sensorGraph.resize(controller->getNumSensors());
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->plot_sensors);

    /* Global X Axis */
    QCPAxisRect *X_Axis = new QCPAxisRect(ui->plot_sensors);
    X_Axis->axis(QCPAxis::atBottom)->setTickLabelType(QCPAxis::ltDateTime);
    X_Axis->axis(QCPAxis::atBottom)->setDateTimeFormat("mm:ss.zzz");
    X_Axis->axis(QCPAxis::atBottom)->setAutoTicks(true);
    X_Axis->axis(QCPAxis::atBottom)->setAutoTickLabels(true);
    X_Axis->axis(QCPAxis::atBottom)->setAutoTickStep(true);
    X_Axis->axis(QCPAxis::atBottom)->setAutoTickCount(4);
    X_Axis->axis(QCPAxis::atBottom)->setAutoSubTicks(true);
    X_Axis->axis(QCPAxis::atBottom)->grid()->setVisible(false);
    X_Axis->axis(QCPAxis::atLeft)->setVisible(false);

    /* Sensors' plot */
    for (int i = 0; i < controller->getNumSensors(); i++) {
        sensor[i] = new QCPAxisRect(ui->plot_sensors);

        /* Y Axis */
        sensor[i]->axis(QCPAxis::atLeft)->setRangeUpper(0xFFFF);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoTicks(true);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoTickStep(true);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoTickCount(3);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoSubTicks(true);
        sensor[i]->axis(QCPAxis::atLeft)->setTickLabels(true);

        /* X Axis */
        sensor[i]->axis(QCPAxis::atBottom)->setTickLabelType(QCPAxis::ltDateTime);
        sensor[i]->axis(QCPAxis::atBottom)->setDateTimeFormat("mm:ss.zzz");
        sensor[i]->axis(QCPAxis::atBottom)->setAutoTicks(true);
        sensor[i]->axis(QCPAxis::atBottom)->setAutoTickStep(true);
        sensor[i]->axis(QCPAxis::atBottom)->setAutoTickCount(4);
        sensor[i]->axis(QCPAxis::atBottom)->setAutoSubTicks(true);
        sensor[i]->axis(QCPAxis::atBottom)->setTickLabels(true);

        /* Full Axes Box */
        sensor[i]->setupFullAxesBox();
        connect(sensor[i]->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), sensor[i]->axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));
        connect(sensor[i]->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)), sensor[i]->axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));

        /* Horizontal drag and zoom */
        sensor[i]->setRangeDrag(Qt::Horizontal);
        sensor[i]->setRangeZoom(Qt::Horizontal);
        ui->plot_sensors->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        connect(sensor[i]->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), X_Axis->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));

        /* Add plot to widget */
        ui->plot_sensors->plotLayout()->addElement(i, 0, sensor[i]);
        sensor[i]->setMarginGroup(QCP::msLeft, marginGroup);

        /* Graph */
        sensorGraph[i] = ui->plot_sensors->addGraph(sensor[i]->axis(QCPAxis::atBottom), sensor[i]->axis(QCPAxis::atLeft));
        sensorGraph[i]->setPen(QPen(Qt::black));
        sensorGraph[i]->pen().setWidth(5);
        sensorGraph[i]->setLineStyle(QCPGraph::lsLine);
    }

    /* Use the Global X Axis to control the range of all the plots (for the horizontal drag event) */
    for (int i = 0; i < controller->getNumSensors(); i++)
        connect(X_Axis->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), sensor[i]->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(X_Axis->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange, QCPRange)), this, SLOT(onXRangeChanged(QCPRange, QCPRange)));

    /* Horizontal drag slider */
    ui->plot_horizontalDrag->setRange(plotXAxis_minRange + X_Axis->axis(QCPAxis::atBottom)->range().size() / 2,
                                      plotXAxis_maxRange - X_Axis->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plot_horizontalDrag->setValue(X_Axis->axis(QCPAxis::atBottom)->range().lower + X_Axis->axis(QCPAxis::atBottom)->range().size() / 2);
    connect(ui->plot_horizontalDrag, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalDragChanged(int)));
}


MainWindow::~MainWindow()
{
    delete controller;
}


void MainWindow::on_start_clicked()
{
    /* Start acquisition */
    controller->subscribeToCurrCharNotifications(TRUE, FALSE);

    /* Start timers */
    controller->startTimer();
    timer->start();

    /* Draw the controls */
    ui->start->setEnabled(FALSE);
    ui->stop->setEnabled(TRUE);
    ui->characteristicSelection->setEnabled(FALSE);
    ui->clear->setEnabled(FALSE);
}


void MainWindow::on_stop_clicked()
{
    /* Stop acquisition */
    controller->unsubscribeToCurrCharNotifications();

    /* Get the time elapsed */
    timer->stop();
    setLCD(TRUE);
    plotXAxis_maxRange = (double) controller->getElapsedTime()/1000;

    /* Draw the table view */
    controller->generateKeys(plotXAxis_maxRange);
    controller->buildCurrCharDataModel();

    /* Draw the plots */
    for (int sensor = 0; sensor < controller->getNumSensors(); sensor++) {
        if ( ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->range().size() > (plotXAxis_maxRange - plotXAxis_minRange) )
            ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_maxRange);
        else
            ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange,
                                                                             plotXAxis_minRange + ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->range().size());

        for (int row = 0; row < controller->getNumRowsCurrCharDataModel(); row++)
            ui->plot_sensors->graph(sensor)->addData(controller->getCurrCharKey(row), controller->getCurrCharValue(row, sensor));

        ui->plot_sensors->graph(sensor)->rescaleValueAxis();
    }
    ui->plot_sensors->replot();

    /* Draw the controls */
    ui->stop->setEnabled(FALSE);
    ui->start->setEnabled(FALSE);
    ui->characteristicSelection->setEnabled(TRUE);
    ui->clear->setEnabled(TRUE);

    ui->plot_horizontalDrag->setRange(plotXAxis_minRange + ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size() / 2,
                                      plotXAxis_maxRange - ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plot_horizontalDrag->setValue(ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().lower +
                                      ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plot_horizontalDrag->setEnabled(true);
}


void MainWindow::on_clear_clicked()
{
    /* Erase the table view */
    controller->clearCurrCharDataModel();

    /* Erase the plots */
    for (int i = 0; i < controller->getNumSensors(); i++)
        ui->plot_sensors->graph(i)->clearData();
    ui->plot_sensors->replot();

    /* Reset LCD */
    lcdTimer->setText("00:00.000");

    /* Set the progress bars value to 0 */
    ui->sensor0->setValue(0);
    ui->sensor1->setValue(0);
    ui->sensor2->setValue(0);
    ui->sensor3->setValue(0);
    ui->sensor4->setValue(0);

    /* Draw the controls */
    if ( controller->isCurrCharDataModelEmpty() ) {
        ui->clear->setEnabled(FALSE);
        ui->start->setEnabled(TRUE);
    }
}


void MainWindow::on_characteristicSelection_currentIndexChanged()
{
    /* Change the characteristic considered selected */
    controller->setCurrChar( (Characteristic *)ui->characteristicSelection->currentData().value<void *>() );
}


void MainWindow::newValuesReceived_updateView(sensors newValues)
{
    /* PROGRESS BARS: Sensor's last values */
    ui->sensor0->setValue(newValues.sensor[0]);
    ui->sensor1->setValue(newValues.sensor[1]);
    ui->sensor2->setValue(newValues.sensor[2]);
    ui->sensor3->setValue(newValues.sensor[3]);
    ui->sensor4->setValue(newValues.sensor[4]);
}


void MainWindow::onXRangeChanged(const QCPRange newRange, const QCPRange oldRange)
{
    double newSize = newRange.size();
    bool sizeToSmall = false;

    /* Plot range can't be larger than the time elapsed */
    if (newSize > (plotXAxis_maxRange - plotXAxis_minRange))
        newSize = plotXAxis_maxRange - plotXAxis_minRange;

    /* Don't want the plot to zoom to much */
    else if (newSize < 0.5)
        sizeToSmall = true;

    for (int i = 0; i < controller->getNumSensors(); i++) {
        /* Plot range can't go below the minimal value */
        if (newRange.lower < plotXAxis_minRange)
            ui->plot_sensors->axisRect(i)->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_minRange + newSize);

        /* Plot range can't go above the maximal value */
        else if (newRange.upper > plotXAxis_maxRange)
            ui->plot_sensors->axisRect(i)->axis(QCPAxis::atBottom)->setRange(plotXAxis_maxRange - newSize, plotXAxis_maxRange);

        /* Plot can't zoom infinitely */
        else if (sizeToSmall) {
            ui->plot_sensors->axisRect(i)->axis(QCPAxis::atBottom)->setRange(oldRange.lower, oldRange.upper);
            newSize = oldRange.size();
        }
    }

    /* Adjust the horizontal drag slider */
    ui->plot_horizontalDrag->setRange(plotXAxis_minRange + newSize / 2, plotXAxis_maxRange - newSize / 2);
    ui->plot_horizontalDrag->setValue(ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().lower + newSize / 2);
}


void MainWindow::onHorizontalDragChanged(const int newCenter)
{
    double rangeSize = ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size();

    /* Redraw the plot with the new zoom level */
    if (ui->plot_horizontalDrag->value() == ui->plot_horizontalDrag->maximum())
        ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_maxRange - rangeSize, plotXAxis_maxRange);

    else if (ui->plot_horizontalDrag->value() == ui->plot_horizontalDrag->minimum())
    ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_minRange + rangeSize);

    else
        ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->setRange(newCenter - rangeSize / 2, newCenter + rangeSize / 2);

    ui->plot_sensors->replot();
}


void MainWindow::setLCD(bool withMillisec)
{
    QTime timeValue;

    int elapsedTimeMS = controller->getElapsedTime();

    int minutes = elapsedTimeMS / 60000;
    int seconds = (elapsedTimeMS - (minutes * 60000)) / 1000;
    int millisec = (elapsedTimeMS - (minutes * 60000) - (seconds * 1000));

    if (!withMillisec)
        millisec = 0;

    timeValue.setHMS(0, minutes, seconds, millisec);

    lcdTimer->setText(timeValue.toString(QString("mm:ss.zzz")));
}
