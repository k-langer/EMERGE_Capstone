#include "inc/uimainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication application( argc, argv );

    UIMainWindow mainWindow;
    mainWindow.show();
    mainWindow.setStatus( CONNECTED );

    return application.exec();
}
