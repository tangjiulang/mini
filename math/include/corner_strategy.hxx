#ifndef CORNER_STRATEGY_H_
#define CORNER_STRATEGY_H_


/// define how inflate transform build inflated polygon
enum class CORNER_STRATEGY
{
    ALLOW_ACUTE_CORNERS,   ///< just inflate the polygon. Acute angles create spikes
    CHAMFER_ACUTE_CORNERS, ///< Acute angles are chamfered
    ROUND_ACUTE_CORNERS,   ///< Acute angles are rounded
    CHAMFER_ALL_CORNERS,   ///< All angles are chamfered.
                           ///< The distance between new and old polygon edges is not
                           ///< constant, but do not change a lot
    ROUND_ALL_CORNERS      ///< All angles are rounded.
                           ///< The distance between new and old polygon edges is constant
};


#endif // CORNER_STRATEGY_H_