// UIMode.h

#include "inc/uimodel.h"

#include <QTimer>
#include <QtSql/QSqlDatabase>

// Update interval in ms
#define UPDATE_INTERVAL 100

#define DATABASE_HOST "localhost"
#define DATABASE_NAME ""
#define DATABASE_USERNAME ""
#define DATABASE_PASSWORD ""

UIModel::UIModel()
{

}

UIModel::~UIModel()
{
    this->disconnectFromDatabase();
}

bool UIModel::connectToDatabase()
{
    if (this->isConnected()) {
        this->disconnectFromDatabase();
    }

    this->database = QSqlDatabase::addDatabase("QMYSQL");
    this->database.setHostName(DATABASE_HOST);
    this->database.setDatabaseName(DATABASE_NAME);
    this->database.setUserName(DATABASE_USERNAME);
    this->database.setPassword(DATABASE_PASSWORD);
    return this->database.open();
}

void UIModel::disconnectFromDatabase()
{
    if (!this->isConnected()) {
        return;
    }

    this->database.close();
}

bool UIModel::isConnected()
{
    return this->database.isOpen();
}

UIRobot UIModel::getCurrentRobotPosition()
{
    UIRobot robot = UIRobot();
    return robot;
}
