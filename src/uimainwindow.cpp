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
    QWidget *cameraView = new QWidget();
    cameraView->setStyleSheet( "QWidget {background:blue;}" );
    this->gridLayout->addWidget( cameraView, 0, 0, 4, 4 );

    // User graph setup
    QWidget *userGraph = new QWidget();
    userGraph->setStyleSheet( "QWidget {background:green;}" );
    this->gridLayout->addWidget( userGraph, 0, 5, 2, 1 );

    // Robot graph setup
    QWidget *robotGraph = new QWidget();
    robotGraph->setStyleSheet( "QWidget {background:green;}" );
    this->gridLayout->addWidget( robotGraph, 2, 5, 2, 1 );

    // Robot type label setup
    this->robotTypeLabel = new QLabel( "PhantomX Reactor" );
    this->robotTypeLabel->setStyleSheet( "QLabel {font:bold large;}" );
    this->gridLayout->addWidget( this->robotTypeLabel, 4, 0, 1, 1 );

    this->_setupStatisticGrid();

    QWidget *window = new QWidget();
    window->setLayout( this->gridLayout );
    this->setCentralWidget( window );

    // Maximize at start
    this->showMaximized();
}

UIMainWindow::~UIMainWindow()
{

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
    this->gridLayout->addWidget( statisticGridWindow, 4, 3, 1, 1 );

    this->statisticLabels = std::vector<QLabel*>();

    // Setup labels
    QLabel *userX = new QLabel( "UserX: " );
    statisticGrid->addWidget( userX, 0, 0, 1, 1 );
    QLabel *userY = new QLabel( "UserY: " );
    statisticGrid->addWidget( userY, 1, 0, 1, 1 );
    QLabel *userZ = new QLabel( "UserZ: " );
    statisticGrid->addWidget( userZ, 2, 0, 1, 1 );
    QLabel *robotX = new QLabel( "RobotX: " );
    statisticGrid->addWidget( robotX, 0, 2, 1, 1 );
    QLabel *robotY = new QLabel( "RobotY: " );
    statisticGrid->addWidget( robotY, 1, 2, 1, 1 );
    QLabel *robotZ = new QLabel( "RobotZ: " );
    statisticGrid->addWidget( robotZ, 2, 2, 1, 1 );

    // Setup value labels
    for( int i = 0; i < 6; i++ ) {
        QLabel *label = new QLabel( "0.0" );
        int row = i % 3;
        int col = ( i < 3 ) ? 1 : 3;
        this->statisticLabels.push_back( label );
        statisticGrid->addWidget( label, row, col, 1, 1 );
    }
}
