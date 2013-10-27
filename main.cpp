#include "inc/uimainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application( argc, argv );

    UIMainWindow mainWindow;
    mainWindow.show();

    UIRobot robot = UIRobot();
    robot.base = QVector3D(0, 0, 0);
    robot.shoulder = QVector3D(1,.5,1);
    robot.wrist = QVector3D(2,1.5,2);
    robot.centerGripper = QVector3D(3, 3, 3);

    mainWindow.setRobotPosition(robot);

    return application.exec();
}
