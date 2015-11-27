#include "main.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    try {
        MainWindow window;
        window.show();
        return app.exec();
    }

    catch(int e) {
        QMessageBox *errorDialog = new QMessageBox();
        errorDialog->setWindowTitle("Error " + QString::number(e));
        if (e == 99) {
            errorDialog->setIcon(QMessageBox::Critical);
            errorDialog->setText("No Bluetooth device connected to this computer.");
        }
        errorDialog->show();
    }
}
