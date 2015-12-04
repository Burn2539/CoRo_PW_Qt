/****************************************************************************
*
* Project Name		: Qt_PSOC_2_PC
*
* File Name			: mainwindow.cpp
* File Version      : 2.3.1
* Qt Version        : Qt 5.5.0 MSVC 2013 64bit
* Compiler          : Microsoft Visual C++ Compiler 12.0 (amd64)
*
* Owner             : A. BERNIER
*
*****************************************************************************/


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mv_controller.h"
#include "qcustomplot.h"


/*****************************************************************************
* Function Name: MainWindow()
******************************************************************************
* Summary:
*   Create and initialize every items used by the main window.
*
* Parameters:
*   Parent.
*
* Return:
*   Address of the new MainWindow.
*
* Note:
*   Using a Model-Controller-View approach.
*
*****************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Create the controller
    try {
        controller = new MV_Controller(this);
    }
    catch(int e) {
        throw e;
    }

    // .ui form
    ui->setupUi(this);


    // Window title
    this->setWindowTitle("CapSense data acquisition");


    // COMBOBOX: Characteristic selection
    for (int i = 0; i < controller->getNumServices(); i++) {
        for (int j = 0; j < controller->getNumCharacteristics(i); j++) {
            if ( !controller->isCharacteristicNameEmpty(i, j) )
                ui->characteristicSelection->addItem(controller->getCharacteristicName(i, j),
                                                     qVariantFromValue((void *)controller->getCharacteristicAddress(i, j)));
        }
    }
    if (ui->characteristicSelection->count() > 0)
        controller->setCurrChar( (Characteristic *)ui->characteristicSelection->currentData().value<void *>() );


    // PUSH BUTTON: Start acquisition
    ui->startAcquisition->setEnabled(false);

    // PUSH BUTTON: Stop acquisition
    ui->stopAcquisition->setEnabled(false);

    // PUSH BUTTON: Download data
    ui->downloadData->setEnabled(false);

    // PUSH BUTTON: Clear
    if (ui->characteristicSelection->count() > 0)
        ui->clear->setEnabled(true);


    // LCD NUMBER: Elapsed time (acquisition)
    lcdTimer = new QLabel;
    timerAcquisition = new QTimer();
    timerAcquisition->setInterval(1000);
    connect(timerAcquisition, SIGNAL(timeout()), this, SLOT(setLCD()));

    timerSending = new QTime();

    lcdTimer->setText("Acquisition: 00:00.000\tSending: 00:00:000");

    ui->statusBar->addWidget(lcdTimer);


    // LABELS: Status
    ui->Ind_Ready_Acq->setVisible(false);
    ui->Ind_Acq->setVisible(false);
    ui->Ind_Ready_Send->setVisible(false);
    ui->Ind_Send->setVisible(false);


    // PROGRESS BARS: Sensors' last values
    int max = 0xFFFF;
    ui->sensor0->setRange(0, max);
    ui->sensor0->setValue(0);
    ui->value_sensor0->setText(QString::number(0));
    ui->sensor1->setRange(0, max);
    ui->sensor1->setValue(0);
    ui->value_sensor1->setText(QString::number(0));
    ui->sensor2->setRange(0, max);
    ui->sensor2->setValue(0);
    ui->value_sensor2->setText(QString::number(0));
    ui->sensor3->setRange(0, max);
    ui->sensor3->setValue(0);
    ui->value_sensor3->setText(QString::number(0));
    ui->sensor4->setRange(0, max);
    ui->sensor4->setValue(0);
    ui->value_sensor4->setText(QString::number(0));
    ui->sensor5->setRange(0, max);
    ui->sensor5->setValue(0);
    ui->value_sensor5->setText(QString::number(0));
    ui->sensor6->setRange(0, max);
    ui->sensor6->setValue(0);
    ui->value_sensor6->setText(QString::number(0));
    ui->sensor7->setRange(0, max);
    ui->sensor7->setValue(0);
    ui->value_sensor7->setText(QString::number(0));
    ui->sensor8->setRange(0, max);
    ui->sensor8->setValue(0);
    ui->value_sensor8->setText(QString::number(0));
    ui->sensor9->setRange(0, max);
    ui->sensor9->setValue(0);
    ui->value_sensor9->setText(QString::number(0));


    // CHECKBOX: Send data synchronously
    ui->sendDataSynchronously->setChecked(false);
    on_sendDataSynchronously_clicked();


    // TABLEVIEW: All sensors' values
    ui->sensorsTable->setModel( controller->getCurrCharDataModelAddress() );


    // PLOT: Chart for each sensor
    ui->plot_sensors->plotLayout()->clear();
    ui->plot_sensors->setAntialiasedElements(QCP::aeAll);

    QPen *pen = new QPen();
    pen->setColor(Qt::black);
    pen->setWidth(3);

    QVector<QCPAxisRect *> sensor;
    sensor.resize(controller->getNumSensors());
    QVector<QCPGraph *> sensorGraph;
    sensorGraph.resize(controller->getNumSensors());
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->plot_sensors);

    // Global X Axis
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

    // Sensors' plot
    for (int i = 0; i < controller->getNumSensors(); i++) {
        sensor[i] = new QCPAxisRect(ui->plot_sensors);

        // Y Axis
        sensor[i]->axis(QCPAxis::atLeft)->setRangeUpper(0xFFFF);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoTicks(true);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoTickStep(true);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoTickCount(3);
        sensor[i]->axis(QCPAxis::atLeft)->setAutoSubTicks(true);
        sensor[i]->axis(QCPAxis::atLeft)->setTickLabels(true);
        sensor[i]->axis(QCPAxis::atLeft)->setLabel( "Sensor " + QString::number(i+1) );

        // X Axis
        sensor[i]->axis(QCPAxis::atBottom)->setTickLabelType(QCPAxis::ltDateTime);
        sensor[i]->axis(QCPAxis::atBottom)->setDateTimeFormat("mm:ss.zzz");
        sensor[i]->axis(QCPAxis::atBottom)->setAutoTicks(true);
        sensor[i]->axis(QCPAxis::atBottom)->setAutoTickStep(true);
        sensor[i]->axis(QCPAxis::atBottom)->setAutoTickCount(4);
        sensor[i]->axis(QCPAxis::atBottom)->setAutoSubTicks(true);
        sensor[i]->axis(QCPAxis::atBottom)->setTickLabels(true);

        // Full Axes Box
        sensor[i]->setupFullAxesBox();
        connect(sensor[i]->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), sensor[i]->axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));
        connect(sensor[i]->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)), sensor[i]->axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));

        // Horizontal drag and zoom
        sensor[i]->setRangeDrag(Qt::Horizontal);
        sensor[i]->setRangeZoom(Qt::Horizontal);
        ui->plot_sensors->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        connect(sensor[i]->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), X_Axis->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));

        // Add plot to widget
        ui->plot_sensors->plotLayout()->addElement(i, 0, sensor[i]);
        sensor[i]->setMarginGroup(QCP::msLeft, marginGroup);

        // Graph
        sensorGraph[i] = ui->plot_sensors->addGraph(sensor[i]->axis(QCPAxis::atBottom), sensor[i]->axis(QCPAxis::atLeft));
        sensorGraph[i]->setPen(*pen);
        sensorGraph[i]->setLineStyle(QCPGraph::lsLine);
    }

    // Use the Global X Axis to control the range of all the plots (for the horizontal drag event)
    for (int i = 0; i < controller->getNumSensors(); i++)
        connect(X_Axis->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), sensor[i]->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(X_Axis->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange, QCPRange)), this, SLOT(onXRangeChanged(QCPRange, QCPRange)));

    // Horizontal drag slider
    ui->plot_horizontalDrag->setRange(plotXAxis_minRange + X_Axis->axis(QCPAxis::atBottom)->range().size() / 2,
                                      plotXAxis_maxRange - X_Axis->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plot_horizontalDrag->setValue(X_Axis->axis(QCPAxis::atBottom)->range().lower + X_Axis->axis(QCPAxis::atBottom)->range().size() / 2);
    connect(ui->plot_horizontalDrag, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalDragChanged(int)));


    // PLOT: Center of mass
    ui->plot_CoM->plotLayout()->clear();
    ui->plot_CoM->setAntialiasedElements(QCP::aeAll);

    QCPAxisRect *CoM_AxisRect = new QCPAxisRect(ui->plot_CoM);
    QCPMarginGroup *marginGroupCoM = new QCPMarginGroup(ui->plot_CoM);

    // Y Axis
    CoM_AxisRect->axis(QCPAxis::atLeft)->setRangeUpper(NUM_SENSORS+0.5);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setRangeLower(0.5);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setAutoTicks(true);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setAutoTickStep(false);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setTickStep(1);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setAutoTickCount(NUM_SENSORS);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setAutoSubTicks(false);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setTickLabels(true);
    CoM_AxisRect->axis(QCPAxis::atLeft)->setLabel("Center of mass");

    // X Axis
    CoM_AxisRect->axis(QCPAxis::atBottom)->setTickLabelType(QCPAxis::ltDateTime);
    CoM_AxisRect->axis(QCPAxis::atBottom)->setDateTimeFormat("mm:ss.zzz");
    CoM_AxisRect->axis(QCPAxis::atBottom)->setAutoTicks(true);
    CoM_AxisRect->axis(QCPAxis::atBottom)->setAutoTickStep(true);
    CoM_AxisRect->axis(QCPAxis::atBottom)->setAutoTickCount(4);
    CoM_AxisRect->axis(QCPAxis::atBottom)->setAutoSubTicks(true);
    CoM_AxisRect->axis(QCPAxis::atBottom)->setTickLabels(true);
    CoM_AxisRect->axis(QCPAxis::atBottom)->setLabel("Time");

    // Full Axes Box
    CoM_AxisRect->setupFullAxesBox();
    connect(CoM_AxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), CoM_AxisRect->axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));
    connect(CoM_AxisRect->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)), CoM_AxisRect->axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));

    // Horizontal drag and zoom
    CoM_AxisRect->setRangeDrag(Qt::Horizontal);
    CoM_AxisRect->setRangeZoom(Qt::Horizontal);
    ui->plot_CoM->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    connect(CoM_AxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), X_Axis->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(X_Axis->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), CoM_AxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));

    // Add plot to widget
    ui->plot_CoM->plotLayout()->addElement(0, 0, CoM_AxisRect);
    CoM_AxisRect->setMarginGroup(QCP::msLeft, marginGroupCoM);

    // Graph
    QCPGraph *CoM_Graph;
    CoM_Graph = ui->plot_CoM->addGraph(CoM_AxisRect->axis(QCPAxis::atBottom), CoM_AxisRect->axis(QCPAxis::atLeft));
    CoM_Graph->setPen(*pen);
    CoM_Graph->setLineStyle(QCPGraph::lsLine);

    // Horizontal drag slider
    ui->plotCoM_horizontalDrag->setRange(plotXAxis_minRange + X_Axis->axis(QCPAxis::atBottom)->range().size() / 2,
                                      plotXAxis_maxRange - X_Axis->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plotCoM_horizontalDrag->setValue(X_Axis->axis(QCPAxis::atBottom)->range().lower + X_Axis->axis(QCPAxis::atBottom)->range().size() / 2);
    connect(ui->plotCoM_horizontalDrag, SIGNAL(sliderMoved(int)), ui->plot_horizontalDrag, SLOT(setValue(int)));
    connect(ui->plotCoM_horizontalDrag, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalDragChanged(int)));


    // Subscribe to notifications
    controller->subscribeToSensorsNotifications(true, false);
    controller->subscribeToStatusNotifications(false, true);

    // First read of the status flags
    controller->readStatusFlags();
}


MainWindow::~MainWindow()
{
    controller->unsubscribeToSensorsNotifications();
    controller->unsubscribeToStatusNotifications();

    delete controller;
    delete ui;
}


MV_Controller *MainWindow::getModel()
{
    return controller;
}


void MainWindow::on_startAcquisition_clicked()
{
    // Clear the CapSense model.
    on_clear_clicked();

    /* Start acquisition */
    controller->startAcquisition(ui->sendDataSynchronously->isChecked());

    /* Start timers */
    controller->startTimer();
    timerAcquisition->start();
}


