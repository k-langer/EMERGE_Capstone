#include "inc/uimainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application( argc, argv );

    UIMainWindow mainWindow;
    mainWindow.show();

    UIRobot robot = UIRobot();
    robot.base = QVector3D(0, 0, 0);
    robot.shoulder = QVector3D(1,.5,0);
    robot.wrist = QVector3D(2,1.5,0);
    robot.centerGripper = QVector3D(3, 1, 0);

    mainWindow.setRobotPosition(robot);
    mainWindow.setStatus( CONNECTED );

    return application.exec();
}
