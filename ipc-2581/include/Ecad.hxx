#pragma once
#include "BaseTypeDef.hxx"
#include "Content.hxx"

struct SpecificationType;

struct Spec {
	std::string name;
	SpecificationType* specType;
	Xform *xform;
	Location *location;
	Outline *outline;
};

struct Approval {
	std::string dateTime;
	std::string personRef;
};

struct ChangeRec {
	std::string dateTime;
	std::string personRef;
	std::string lifecyclePhase;
	std::string application;
	std::string change;
	std::vector<Approval> approvals;
};

struct Span {
	std::string fromLayer;
	std::string toLayer;
};

struct Layer {
	std::string name;
	LayerFunction layerFunction;
	Side side;
	Polarity polarity;
	std::vector<Spec*> specs;
	Span span;
	std::vector<Contour*> profiles;
};

struct StackupLayer {
	std::string name;
	double thickness;
	double tolPlus;
	double tolMinus;
	double sequence;
	bool tolPercent;
	std::string matDes;
	std::string comment;
};

struct StackupGroup {
	bool ReadStackupLayer(tinyxml2::XMLElement* aStackupLayer);

	std::string name;
	double thickness;
	double tolPlus;
	double tolMinus;
	bool tolPercent;
	std::string matDes;
	std::string comment;

	std::vector<StackupLayer> stackupLayers;
	std::vector<Spec*> specs;
};

struct Stackup {
	bool ReadStackupGroup(tinyxml2::XMLElement* aStackupGroup);

	std::string name;
	double overallThickness;
	double tolPlus;
	double tolMinus;
	bool tolPercent;
	WhereMeasured whereMeasured;
	std::string matDes;
	std::string comment;
	StackupStatus stackupStatus;
	std::vector<Spec*> specs;
	std::vector<StackupGroup> stackupGroup;

};

struct NonstandardAttribute {
	std::string name;
	CadPropertyType type;
	std::string value;
};

struct PadstackHoleDef {
	std::string name;
	double diameter;
	PlatingStatus platingStatus;
	double plusTol;
	double minusTol;
	double x;
	double y;
};

struct PadstackPadDef {
	std::string layerRef;
	PadUse padUse;
	std::string comment;
	Xform xform;
	Location location;
	Shape* feature;
};

struct PadStackDef {
	std::string name;
	std::vector<PadstackHoleDef> holes;
	std::vector<PadstackPadDef> pads;
};

struct Datum {
	double x, y;
};
 
struct StepRepeat {
	std::string stepRef;
	double x;
	double y;
	double nx;
	double ny;
	double dx;
	double dy;
	double angle;
	bool mirror;
};

struct Pad {
	std::string padstackDefRef;
	Xform xform;
	Location location;
	Shape* feature;
	std::vector<PinRef> pinRefs;
};

struct Target {
	Xform xform;
	Location location;
	Shape* shape;
};

struct LandPattern {
	std::vector<Pad> pads;
	std::vector<Target> target;
};

struct Marking {
	MarkingUsage markingUsage;
	Xform xform;
	Location location;
	std::vector<Shape*> feature;

};

struct SilkScreen {
	std::vector<Outline> outlines;
	std::vector<Marking> markings;
};

struct AssemblyDrawing {
	Outline outline;
	std::vector<Marking> markings;
};

struct Pin {
	std::string number;
	std::string name;
	CadPinType cadPin;
	PinElectricalType electrical;
	PinMountType mountType;
	PinPolarityType pinPolarity;
	Xform xform;
	Location location;
	Shape* shape;
};

struct Topside {
	Outline outline;
	LandPattern landPattern;
	SilkScreen silkScreen;
	AssemblyDrawing assemblyDrawing;
	std::vector<Pin> pins;
};

struct OtherSideView {
	Outline outline;
	SilkScreen silkScreen;
	AssemblyDrawing assemblyDrawing;
};

struct Package {
	std::string name;
	PackageType packageType;
	std::string pinOne;
	PinOneOrientation pinOneOrientation;
	double height;
	double negativeBodyExtension;
	std::string comment;
	Outline outline;
	Location pickupPoint;
	LandPattern landPattern;
	SilkScreen silkStreen;
	AssemblyDrawing assemblyDrawing;
	std::vector<Pin> pins;
	Topside topside;
	OtherSideView otherSideView;

};

struct SlotCavityRef {
	std::string id;
};

struct Component {
	std::string refDes;
	std::string matDes;
	std::string packageRef;
	std::string part;
	std::string layerRef;
	MountType mountType;
	std::string modelRef;
	double weight;
	double height;
	double standoff;
	std::vector<NonstandardAttribute> nonstandardAttributes;
	Xform xform;
	Location location;
	SlotCavityRef slotCavityRef;
	std::vector<Spec> specs;
};

struct Hole {
	std::string name;
	HoleShape type;
	double diameter;
	PlatingStatus platingStatus;
	double plusTol;
	double minusTol;
	double x;
	double y;
	std::vector<Spec> specs;
	Xform xform;
};

struct MaterialCut {
	double depth;
	std::string startCutLayer;
	std::string direction;
	double plusTol;
	double minusTol;
};

struct MaterialLeft {
	double thickness;
	std::string startCutLayer;
	std::string direction;
	double plusTol;
	double minusTol;

};

