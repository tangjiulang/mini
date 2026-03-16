#include "data_text.hxx"
#include <text_calculate.hxx>
#include <render_settings.hxx>

using namespace MINI;

MINI::DATA_Text::DATA_Text(std::string aText, TEXT_ATTRIBUTES aTextAttrs, VECTOR2I aPosition) :
        BOARD_ITEM(ITEM_TYPE::TEXT), m_text(std::move(aText)), m_textAttrs(aTextAttrs), m_position(aPosition)
{
}

MINI::DATA_Text::DATA_Text(std::string aText, TEXT_ATTRIBUTES aTextAttrs, VECTOR2I aPosition, PCB_LAYER_ID aLayer) :
        BOARD_ITEM(ITEM_TYPE::TEXT, aLayer), m_text(std::move(aText)), m_textAttrs(aTextAttrs), m_position(aPosition)
{
}

int MINI::DATA_Text::GetEffectiveTextPenWidth(int aDefaultPenWidth) const
{
    int penWidth = m_textAttrs.m_StrokeWidth;

    if(penWidth <= 1)
    {
        penWidth = aDefaultPenWidth;

        if(m_textAttrs.m_Bold)
            penWidth = GetPenSizeForBold(m_textAttrs.m_Size.x);
        else if(penWidth <= 1)
            penWidth = GetPenSizeForBold(m_textAttrs.m_Size.x);
    }

    // Clip pen size for small texts:
    penWidth = ClampTextPenSize(penWidth, m_textAttrs.m_Size);

    return penWidth;
}

BOX2I MINI::DATA_Text::GetTextBox(int aLine) const
{
    VECTOR2I drawPos = m_position;
    BOX2I    bbox;
    int      thickness = GetEffectiveTextPenWidth();
    std::vector<std::string> strings;
    

    return BOX2I();
}

const BOX2I MINI::DATA_Text::GetBoundingBox() const
{
    return BOX2I{ m_position, VECTOR2I{ 1, 1 } };
    //return GetTextBox();
}

KIFONT::FONT* MINI::DATA_Text::GetDrawFont(const RENDER_SETTINGS* aSettings) const
{
    KIFONT::FONT* font = m_textAttrs.m_Font;

    if(!font)
    {
        if(aSettings)
            font = KIFONT::FONT::GetFont(aSettings->GetDefaultFont(), m_textAttrs.m_Bold, m_textAttrs.m_Italic);
        else
            font = KIFONT::FONT::GetFont("", m_textAttrs.m_Bold, m_textAttrs.m_Italic);
    }

    return font;
}