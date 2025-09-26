#ifndef VECTOR2WX_H_
#define VECTOR2WX_H_

#include "vector2d.hxx"

inline VECTOR2I ToVECTOR2I(const QSize& aSize)
{
    return { aSize.width(), aSize.height()};
}

inline VECTOR2I ToVECTOR2I( const QPoint & aSize )
{
    return { aSize.x(), aSize.y()};
}

inline VECTOR2D ToVECTOR2D( const QPoint& aPoint )
{
    return VECTOR2D( aPoint.x(), aPoint.y());
}

inline VECTOR2D ToVECTOR2D( const QSize& aPoint )
{
    return VECTOR2D( aPoint.width(), aPoint.height());
}

inline QPoint ToQPoint( const VECTOR2I& aSize )
{
    return QPoint( aSize.x, aSize.y );
}

inline QSize ToQSize( const VECTOR2I& aSize )
{
    return QSize( aSize.x, aSize.y );
}

#endif VECTOR2WX_H_