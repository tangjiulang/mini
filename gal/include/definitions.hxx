#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

namespace KIGFX
{
/**
 * RENDER_TARGET: Possible rendering targets
 */
enum RENDER_TARGET
{
    TARGET_CACHED = 0,      ///< Main rendering target (cached)
    TARGET_NONCACHED,       ///< Auxiliary rendering target (noncached)
    TARGET_OVERLAY,         ///< Items that may change while the view stays the same (noncached)
    TARGET_TEMP,            ///< Temporary target for drawing in separate layer
    TARGETS_NUMBER          ///< Number of available rendering targets
};

// Used in view.h to initialize VIEW_MAX_LAYERS and graphic_abstraction_layer.cpp
#define MAX_LAYERS_FOR_VIEW 2048
} // namespace KIGFX

#endif /* DEFINITIONS_H_ */
