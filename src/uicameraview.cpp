#include "inc/uicameraview.h"
#include <QInputDialog>

UICameraView::UICameraView() : QCameraViewfinder()
{
    this->camera = new QCamera();
    this->pickCamera();
}

UICameraView::~UICameraView()
{
    delete camera;
}

void UICameraView::pickCamera()
{
    // Creates a mapping of cameraDescription -> deviceName
    // Also creates a cameraDescription QStringList for popup dialogue
    QMap<QString, QByteArray> cameras = QMap<QString, QByteArray>();
    QStringList cameraDescriptions;
    foreach( const QByteArray &deviceName, QCamera::availableDevices() ) {
        QString description = this->camera->deviceDescription( deviceName );
        cameraDescriptions << description;
        cameras[description] = deviceName;
    }

    // Presents dialogue
    bool ok;
    QString chosenDescription = QInputDialog::getItem( this, tr( "Choose a video camera" ), tr( "Camera:" ), cameraDescriptions, 0, false, &ok, 0 );

    // If user selects an item, choose that item as the camera
    QByteArray cameraDevice;
    if( ok && !chosenDescription.isEmpty() ) {
        cameraDevice = cameras[chosenDescription];
    }
    this->setCamera( cameraDevice );
}

void UICameraView::setCamera( const QByteArray &cameraDevice )
{
    // Deletes current camaera
    delete this->camera;

    if( cameraDevice.isEmpty() ){
        // Sets up the default camera (if there is one)
        camera = new QCamera();
    } else{
        // Sets up the camera the user chose
        camera = new QCamera( cameraDevice );
    }

    camera->setViewfinder( this );
    camera->start();
}

void UICameraView::mousePressEvent( QMouseEvent *e )
{
    // Picks camera based on user input
    if( e->button() == Qt::RightButton ){
        //this->pickCamera();
    }
}
