#pragma once
#include <string>

enum class LineEndType {
	NONE,
	ROUND,
	SQUARE
};

enum class FillPropertyType {
	HOLLOW,
	HATCH,
	MESH,
	FILL,
	VOID
};

enum class UnitsType {
	MILLMETER,
	MICRON,
	INCH
};

enum class WrongType {
	WrongRead = -1,
	Success,
	NotIPC2581File,
	UnsupportedRevision,
	UnSuccessfulReadSection
};

enum class LayerFunction {
	NONE,
	ASSEMBLY,
	BOARDFAB,
	BOARD_OUTLINE,
	CAPACITIVE,
	COATINGCOND,
	COATINGNONCOND,
	COMPONENT,
	COMPONENT_BOTTOM,
	COMPONENT_TOP,
	COMPONENT_EMBEDDED,
	COMPONENT_FORMED,
	CONDFILM,
	CONDFOIL,
	CONDUCTIVE_ADHESIVE,
	CONDUCTOR,
	COURTYARD,
	DIELBASE,
	DIELCORE,
	DIELPREG,
	DIELADHV,
	DIELBONDPLY,
	DIELCOVERLAY, 
	DOCUMENT,
	DRILL,
	FIXTURE,
	GLUE,
	GRAPHIC,
	HOLEFILL,
	SOLDERBUMP,
	PASTEMASK,
	LANDPATTERN,
	LEGEND,
	MIXED,
	OTHER,
	PIN,
	PLANE,
	PROBE,
	RESISTIVE,
	SIGNAL,
	SILKSCREEN,
	SOLDERMASK,
	SOLDERPASTE,
	STACKUP_COMPOSITE,
	REWORK,
	ROUT,
	V_CUT,
	EDGE_CHAMFER,
	EDGE_PLATING,
	THIEVING_KEEP_INOUT,
	STIFFENER
};

enum class Side {
	TOP,
	BOTTOM,
	BOTH,
	INTERFACE,
	ALL,
	NONE
};

enum class Polarity {
	POSITIVE = 1,
	NEGATIVE
};

enum class WhereMeasured {
	LAMINATE,
	METAL,
	MASK,
	OTHER
};

enum class StackupStatus {
	SPECIFIED,
	PROPOSED,
	APPROVED
};

enum class StepType {
	BOARD,
	ARRAY,
	IC
};

enum class CadPropertyType {
	BOOLEAN,
	DOUBLE,
	INTEGER,
	STRING
};

enum class PlatingStatus {
	PLATED,
	NONPLATED,
	VIA,
	VIA_CAPPED
};

enum class PadUse {
	REGULAR,
	ANTIPAD,
	THERMAL,
	OTHER
};

enum class PackageType {
	AXIAL_LEADED, 
	BARE_DIE, 
	CERAMIC_BGA, 
	CERAMIC_DIP,
	CERAMIC_FLATPACK, 
	CERAMIC_QUAD_FLATPACK,
	CERAMIC_SIP, 
	CHIP, 
	CHIP_SCALE, 
	CHOKE_SWITCH_SM,
	COIL, 
	CONNECTOR_SM, 
	CONNECTOR_TH, 
	EMBEDDED,
	FLIPCHIP, 
	HERMETIC_HYBRID,
	LEADLESS_CERAMIC_CHIP_CARRIER, 
	MCM, 
	MELF,
	FINEPITCH_BGA, 
	MOLDED, 
	NETWORK, 
	PGA, 
	PLASTIC_BGA, 
	PLASTIC_CHIP_CARRIER, 
	PLASTIC_DIP, 
	PLASTIC_SIP,
	POWER_TRANSISTOR, 
	RADIAL_LEADED,
	RECTANGULAR_QUAD_FLATPACK, 
	RELAY_SM, 
	RELAY_TH,
	SOD123, 
	SOIC, 
	SOJ, 
	SOPIC, 
	SOT143, 
	SOT23, 
	SOT52,
	SOT89, 
	SQUARE_QUAD_FLATPACK, 
	SSOIC, 
	SWITCH_TH,
	TANTALUM, 
	TO_TYPE, 
	TRANSFORMER, 
	TRIMPOT_SM,
	TRIMPOT_TH, 
	OTHER
};

enum class PinOneOrientation {
	LOWER_LEFT,
	LEFT,
	LEFT_CENTER,
	UPPER_LEFT,
	UPPER_CENTER,
	UPPER_RIGHT,
	RIGHT,
	RIGHT_CENTER,
	LOWER_RIGHT,
	LOWER_CENTER,
	OTHER
};

