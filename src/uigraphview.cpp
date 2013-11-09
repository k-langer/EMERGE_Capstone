#include "inc/uigraphview.h"
#include "inc/uiutil.h"
#include <Qt3D/QGLSphere>
#include <Qt3D/QGLCylinder>
#include <Qt3D/QGLBuilder>
#include <Qt3D/QGraphicsRotation3D>
#include <Qt3D/QGraphicsTranslation3D>

UIGraphView::UIGraphView() : QGLView()
{
    this->nodes = new std::vector<QGLSceneNode*>;
}

UIGraphView::~UIGraphView()
{
    delete nodes;
}

void UIGraphView::initializeGL( QGLPainter *painter )
{
    painter->setStandardEffect( QGL::LitMaterial );

    // Rotate camera 80 deg
    camera()->rotateCenter( QQuaternion::fromAxisAndAngle( 0, 1, 0, -80 ) );

    // Move down center so 0,0,0 is close to bottom of screen
    QVector3D center = camera()->center();
    center.setY( (double)camera()->viewSize().height() / 2);
    camera()->setCenter( center );
}

void UIGraphView::paintGL( QGLPainter *painter )
{
    // Paints each node in this->nodes
    for( int i = 0; i < (int)this->nodes->size(); i++ )
    {
        QGLSceneNode *node = this->nodes->at( i );
        node->draw( painter );
    }
}

void UIGraphView::addSphere( const QVector3D vector, const double diameter )
{
    QGLSceneNode *sphereNode = this->generateSphere( vector, diameter );
    this->nodes->push_back( sphereNode );
}

void UIGraphView::addCylinder( const QVector3D vector1, const QVector3D vector2, const double diameter )
{
    QGLSceneNode *cylinderNode = this->generateCylinder( vector1, vector2, diameter );
    this->nodes->push_back( cylinderNode );
}

QGLSceneNode * UIGraphView::generateSphere( const QVector3D vector, const double diameter )
{
    // Sets up builder
    QGLBuilder builder;
    builder.newSection();
    builder << QGLSphere( diameter );

    // Creates sphere and moves it to position
    QGLSceneNode *sphere = builder.finalizedSceneNode();
    sphere->setPosition( vector );

    return sphere;
}

QGLSceneNode * UIGraphView::generateCylinder( const QVector3D vector1, const QVector3D vector2, const double diameter )
{
    double cylinderLength = vector1.distanceToLine( vector2, QVector3D() );

    // Sets up builder
    QGLBuilder builder;
    builder.newSection();
    builder << QGLCylinder( diameter, diameter, cylinderLength );

    // Creates cylinder and moves it to sphere1
    QGLSceneNode *cylinder = builder.finalizedSceneNode();
    cylinder->setPosition( vector1 );

    // Determines normalized vector to rotate by
    QVector3D normalizedV1( 0 , 0, 1 );
    QVector3D normalizedV2( vector2.x() - vector1.x() , vector2.y() - vector1.y(), vector2.z() - vector1.z() );
    QVector3D normalVector = QVector3D::normal( normalizedV1, normalizedV2 );

    // Rotates based on normalized vector
    QGraphicsRotation3D *rotation = new QGraphicsRotation3D();
    double angle = angleBetweenVectors( normalizedV1, normalizedV2, true );
    rotation->setAxis( normalVector );
    rotation->setAngle( angle );
    cylinder->addTransform( rotation );

    // Translates half way so cylinder connects spheres
    QGraphicsTranslation3D *translation = new QGraphicsTranslation3D();
    QVector3D translationVector = ( vector2 - vector1 ) / 2;
    translation->setTranslate( translationVector );
    cylinder->addTransform( translation );

    return cylinder;
}

void UIGraphView::reset()
{
    this->nodes->clear();
    this->update();
}
