#ifndef UIGRAPHVIEW_H
#define UIGRAPHVIEW_H

#include <Qt3D/QGLView>
#include <Qt3D/QGLSphere>
#include <Qt3D/QGLBuilder>

class UIGraphView : public QGLView
{

public:
    UIGraphView();
    ~UIGraphView();

protected:
    void paintGL( QGLPainter *painter );
    void initializeGL( QGLPainter *painter );\

private:
    QGLSceneNode *scene;

    QGLSceneNode *sphere1;
    QGLSceneNode *sphere2;
};

#endif // UIGRAPHVIEW_H