enum class MarkingUsage
{
	REFDES,
	PARTNAME,
	TARGET,
	POLARITY_MARKING,
	ATTRIBUTE_GRAPHICS,
	PIN_ONE,
	NONE
};

enum class CadPinType {
	THRU,
	BLIND,
	SURFACE
};

enum class PinElectricalType {
	ELECTRICAL,
	MECHANICAL,
	UNDEFINED
};

enum class PinPolarityType {
	PLUS,
	MINUS,
	ANODE,
	CATHODE
};

enum class PinMountType
{
	SURFACE_MOUNT_PIN,
	SURFACE_MOUNT_PAD,
	THROUGH_HOLE_PIN,
	THROUGH_HOLE_HOLE,
	PRESSFIT,
	NONBOARD,
	HOLE,
	WIRE_BOND,
	UNDEFINED
};

enum class MountType
{
	SMT,
	THMT,
	EMBEDDED,
	PRESSFIT,
	WIRE_BONDED,
	GLUED,
	CLAMPED,
	SOCKETED,
	FORMED,
	OTHER
};

enum class GeometryUsage
{
	THIEVING,
	THERMAL_RELIEF,
	TEXT,
	TEARDROP,
	GRAPHIC,
	NONE
};

enum class HoleShape {
	CIRCLE,
	SQUARE
};

inline LayerFunction GetLayerFunction(std::string layerFunc) {
	if (layerFunc == "ASSEMBLY")
		return LayerFunction::ASSEMBLY;
	else if (layerFunc == "BOARDFAB")
		return LayerFunction::BOARDFAB;
	else if (layerFunc == "BOARD_OUTLINE")
		return LayerFunction::BOARD_OUTLINE;
	else if (layerFunc == "CAPACITIVE")
		return LayerFunction::CAPACITIVE;
	else if (layerFunc == "COATINGCOND")
		return LayerFunction::COATINGCOND;
	else if (layerFunc == "COATINGNONCOND")
		return LayerFunction::COATINGNONCOND;
	else if (layerFunc == "COMPONENT_BOTTOM")
		return LayerFunction::COMPONENT_BOTTOM;
	else if (layerFunc == "COMPONENT_TOP")
		return LayerFunction::COMPONENT_TOP;
	else if (layerFunc == "COMPONENT_EMBEDDED")
		return LayerFunction::COMPONENT_EMBEDDED;
	else if (layerFunc == "COMPONENT_FORMED")
		return LayerFunction::COMPONENT_FORMED;
	else if (layerFunc == "CONDFILM")
		return LayerFunction::CONDFILM;
	else if (layerFunc == "CONDFOIL")
		return LayerFunction::CONDFOIL;
	else if (layerFunc == "CONDUCTIVE_ADHESIVE")
		return LayerFunction::CONDUCTIVE_ADHESIVE;
	else if (layerFunc == "CONDUCTOR")
		return LayerFunction::CONDUCTOR;
	else if (layerFunc == "COURTYARD")
		return LayerFunction::COURTYARD;
	else if (layerFunc == "DIELBASE")
		return LayerFunction::DIELBASE;
	else if (layerFunc == "DIELCORE")
		return LayerFunction::DIELCORE;
	else if (layerFunc == "DIELPREG")
		return LayerFunction::DIELPREG;
	else if (layerFunc == "DIELADHV")
		return LayerFunction::DIELADHV;
	else if (layerFunc == "DIELBONDPLY")
		return LayerFunction::DIELBONDPLY;
	else if (layerFunc == "DIELCOVERLAY")
		return LayerFunction::DIELCOVERLAY;
	else if (layerFunc == "DOCUMENT")
		return LayerFunction::DOCUMENT;
	else if (layerFunc == "DRILL")
		return LayerFunction::DRILL;
	else if (layerFunc == "FIXTURE")
		return LayerFunction::FIXTURE;
	else if (layerFunc == "GLUE")
		return LayerFunction::GLUE;
	else if (layerFunc == "GRAPHIC")
		return LayerFunction::GRAPHIC;
	else if (layerFunc == "HOLEFILL")
		return LayerFunction::HOLEFILL;
	else if (layerFunc == "SOLDERBUMP")
		return LayerFunction::SOLDERBUMP;
	else if (layerFunc == "PASTEMASK")
		return LayerFunction::PASTEMASK;
	else if (layerFunc == "LANDPATTERN")
		return LayerFunction::LANDPATTERN;
	else if (layerFunc == "LEGEND")
		return LayerFunction::LEGEND;
	else if (layerFunc == "MIXED")
		return LayerFunction::MIXED;
	else if (layerFunc == "OTHER")
		return LayerFunction::OTHER;
	else if (layerFunc == "PIN")
		return LayerFunction::PIN;
	else if (layerFunc == "PLANE")
		return LayerFunction::PLANE;
	else if (layerFunc == "PROBE")
		return LayerFunction::PROBE;
	else if (layerFunc == "RESISTIVE")
		return LayerFunction::RESISTIVE;
	else if (layerFunc == "SIGNAL")
		return LayerFunction::SIGNAL;
	else if (layerFunc == "SILKSCREEN")
		return LayerFunction::SILKSCREEN;
	else if (layerFunc == "SOLDERPASTE")
		return LayerFunction::SOLDERPASTE;
	else if (layerFunc == "STACKUP_COMPOSITE")
		return LayerFunction::STACKUP_COMPOSITE;
	else if (layerFunc == "REWORK")
		return LayerFunction::REWORK;
	else if (layerFunc == "ROUT")
		return LayerFunction::ROUT;
	else if (layerFunc == "V_CUT")
		return LayerFunction::V_CUT;
	else if (layerFunc == "EDGE_CHAMFER")
		return LayerFunction::EDGE_CHAMFER;
	else if (layerFunc == "EDGE_PLATING")
		return LayerFunction::EDGE_PLATING;
	else if (layerFunc == "THIEVING_KEEP_INOUT")
		return LayerFunction::THIEVING_KEEP_INOUT;
	else if (layerFunc == "STIFFENER")
		return LayerFunction::STIFFENER;
	return LayerFunction::NONE;
}

