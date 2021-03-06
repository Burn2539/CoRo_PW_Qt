#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"
#include "mv_controller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    MV_Controller *getModel();
    bool isSynchronous();

public slots:
    void newValuesReceived_updateView(sensors newValues);
    void statusUpdate_updateView(Status flags);

private slots:
    void on_startAcquisition_clicked();
    void on_stopAcquisition_clicked();
    void on_downloadData_clicked();
    void downloadDataDone();
    void on_clear_clicked();
    void on_characteristicSelection_currentIndexChanged();
    void on_sendDataSynchronously_clicked();
    void onXRangeChanged(const QCPRange newRange, const QCPRange oldRange);
    void onHorizontalDragChanged(const int newCenter);
    void setLCD(bool finalDisplay = false);
    void on_actionCopy_triggered();
    void keyPressEvent(QKeyEvent * event);

private:
    Ui::MainWindow *ui;
    MV_Controller *controller;
    double plotXAxis_minRange = 0;
    double plotXAxis_maxRange = 10;
    QTimer *timerAcquisition;
    int timeAcquisitionElapsed = 0;
    QLabel *lcdTimer;
    QTime *timerSending;
    int timeSendingElapsed = 0;
};
