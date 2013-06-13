#include "inc/uimainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application( argc, argv );

    //This code will be moved into the EMERGE module
    UIMainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