inline Side GetSide(std::string side) {
	if (side == "TOP")
		return Side::TOP;
	else if (side == "BOTTOM")
		return Side::BOTTOM;
	else if (side == "BOTH")
		return Side::BOTH;
	else if (side == "INTERFACE")
		return Side::INTERFACE;
	else if (side == "ALL")
		return Side::ALL;
	else
		return Side::NONE;
}

inline Polarity GetPolarity(std::string polarity) {
	if (polarity == "POSITIVE")
		return Polarity::POSITIVE;
	return Polarity::NEGATIVE;
}

inline WhereMeasured GetWhereMeasured(std::string whereMeasured) {
	if (whereMeasured == "LAMINATE")
		return WhereMeasured::LAMINATE;
	else if (whereMeasured == "METAL")
		return WhereMeasured::METAL;
	else if (whereMeasured == "MASK")
		return WhereMeasured::MASK;
	else
		return WhereMeasured::OTHER;
}

inline StackupStatus GetStackupStatus(std::string stackupStatus) {
	if (stackupStatus == "SPECIFIED")
		return StackupStatus::SPECIFIED;
	else if (stackupStatus == "PROPOSED")
		return StackupStatus::PROPOSED;
	else
		return StackupStatus::APPROVED;
}

inline StepType GetStepType(std::string stepType) {
	if (stepType == "BOARD")
		return StepType::BOARD;
	else if (stepType == "ARRAY")
		return StepType::ARRAY;
	else return StepType::IC;
}

inline CadPropertyType GetCadPropertyType(std::string cadPropertyType) {
	if (cadPropertyType == "BOOLEAN")
		return CadPropertyType::BOOLEAN;
	else if (cadPropertyType == "DOUBLE")
		return CadPropertyType::DOUBLE;
	else if (cadPropertyType == "INTEGER")
		return CadPropertyType::INTEGER;
	else 
		return CadPropertyType::STRING;
}

inline PlatingStatus GetPlatingStatus(std::string platingStatus) {
	if (platingStatus == "PLATED")
		return PlatingStatus::PLATED;
	else if (platingStatus == "NONPLATED")
		return PlatingStatus::NONPLATED;
	else if (platingStatus == "VIA")
		return PlatingStatus::VIA;
	else
		return PlatingStatus::VIA_CAPPED;
}

inline PadUse GetPadUse(std::string padUse) {
	if (padUse == "REGULAR")
		return PadUse::REGULAR;
	else if (padUse == "ANTIPAD")
		return PadUse::ANTIPAD;
	else if (padUse == "THERMAL")
		return PadUse::THERMAL;
	else
		return PadUse::OTHER;
}

