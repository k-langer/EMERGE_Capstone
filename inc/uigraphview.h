#ifndef UIGRAPHVIEW_H
#define UIGRAPHVIEW_H

#include <Qt3D/QGLView>
#include <QVector3D>
#include <vector>

class UIGraphView : public QGLView
{

public:
    UIGraphView();
    ~UIGraphView();

protected:
    void paintGL( QGLPainter *painter );
    void initializeGL( QGLPainter *painter );

private:
    std::vector<QGLSceneNode*> * nodes;

    void addSphere( const QVector3D vector, const double diameter = 1.0f );
    void addCylinder( const QVector3D vector1, const QVector3D vector2, const double diameter = 1.0f );

    static QGLSceneNode * generateSphere( const QVector3D vector, const double diameter = 1.0f );
    static QGLSceneNode * generateCylinder( const QVector3D vector1, const QVector3D vector2, const double diameter = 1.0f );
};

#endif // UIGRAPHVIEW_H