void MainWindow::on_stopAcquisition_clicked()
{
    /* Stop acquisition */
    controller->stopAcquisition(ui->sendDataSynchronously->isChecked());

    /* Get the time elapsed */
    timerAcquisition->stop();
    timeAcquisitionElapsed = controller->getElapsedTime();
    setLCD(true);

    plotXAxis_maxRange = (double) timeAcquisitionElapsed/1000;

    // If we were receiving the data synchronously.
    if (ui->sendDataSynchronously->isChecked()) {
        int delay = 0;
        while (delay++ < 10000) {
        }
        downloadDataDone();
    }
}


void MainWindow::on_downloadData_clicked()
{
    // Start sending data.
    controller->startSendingData();

    // Start timer.
    timerSending->start();
}


void MainWindow::downloadDataDone()
{
    // Display timers in status bar.
    timeSendingElapsed = timerSending->elapsed();
    setLCD(true);

    // Validate data and fill the "values" vector.
    controller->validateAndFillValues();

    // Draw the table view.
    controller->generateKeys(plotXAxis_maxRange);
    controller->buildCurrCharDataModel();

    // Draw the sensors plots.
    for (int sensor = 0; sensor < controller->getNumSensors(); sensor++) {
        if ( ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->range().size() > (plotXAxis_maxRange - plotXAxis_minRange) )
            ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_maxRange);
        else
            ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange,
                                                                                  plotXAxis_minRange + ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atBottom)->range().size());

        for (int row = 0; row < controller->getNumRowsCurrCharDataModel(); row++)
            ui->plot_sensors->graph(sensor)->addData(controller->getCurrCharKey(row), controller->getCurrCharValue(row, sensor));

        ui->plot_sensors->graph(sensor)->rescaleValueAxis();
        ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atLeft)->setRangeUpper(0xFFFF);
        if (ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atLeft)->range().lower < 0)
            ui->plot_sensors->axisRect(sensor)->axis(QCPAxis::atLeft)->setRangeLower(0);
    }
    ui->plot_sensors->replot();

    // Draw the center of mass plot.
    if ( ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->range().size() > (plotXAxis_maxRange - plotXAxis_minRange) )
        ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_maxRange);
    else
        ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange,
                                                                              plotXAxis_minRange + ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->range().size());

    for (int row = 0; row < controller->getNumRowsCurrCharDataModel(); row++)
        ui->plot_CoM->graph()->addData(controller->getCurrCharKey(row), controller->getCurrCharCoM(row));

    ui->plot_CoM->replot();

    // Draw the controls.
    ui->plot_horizontalDrag->setRange(plotXAxis_minRange + ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size() / 2,
                                      plotXAxis_maxRange - ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plot_horizontalDrag->setValue(ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().lower +
                                      ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size() / 2);
    ui->plot_horizontalDrag->setEnabled(true);
}


