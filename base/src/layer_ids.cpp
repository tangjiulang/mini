#include <format>
#include <magic_enum.hpp>
#include <layer_ids.hxx>

 /**
  * Returns the default display name for a given layer.  These are not the same as the canonical
  * name in LSET::Name(), which is used in board files and cannot be translated or changed.
  * WARNING: do not translate board physical layers names (F.Cu to User.9): because canonical names
  * are used in files (boards and fab files), using translated names in UI create mistakes for users.
  * Board physical layers names must be seen as proper nouns.
  */
std::string LayerName(int aLayer)
{
    switch (aLayer)
    {
        // PCB_LAYER_ID
    case UNDEFINED_LAYER:                 return ("undefined");

        // Copper
    case PCB_LAYER_ID::F_Cu:              return "F.Cu";
    case PCB_LAYER_ID::B_Cu:              return "B.Cu";

        // Technicals
    case PCB_LAYER_ID::B_Adhes:           return ("B.Adhesive");
    case PCB_LAYER_ID::F_Adhes:           return ("F.Adhesive");
    case PCB_LAYER_ID::B_Paste:           return ("B.Paste");
    case PCB_LAYER_ID::F_Paste:           return ("F.Paste");
    case PCB_LAYER_ID::B_SilkS:           return ("B.Silkscreen");
    case PCB_LAYER_ID::F_SilkS:           return ("F.Silkscreen");
    case PCB_LAYER_ID::B_Mask:            return ("B.Mask");
    case PCB_LAYER_ID::F_Mask:            return ("F.Mask");

        // Users
    case PCB_LAYER_ID::Dwgs_User:         return ("User.Drawings");
    case PCB_LAYER_ID::Cmts_User:         return ("User.Comments");
    case PCB_LAYER_ID::Eco1_User:         return ("User.Eco1");
    case PCB_LAYER_ID::Eco2_User:         return ("User.Eco2");
    case PCB_LAYER_ID::Edge_Cuts:         return ("Edge.Cuts");
    case PCB_LAYER_ID::Margin:            return ("Margin");

        // Footprint
    case PCB_LAYER_ID::F_CrtYd:           return ("F.Courtyard");
    case PCB_LAYER_ID::B_CrtYd:           return ("B.Courtyard");
    case PCB_LAYER_ID::F_Fab:             return ("F.Fab");
    case PCB_LAYER_ID::B_Fab:             return ("B.Fab");

        // Rescue
    case PCB_LAYER_ID::Rescue:            return ("Rescue");

        // SCH_LAYER_ID

    case LAYER_WIRE:                    return ("Wires");
    case LAYER_BUS:                     return ("Buses");
    case LAYER_BUS_JUNCTION:            return ("Bus junctions");
    case LAYER_JUNCTION:                return ("Junctions");
    case LAYER_LOCLABEL:                return ("Labels");
    case LAYER_GLOBLABEL:               return ("Global labels");
    case LAYER_HIERLABEL:               return ("Hierarchical labels");
    case LAYER_PINNUM:                  return ("Pin numbers");
    case LAYER_PINNAM:                  return ("Pin names");
    case LAYER_REFERENCEPART:           return ("Symbol references");
    case LAYER_VALUEPART:               return ("Symbol values");
    case LAYER_FIELDS:                  return ("Symbol fields");
    case LAYER_INTERSHEET_REFS:         return ("Sheet references");
    case LAYER_NETCLASS_REFS:           return ("Net class references");
    case LAYER_RULE_AREAS:              return ("Rule areas");
    case LAYER_DEVICE:                  return ("Symbol body outlines");
    case LAYER_DEVICE_BACKGROUND:       return ("Symbol body fills");
    case LAYER_SHAPES_BACKGROUND:       return ("Shape fills");
    case LAYER_NOTES:                   return ("Schematic text && graphics");
    case LAYER_PRIVATE_NOTES:           return ("Symbol private text && graphics");
    case LAYER_NOTES_BACKGROUND:        return ("Schematic text && graphics backgrounds");
    case LAYER_PIN:                     return ("Pins");
    case LAYER_SHEET:                   return ("Sheet borders");
    case LAYER_SHEET_BACKGROUND:        return ("Sheet backgrounds");
    case LAYER_SHEETNAME:               return ("Sheet names");
    case LAYER_SHEETFIELDS:             return ("Sheet fields");
    case LAYER_SHEETFILENAME:           return ("Sheet file names");
    case LAYER_SHEETLABEL:              return ("Sheet pins");
    case LAYER_NOCONNECT:               return ("No-connect symbols");
    case LAYER_DNP_MARKER:              return ("DNP markers");
    case LAYER_EXCLUDED_FROM_SIM:       return ("Excluded-from-simulation markers");
    case LAYER_ERC_WARN:                return ("ERC warnings");
    case LAYER_ERC_ERR:                 return ("ERC errors");
    case LAYER_ERC_EXCLUSION:           return ("ERC exclusions");
    case LAYER_SCHEMATIC_ANCHOR:        return ("Anchors");
    case LAYER_SCHEMATIC_AUX_ITEMS:     return ("Helper items");
    case LAYER_SCHEMATIC_GRID:          return ("Grid");
    case LAYER_SCHEMATIC_GRID_AXES:     return ("Axes");
    case LAYER_SCHEMATIC_BACKGROUND:    return ("Background");
    case LAYER_SCHEMATIC_CURSOR:        return ("Cursor");
    case LAYER_HOVERED:                 return ("Hovered items");
    case LAYER_BRIGHTENED:              return ("Highlighted items");
    case LAYER_HIDDEN:                  return ("Hidden items");
    case LAYER_SELECTION_SHADOWS:       return ("Selection highlight");
    case LAYER_NET_COLOR_HIGHLIGHT:     return ("Net color highlight");
    case LAYER_SCHEMATIC_DRAWINGSHEET:  return ("Drawing sheet");
    case LAYER_SCHEMATIC_PAGE_LIMITS:   return ("Page limits");
    case LAYER_OP_VOLTAGES:             return ("Operating point voltages");
    case LAYER_OP_CURRENTS:             return ("Operating point currents");

        // GAL_LAYER_ID

    case LAYER_FOOTPRINTS_FR:           return ("Footprints front");
    case LAYER_FOOTPRINTS_BK:           return ("Footprints back");
    case LAYER_FP_VALUES:               return ("Values");
    case LAYER_FP_REFERENCES:           return ("Reference designators");
    case LAYER_FP_TEXT:                 return ("Footprint text");
    case LAYER_TRACKS:                  return ("Tracks");
    case LAYER_VIA_THROUGH:             return ("Through vias");
    case LAYER_VIA_BLIND:               return ("Blind vias");
    case LAYER_VIA_BURIED:              return ("Buried vias");
    case LAYER_VIA_MICROVIA:            return ("Micro-vias");
    case LAYER_VIA_HOLES:               return ("Via holes");
    case LAYER_VIA_HOLEWALLS:           return ("Via hole walls");
    case LAYER_PAD_PLATEDHOLES:         return ("Plated holes");
    case LAYER_PAD_HOLEWALLS:           return ("Plated hole walls");
    case LAYER_NON_PLATEDHOLES:         return ("Non-plated holes");
    case LAYER_RATSNEST:                return ("Ratsnest");
    case LAYER_DRC_WARNING:             return ("DRC warnings");
    case LAYER_DRC_ERROR:               return ("DRC errors");
    case LAYER_DRC_SHAPES:              return ("DRC shapes");
    case LAYER_DRC_EXCLUSION:           return ("DRC exclusions");
    case LAYER_MARKER_SHADOWS:          return ("DRC marker shadows");
    case LAYER_ANCHOR:                  return ("Anchors");
    case LAYER_POINTS:                  return ("Points");
    case LAYER_DRAWINGSHEET:            return ("Drawing sheet");
    case LAYER_PAGE_LIMITS:             return ("Page limits");
    case LAYER_CURSOR:                  return ("Cursor");
    case LAYER_AUX_ITEMS:               return ("Helper items");
    case LAYER_GRID:                    return ("Grid");
    case LAYER_GRID_AXES:               return ("Grid axes");
    case LAYER_PCB_BACKGROUND:          return ("Background");
    case LAYER_SELECT_OVERLAY:          return ("Selection highlight");
    case LAYER_LOCKED_ITEM_SHADOW:      return ("Locked item shadow");
    case LAYER_CONFLICTS_SHADOW:        return ("Courtyard collision shadow");
    case LAYER_BOARD_OUTLINE_AREA:      return ("Board outline area");
    case NETNAMES_LAYER_ID_START:       return ("Track net names");
    case LAYER_PAD_NETNAMES:            return ("Pad net names");
    case LAYER_VIA_NETNAMES:            return ("Via net names");

    default:
        // Catch the general board layers that have numerically increasing names
        if (aLayer > 0 && aLayer < PCB_LAYER_ID_COUNT && aLayer & 1)
            return std::format("User.%d", (aLayer - PCB_LAYER_ID::User_1) / 2 + 1);

        return std::format("In%d.Cu", (aLayer - PCB_LAYER_ID::In1_Cu) / 2 + 1);
    }
}