struct Z_AxisDim {
	MaterialCut materialCut;
	MaterialLeft materialLeft;
};

struct Fill {
	double depthRemaining;
	std::string matDes;
	std::string SpecRef;
};

struct SlotCavity {
	std::string name;
	PlatingStatus platingStatus;
	double plusTol;
	double miniusTol;
	Location location;
	Xform xform;
	Shape* shape;
	Z_AxisDim zAxis;
	Fill fill;
};

struct Features {
	Xform xform;
	Location location;
	std::vector<Shape*> featureShapes;
};

struct NetShort {};

struct Set {
	std::string net;
	std::string netPair;
	Polarity polarity;
	PadUsage padUsage;
	bool testPoint;
	std::string geometry;
	bool plate;
	std::string componentRef;
	GeometryUsage geometryUsage;
	std::vector<NonstandardAttribute> nonstandardAttributes;
	std::vector<Pad> pads;
	std::vector<Hole> holes;
	std::vector<SlotCavity> slotCavities;
	std::vector<Features> features;
	std::vector<LineDesc*> lineDescs;
	std::vector<NetShort> netShort;

};

struct LayerFeature {
	std::string layerRef;
	std::vector<Set> sets;
};

struct Step {
	std::string name;
	StepType stepType;
	std::string stackupRef;
	std::vector<NonstandardAttribute> nonStandardAttributs;
	std::vector<PadStackDef> padStackDefs;
	Datum datum;
	Contour* profile;
	std::vector<StepRepeat> stepRepeats;
	std::vector<Package> packages;
	std::vector<Component> components;
	// Todo LogicalNet, PhyNetGroup
	std::vector<LayerFeature> layerFeatures;
	// Todo BendArea, StackupZone, Port, Model, DfxMeasurementList
};

class EcadSection {
public:
	EcadSection() = default;
	EcadSection(tinyxml2::XMLElement* aEcad, ContentSection* aContent);
	WrongType Read();
	bool IsValid();
private:
	bool ReadSpec(tinyxml2::XMLElement* aEcad);
	bool ReadChangeRec(tinyxml2::XMLElement* aEcad);
	bool ReadLayer(tinyxml2::XMLElement* aLayer);
	bool ReadStackup(tinyxml2::XMLElement* aStackup);
	bool ReadStep(tinyxml2::XMLElement* aStep);
	bool ReadNonstandardAttribute(tinyxml2::XMLElement* aNonstandardAttribute, NonstandardAttribute& nonstandardAttribute);
	bool ReadPadStackDef(tinyxml2::XMLElement* aPadStackDef, PadStackDef &padStackDef);
	bool ReadDatum(tinyxml2::XMLElement* aDaum, Datum& datum);
	bool ReadPadStackHoleDef(tinyxml2::XMLElement* aPadStackHoleDef, PadstackHoleDef& padStackHoleDef);
	bool ReadPadStackPadDef(tinyxml2::XMLElement* aPadStackPadDef, PadstackPadDef& padStackPadDef);
	bool ReadStepRepeat(tinyxml2::XMLElement* aStepRepeatDoc, StepRepeat& stepRepeat);
	bool ReadPackage(tinyxml2::XMLElement* aPackageDoc, Package& package);
	bool ReadLandPattern(tinyxml2::XMLElement* aLandPatternDoc, LandPattern& landPattern);
	bool ReadPad(tinyxml2::XMLElement* aPadDoc, Pad& pad);
	bool ReadTarget(tinyxml2::XMLElement* aTargetDoc, Target& target);
	bool ReadSilkScreen(tinyxml2::XMLElement* aSilkScreenDoc, SilkScreen& silkScreen);
	bool ReadAssemblyDrawing(tinyxml2::XMLElement* aAssemblyDrawingDoc, AssemblyDrawing& assemblyDrawing);
	bool ReadPin(tinyxml2::XMLElement* pinDoc, Pin& pin);
	bool ReadTopside(tinyxml2::XMLElement* topsideDoc, Topside& topside);
	bool ReadOtherSideView(tinyxml2::XMLElement* otherSideViewDoc, OtherSideView& otherSideView);
	bool ReadMarking(tinyxml2::XMLElement* markingDoc, Marking& marking);
	bool ReadComponent(tinyxml2::XMLElement* aComponentDoc, Component& component);
	bool ReadLayerFeature(tinyxml2::XMLElement* aLayerFeatureDoc, LayerFeature& layerFeature);
	bool ReadSlotCavityRef(tinyxml2::XMLElement* aSlotCavityRefDoc, SlotCavityRef& slotCavityRef);
	bool ReadSet(tinyxml2::XMLElement* aSetDoc, Set& set);
	bool ReadHole(tinyxml2::XMLElement* aHoleDoc, Hole& hole);
	bool ReadSlotCavity(tinyxml2::XMLElement* aSlotCavityDoc, SlotCavity& slotCavity);
	bool ReadFeatures(tinyxml2::XMLElement* aFeaturesDoc, Features& features);
private:
	tinyxml2::XMLElement* m_ecad;
	ContentSection* m_content;
	StandardShape*  m_standardShape;
public:
	UnitsType				m_unit;
	std::vector<Spec>		m_specs;
	std::vector<ChangeRec>	m_changeRecs;
	std::vector<Layer>		m_layers;
	std::vector<Stackup>	m_stackups;
	std::vector<Step>		m_steps;
};