void MainWindow::on_clear_clicked()
{
    // If the model isn't alreay empty.
    if ( !controller->isCurrCharDataModelEmpty() ) {
        /* Erase the table view */
        controller->clearCurrCharDataModel();

        /* Erase the plots */
        for (int i = 0; i < controller->getNumSensors(); i++)
            ui->plot_sensors->graph(i)->clearData();
        ui->plot_CoM->graph()->clearData();
    }

    // Redraw the plots
    ui->plot_sensors->replot();
    ui->plot_CoM->replot();

    /* Reset LCD */
    lcdTimer->setText("Acquisition: 00:00.000\tSending: 00:00:000");
    timeAcquisitionElapsed = 0;
    timeSendingElapsed = 0;

    /* Set the progress bars value to 0 */
    ui->sensor0->setValue(0);
    ui->value_sensor0->setText(QString::number(0));
    ui->sensor1->setValue(0);
    ui->value_sensor1->setText(QString::number(0));
    ui->sensor2->setValue(0);
    ui->value_sensor2->setText(QString::number(0));
    ui->sensor3->setValue(0);
    ui->value_sensor3->setText(QString::number(0));
    ui->sensor4->setValue(0);
    ui->value_sensor4->setText(QString::number(0));
    ui->sensor5->setValue(0);
    ui->value_sensor5->setText(QString::number(0));
    ui->sensor6->setValue(0);
    ui->value_sensor6->setText(QString::number(0));
    ui->sensor7->setValue(0);
    ui->value_sensor7->setText(QString::number(0));
    ui->sensor8->setValue(0);
    ui->value_sensor8->setText(QString::number(0));
    ui->sensor9->setValue(0);
    ui->value_sensor9->setText(QString::number(0));
}


