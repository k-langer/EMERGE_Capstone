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

enum status_t {
    NOT_CONNECTED,
    READY_FOR_CALIBRATION,
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

private:
    // Private members
    QLabel *statusBarLabel;
    status_t status;
    std::vector<QLabel*> statisticLabels;

    // Layout objects
    QGridLayout *gridLayout;
    QLabel *robotTypeLabel;
    UIGraphView *robotGraphView;
    UIGraphView *userGraphView;

    // Private methods
    void _setupStatisticGrid();
};

#endif // UIMAINWINDOW_H
