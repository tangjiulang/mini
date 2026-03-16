#pragma once

#include "data_board_item.hxx"
#include "font.hxx"
#include <color4d.hxx>
#include <render_settings.hxx>
#include <text_attributes.hxx>

namespace MINI
{
class DATA_Text : public BOARD_ITEM
{
public:
    DATA_Text(std::string aText, TEXT_ATTRIBUTES aTextAttrs, VECTOR2I aPosition);
    DATA_Text(std::string aText, TEXT_ATTRIBUTES aTextAttrs, VECTOR2I aPosition, PCB_LAYER_ID aLayer);

    int GetEffectiveTextPenWidth(int aDefaultPenWidth = 0) const;

    BOX2I GetTextBox(int aLine = -1) const;

    virtual const BOX2I GetBoundingBox() const override;

    std::string GetClass() const override { return "Text"; }

    KIFONT::FONT* GetDrawFont(const RENDER_SETTINGS* aSettings) const;

    TEXT_ATTRIBUTES m_textAttrs;
    std::string     m_text;
    VECTOR2I	   m_position;
};
}
