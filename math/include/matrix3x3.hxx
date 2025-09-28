#ifndef MATRIX3X3_H_
#define MATRIX3X3_H_

#include "vector2d.hxx"
#include "vector3.hxx"

/**
 * MATRIX3x3 describes a general 3x3 matrix.
 *
 * Any linear transformation in 2D can be represented
 * by a homogeneous 3x3 transformation matrix. Given a vector x, the linear transformation
 * with the transformation matrix M is given as
 *
 * x' = M * x
 *
 * To represent an affine transformation, homogeneous coordinates have to be used. That means
 * the 2D-vector (x, y) has to be extended to a 3D-vector by a third component (x, y, 1).
 *
 * Transformations can be easily combined by matrix multiplication.
 *
 * A * (B * x ) = (A * B) * x
 * ( A, B: transformation matrices, x: vector )
 *
 * This class was implemented using templates, so flexible type combinations are possible.
 *
 */

// Forward declaration for template friends
template <class T>
class MATRIX3x3;

template <class T>
std::ostream& operator<<( std::ostream& aStream, const MATRIX3x3<T>& aMatrix );

template <class T>
class MATRIX3x3
{
public:
    T m_data[3][3];

    /**
     * Initialize all matrix members to zero.
     */
    MATRIX3x3();

    /**
     * Initialize with 3 vectors
     */
    MATRIX3x3( VECTOR3<T> a1, VECTOR3<T> a2, VECTOR3<T> a3 );

    /**
     * Initialize with given matrix members
     *
     * @param a00 is the component [0,0].
     * @param a01 is the component [0,1].
     * @param a02 is the component [0,2].
     * @param a10 is the component [1,0].
     * @param a11 is the component [1,1].
     * @param a12 is the component [1,2].
     * @param a20 is the component [2,0].
     * @param a21 is the component [2,1].
     * @param a22 is the component [2,2].
     */
    MATRIX3x3( T a00, T a01, T a02, T a10, T a11, T a12, T a20, T a21, T a22 );

    /**
     * Set the matrix to the identity matrix.
     *
     * The diagonal components of the matrix are set to 1.
     */
    void SetIdentity();

    /**
     * Set the translation components of the matrix.
     *
     * @param aTranslation is the translation, specified as 2D-vector.
     */
    void SetTranslation( VECTOR2<T> aTranslation );

    /**
     * Get the translation components of the matrix.
     *
     * @return is the translation (2D-vector).
     */
    VECTOR2<T> GetTranslation() const;

    /**
     * Set the rotation components of the matrix.
     *
     * The angle needs to have a positive value for an anti-clockwise rotation.
     *
     * @param aAngle is the rotation angle in [rad].
     */
    void SetRotation( T aAngle );

    /**
     * Set the scale components of the matrix.
     *
     * @param aScale contains the scale factors, specified as 2D-vector.
     */
    void SetScale( VECTOR2<T> aScale );

    /**
     * Get the scale components of the matrix.
     *
     * @return the scale factors, specified as 2D-vector.
     */
    VECTOR2<T> GetScale() const;

    /**
     * Compute the determinant of the matrix.
     *
     * @return the determinant value.
     */
    T Determinant() const;

    /**
     * Determine the inverse of the matrix.
     *
     * The inverse of a transformation matrix can be used to revert a transformation.
     *
     * x = Minv * ( M * x )
     * ( M: transformation matrix, Minv: inverse transformation matrix, x: vector)
     *
     * @return the inverse matrix.
     */
    MATRIX3x3 Inverse() const;

    /**
     * Get the transpose of the matrix.
     *
     * @return the transpose matrix.
     */
    MATRIX3x3 Transpose() const;

    /**
     * Output to a stream.
     */
    friend std::ostream& operator<<<T>( std::ostream& aStream, const MATRIX3x3<T>& aMatrix );

    ///< Equality operator
    bool operator==( const MATRIX3x3<T>& aOtherMatrix ) const;

    ///< Not equality operator
    bool operator!=( const MATRIX3x3<T>& aOtherMatrix ) const;
};

// Operators

//! Matrix multiplication
template <class T> MATRIX3x3<T> const operator*( MATRIX3x3<T> const& aA, MATRIX3x3<T> const& aB );

//! Multiplication with a 2D vector, the 3rd z-component is assumed to be 1
template <class T> VECTOR2<T> const operator*( MATRIX3x3<T> const& aA, VECTOR2<T> const& aB );

template <class T> VECTOR3<T> const operator*( MATRIX3x3<T> const& aA, VECTOR3<T> const& aB );

