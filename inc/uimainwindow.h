/* UIMainWindow.h
 * Runs the main window of the user interface
 * Example Usage:
 *  UIMainWindow mainWindow;
 *  mainWindow.show();
*/

#ifndef UIMAINWINDOW_H
#define UIMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <vector>
#include "inc/uigraphview.h"
#include "inc/uicameraview.h"
#include "inc/uirobot.h"

enum status_t {
    NOT_CONNECTED,
    CONNECTED,
    PAUSED
};

class UIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor and destructor
    UIMainWindow( QWidget *parent = 0 );
    ~UIMainWindow();

    void setStatus( status_t );
    status_t getStatus();

    void setRobotPosition( UIRobot robotPosition );

private:
    // Private members
    QLabel *statusBarLabel;
    status_t status;
    std::vector<QLabel*> statisticLabels;

    // Layout objects
    QGridLayout *gridLayout;
    UIGraphView *robotGraphView;
    UICameraView *cameraView;

    // Private methods
    void _setupStatisticGrid();
    void _setStatisticsWithRobotPosition( UIRobot robotPosition );
};

#endif // UIMAINWINDOW_H