inline PackageType GetPackageType(const std::string& packageType)
{
	if (packageType == "AXIAL_LEADED")
		return PackageType::AXIAL_LEADED;
	else if (packageType == "BARE_DIE")
		return PackageType::BARE_DIE;
	else if (packageType == "CERAMIC_BGA")
		return PackageType::CERAMIC_BGA;
	else if (packageType == "CERAMIC_DIP")
		return PackageType::CERAMIC_DIP;
	else if (packageType == "CERAMIC_FLATPACK")
		return PackageType::CERAMIC_FLATPACK;
	else if (packageType == "CERAMIC_QUAD_FLATPACK")
		return PackageType::CERAMIC_QUAD_FLATPACK;
	else if (packageType == "CERAMIC_SIP")
		return PackageType::CERAMIC_SIP;
	else if (packageType == "CHIP")
		return PackageType::CHIP;
	else if (packageType == "CHIP_SCALE")
		return PackageType::CHIP_SCALE;
	else if (packageType == "CHOKE_SWITCH_SM")
		return PackageType::CHOKE_SWITCH_SM;
	else if (packageType == "COIL")
		return PackageType::COIL;
	else if (packageType == "CONNECTOR_SM")
		return PackageType::CONNECTOR_SM;
	else if (packageType == "CONNECTOR_TH")
		return PackageType::CONNECTOR_TH;
	else if (packageType == "EMBEDDED")
		return PackageType::EMBEDDED;
	else if (packageType == "FLIPCHIP")
		return PackageType::FLIPCHIP;
	else if (packageType == "HERMETIC_HYBRID")
		return PackageType::HERMETIC_HYBRID;
	else if (packageType == "LEADLESS_CERAMIC_CHIP_CARRIER")
		return PackageType::LEADLESS_CERAMIC_CHIP_CARRIER;
	else if (packageType == "MCM")
		return PackageType::MCM;
	else if (packageType == "MELF")
		return PackageType::MELF;
	else if (packageType == "FINEPITCH_BGA")
		return PackageType::FINEPITCH_BGA;
	else if (packageType == "MOLDED")
		return PackageType::MOLDED;
	else if (packageType == "NETWORK")
		return PackageType::NETWORK;
	else if (packageType == "PGA")
		return PackageType::PGA;
	else if (packageType == "PLASTIC_BGA")
		return PackageType::PLASTIC_BGA;
	else if (packageType == "PLASTIC_CHIP_CARRIER")
		return PackageType::PLASTIC_CHIP_CARRIER;
	else if (packageType == "PLASTIC_DIP")
		return PackageType::PLASTIC_DIP;
	else if (packageType == "PLASTIC_SIP")
		return PackageType::PLASTIC_SIP;
	else if (packageType == "POWER_TRANSISTOR")
		return PackageType::POWER_TRANSISTOR;
	else if (packageType == "RADIAL_LEADED")
		return PackageType::RADIAL_LEADED;
	else if (packageType == "RECTANGULAR_QUAD_FLATPACK")
		return PackageType::RECTANGULAR_QUAD_FLATPACK;
	else if (packageType == "RELAY_SM")
		return PackageType::RELAY_SM;
	else if (packageType == "RELAY_TH")
		return PackageType::RELAY_TH;
	else if (packageType == "SOD123")
		return PackageType::SOD123;
	else if (packageType == "SOIC")
		return PackageType::SOIC;
	else if (packageType == "SOJ")
		return PackageType::SOJ;
	else if (packageType == "SOPIC")
		return PackageType::SOPIC;
	else if (packageType == "SOT143")
		return PackageType::SOT143;
	else if (packageType == "SOT23")
		return PackageType::SOT23;
	else if (packageType == "SOT52")
		return PackageType::SOT52;
	else if (packageType == "SOT89")
		return PackageType::SOT89;
	else if (packageType == "SQUARE_QUAD_FLATPACK")
		return PackageType::SQUARE_QUAD_FLATPACK;
	else if (packageType == "SSOIC")
		return PackageType::SSOIC;
	else if (packageType == "SWITCH_TH")
		return PackageType::SWITCH_TH;
	else if (packageType == "TANTALUM")
		return PackageType::TANTALUM;
	else if (packageType == "TO_TYPE")
		return PackageType::TO_TYPE;
	else if (packageType == "TRANSFORMER")
		return PackageType::TRANSFORMER;
	else if (packageType == "TRIMPOT_SM")
		return PackageType::TRIMPOT_SM;
	else if (packageType == "TRIMPOT_TH")
		return PackageType::TRIMPOT_TH;
	else
		return PackageType::OTHER;
}