void MainWindow::on_characteristicSelection_currentIndexChanged()
{
    /* Change the characteristic considered selected */
    controller->setCurrChar( (Characteristic *)ui->characteristicSelection->currentData().value<void *>() );
}


void MainWindow::newValuesReceived_updateView(sensors newValues)
{
    /* PROGRESS BARS: Sensor's last values */
    if ( ui->sendDataSynchronously->isChecked() ) {
        ui->sensor0->setValue(newValues.sensor[0]);
        ui->value_sensor0->setText(QString::number(newValues.sensor[0]));
        ui->sensor1->setValue(newValues.sensor[1]);
        ui->value_sensor1->setText(QString::number(newValues.sensor[1]));
        ui->sensor2->setValue(newValues.sensor[2]);
        ui->value_sensor2->setText(QString::number(newValues.sensor[2]));
        ui->sensor3->setValue(newValues.sensor[3]);
        ui->value_sensor3->setText(QString::number(newValues.sensor[3]));
        ui->sensor4->setValue(newValues.sensor[4]);
        ui->value_sensor4->setText(QString::number(newValues.sensor[4]));
        ui->sensor5->setValue(newValues.sensor[5]);
        ui->value_sensor5->setText(QString::number(newValues.sensor[5]));
        ui->sensor6->setValue(newValues.sensor[6]);
        ui->value_sensor6->setText(QString::number(newValues.sensor[6]));
        ui->sensor7->setValue(newValues.sensor[7]);
        ui->value_sensor7->setText(QString::number(newValues.sensor[7]));
        ui->sensor8->setValue(newValues.sensor[8]);
        ui->value_sensor8->setText(QString::number(newValues.sensor[8]));
        ui->sensor9->setValue(newValues.sensor[9]);
        ui->value_sensor9->setText(QString::number(newValues.sensor[9]));
    }
}


