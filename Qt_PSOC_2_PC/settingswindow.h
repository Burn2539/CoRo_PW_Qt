#pragma once

#include "main.h"

namespace Ui {
    class Settings;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

signals:

public slots:

private:
    Ui::Settings *ui;

};
