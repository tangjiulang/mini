#ifndef TEXT_ATTRIBUTES_H
#define TEXT_ATTRIBUTES_H

#include <vector2d.hxx>
#include <color4d.hxx>
#include <eda_angle.hxx>
#include <gal/include/graphics_abstraction_layer.hxx>


namespace KIFONT
{
class FONT;
};

/**
 * Get the reverse alignment: left-right are swapped, others are unchanged.
 */
constexpr GR_TEXT_H_ALIGN_T GetFlippedAlignment( GR_TEXT_H_ALIGN_T aAlign )
{
    // Could use the -1/1 promise of the enum too.
    switch( aAlign )
    {
    case GR_TEXT_H_ALIGN_LEFT:
        return GR_TEXT_H_ALIGN_RIGHT;
    case GR_TEXT_H_ALIGN_RIGHT:
        return GR_TEXT_H_ALIGN_LEFT;
    case GR_TEXT_H_ALIGN_CENTER:
    case GR_TEXT_H_ALIGN_INDETERMINATE:
        break;
    }
    return aAlign;
};


/**
 * Get the reverse alignment: top-bottom are swapped, others are unchanged.
 */
constexpr GR_TEXT_V_ALIGN_T GetFlippedAlignment( GR_TEXT_V_ALIGN_T aAlign )
{
    switch( aAlign )
    {
    case GR_TEXT_V_ALIGN_BOTTOM:
        return GR_TEXT_V_ALIGN_TOP;
    case GR_TEXT_V_ALIGN_TOP:
        return GR_TEXT_V_ALIGN_BOTTOM;
    case GR_TEXT_V_ALIGN_CENTER:
    case GR_TEXT_V_ALIGN_INDETERMINATE:
        break;
    }
    return aAlign;
};


/**
 * Convert an integral value to horizontal alignment.
 *
 *  * x < 0: Left align
 *  * x == 0: Center
 *  * x > 0: Right align
 */
constexpr GR_TEXT_H_ALIGN_T ToHAlignment( int x )
{
    if( x < 0 )

        return GR_TEXT_H_ALIGN_LEFT;
    else if( x > 0 )
        return GR_TEXT_H_ALIGN_RIGHT;

    return GR_TEXT_H_ALIGN_CENTER;
}


class TEXT_ATTRIBUTES
{
public:
    TEXT_ATTRIBUTES( KIFONT::FONT* aFont = nullptr );

    int Compare( const TEXT_ATTRIBUTES& aRhs ) const;

    bool operator==( const TEXT_ATTRIBUTES& aRhs ) const { return Compare( aRhs ) == 0; }
    bool operator>( const TEXT_ATTRIBUTES& aRhs ) const { return Compare( aRhs ) > 0; }
    bool operator<( const TEXT_ATTRIBUTES& aRhs ) const { return Compare( aRhs ) < 0; }

    KIFONT::FONT*     m_Font;
    GR_TEXT_H_ALIGN_T m_Halign;
    GR_TEXT_V_ALIGN_T m_Valign;
    EDA_ANGLE         m_Angle;
    double            m_LineSpacing;
    int               m_StrokeWidth;
    bool              m_Italic;
    bool              m_Bold;
    bool              m_Underlined;
    bool              m_Hover;
    MINI::COLOR4D    m_Color;
    bool              m_Mirrored;
    bool              m_Multiline;
    VECTOR2I          m_Size;

    // If true, keep rotation angle between -90...90 degrees for readability
    bool              m_KeepUpright;
    int               m_StoredStrokeWidth;
};


std::ostream& operator<<( std::ostream& aStream,
                                         const TEXT_ATTRIBUTES& aAttributes );


template<>
struct std::hash<TEXT_ATTRIBUTES>
{
    std::size_t operator()( const TEXT_ATTRIBUTES& aAttributes ) const
    {
        return hash_val( aAttributes.m_Font, aAttributes.m_Halign, aAttributes.m_Valign,
                         aAttributes.m_Angle.AsDegrees(), aAttributes.m_LineSpacing,
                         aAttributes.m_StrokeWidth, aAttributes.m_Italic, aAttributes.m_Bold,
                         aAttributes.m_Underlined, aAttributes.m_Color, aAttributes.m_Mirrored,
                         aAttributes.m_Multiline, aAttributes.m_Size.x, aAttributes.m_Size.y );
    }
};

#endif //TEXT_ATTRIBUTES_H