void MainWindow::statusUpdate_updateView(Status flags)
{
    QFont boldFont;
    boldFont.setBold(true);

    QFont notBoldFont;
    notBoldFont.setBold(false);

    // Ready
    if (flags.Ready) {
        // Status widget
        ui->Ind_Ready_Acq->setVisible(true);
        ui->Lab_Ready_Acq->setFont(boldFont);

        // Buttons
        ui->startAcquisition->setEnabled(true);
        ui->sendDataSynchronously->setEnabled(true);
    }
    else {
        // Status widget
        ui->Ind_Ready_Acq->setVisible(false);
        ui->Lab_Ready_Acq->setFont(notBoldFont);

        // Buttons
        ui->startAcquisition->setEnabled(false);
        ui->sendDataSynchronously->setEnabled(false);
    }

    // Acquiring
    if (flags.Acquiring) {
        // Status widget
        ui->Ind_Acq->setVisible(true);
        ui->Lab_Acq->setFont(boldFont);

        // Buttons
        ui->stopAcquisition->setEnabled(true);

        // No more space
        if (flags.NoMoreSpace) {
            on_stopAcquisition_clicked();
        }
    }
    else {
        // Status widget
        ui->Ind_Acq->setVisible(false);
        ui->Lab_Acq->setFont(notBoldFont);

        // Buttons
        ui->stopAcquisition->setEnabled(false);
    }

    // Data acquired
    if (flags.DataAcquired && !ui->sendDataSynchronously->isChecked()) {
        // Status widget
        ui->Ind_Ready_Send->setVisible(true);
        ui->Lab_Ready_Send->setFont(boldFont);

        // Buttons
        ui->downloadData->setEnabled(true);
    }
    else {
        // Status widget
        ui->Ind_Ready_Send->setVisible(false);
        ui->Lab_Ready_Send->setFont(notBoldFont);

        // Buttons
        ui->downloadData->setEnabled(false);
    }

    // Sending
    if (flags.Sending) {
        // Status widget
        ui->Ind_Send->setVisible(true);
        ui->Lab_Send->setFont(boldFont);

        // No more data
        if (flags.NoMoreData) {
            controller->stopSendingData();
            downloadDataDone();
        }
    }
    else {
        // Status widget
        ui->Ind_Send->setVisible(false);
        ui->Lab_Send->setFont(notBoldFont);
    }
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
        if (newRange.lower < plotXAxis_minRange) {
            ui->plot_sensors->axisRect(i)->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_minRange + newSize);
            ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_minRange + newSize);
        }

        /* Plot range can't go above the maximal value */
        else if (newRange.upper > plotXAxis_maxRange) {
            ui->plot_sensors->axisRect(i)->axis(QCPAxis::atBottom)->setRange(plotXAxis_maxRange - newSize, plotXAxis_maxRange);
            ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_maxRange - newSize, plotXAxis_maxRange);
        }

        /* Plot can't zoom infinitely */
        else if (sizeToSmall) {
            ui->plot_sensors->axisRect(i)->axis(QCPAxis::atBottom)->setRange(oldRange.lower, oldRange.upper);
            ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(oldRange.lower, oldRange.upper);
            newSize = oldRange.size();
        }
    }

    ui->plot_sensors->replot();
    ui->plot_CoM->replot();

    /* Adjust the horizontal drag sliders */
    ui->plot_horizontalDrag->setRange(plotXAxis_minRange + newSize / 2, plotXAxis_maxRange - newSize / 2);
    ui->plot_horizontalDrag->setValue(ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().lower + newSize / 2);

    ui->plotCoM_horizontalDrag->setRange(plotXAxis_minRange + newSize / 2, plotXAxis_maxRange - newSize / 2);
    ui->plotCoM_horizontalDrag->setValue(ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().lower + newSize / 2);
}