PCB_LAYER_ID FlipLayer(PCB_LAYER_ID aLayerId, int aCopperLayersCount)
{
    switch (aLayerId)
    {
    case B_Cu:              return F_Cu;
    case F_Cu:              return B_Cu;

    case B_SilkS:           return F_SilkS;
    case F_SilkS:           return B_SilkS;

    case B_Adhes:           return F_Adhes;
    case F_Adhes:           return B_Adhes;

    case B_Mask:            return F_Mask;
    case F_Mask:            return B_Mask;

    case B_Paste:           return F_Paste;
    case F_Paste:           return B_Paste;

    case B_CrtYd:           return F_CrtYd;
    case F_CrtYd:           return B_CrtYd;

    case B_Fab:             return F_Fab;
    case F_Fab:             return B_Fab;

    default:    // change internal layer if aCopperLayersCount is >= 4
        if (IsCopperLayer(aLayerId) && aCopperLayersCount >= 4)
        {
            // internal copper layers count is aCopperLayersCount-2
            PCB_LAYER_ID fliplayer = PCB_LAYER_ID(aCopperLayersCount - 2 - (aLayerId - In1_Cu));
            // Ensure fliplayer has a value which does not crash Pcbnew:
            if (fliplayer < F_Cu)
                fliplayer = F_Cu;

            if (fliplayer > B_Cu)
                fliplayer = B_Cu;

            return fliplayer;
        }

        // No change for the other layers
        return aLayerId;
    }
}


