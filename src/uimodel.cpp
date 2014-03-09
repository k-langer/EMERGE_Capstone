// UIModel.h

#include "inc/uimodel.h"

#include <QDebug>
#include <QTimer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlField>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QStringList>
#include <QVariant>

#define DATABASE_HOST "localhost"
#define DATABASE_NAME "capstone"
#define DATABASE_USERNAME "zhen"
#define DATABASE_PASSWORD ""
#define DATABASE_PROCEDURE "get_ui_data"

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
    QString queryString = QString("call %1(@value)").arg(DATABASE_PROCEDURE);
    QSqlQuery query(this->database);
    query.exec(queryString);
    query.exec("select @value");
    query.next();

    QVariant rawValues = query.value(0);
    QString allValues = rawValues.toString();
    QStringList valueList = allValues.split(" ");

    if (allValues.length() < 11) {
        qDebug() << "Database length was incorrect";
        return UIRobot();
    }

    UIRobot robot = UIRobot();

    robot.base = QVector3D(0, 0, 0);

    QString rawShoulderX = valueList.at(0);
    qreal shoulderX = rawShoulderX.toDouble();
    QString rawShoulderY = valueList.at(1);
    qreal shoulderY = rawShoulderY.toDouble();
    QString rawShoulderZ = valueList.at(2);
    qreal shoulderZ = rawShoulderZ.toDouble();
    robot.shoulder = QVector3D(shoulderX * .01, shoulderY * .01, shoulderZ * .01);

    QString rawWristX = valueList.at(3);
    qreal wristX = rawWristX.toDouble();
    QString rawWristY = valueList.at(4);
    qreal wristY = rawWristY.toDouble();
    QString rawWristZ = valueList.at(5);
    qreal wristZ = rawWristZ.toDouble();
    robot.wrist = QVector3D(wristX * .01, wristY * .01, wristZ * .01);

    QString rawGripperX = valueList.at(6);
    qreal gripperX = rawGripperX.toDouble();
    QString rawGripperY = valueList.at(7);
    qreal gripperY = rawGripperY.toDouble();
    QString rawGripperZ = valueList.at(8);
    qreal gripperZ = rawGripperZ.toDouble();
    robot.centerGripper = QVector3D(gripperX * .01, gripperY * .01, gripperZ * .01);

    QString rawGripperLength = valueList.at(9);
    qreal gripperLength = rawGripperLength.toDouble() * .02;
    QVector3D leftGripper = robot.centerGripper;
    leftGripper.setY(leftGripper.y() - gripperLength / 2);
    robot.leftGripper = leftGripper;
    QVector3D rightGripper = robot.centerGripper;
    rightGripper.setY(rightGripper.y() + gripperLength / 2);
    robot.rightGripper = rightGripper;

    return robot;
}