void MainWindow::onHorizontalDragChanged(const int newCenter)
{
    double rangeSize = ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->range().size();

    /* Redraw the plot with the new zoom level */
    if (ui->plot_horizontalDrag->value() == ui->plot_horizontalDrag->maximum()) {
        ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_maxRange - rangeSize, plotXAxis_maxRange);
        ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_maxRange - rangeSize, plotXAxis_maxRange);
    }

    else if (ui->plot_horizontalDrag->value() == ui->plot_horizontalDrag->minimum()) {
        ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_minRange + rangeSize);
        ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(plotXAxis_minRange, plotXAxis_minRange + rangeSize);
    }

    else {
        ui->plot_sensors->axisRect()->axis(QCPAxis::atBottom)->setRange(newCenter - rangeSize / 2, newCenter + rangeSize / 2);
        ui->plot_CoM->axisRect()->axis(QCPAxis::atBottom)->setRange(newCenter - rangeSize / 2, newCenter + rangeSize / 2);
    }

    ui->plot_sensors->replot();
    ui->plot_CoM->replot();
}


void MainWindow::setLCD(bool finalDisplay)
{
    QTime timeValueAcquisition;
    QTime timeValueSending;
    int acquisitionElapsedTime = timeAcquisitionElapsed;

    if (!finalDisplay)
        acquisitionElapsedTime = controller->getElapsedTime();

    // Time of acquisition of data.
    int minutes = acquisitionElapsedTime / 60000;
    int seconds = (acquisitionElapsedTime - (minutes * 60000)) / 1000;
    int millisec = (acquisitionElapsedTime - (minutes * 60000) - (seconds * 1000));

    if (!finalDisplay)
        millisec = 0;

    timeValueAcquisition.setHMS(0, minutes, seconds, millisec);

    // Time of sending of data.
    if (!ui->sendDataSynchronously->isChecked()) {
        minutes = timeSendingElapsed / 60000;
        seconds = (timeSendingElapsed - (minutes * 60000)) / 1000;
        millisec = (timeSendingElapsed - (minutes * 60000) - (seconds * 1000));
    }

    timeValueSending.setHMS(0, minutes, seconds, millisec);

    // Set label text.
    lcdTimer->setText("Acquisition: " + timeValueAcquisition.toString(QString("mm:ss.zzz")) +
                      "\t Sending: " + timeValueSending.toString(QString("mm:ss.zzz")));
}


void MainWindow::on_sendDataSynchronously_clicked()
{
    if (ui->sendDataSynchronously->isChecked()) {
        ui->Ind_Ready_Send->setVisible(false);
        ui->Lab_Ready_Send->setVisible(false);
        ui->downloadData->setVisible(false);
        ui->sensors_lastValues->setEnabled(true);
    }

    else {
        ui->Lab_Ready_Send->setVisible(true);
        ui->downloadData->setVisible(true);
        ui->sensors_lastValues->setEnabled(false);
    }
}

void MainWindow::on_actionCopy_triggered()
{
    QModelIndexList cells = ui->sensorsTable->selectionModel()->selectedIndexes();
    qSort(cells); // Necessary, otherwise they are in column order

    QString text;
    int currentRow = 0; // To determine when to insert newlines
    foreach (const QModelIndex& cell, cells) {
        if (text.length() == 0) {
            // First item
        } else if (cell.row() != currentRow) {
            // New row
            text += '\n';
        } else {
            // Next cell
            text += '\t';
        }
        currentRow = cell.row();
        text += cell.data().toString();
    }

    QApplication::clipboard()->setText(text);
}


void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if ( event->matches(QKeySequence::Copy) )
        on_actionCopy_triggered();
}


bool MainWindow::isSynchronous()
{
    return ui->sendDataSynchronously->isChecked();
}
