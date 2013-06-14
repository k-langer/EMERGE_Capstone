#include "inc/uigraphview.h"

UIGraphView::UIGraphView() : QGLView()
{
    QGLBuilder builder;
    builder.newSection( QGL::Faceted );

    this->sphere1 = builder.newNode();
    builder << QGLSphere( 0.3f );
    this->sphere2 = builder.newNode();
    builder << QGLSphere( 0.3f );

    this->scene = builder.finalizedSceneNode();
}

UIGraphView::~UIGraphView()
{
    delete scene;
    delete sphere1;
    delete sphere2;
}

void UIGraphView::initializeGL( QGLPainter *painter )
{
    painter->setStandardEffect( QGL::LitMaterial );
}

void UIGraphView::paintGL( QGLPainter *painter )
{
    this->sphere1->setPosition( QVector3D(.4,0,0) );
    this->sphere1->draw( painter );

    this->sphere2->setPosition( QVector3D(.8,0,0) );
    this->sphere2->draw( painter );
}
