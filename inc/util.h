#ifndef UTIL_H
#define UTIL_H

#include <QVector3D>
#include <cmath>

double angleBetweenVectors( const QVector3D v1, const QVector3D v2, bool inDegrees )
{
    double dotProduct = QVector3D::dotProduct( v1, v2 );
    double magnitude1 = sqrt( pow( v1.x(), 2.0f ) + pow( v1.y(), 2.0f ) + pow( v1.z(), 2.0f ) );
    double magnitude2 = sqrt( pow( v2.x(), 2.0f ) + pow( v2.y(), 2.0f ) + pow( v2.z(), 2.0f ) );
    double magnitudeCombination = magnitude1 * magnitude2;
    double radianAngle = ( fabs( dotProduct - magnitudeCombination ) < .00001 ) ? 0 : acos( dotProduct / magnitudeCombination );

    if( inDegrees ) {
        return radianAngle * 180.0 / 3.141592654;
    } else {
        return radianAngle;
    }
}

#endif // UTIL_H