//! Multiplication with a scalar
template <class T, class S> MATRIX3x3<T> const operator*( MATRIX3x3<T> const& aA, T aScalar );
template <class T, class S> MATRIX3x3<T> const operator*( T aScalar, MATRIX3x3<T> const& aMatrix );

// ----------------------
// --- Implementation ---
// ----------------------

template <class T>
MATRIX3x3<T>::MATRIX3x3()
{
    for( int j = 0; j < 3; j++ )
    {
        for( int i = 0; i < 3; i++ )
        {
            m_data[i][j] = 0.0;
        }
    }
}


template <class T>
MATRIX3x3<T>::MATRIX3x3( VECTOR3<T> a1, VECTOR3<T> a2, VECTOR3<T> a3 )
{
    m_data[0][0] = a1.x;
    m_data[0][1] = a1.y;
    m_data[0][2] = a1.z;

    m_data[1][0] = a2.x;
    m_data[1][1] = a2.y;
    m_data[1][2] = a2.z;

    m_data[2][0] = a3.x;
    m_data[2][1] = a3.y;
    m_data[2][2] = a3.z;
}


template <class T>
MATRIX3x3<T>::MATRIX3x3( T a00, T a01, T a02, T a10, T a11, T a12, T a20, T a21, T a22 )
{
    m_data[0][0] = a00;
    m_data[0][1] = a01;
    m_data[0][2] = a02;

    m_data[1][0] = a10;
    m_data[1][1] = a11;
    m_data[1][2] = a12;

    m_data[2][0] = a20;
    m_data[2][1] = a21;
    m_data[2][2] = a22;
}


template <class T>
void MATRIX3x3<T>::SetIdentity( void )
{
    for( int j = 0; j < 3; j++ )
    {
        for( int i = 0; i < 3; i++ )
        {
            if( i == j )
                m_data[i][j] = 1.0;
            else
                m_data[i][j] = 0.0;
        }
    }
}


template <class T>
void MATRIX3x3<T>::SetTranslation( VECTOR2<T> aTranslation )
{
    m_data[0][2] = aTranslation.x;
    m_data[1][2] = aTranslation.y;
}


template <class T>
VECTOR2<T> MATRIX3x3<T>::GetTranslation() const
{
    VECTOR2<T> result;
    result.x = m_data[0][2];
    result.y = m_data[1][2];

    return result;
}


template <class T>
void MATRIX3x3<T>::SetRotation( T aAngle )
{
    T cosValue = cos( aAngle );
    T sinValue = sin( aAngle );
    m_data[0][0] = cosValue;
    m_data[0][1] = -sinValue;
    m_data[1][0] = sinValue;
    m_data[1][1] = cosValue;
}


template <class T>
void MATRIX3x3<T>::SetScale( VECTOR2<T> aScale )
{
    m_data[0][0] = aScale.x;
    m_data[1][1] = aScale.y;
}


template <class T>
VECTOR2<T> MATRIX3x3<T>::GetScale() const
{
    VECTOR2<T> result( m_data[0][0], m_data[1][1] );

    return result;
}


template <class T>
MATRIX3x3<T> const operator*( MATRIX3x3<T> const& aA, MATRIX3x3<T> const& aB )
{
    MATRIX3x3<T> result;

    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            result.m_data[i][j] = aA.m_data[i][0] * aB.m_data[0][j] +
                                  aA.m_data[i][1] * aB.m_data[1][j] +
                                  aA.m_data[i][2] * aB.m_data[2][j];
        }
    }

    return result;
}


template <class T>
VECTOR2<T> const operator*( MATRIX3x3<T> const& aMatrix, VECTOR2<T> const& aVector )
{
    VECTOR2<T> result( 0, 0 );
    result.x = aMatrix.m_data[0][0] * aVector.x + aMatrix.m_data[0][1] * aVector.y
               + aMatrix.m_data[0][2];
    result.y = aMatrix.m_data[1][0] * aVector.x + aMatrix.m_data[1][1] * aVector.y
               + aMatrix.m_data[1][2];

    return result;
}


template <class T>
VECTOR3<T> const operator*( MATRIX3x3<T> const& aMatrix, VECTOR3<T> const& aVector )
{
    VECTOR3<T> result( 0, 0, 0 );
    result.x = aMatrix.m_data[0][0] * aVector.x + aMatrix.m_data[0][1] * aVector.y
               + aMatrix.m_data[0][2] * aVector.z;
    result.y = aMatrix.m_data[1][0] * aVector.x + aMatrix.m_data[1][1] * aVector.y
               + aMatrix.m_data[1][2] * aVector.z;
    result.z = aMatrix.m_data[2][0] * aVector.x + aMatrix.m_data[2][1] * aVector.y
               + aMatrix.m_data[2][2] * aVector.z;

    return result;
}


