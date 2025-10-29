#pragma once

namespace KIGFX {
/**
    * Define the how severely the appearance of the item has been changed.
    */
enum VIEW_UPDATE_FLAGS {
    NONE = 0x00,     ///< No updates are required.
    APPEARANCE = 0x01,     ///< Visibility flag has changed.
    COLOR = 0x02,     ///< Color has changed.
    GEOMETRY = 0x04,     ///< Position or shape has changed.
    LAYERS = 0x08,     ///< Layers have changed.
    INITIAL_ADD = 0x10,     ///< Item is being added to the view.
    REPAINT = 0x20,     ///< Item needs to be redrawn.
    ALL = 0xef      ///< All except INITIAL_ADD.
};

/**
    * Define the visibility of the item (temporarily hidden, invisible, etc).
    */
enum VIEW_VISIBILITY_FLAGS {
    VISIBLE = 0x01,  ///< Item is visible (in general)

    /// Item is temporarily hidden (usually in favor of a being drawn from an overlay, such as a
    /// #SELECTION).  Overrides #VISIBLE flag.
    HIDDEN = 0x02,
    OVERLAY_HIDDEN = 0x04   ///< Item is temporarily hidden from being drawn on an overlay.
};
}