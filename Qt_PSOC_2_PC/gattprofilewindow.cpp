#include "gattprofilewindow.h"
#include "ui_gattprofilewindow.h"

#include "mainwindow.h"
#include "mv_controller.h"

GattProfileWindow::GattProfileWindow(MainWindow *parent) :
    QDialog(parent), ui(new Ui::GattProfile)
{
    ui->setupUi(this);

    MV_Controller *model = parent->getModel();
    QString text;

//    text.append("Service " + model->iServices + ":\n" +
//            "\tAttribute Handle = " + service->AttributeHandle + "\n" +
//            "\tService UUID (short) = " + service->ServiceUuid.Value.ShortUuid + "\n");

    ui->textDisplay->setText(text);
}


GattProfileWindow::~GattProfileWindow()
{
    delete ui;
}
