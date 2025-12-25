#ifndef EDA_ITEM_FLAGS_H
#define EDA_ITEM_FLAGS_H

#include <cstdint>

 // These define are used for the .m_flags member of the class EDA_ITEM
 //
 // NB: DO NOT ADD FLAGS ANYWHERE BUT AT THE END: THE FLAG-SET IS STORED AS AN INTEGER IN FILES.
 //
#define IS_CHANGED              (1UL << 0)    ///< Item was edited, and modified
#define IS_LINKED               (1UL << 1)    ///< Used in calculation to mark linked items (temporary use)
#define IN_EDIT                 (1UL << 2)    ///< Item currently edited
#define IS_MOVING               (1UL << 3)    ///< Item being moved
#define IS_NEW                  (1UL << 4)    ///< New item, just created
#define IS_BROKEN               (1UL << 5)    ///< Is a segment just broken by BreakSegment

#define IS_DELETED              (1UL << 7)

#define STARTPOINT              (1UL << 9)    ///< When a line is selected, these flags indicate which
#define ENDPOINT                (1UL << 10)   ///< ends.  (Used to support dragging.)
#define SELECTED                (1UL << 11)   ///< Item was manually selected by the user
#define SELECTED_BY_DRAG        (1UL << 12)   ///< Item was algorithmically selected as a dragged item
#define STRUCT_DELETED          (1UL << 13)   ///< flag indication structures to be erased
#define CANDIDATE               (1UL << 14)   ///< flag indicating that the structure is connected
#define SKIP_STRUCT             (1UL << 15)   ///< flag indicating that the structure should be ignored

#define IS_PASTED               (1UL << 17)   ///< Modifier on IS_NEW which indicates it came from clipboard
#define IS_SHOWN_AS_BITMAP      (1UL << 18)
#define COURTYARD_CONFLICT      (1UL << 19)   ///< temporary set when moving footprints
                                              ///< having courtyard overlapping
#define MALFORMED_F_COURTYARD   (1UL << 20)
#define MALFORMED_B_COURTYARD   (1UL << 21)
#define MALFORMED_COURTYARDS    ( MALFORMED_F_COURTYARD | MALFORMED_B_COURTYARD )

#define ROUTER_TRANSIENT        (1UL << 22)   ///< transient items that should NOT be cached

#define CONNECTIVITY_CANDIDATE  (1UL << 23)   ///< flag indicating that the structure is connected for connectivity

#define HOLE_PROXY              (1UL << 24)   ///< Indicates the BOARD_ITEM is a proxy for its hole
#define SHOW_ELEC_TYPE          (1UL << 25)   ///< Show pin electrical type
#define BRIGHTENED              (1UL << 26)   ///< item is drawn with a bright contour

#define MCT_SKIP_STRUCT (1 << 27)    ///< flag used by the multichannel tool to mark items that should be skipped

#define UR_TRANSIENT             (1UL << 28)   ///< indicates the item is owned by the undo/redo stack

#define IS_DANGLING              (1UL << 29)   ///< indicates a pin is dangling
#define ENTERED                  (1UL << 30)   ///< indicates a group has been entered
#define SELECTION_CANDIDATE      (1UL << 31)   ///< indicates an item is a candidate for selection

// WARNING: if you add flags, you'll probably need to adjust the masks in GetEditFlags() and
// ClearTempFlags().

#define ITEM_ALL_FLAGS UINT32_MAX

typedef std::uint32_t ITEM_FLAGS;

// Helper function to convert flags to string descriptions
#include <string>
#include <vector>
#include <sstream>

inline std::string ItemFlagsToString(ITEM_FLAGS flags)
{
    struct FlagDesc
    {
        ITEM_FLAGS value;
        const char* name;
    };

    static const FlagDesc flagDescs[] = { { IS_CHANGED, "IS_CHANGED" },
                                          { IS_LINKED, "IS_LINKED" },
                                          { IN_EDIT, "IN_EDIT" },
                                          { IS_MOVING, "IS_MOVING" },
                                          { IS_NEW, "IS_NEW" },
                                          { IS_BROKEN, "IS_BROKEN" },
                                          { IS_DELETED, "IS_DELETED" },
                                          { STARTPOINT, "STARTPOINT" },
                                          { ENDPOINT, "ENDPOINT" },
                                          { SELECTED, "SELECTED" },
                                          { SELECTED_BY_DRAG, "SELECTED_BY_DRAG" },
                                          { STRUCT_DELETED, "STRUCT_DELETED" },
                                          { CANDIDATE, "CANDIDATE" },
                                          { SKIP_STRUCT, "SKIP_STRUCT" },
                                          { IS_PASTED, "IS_PASTED" },
                                          { IS_SHOWN_AS_BITMAP, "IS_SHOWN_AS_BITMAP" },
                                          { COURTYARD_CONFLICT, "COURTYARD_CONFLICT" },
                                          { MALFORMED_F_COURTYARD, "MALFORMED_F_COURTYARD" },
                                          { MALFORMED_B_COURTYARD, "MALFORMED_B_COURTYARD" },
                                          { ROUTER_TRANSIENT, "ROUTER_TRANSIENT" },
                                          { CONNECTIVITY_CANDIDATE, "CONNECTIVITY_CANDIDATE" },
                                          { HOLE_PROXY, "HOLE_PROXY" },
                                          { SHOW_ELEC_TYPE, "SHOW_ELEC_TYPE" },
                                          { BRIGHTENED, "BRIGHTENED" },
                                          { UR_TRANSIENT, "UR_TRANSIENT" },
                                          { IS_DANGLING, "IS_DANGLING" },
                                          { ENTERED, "ENTERED" },
                                          { SELECTION_CANDIDATE, "SELECTION_CANDIDATE" } };

    std::vector<std::string> setFlags;
    for (const auto& desc : flagDescs)
    {
        if (flags & desc.value)
            setFlags.push_back(desc.name);
    }

    std::ostringstream oss;
    for (size_t i = 0; i < setFlags.size(); ++i)
    {
        if (i > 0)
            oss << " | ";
        oss << setFlags[i];
    }
    if (setFlags.empty())
        return "0";
    return oss.str();
}


#endif