PCB_LAYER_ID BoardLayerFromLegacyId(int aLegacyId)
{
    switch (aLegacyId)
    {
    case 0:  return F_Cu;
    case 31: return B_Cu;

    default:
        if (aLegacyId < 0)
            return magic_enum::enum_cast<PCB_LAYER_ID>(aLegacyId).value_or(UNDEFINED_LAYER);

        if (aLegacyId < 31)
            return static_cast<PCB_LAYER_ID>(In1_Cu + (aLegacyId - 1) * 2);

        switch (aLegacyId)
        {
        case 32: return B_Adhes;
        case 33: return F_Adhes;
        case 34: return B_Paste;
        case 35: return F_Paste;
        case 36: return B_SilkS;
        case 37: return F_SilkS;
        case 38: return B_Mask;
        case 39: return F_Mask;
        case 40: return Dwgs_User;
        case 41: return Cmts_User;
        case 42: return Eco1_User;
        case 43: return Eco2_User;
        case 44: return Edge_Cuts;
        case 45: return Margin;
        case 46: return B_CrtYd;
        case 47: return F_CrtYd;
        case 48: return B_Fab;
        case 49: return F_Fab;
        case 50: return User_1;
        case 51: return User_2;
        case 52: return User_3;
        case 53: return User_4;
        case 54: return User_5;
        case 55: return User_6;
        case 56: return User_7;
        case 57: return User_8;
        case 58: return User_9;
        case 59: return Rescue;
        default: return UNDEFINED_LAYER;
        }
    }
}


PCB_LAYER_ID Map3DLayerToPCBLayer(int aLayer)
{
    // NOTE: User_1..User45 are NOT consecutive numbers!

    switch (aLayer)
    {
    case LAYER_3D_COPPER_TOP:        return F_Cu;
    case LAYER_3D_COPPER_BOTTOM:     return B_Cu;
    case LAYER_3D_SILKSCREEN_BOTTOM: return B_SilkS;
    case LAYER_3D_SILKSCREEN_TOP:    return F_SilkS;
    case LAYER_3D_SOLDERMASK_BOTTOM: return B_Mask;
    case LAYER_3D_SOLDERMASK_TOP:    return F_Mask;
    case LAYER_3D_USER_COMMENTS:     return Cmts_User;
    case LAYER_3D_USER_DRAWINGS:     return Dwgs_User;
    case LAYER_3D_USER_ECO1:         return Eco1_User;
    case LAYER_3D_USER_ECO2:         return Eco2_User;
    case LAYER_3D_USER_1:            return User_1;
    case LAYER_3D_USER_2:            return User_2;
    case LAYER_3D_USER_3:            return User_3;
    case LAYER_3D_USER_4:            return User_4;
    case LAYER_3D_USER_5:            return User_5;
    case LAYER_3D_USER_6:            return User_6;
    case LAYER_3D_USER_7:            return User_7;
    case LAYER_3D_USER_8:            return User_8;
    case LAYER_3D_USER_9:            return User_9;
    case LAYER_3D_USER_10:           return User_10;
    case LAYER_3D_USER_11:           return User_11;
    case LAYER_3D_USER_12:           return User_12;
    case LAYER_3D_USER_13:           return User_13;
    case LAYER_3D_USER_14:           return User_14;
    case LAYER_3D_USER_15:           return User_15;
    case LAYER_3D_USER_16:           return User_16;
    case LAYER_3D_USER_17:           return User_17;
    case LAYER_3D_USER_18:           return User_18;
    case LAYER_3D_USER_19:           return User_19;
    case LAYER_3D_USER_20:           return User_20;
    case LAYER_3D_USER_21:           return User_21;
    case LAYER_3D_USER_22:           return User_22;
    case LAYER_3D_USER_23:           return User_23;
    case LAYER_3D_USER_24:           return User_24;
    case LAYER_3D_USER_25:           return User_25;
    case LAYER_3D_USER_26:           return User_26;
    case LAYER_3D_USER_27:           return User_27;
    case LAYER_3D_USER_28:           return User_28;
    case LAYER_3D_USER_29:           return User_29;
    case LAYER_3D_USER_30:           return User_30;
    case LAYER_3D_USER_31:           return User_31;
    case LAYER_3D_USER_32:           return User_32;
    case LAYER_3D_USER_33:           return User_33;
    case LAYER_3D_USER_34:           return User_34;
    case LAYER_3D_USER_35:           return User_35;
    case LAYER_3D_USER_36:           return User_36;
    case LAYER_3D_USER_37:           return User_37;
    case LAYER_3D_USER_38:           return User_38;
    case LAYER_3D_USER_39:           return User_39;
    case LAYER_3D_USER_40:           return User_40;
    case LAYER_3D_USER_41:           return User_41;
    case LAYER_3D_USER_42:           return User_42;
    case LAYER_3D_USER_43:           return User_43;
    case LAYER_3D_USER_44:           return User_44;
    case LAYER_3D_USER_45:           return User_45;
    default:               return UNDEFINED_LAYER;
    }
}


