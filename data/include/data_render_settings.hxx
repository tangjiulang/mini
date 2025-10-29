#pragma once

#include "render_settings.hxx"


namespace KIGFX {
    class DATA_PAINTER;
    /**
    * PCB specific render settings.
    */
    class DATA_RENDER_SETTINGS : public RENDER_SETTINGS
    {
    public:
        friend class DATA_PAINTER;

        DATA_RENDER_SETTINGS();

        /**
            * Load settings related to display options (high-contrast mode, full or outline modes
            * for vias/pads/tracks and so on).
            *
            * @param aOptions are settings that you want to use for displaying items.
            */
            //void LoadDisplayOptions(const PCB_DISPLAY_OPTIONS& aOptions);

        void LoadColors(const COLOR_SETTINGS* aSettings) override;

        /// @copydoc RENDER_SETTINGS::GetColor()
        COLOR4D GetColor(const VIEW_ITEM* aItem, int aLayer) const override {
            return COLOR4D(1, 1, 1, 1);
        }

        ///< Board-specific version
        //COLOR4D GetColor(const BOARD_ITEM* aItem, int aLayer) const;

        ///< nullptr version
        COLOR4D GetColor(std::nullptr_t, int aLayer) const
        {
            //return GetColor(static_cast<const BOARD_ITEM*>(nullptr), aLayer);
        }

        bool GetShowPageLimits() const override;

        inline bool IsBackgroundDark() const override
        {
            auto it = m_layerColors.find(LAYER_PCB_BACKGROUND);

            if (it == m_layerColors.end())
                return false;

            return it->second.GetBrightness() < 0.5;
        }

        const COLOR4D& GetBackgroundColor() const override
        {
            auto it = m_layerColors.find(LAYER_PCB_BACKGROUND);
            return it == m_layerColors.end() ? COLOR4D::BLACK : it->second;
        }

        void SetBackgroundColor(const COLOR4D& aColor) override
        {
            m_layerColors[LAYER_PCB_BACKGROUND] = aColor;
        }

        const COLOR4D& GetGridColor() override { return m_layerColors[LAYER_GRID]; }

        const COLOR4D& GetCursorColor() override { return m_layerColors[LAYER_CURSOR]; }

        //NET_COLOR_MODE GetNetColorMode() const { return m_netColorMode; }
        //void SetNetColorMode(NET_COLOR_MODE aMode) { m_netColorMode = aMode; }

        std::map<int, KIGFX::COLOR4D>& GetNetColorMap() { return m_netColors; }

        std::set<int>& GetHiddenNets() { return m_hiddenNets; }
        const std::set<int>& GetHiddenNets() const { return m_hiddenNets; }

    public:
        bool               m_ForcePadSketchModeOn;
        bool               m_ForceShowFieldsWhenFPSelected;

        //ZONE_DISPLAY_MODE  m_ZoneDisplayMode;
        //HIGH_CONTRAST_MODE m_ContrastModeDisplay;

        //PAD* m_PadEditModePad;       // Pad currently in Pad Edit Mode (if any)

    protected:
        ///< Maximum font size for netnames (and other dynamically shown strings)
        static const double MAX_FONT_SIZE;

        ///< How to display nets and netclasses with color overrides
        //NET_COLOR_MODE     m_netColorMode;

        ///< Overrides for specific netclass colors
        std::map<std::string, KIGFX::COLOR4D> m_netclassColors;

        ///< Overrides for specific net colors, stored as netcodes for the ratsnest to access easily
        std::map<int, KIGFX::COLOR4D> m_netColors;

        ///< Set of net codes that should not have their ratsnest displayed
        std::set<int> m_hiddenNets;

        // These opacity overrides multiply with any opacity in the base layer color
        double m_trackOpacity;     ///< Opacity override for all tracks
        double m_viaOpacity;       ///< Opacity override for all types of via
        double m_padOpacity;       ///< Opacity override for SMD pads and PTHs
        double m_zoneOpacity;      ///< Opacity override for filled zones
        double m_imageOpacity;     ///< Opacity override for user images
        double m_filledShapeOpacity;     ///< Opacity override for graphic shapes
    };

}