template <class T>
T MATRIX3x3<T>::Determinant() const
{
    return m_data[0][0] * ( m_data[1][1] * m_data[2][2] - m_data[1][2] * m_data[2][1] )
           - m_data[0][1] * ( m_data[1][0] * m_data[2][2] - m_data[1][2] * m_data[2][0] )
           + m_data[0][2] * ( m_data[1][0] * m_data[2][1] - m_data[1][1] * m_data[2][0] );
}


template <class T, class S>
MATRIX3x3<T> const operator*( MATRIX3x3<T> const& aMatrix, S aScalar )
{
    MATRIX3x3<T> result;

    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            result.m_data[i][j] = aMatrix.m_data[i][j] * aScalar;
        }
    }

    return result;
}


template <class T, class S>
MATRIX3x3<T> const operator*( S aScalar, MATRIX3x3<T> const& aMatrix )
{
    return aMatrix * aScalar;
}


template <class T>
MATRIX3x3<T> MATRIX3x3<T>::Inverse() const
{
    MATRIX3x3<T> result;

    result.m_data[0][0] = m_data[1][1] * m_data[2][2] - m_data[2][1] * m_data[1][2];
    result.m_data[0][1] = m_data[0][2] * m_data[2][1] - m_data[2][2] * m_data[0][1];
    result.m_data[0][2] = m_data[0][1] * m_data[1][2] - m_data[1][1] * m_data[0][2];

    result.m_data[1][0] = m_data[1][2] * m_data[2][0] - m_data[2][2] * m_data[1][0];
    result.m_data[1][1] = m_data[0][0] * m_data[2][2] - m_data[2][0] * m_data[0][2];
    result.m_data[1][2] = m_data[0][2] * m_data[1][0] - m_data[1][2] * m_data[0][0];

    result.m_data[2][0] = m_data[1][0] * m_data[2][1] - m_data[2][0] * m_data[1][1];
    result.m_data[2][1] = m_data[0][1] * m_data[2][0] - m_data[2][1] * m_data[0][0];
    result.m_data[2][2] = m_data[0][0] * m_data[1][1] - m_data[1][0] * m_data[0][1];

    return result * ( 1.0 / Determinant() );
}


template <class T>
MATRIX3x3<T> MATRIX3x3<T>::Transpose() const
{
    MATRIX3x3<T> result;

    for( int i = 0; i < 3; i++ )
    {
        for( int j = 0; j < 3; j++ )
        {
            result.m_data[j][i] = m_data[i][j];
        }
    }

    return result;
}


template <class T>
std::ostream& operator<<( std::ostream& aStream, const MATRIX3x3<T>& aMatrix )
{
    for( int i = 0; i < 3; i++ )
    {
        aStream << "| ";

        for( int j = 0; j < 3; j++ )
        {
            aStream << aMatrix.m_data[i][j];
            aStream << " ";
        }

        aStream << "|";
        aStream << "\n";
    }

    return aStream;
}


template <class T>
bool MATRIX3x3<T>::operator==( MATRIX3x3<T> const& aOtherMatrix ) const
{
    return aOtherMatrix.m_data[0][0] == m_data[0][0] &&
        aOtherMatrix.m_data[0][1] == m_data[0][1] &&
        aOtherMatrix.m_data[0][2] == m_data[0][2] &&
        aOtherMatrix.m_data[1][0] == m_data[1][0] &&
        aOtherMatrix.m_data[1][1] == m_data[1][1] &&
        aOtherMatrix.m_data[1][2] == m_data[1][2] &&
        aOtherMatrix.m_data[2][0] == m_data[2][0] &&
        aOtherMatrix.m_data[2][1] == m_data[2][1] &&
        aOtherMatrix.m_data[2][2] == m_data[2][2];
}


template <class T>
bool MATRIX3x3<T>::operator!=( MATRIX3x3<T> const& aOtherMatrix ) const
{
    return aOtherMatrix.m_data[0][0] != m_data[0][0] ||
        aOtherMatrix.m_data[0][1] != m_data[0][1] ||
        aOtherMatrix.m_data[0][2] != m_data[0][2] ||
        aOtherMatrix.m_data[1][0] != m_data[1][0] ||
        aOtherMatrix.m_data[1][1] != m_data[1][1] ||
        aOtherMatrix.m_data[1][2] != m_data[1][2] ||
        aOtherMatrix.m_data[2][0] != m_data[2][0] ||
        aOtherMatrix.m_data[2][1] != m_data[2][1] ||
        aOtherMatrix.m_data[2][2] != m_data[2][2];
}


/* Default specializations */
typedef MATRIX3x3<double> MATRIX3x3D;

#endif /* MATRIX3X3_H_ */
