#ifndef UICAMERAVIEW_H
#define UICAMERAVIEW_H

#include <QWidget>
#include <QtMultimedia/QCamera>
#include <QtMultimediaWidgets/QCameraViewfinder>
#include <QMouseEvent>

class UICameraView : public QCameraViewfinder
{

public:
    UICameraView();
    ~UICameraView();

    void pickCamera();

protected:
    void mousePressEvent( QMouseEvent *e );

private:
    QCamera *camera;

    void setCamera( const QByteArray &cameraDevice );

};

#endif // UICAMERAVIEW_H
