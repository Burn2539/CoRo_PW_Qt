#pragma once

#include "main.h"
#include "qcustomplot.h"
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

public slots:
    void newValuesReceived_updateView(sensors newValues);

private slots:
    void on_start_clicked();
    void on_stop_clicked();
    void on_clear_clicked();
    void on_characteristicSelection_currentIndexChanged();
    void onXRangeChanged(const QCPRange newRange, const QCPRange oldRange);
    void onHorizontalDragChanged(const int newCenter);
    void setLCD(bool withMillisec = FALSE);

private:
    Ui::MainWindow *ui;
    MV_Controller *controller;
    double plotXAxis_minRange = 0;
    double plotXAxis_maxRange = 10;
    QTimer *timer;
    QLabel *lcdTimer;
};