int MapPCBLayerTo3DLayer(PCB_LAYER_ID aLayer)
{
    // NOTE: User_1..User45 are NOT consecutive numbers!

    switch (aLayer)
    {
    case F_Cu:      return LAYER_3D_COPPER_TOP;
    case B_Cu:      return LAYER_3D_COPPER_BOTTOM;
    case B_SilkS:   return LAYER_3D_SILKSCREEN_BOTTOM;
    case F_SilkS:   return LAYER_3D_SILKSCREEN_TOP;
    case B_Mask:    return LAYER_3D_SOLDERMASK_BOTTOM;
    case F_Mask:    return LAYER_3D_SOLDERMASK_TOP;
    case Cmts_User: return LAYER_3D_USER_COMMENTS;
    case Dwgs_User: return LAYER_3D_USER_DRAWINGS;
    case Eco1_User: return LAYER_3D_USER_ECO1;
    case Eco2_User: return LAYER_3D_USER_ECO2;
    case User_1:    return LAYER_3D_USER_1;
    case User_2:    return LAYER_3D_USER_2;
    case User_3:    return LAYER_3D_USER_3;
    case User_4:    return LAYER_3D_USER_4;
    case User_5:    return LAYER_3D_USER_5;
    case User_6:    return LAYER_3D_USER_6;
    case User_7:    return LAYER_3D_USER_7;
    case User_8:    return LAYER_3D_USER_8;
    case User_9:    return LAYER_3D_USER_9;
    case User_10:   return LAYER_3D_USER_10;
    case User_11:   return LAYER_3D_USER_11;
    case User_12:   return LAYER_3D_USER_12;
    case User_13:   return LAYER_3D_USER_13;
    case User_14:   return LAYER_3D_USER_14;
    case User_15:   return LAYER_3D_USER_15;
    case User_16:   return LAYER_3D_USER_16;
    case User_17:   return LAYER_3D_USER_17;
    case User_18:   return LAYER_3D_USER_18;
    case User_19:   return LAYER_3D_USER_19;
    case User_20:   return LAYER_3D_USER_20;
    case User_21:   return LAYER_3D_USER_21;
    case User_22:   return LAYER_3D_USER_22;
    case User_23:   return LAYER_3D_USER_23;
    case User_24:   return LAYER_3D_USER_24;
    case User_25:   return LAYER_3D_USER_25;
    case User_26:   return LAYER_3D_USER_26;
    case User_27:   return LAYER_3D_USER_27;
    case User_28:   return LAYER_3D_USER_28;
    case User_29:   return LAYER_3D_USER_29;
    case User_30:   return LAYER_3D_USER_30;
    case User_31:   return LAYER_3D_USER_31;
    case User_32:   return LAYER_3D_USER_32;
    case User_33:   return LAYER_3D_USER_33;
    case User_34:   return LAYER_3D_USER_34;
    case User_35:   return LAYER_3D_USER_35;
    case User_36:   return LAYER_3D_USER_36;
    case User_37:   return LAYER_3D_USER_37;
    case User_38:   return LAYER_3D_USER_38;
    case User_39:   return LAYER_3D_USER_39;
    case User_40:   return LAYER_3D_USER_40;
    case User_41:   return LAYER_3D_USER_41;
    case User_42:   return LAYER_3D_USER_42;
    case User_43:   return LAYER_3D_USER_43;
    case User_44:   return LAYER_3D_USER_44;
    case User_45:   return LAYER_3D_USER_45;
    default:        return UNDEFINED_LAYER;
    }
}
