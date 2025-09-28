#pragma once

enum class FLIP_DIRECTION
{
    LEFT_RIGHT, ///< Flip left to right (around the Y axis)
    TOP_BOTTOM  ///< Flip top to bottom (around the X axis)
};

/**
 *  Returns the mirror of aPoint relative to the aMirrorRef.
 */
template <typename T>
constexpr T MIRRORVAL( T aPoint, T aMirrorRef )
{
    return -( aPoint - aMirrorRef ) + aMirrorRef;
}

/**
 *  Updates aPoint with the mirror of aPoint relative to the aMirrorRef.
 */
template <typename T>
constexpr void MIRROR( T& aPoint, const T& aMirrorRef )
{
    aPoint = MIRRORVAL( aPoint, aMirrorRef );
}


/**
 * Updates aPoint with the mirror of aPoint relative to the aMirrorRef,
 * in the specified direction.
 */
template <typename VT>
constexpr void MIRROR( VT& aPoint, const VT& aMirrorRef, FLIP_DIRECTION aFlipDirection )
{
    if( aFlipDirection == FLIP_DIRECTION::LEFT_RIGHT )
        MIRROR( aPoint.x, aMirrorRef.x );
    else
        MIRROR( aPoint.y, aMirrorRef.y );
}
