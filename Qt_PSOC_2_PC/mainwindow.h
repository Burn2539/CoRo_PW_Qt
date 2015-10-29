#pragma once

#include "main.h"
#include "qcustomplot.h"
#include "mv_controller.h"
#include "gattprofilewindow.h"
#include "settingswindow.h"

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
    void on_actionGATT_Profile_triggered();
    void on_actionSettings_triggered();
    void onXRangeChanged(const QCPRange newRange, const QCPRange oldRange);
    void onHorizontalDragChanged(const int newCenter);
    void setLCD(bool withMillisec = FALSE);

private:
    Ui::MainWindow *ui;
    SettingsWindow *settingsWindow;
    GattProfileWindow *gattProfileWindow;
    MV_Controller *controller;
    double plotXAxis_minRange = 0;
    double plotXAxis_maxRange = 10;
    QTimer *timer;
    QLabel *lcdTimer;
};
