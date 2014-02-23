#ifndef UIMODEL_H
#define UIMODEL_H

#include "inc/uimainwindow.h"

#include <QTimer>
#include <QtSql/QSqlDatabase>
#include <inc/uirobot.h>

class UIModel
{

public:
    UIModel();
    ~UIModel();

    bool isConnected();
    bool connectToDatabase();
    void disconnectFromDatabase();
    UIRobot getCurrentRobotPosition();

private:
    QSqlDatabase database;

};

#endif // UIMODEL_H
