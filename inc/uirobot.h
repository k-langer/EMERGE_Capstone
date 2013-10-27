#ifndef UIROBOT_H
#define UIROBOT_H

#include <QVector3D>

class UIRobot {

public:
    QVector3D centerGripper;
    QVector3D wrist;
    QVector3D shoulder;
    QVector3D base;
};

#endif // UIROBOT_H