inline PinOneOrientation GetPinOneOrientation(const std::string& value)
{
	if (value == "LOWER_LEFT")
		return PinOneOrientation::LOWER_LEFT;
	else if (value == "LEFT")
		return PinOneOrientation::LEFT;
	else if (value == "LEFT_CENTER")
		return PinOneOrientation::LEFT_CENTER;
	else if (value == "UPPER_LEFT")
		return PinOneOrientation::UPPER_LEFT;
	else if (value == "UPPER_CENTER")
		return PinOneOrientation::UPPER_CENTER;
	else if (value == "UPPER_RIGHT")
		return PinOneOrientation::UPPER_RIGHT;
	else if (value == "RIGHT")
		return PinOneOrientation::RIGHT;
	else if (value == "RIGHT_CENTER")
		return PinOneOrientation::RIGHT_CENTER;
	else if (value == "LOWER_RIGHT")
		return PinOneOrientation::LOWER_RIGHT;
	else if (value == "LOWER_CENTER")
		return PinOneOrientation::LOWER_CENTER;
	else
		return PinOneOrientation::OTHER;
}

inline MarkingUsage GetMarkingUsage(const std::string& value)
{
	if (value == "REFDES")
		return MarkingUsage::REFDES;
	else if (value == "PARTNAME")
		return MarkingUsage::PARTNAME;
	else if (value == "TARGET")
		return MarkingUsage::TARGET;
	else if (value == "POLARITY_MARKING")
		return MarkingUsage::POLARITY_MARKING;
	else if (value == "ATTRIBUTE_GRAPHICS")
		return MarkingUsage::ATTRIBUTE_GRAPHICS;
	else if (value == "PIN_ONE")
		return MarkingUsage::PIN_ONE;
	else
		return MarkingUsage::NONE;
}

inline CadPinType GetCadPinType(const std::string& cadPin) {
	if (cadPin == "THRU")
		return CadPinType::THRU;
	else if (cadPin == "BLIND")
		return CadPinType::BLIND;
	else
		return CadPinType::SURFACE;
}

inline PinElectricalType GetPinElectricalType(const std::string& pinElectrical) {
	if (pinElectrical == "ELECTRICAL")
		return PinElectricalType::ELECTRICAL;
	else if (pinElectrical == "MECHANICAL")
		return PinElectricalType::MECHANICAL;
	return
		PinElectricalType::UNDEFINED;
}

inline PinPolarityType GetPinPolarityType(const std::string& pinPolarity) {
	if (pinPolarity == "PLUS")
		return PinPolarityType::PLUS;
	else if (pinPolarity == "MINUS")
		return PinPolarityType::MINUS;
	else if (pinPolarity == "ANODE")
		return PinPolarityType::ANODE;
	return
		PinPolarityType::CATHODE;
}

inline PinMountType GetPinMountType(const std::string& value)
{
	if (value == "SURFACE_MOUNT_PIN")
		return PinMountType::SURFACE_MOUNT_PIN;
	else if (value == "SURFACE_MOUNT_PAD")
		return PinMountType::SURFACE_MOUNT_PAD;
	else if (value == "THROUGH_HOLE_PIN")
		return PinMountType::THROUGH_HOLE_PIN;
	else if (value == "THROUGH_HOLE_HOLE")
		return PinMountType::THROUGH_HOLE_HOLE;
	else if (value == "PRESSFIT")
		return PinMountType::PRESSFIT;
	else if (value == "NONBOARD")
		return PinMountType::NONBOARD;
	else if (value == "HOLE")
		return PinMountType::HOLE;
	else if (value == "WIRE_BOND")
		return PinMountType::WIRE_BOND;
	else
		return PinMountType::UNDEFINED;
}

inline MountType GetMountType(const std::string& value)
{
	if (value == "SMT")
		return MountType::SMT;
	else if (value == "THMT")
		return MountType::THMT;
	else if (value == "EMBEDDED")
		return MountType::EMBEDDED;
	else if (value == "PRESSFIT")
		return MountType::PRESSFIT;
	else if (value == "WIRE_BONDED")
		return MountType::WIRE_BONDED;
	else if (value == "GLUED")
		return MountType::GLUED;
	else if (value == "CLAMPED")
		return MountType::CLAMPED;
	else if (value == "SOCKETED")
		return MountType::SOCKETED;
	else if (value == "FORMED")
		return MountType::FORMED;
	else
		return MountType::OTHER;
}

inline GeometryUsage GetGeometryUsage(const std::string& value)
{
	if (value == "THIEVING")
		return GeometryUsage::THIEVING;
	else if (value == "THERMAL_RELIEF")
		return GeometryUsage::THERMAL_RELIEF;
	else if (value == "TEXT")
		return GeometryUsage::TEXT;
	else if (value == "TEARDROP")
		return GeometryUsage::TEARDROP;
	else if (value == "GRAPHIC")
		return GeometryUsage::GRAPHIC;
	else
		return GeometryUsage::NONE;
}

inline HoleShape GetHoleShape(const std::string value) {
	if (value == "CIRCLE")
		return HoleShape::CIRCLE;
	else
		return HoleShape::SQUARE;
}