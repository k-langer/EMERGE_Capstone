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
    // Draws the first sphere
    this->sphere1->draw( painter );

    // Translates and draws the second sphere
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate( 1.0f, 0.0f, 0.0f );
    this->sphere2->draw( painter );
    painter->modelViewMatrix().pop();
}
