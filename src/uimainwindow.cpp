// UIMainWindow.cpp

#include "inc/uimainwindow.h"
#include <QRect>
#include <QMenuBar>
#include <QStatusBar>

UIMainWindow::UIMainWindow( QWidget *parent )
    : QMainWindow( parent )
{
    // Setup status bar
    this->statusBar()->setSizeGripEnabled( false );
    this->statusBar()->setStyleSheet( "QStatusBar::item {border: 0px solid black;}" );
    this->statusBarLabel = new QLabel();
    this->statusBarLabel->setAlignment( Qt::AlignCenter );
    this->statusBar()->addWidget( statusBarLabel, 1 );
    this->setStatus( NOT_CONNECTED );

    // Gridview setup
    this->gridLayout = new QGridLayout();
    this->gridLayout->setHorizontalSpacing( 20 );
    this->gridLayout->setVerticalSpacing( 20 );

    // Camera view setup
    this->cameraView = new UICameraView();
    this->gridLayout->addWidget( this->cameraView, 0, 0, 4, 4 );

    // Robot graph setup
    this->robotGraphView = new UIGraphView();
    QWidget *robotGraphContainer = QWidget::createWindowContainer(this->robotGraphView);
    robotGraphContainer->setMinimumSize(1,1);
    robotGraphContainer->setMaximumSize(1000,1000);
    this->gridLayout->addWidget( robotGraphContainer, 0, 5, 2, 1 );

    // Robot type label setup
    this->robotTypeLabel = new QLabel( "PhantomX Reactor" );
    this->robotTypeLabel->setStyleSheet( "QLabel {font:bold large;}" );
    this->gridLayout->addWidget( this->robotTypeLabel, 4, 0, 1, 1 );

    this->_setupStatisticGrid();

    QWidget *window = new QWidget();
    window->setLayout( this->gridLayout );
    this->setCentralWidget( window );

    // Maximize at start
//    this->showMaximized();
}

UIMainWindow::~UIMainWindow()
{
    delete statusBarLabel;
    delete gridLayout;
    delete robotTypeLabel;
    delete robotGraphView;
    delete cameraView;
}

void UIMainWindow::setStatus( status_t newStatus )
{
    this->status = newStatus;

    switch ( newStatus ) {
    case NOT_CONNECTED:
        this->statusBarLabel->setText( "Not connected" );
        this->statusBarLabel->setStyleSheet( "QLabel {color:red;}" );
        break;

    case READY_FOR_CALIBRATION:
        this->statusBarLabel->setText( "Ready for calibration" );
        this->statusBarLabel->setStyleSheet( "QLabel {color:orange;}" );
        break;

    case CONNECTED:
        this->statusBarLabel->setText( "Connected" );
        this->statusBarLabel->setStyleSheet( "QLabel {color:green;}" );
        break;

    case PAUSED:
        this->statusBarLabel->setText( "Paused" );
        this->statusBarLabel->setStyleSheet( "QLabel {color:blue;}" );
        break;
    }
}

status_t UIMainWindow::getStatus()
{
    return this->status;
}

void UIMainWindow::_setupStatisticGrid()
{
    // Sets up statistics
    QGridLayout *statisticGrid = new QGridLayout();
    QWidget *statisticGridWindow = new QWidget();
    statisticGridWindow->setLayout( statisticGrid );
    this->gridLayout->addWidget( statisticGridWindow, 2, 5, 2, 1 );

    this->statisticLabels = std::vector<QLabel*>();

    // Setup labels
    QLabel *userX = new QLabel( "Base: " );
    statisticGrid->addWidget( userX, 0, 0, 1, 1 );
    QLabel *userY = new QLabel( "Shoulder: " );
    statisticGrid->addWidget( userY, 1, 0, 1, 1 );
    QLabel *userZ = new QLabel( "Wrist: " );
    statisticGrid->addWidget( userZ, 2, 0, 1, 1 );
    QLabel *robotX = new QLabel( "Gripper: " );
    statisticGrid->addWidget( robotX, 3, 0, 1, 1 );

    // Setup value labels
    for( int i = 0; i < 12; i++ ) {
        QLabel *label = new QLabel( QString::number(0) );
        int row = i / 3;
        int col = i % 3 + 1;
        this->statisticLabels.push_back( label );
        statisticGrid->addWidget( label, row, col, 1, 1 );
    }
}

void UIMainWindow::_setStatisticsWithRobotPosition( UIRobot robotPosition )
{
    QLabel *label;

    // Base
    label = this->statisticLabels[0];
    label->setText( QString::number(robotPosition.base.x(), 'f', 3) );
    label = this->statisticLabels[1];
    label->setText( QString::number(robotPosition.base.y(), 'f', 3) );
    label = this->statisticLabels[2];
    label->setText( QString::number(robotPosition.base.z(), 'f', 3) );

    // Shoulder
    label = this->statisticLabels[3];
    label->setText( QString::number(robotPosition.shoulder.x(), 'f', 3) );
    label = this->statisticLabels[4];
    label->setText( QString::number(robotPosition.shoulder.y(), 'f', 3) );
    label = this->statisticLabels[5];
    label->setText( QString::number(robotPosition.shoulder.z(), 'f', 3) );

    // Wrist
    label = this->statisticLabels[6];
    label->setText( QString::number(robotPosition.wrist.x(), 'f', 3) );
    label = this->statisticLabels[7];
    label->setText( QString::number(robotPosition.wrist.y(), 'f', 3) );
    label = this->statisticLabels[8];
    label->setText( QString::number(robotPosition.wrist.z(), 'f', 3) );

    // Gripper
    label = this->statisticLabels[9];
    label->setText( QString::number(robotPosition.centerGripper.x(), 'f', 3) );
    label = this->statisticLabels[10];
    label->setText( QString::number(robotPosition.centerGripper.y(), 'f', 3) );
    label = this->statisticLabels[11];
    label->setText( QString::number(robotPosition.centerGripper.z(), 'f', 3) );
}

void UIMainWindow::setRobotPosition( UIRobot robotPosition )
{
    this->robotGraphView->reset();

    this->robotGraphView->addSphere( robotPosition.base, .8 );
    this->robotGraphView->addCylinder( robotPosition.base, robotPosition.shoulder, .1 );
    this->robotGraphView->addSphere( robotPosition.shoulder, .5 );
    this->robotGraphView->addCylinder( robotPosition.shoulder, robotPosition.wrist, .1 );
    this->robotGraphView->addSphere( robotPosition.wrist, .5 );
    this->robotGraphView->addCylinder( robotPosition.wrist, robotPosition.centerGripper, .1);
    this->robotGraphView->addSphere( robotPosition.centerGripper, .5 );

    this->_setStatisticsWithRobotPosition( robotPosition );
}
