/****************************************************************************
*
* Project Name		: Qt_PSOC_2_PC
*
* File Name			: main.cpp
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
#include "main.h"
#include "mainwindow.h"


/*****************************************************************************
* Function Name: main()
******************************************************************************
* Summary:
*   Create and opens the main window.
*
* Parameters:
*   Command line arguments.
*
* Return:
*   The state of execution of the app.
*
* Note:
*   Will generate a error dialog if no Bluetooth device is connected to
*   the PC.
*
*****************************************************************************/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Main routine.
    try {
        MainWindow window;
        window.show();
        return app.exec();
    }

    // Error handler.
    catch(int e) {
        // Create error dialog.
        QMessageBox *errorDialog = new QMessageBox();
        errorDialog->setWindowTitle("Error " + QString::number(e));

        // Error 99: No Bluetooth device connected.
        if (e == 99) {
            errorDialog->setIcon(QMessageBox::Critical);
            errorDialog->setText("No Bluetooth device connected to this computer.");
        }

        // Show the error dialog.
        errorDialog->show();
    }
}
