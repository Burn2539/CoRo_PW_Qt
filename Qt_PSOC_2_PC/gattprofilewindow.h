#pragma once

#include "main.h"

namespace Ui {
    class GattProfile;
}

class MainWindow;

class GattProfileWindow : public QDialog
{
    Q_OBJECT

public:
    GattProfileWindow(MainWindow *parent = 0);
    ~GattProfileWindow();

signals:

public slots:

private:
    Ui::GattProfile *ui;
};
