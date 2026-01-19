#include "Ecad.hxx"
#include "StandardShape.hxx"

EcadSection::EcadSection(tinyxml2::XMLElement* aEcad, ContentSection* aContent)
	: m_ecad(aEcad),
	  m_content(aContent)
{
	m_standardShape = new StandardShape(m_content);
}

WrongType EcadSection::Read()
{
	// Read CadHeader in Ecad
	auto cadHeader = m_ecad->FirstChildElement("CadHeader");
	UnitsType units;
	std::string unitName = cadHeader->FindAttribute("units")->Value();
	if (unitName == "MILLMETER")
		units = UnitsType::MILLMETER;
	else if (unitName == "MICRON")
		units = UnitsType::MICRON;
	else
		units = UnitsType::INCH;
	for (auto spec = cadHeader->FirstChildElement("Spec"); spec; spec = spec->NextSiblingElement()) {
		ReadSpec(spec);
	}

	for (auto changeRec = cadHeader->FirstChildElement("ChangeRec"); changeRec; changeRec = changeRec->NextSiblingElement()) {
		ReadChangeRec(changeRec);
	}

	auto cadData = m_ecad->FirstChildElement("CadData");
	for (auto layer = cadData->FirstChildElement("Layer"); layer; layer = layer->NextSiblingElement("Layer")) {
		ReadLayer(layer);
	}

	auto stackup = cadData->FirstChildElement("Stackup");
	if (stackup != nullptr)
	ReadStackup(stackup);

	for (auto step = cadData->FirstChildElement("Step"); step; step = step->NextSiblingElement("Step")) {
		ReadStep(step);
	}

	return WrongType();
}

bool EcadSection::IsValid()
{
	return m_ecad != nullptr;
}

bool EcadSection::ReadSpec(tinyxml2::XMLElement* aElement)
{
	Spec spec;

	spec.name = aElement->FindAttribute("name")->Value();
	// Todo Read SpecificationType


	auto xForm = aElement->FirstChildElement("Xform");
	if (xForm != nullptr) {
		spec.xform = new Xform;
		ReadXform(xForm, *spec.xform);
	}

	auto location = aElement->FirstChildElement("Location");
	if (location != nullptr) {
		spec.location = new Location;
		ReadLocation(location, *spec.location);
	}

	auto outline = aElement->FirstChildElement("Outline");
	if (outline != nullptr)
		spec.outline = static_cast<Outline*>(m_standardShape->ReadOutline(outline));

	m_specs.push_back(spec);

	return true;
}

bool EcadSection::ReadChangeRec(tinyxml2::XMLElement* aElement)
{
	ChangeRec changeRec;

	changeRec.dateTime = aElement->FindAttribute("datetime")->Value();
	changeRec.personRef = aElement->FindAttribute("personRef")->Value();
	changeRec.application = aElement->FindAttribute("application")->Value();
	changeRec.change = aElement->FindAttribute("change")->Value();

	auto lifecyclePhase = aElement->FindAttribute("lifecyclePhase");
	if (lifecyclePhase != nullptr)
		changeRec.lifecyclePhase = lifecyclePhase->Value();

	// Todo Read Approval

	m_changeRecs.push_back(changeRec);

	return true;
}

bool EcadSection::ReadLayer(tinyxml2::XMLElement* aLayer)
{
	Layer lay;
	lay.name = aLayer->FindAttribute("name")->Value();
	if (aLayer->FindAttribute("layerFunction")) {
		lay.layerFunction = GetLayerFunction(aLayer->FindAttribute("layerFunction")->Value());
		lay.polarity = GetPolarity(aLayer->FindAttribute("polarity")->Value());
		lay.side = GetSide(aLayer->FindAttribute("side")->Value());
		auto span = aLayer->FirstChildElement("Span");

		for (auto profile = aLayer->FirstChildElement("profile"); profile; profile = profile->NextSiblingElement()) {
			lay.profiles.push_back(m_standardShape->ReadContour(profile));
		}

	}
	m_layers.push_back(lay);
	return true;
}

bool EcadSection::ReadStackup(tinyxml2::XMLElement* aStackupDoc)
{
	Stackup stackup;
	stackup.name = aStackupDoc->FindAttribute("name")->Value();
	stackup.overallThickness = aStackupDoc->FindAttribute("overallThickness")->DoubleValue();
	stackup.tolPlus = aStackupDoc->FindAttribute("tolPlus")->DoubleValue();
	stackup.tolMinus = aStackupDoc->FindAttribute("tolMinus")->DoubleValue();
	stackup.tolPercent = aStackupDoc->FindAttribute("tolPercent") != nullptr ? aStackupDoc->FindAttribute("tolPercent")->BoolValue() : 0;
	stackup.whereMeasured = GetWhereMeasured(aStackupDoc->FindAttribute("whereMeasured")->Value());
	stackup.matDes = aStackupDoc->FindAttribute("matDes") != nullptr ? aStackupDoc->FindAttribute("matDes")->Value() : "";
	stackup.comment = aStackupDoc->FindAttribute("comment") != nullptr ? aStackupDoc->FindAttribute("comment")->Value() : "";
	stackup.stackupStatus = GetStackupStatus(aStackupDoc->FindAttribute("stackupStatus")->Value());

	for (auto stackupGroup = aStackupDoc->FirstChildElement("StackupGroup"); stackupGroup; stackupGroup = stackupGroup->NextSiblingElement("StackupGroup")) {
		stackup.ReadStackupGroup(stackupGroup);
	}

	m_stackups.emplace_back(stackup);
	return true;
}

bool EcadSection::ReadStep(tinyxml2::XMLElement* aStep)
{
	Step step;
	// Read name type value
	step.name = aStep->FindAttribute("name")->Value();
	
	auto type = aStep->FindAttribute("type");
	if (type != nullptr)
		step.stepType = GetStepType(type->Value());

	auto stackupRef = aStep->FindAttribute("stackupRef");
	if (stackupRef != nullptr)
		step.stackupRef = stackupRef->Value();

	// Read NonstandardAttribute
	for (auto nonstandardAttribute = aStep->FirstChildElement("NonstandardAttribute"); nonstandardAttribute; nonstandardAttribute = nonstandardAttribute->NextSiblingElement("NonstandardAttribute")) {
		NonstandardAttribute non;
		ReadNonstandardAttribute(nonstandardAttribute, non);
		step.nonStandardAttributs.emplace_back(non);
	}

	// Read PadStackDef
	for (auto padStackDef = aStep->FirstChildElement("PadStackDef"); padStackDef; padStackDef = padStackDef->NextSiblingElement("PadStackDef")) {
		PadStackDef pad;
		ReadPadStackDef(padStackDef, pad);
		step.padStackDefs.emplace_back(pad);
	}

	// Read Datum
	ReadDatum(aStep->FirstChildElement("Datum"), step.datum);


	// Read Profile(Contour)
	auto profile = aStep->FirstChildElement("Profile");
	if (profile != nullptr)
		step.profile = m_standardShape->ReadContour(profile);

	// Read StepRepeat
	for (auto stepRepeatDoc = aStep->FirstChildElement("StepRepeat"); stepRepeatDoc; stepRepeatDoc = stepRepeatDoc->NextSiblingElement("StepRepeat")) {
		StepRepeat stepRepeat;
		// Read StepRepeat
		ReadStepRepeat(stepRepeatDoc, stepRepeat);
		step.stepRepeats.emplace_back(stepRepeat);
	}

	// Read Package
	for (auto packageDoc = aStep->FirstChildElement("Package"); packageDoc; packageDoc = packageDoc->NextSiblingElement("Package")) {
		Package package;
		// Read Package
		ReadPackage(packageDoc, package);
		step.packages[package.name] = package;
	}

	// Read Component
	for (auto componentDoc = aStep->FirstChildElement("Component"); componentDoc; componentDoc = componentDoc->NextSiblingElement("Component")) {
		Component component;
		// Read Component
		ReadComponent(componentDoc, component);
		step.components.emplace_back(component);
	}

	// Read LogicalNet

	// Read PhyNetGroup

	// Read LayerFeature
	for (auto layerFeatureDoc = aStep->FirstChildElement("LayerFeature"); layerFeatureDoc; layerFeatureDoc = layerFeatureDoc->NextSiblingElement("LayerFeature")) {
		LayerFeature layerFeature;
		// Read LayerFeature
		ReadLayerFeature(layerFeatureDoc, layerFeature);

		step.layerFeatures.emplace_back(layerFeature);
	}

	// Read BendArea

	// Read StackupZone

	// Read Port

	// Read Model

	// Read DfxMeasurementList

	m_steps.emplace_back(step);

	return true;
}

bool Stackup::ReadStackupGroup(tinyxml2::XMLElement* aStackupGroup)
{

	StackupGroup stackupGroup;
	stackupGroup.name = aStackupGroup->FindAttribute("name")->Value();
	stackupGroup.tolPlus = aStackupGroup->FindAttribute("tolPlus")->DoubleValue();
	stackupGroup.tolMinus = aStackupGroup->FindAttribute("tolMinus")->DoubleValue();
	stackupGroup.tolPercent = aStackupGroup->FindAttribute("tolPercent") != nullptr ? aStackupGroup->FindAttribute("tolPercent")->BoolValue() : 0;
	stackupGroup.matDes = aStackupGroup->FindAttribute("matDes") != nullptr ? aStackupGroup->FindAttribute("matDes")->Value() : "";
	stackupGroup.comment = aStackupGroup->FindAttribute("comment") != nullptr ? aStackupGroup->FindAttribute("comment")->Value() : "";

	for (auto stackLayer = aStackupGroup->FirstChildElement("StackupLayer"); stackLayer; stackLayer = stackLayer->NextSiblingElement("StackupLayer")) {
		stackupGroup.ReadStackupLayer(stackLayer);
	}

	this->stackupGroup.emplace_back(stackupGroup);

	return true;
}

bool StackupGroup::ReadStackupLayer(tinyxml2::XMLElement* aStackupLayer)
{

	StackupLayer stackupLayer;
	stackupLayer.name = aStackupLayer->FindAttribute("layerOrGroupRef")->Value();
	stackupLayer.tolPlus = aStackupLayer->FindAttribute("tolPlus")->DoubleValue();
	stackupLayer.tolMinus = aStackupLayer->FindAttribute("tolMinus")->DoubleValue();
	stackupLayer.tolPercent = aStackupLayer->FindAttribute("tolPercent") != nullptr ? aStackupLayer->FindAttribute("tolPercent")->BoolValue() : 0;
	stackupLayer.matDes = aStackupLayer->FindAttribute("matDes") != nullptr ? aStackupLayer->FindAttribute("matDes")->Value() : "";
	stackupLayer.comment = aStackupLayer->FindAttribute("comment") != nullptr ? aStackupLayer->FindAttribute("comment")->Value() : "";
	stackupLayer.sequence = aStackupLayer->FindAttribute("sequence") != nullptr ? aStackupLayer->FindAttribute("sequence")->DoubleValue() : 0;
	
	this->stackupLayers.emplace_back(stackupLayer);

	return true;
}

bool EcadSection::ReadNonstandardAttribute(tinyxml2::XMLElement* aNonstandardAttribute, NonstandardAttribute &nonstandardAttribute)
{
	nonstandardAttribute.name = aNonstandardAttribute->FindAttribute("name")->Value();
	nonstandardAttribute.type = GetCadPropertyType(aNonstandardAttribute->FindAttribute("type")->Value());
	nonstandardAttribute.value = aNonstandardAttribute->FindAttribute("value")->Value();

	return true;
}

bool EcadSection::ReadPadStackDef(tinyxml2::XMLElement* aPadStackDef, PadStackDef &padStackDef)
{
	padStackDef.name = aPadStackDef->FindAttribute("name")->Value();

	// Read PadstackHoleDef
	for (auto padstackHoleDefDoc = aPadStackDef->FirstChildElement("PadstackHoleDef"); padstackHoleDefDoc; padstackHoleDefDoc = padstackHoleDefDoc->NextSiblingElement("PadstackHoleDef")) {
		PadstackHoleDef padstackHoleDef;
		ReadPadStackHoleDef(padstackHoleDefDoc, padstackHoleDef);
		padStackDef.holes.emplace_back(padstackHoleDef);
	}

	// Read PadstackPadDef
	for (auto padstackPadDefDoc = aPadStackDef->FirstChildElement("PadstackPadDef"); padstackPadDefDoc; padstackPadDefDoc = padstackPadDefDoc->NextSiblingElement("PadstackPadDef")) {
		PadstackPadDef padstackPadDef;
		ReadPadStackPadDef(padstackPadDefDoc, padstackPadDef);
		padStackDef.pads.emplace_back(padstackPadDef);
	}

	return true;
}

bool EcadSection::ReadDatum(tinyxml2::XMLElement* aDatum, Datum& datum)
{
	datum.x = aDatum->FindAttribute("x")->DoubleValue();
	datum.y = aDatum->FindAttribute("y")->DoubleValue();

	return true;
}

bool EcadSection::ReadPadStackHoleDef(tinyxml2::XMLElement* aPadStackHoleDefDoc, PadstackHoleDef& padStackHoleDef)
{
	padStackHoleDef.name = aPadStackHoleDefDoc->FindAttribute("name")->Value();
	padStackHoleDef.diameter = aPadStackHoleDefDoc->FindAttribute("diameter")->DoubleValue();
	padStackHoleDef.platingStatus = GetPlatingStatus(aPadStackHoleDefDoc->FindAttribute("platingStatus")->Value());
	padStackHoleDef.plusTol = aPadStackHoleDefDoc->FindAttribute("plusTol")->DoubleValue();
	padStackHoleDef.minusTol = aPadStackHoleDefDoc->FindAttribute("minusTol")->DoubleValue();
	padStackHoleDef.x = aPadStackHoleDefDoc->FindAttribute("x")->DoubleValue();
	padStackHoleDef.y = aPadStackHoleDefDoc->FindAttribute("y")->DoubleValue();

	return true;
}

bool EcadSection::ReadPadStackPadDef(tinyxml2::XMLElement* aPadStackPadDefDoc, PadstackPadDef& padStackPadDef)
{
	padStackPadDef.layerRef = aPadStackPadDefDoc->FindAttribute("layerRef")->Value();
	padStackPadDef.padUse = GetPadUse(aPadStackPadDefDoc->FindAttribute("padUse")->Value());
	padStackPadDef.comment = aPadStackPadDefDoc->FindAttribute("comment") != nullptr ? aPadStackPadDefDoc->FindAttribute("comment")->Value() : "";
	auto xform = aPadStackPadDefDoc->FirstChildElement("Xform");
	if (xform != nullptr)
		ReadXform(xform, padStackPadDef.xform);

	auto locationDoc = aPadStackPadDefDoc->FirstChildElement("Location");
	ReadLocation(locationDoc, padStackPadDef.location);

	// Read Feature
	padStackPadDef.feature = m_standardShape->ReadFeature(locationDoc->NextSiblingElement());

	return true;
}

bool EcadSection::ReadStepRepeat(tinyxml2::XMLElement* aStepRepeat, StepRepeat& stepRepeat)
{
	stepRepeat.x = aStepRepeat->FindAttribute("x")->DoubleValue();
	stepRepeat.y = aStepRepeat->FindAttribute("y")->DoubleValue();
	stepRepeat.nx = aStepRepeat->FindAttribute("nx")->DoubleValue();
	stepRepeat.ny = aStepRepeat->FindAttribute("ny")->DoubleValue();
	stepRepeat.dx = aStepRepeat->FindAttribute("dx")->DoubleValue();
	stepRepeat.dy = aStepRepeat->FindAttribute("dy")->DoubleValue();
	stepRepeat.angle = aStepRepeat->FindAttribute("angle")->DoubleValue();
	stepRepeat.mirror = aStepRepeat->FindAttribute("mirror")->BoolValue();

	return true;
}

bool EcadSection::ReadPackage(tinyxml2::XMLElement* aPackageDoc, Package& package)
{
	package.name = aPackageDoc->FindAttribute("name")->Value();
	package.packageType = GetPackageType(aPackageDoc->FindAttribute("type")->Value());
	package.pinOne = aPackageDoc->FindAttribute("pinOne") != nullptr ? aPackageDoc->FindAttribute("pinOne")->Value() : "";
	package.pinOneOrientation = GetPinOneOrientation(aPackageDoc->FindAttribute("pinOneOrientation")->Value());
	package.height = aPackageDoc->FindAttribute("height") != nullptr ? aPackageDoc->FindAttribute("height")->DoubleValue() : -1;
	package.comment = aPackageDoc->FindAttribute("comment") != nullptr ? aPackageDoc->FindAttribute("comment")->Value() : "";
	package.negativeBodyExtension = aPackageDoc->FindAttribute("negativeBodyExtension") != nullptr ? aPackageDoc->FindAttribute("negativeBodyExtension")->DoubleValue() : -1;
	
	package.outline = static_cast<Outline*>(m_standardShape->ReadOutline(aPackageDoc->FirstChildElement("Outline")));
	
	auto pickupPointDoc = aPackageDoc->FirstChildElement("PickupPoint");
	if (pickupPointDoc != nullptr)
		ReadLocation(pickupPointDoc, package.pickupPoint);

	auto landPatternDoc = aPackageDoc->FirstChildElement("LandPattern");
	if (landPatternDoc != nullptr)
		ReadLandPattern(landPatternDoc, package.landPattern);

	auto silkScreenDoc = aPackageDoc->FirstChildElement("SilkScreen");
	if (silkScreenDoc != nullptr)
		ReadSilkScreen(silkScreenDoc, package.silkStreen);

	auto assemblyDrawingDoc = aPackageDoc->FirstChildElement("AssemblyDrawing");
	if (assemblyDrawingDoc != nullptr)
		ReadAssemblyDrawing(assemblyDrawingDoc, package.assemblyDrawing);

	for (auto pinDoc = aPackageDoc->FirstChildElement("Pin"); pinDoc; pinDoc = pinDoc->NextSiblingElement("Pin")) {
		Pin pin;
		ReadPin(pinDoc, pin);
		package.pins.emplace_back(pin);
	}

	auto topsideDoc = aPackageDoc->FirstChildElement("Topside");
	if (topsideDoc != nullptr)
		ReadTopside(topsideDoc, package.topside);

	auto otherSideView = aPackageDoc->FirstChildElement("OtherSideView");
	if (otherSideView != nullptr)
		ReadOtherSideView(otherSideView, package.otherSideView);

	return false;
}

bool EcadSection::ReadLandPattern(tinyxml2::XMLElement* aLandPattern, LandPattern& landPattern)
{
	for (auto padDoc = aLandPattern->FirstChildElement("Pad"); padDoc; padDoc = padDoc->NextSiblingElement("Pad")) {
		Pad pad;
		ReadPad(padDoc, pad);
		landPattern.pads.emplace_back(pad);
	}

	for (auto targetDoc = aLandPattern->FirstChildElement("TargetDoc"); targetDoc; targetDoc = targetDoc->NextSiblingElement("TargetDoc")) {
		Target target;
		ReadTarget(targetDoc, target);
		landPattern.target.emplace_back(target);
	}

	return false;
}

bool EcadSection::ReadPad(tinyxml2::XMLElement* aPadDoc, Pad& pad)
{
	auto xformDoc = aPadDoc->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, pad.xform);

	ReadLocation(aPadDoc->FirstChildElement("Location"), pad.location);

	if (aPadDoc->FirstChildElement("StandardPrimitiveRef") != nullptr) {
		auto standardPrimitiveRef = aPadDoc->FirstChildElement("StandardPrimitiveRef");
		std::string shapeId = standardPrimitiveRef->FindAttribute("id")->Value();
		pad.feature = m_content->m_standaredPrimitive[shapeId];
	}
	else {
		auto userPrimitiveRef = aPadDoc->FirstChildElement("UserPrimitiveRef");
		std::string shapeId = userPrimitiveRef->FindAttribute("id")->Value();
		pad.feature = m_content->m_userPrimitive[shapeId];
	}

	for (auto pinRefDoc = aPadDoc->FirstChildElement("PinRef"); pinRefDoc; pinRefDoc = pinRefDoc->NextSiblingElement("PinRef")) {
		PinRef pinRef;
		ReadPinRef(pinRefDoc, pinRef);
		pad.pinRefs.push_back(pinRef);
	}


	return true;
}

bool EcadSection::ReadTarget(tinyxml2::XMLElement* aTargetDoc, Target& target)
{
	auto xformDoc = aTargetDoc->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, target.xform);

	ReadLocation(aTargetDoc->FirstChildElement("Location"), target.location);

	auto standardPrimitiveRef = aTargetDoc->FirstChildElement("StandardPrimitiveRef");
	std::string shapeId = standardPrimitiveRef->FindAttribute("id")->Value();
	target.shape = m_content->m_standaredPrimitive[shapeId];

	return true;
}

bool EcadSection::ReadSilkScreen(tinyxml2::XMLElement* aSilkScreenDoc, SilkScreen& silkScreen)
{
	for (auto outlineDoc = aSilkScreenDoc->FirstChildElement("Outline"); outlineDoc; outlineDoc = outlineDoc->NextSiblingElement("Outline"))
		silkScreen.outlines.emplace_back(static_cast<Outline*>(m_standardShape->ReadOutline(outlineDoc)));

	for (auto markingDoc = aSilkScreenDoc->FirstChildElement("Marking"); markingDoc; markingDoc = markingDoc->NextSiblingElement("Marking")) {
		Marking marking;
		ReadMarking(markingDoc, marking);
		silkScreen.markings.emplace_back(marking);
	}

	return false;
}

bool EcadSection::ReadAssemblyDrawing(tinyxml2::XMLElement* aAssemblyDrawingDoc, AssemblyDrawing& assemblyDrawing)
{
	return false;
}

bool EcadSection::ReadPin(tinyxml2::XMLElement* pinDoc, Pin& pin)
{
	pin.number = pinDoc->FindAttribute("number")->Value();
	pin.name = pinDoc->FindAttribute("name") ? pinDoc->FindAttribute("name")->Value() : "";
	pin.cadPin = GetCadPinType(pinDoc->FindAttribute("type")->Value());
	pin.electrical = pinDoc->FindAttribute("electricalType") ? GetPinElectricalType(pinDoc->FindAttribute("electricalType")->Value()) : PinElectricalType::UNDEFINED;
	pin.mountType = pinDoc->FindAttribute("mountType") ? GetPinMountType(pinDoc->FindAttribute("mountType")->Value()) : PinMountType::UNDEFINED;
	pin.pinPolarity = pinDoc->FindAttribute("pinPolarity") ? GetPinPolarityType(pinDoc->FindAttribute("pinPolarity")->Value()) : PinPolarityType::UNDEFINED;

	auto xform = pinDoc->FirstChildElement("Xform");
	if (xform != nullptr)
		ReadXform(xform, pin.xform);

	auto location = pinDoc->FirstChildElement("Location");
	if (location != nullptr)
		ReadLocation(location, pin.location);

	auto standardPrimitive = pinDoc->FirstChildElement("StandardPrimitiveRef");
	std::string id = standardPrimitive->FindAttribute("id")->Value();
	pin.shape = m_content->m_standaredPrimitive[id];

	return false;
}

bool EcadSection::ReadTopside(tinyxml2::XMLElement* topsideDoc, Topside& topside)
{
	return false;
}

bool EcadSection::ReadOtherSideView(tinyxml2::XMLElement* otherSideViewDoc, OtherSideView& otherSideView)
{
	return false;
}

bool EcadSection::ReadMarking(tinyxml2::XMLElement* aMarkingDoc, Marking& marking)
{
	marking.markingUsage = GetMarkingUsage(aMarkingDoc->FindAttribute("markingUsage")->Value());
	
	auto xformDoc = aMarkingDoc->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, marking.xform);

	auto location = aMarkingDoc->FirstChildElement("Location");
	if (location != nullptr)
		ReadLocation(location, marking.location);


	return true;
}

bool EcadSection::ReadComponent(tinyxml2::XMLElement* aComponentDoc, Component& component)
{
	component.refDes = aComponentDoc->FindAttribute("refDes") ? aComponentDoc->FindAttribute("refDes")->Value() : "";
	component.matDes = aComponentDoc->FindAttribute("matDes") ? aComponentDoc->FindAttribute("matDes")->Value() : "";
	component.packageRef = aComponentDoc->FindAttribute("packageRef") ? aComponentDoc->FindAttribute("packageRef")->Value() : "";
	component.part = aComponentDoc->FindAttribute("part")->Value();
	component.layerRef = aComponentDoc->FindAttribute("layerRef")->Value();
	component.mountType = GetMountType(aComponentDoc->FindAttribute("mountType")->Value());
	component.modelRef = aComponentDoc->FindAttribute("modelRef") ? aComponentDoc->FindAttribute("modelRef")->Value() : "";
	component.weight = aComponentDoc->FindAttribute("weight") ? aComponentDoc->FindAttribute("weight")->DoubleValue() : -1;
	component.height = aComponentDoc->FindAttribute("height") ? aComponentDoc->FindAttribute("height")->DoubleValue() : -1;
	component.standoff = aComponentDoc->FindAttribute("standoff") ? aComponentDoc->FindAttribute("standoff")->DoubleValue() : -1;

	for (auto nonstandardAttributeDoc = aComponentDoc->FirstChildElement("NonstandardAttribute"); nonstandardAttributeDoc; nonstandardAttributeDoc = nonstandardAttributeDoc->NextSiblingElement("NonstandardAttribute")) {
		NonstandardAttribute nonstandardAtrribute;
		ReadNonstandardAttribute(nonstandardAttributeDoc, nonstandardAtrribute);
		component.nonstandardAttributes.emplace_back(nonstandardAtrribute);
	}

	auto xformDoc = aComponentDoc->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, component.xform);

	auto locationDoc = aComponentDoc->FirstChildElement("Location");
	if (locationDoc != nullptr)
		ReadLocation(locationDoc, component.location);

	auto slotCavityRefDoc = aComponentDoc->FirstChildElement("SlotCavityRef");
	if (slotCavityRefDoc != nullptr)
		ReadSlotCavityRef(slotCavityRefDoc, component.slotCavityRef);

	for (auto specRefDoc = aComponentDoc->FirstChildElement("SpecRef"); specRefDoc; specRefDoc = specRefDoc->NextSiblingElement("SpecRef")) {
		Spec specRef;
		// Todo Read SpecRef
	}

	return true;
}

bool EcadSection::ReadSlotCavityRef(tinyxml2::XMLElement* aSlotCavityRefDoc, SlotCavityRef& slotCavityRef)
{
	slotCavityRef.id = aSlotCavityRefDoc->FindAttribute("id")->Value();
	return true;
}

bool EcadSection::ReadLayerFeature(tinyxml2::XMLElement* aLayerFeatureDoc, LayerFeature& layerFeature)
{
	layerFeature.layerRef = aLayerFeatureDoc->FindAttribute("layerRef")->Value();

	for (auto setDoc = aLayerFeatureDoc->FirstChildElement("Set"); setDoc; setDoc = setDoc->NextSiblingElement("Set")) {
		Set set;
		ReadSet(setDoc, set);
		layerFeature.sets.emplace_back(set);
	}

	return true;
}

bool EcadSection::ReadSet(tinyxml2::XMLElement* aSetDoc, Set& set)
{
	set.net = aSetDoc->FindAttribute("net") ? aSetDoc->FindAttribute("net")->Value() : "";
	set.netPair = aSetDoc->FindAttribute("netPair") ? aSetDoc->FindAttribute("netPair")->Value() : "";
	set.polarity = aSetDoc->FindAttribute("polarity") ? GetPolarity(aSetDoc->FindAttribute("polarity")->Value()) : Polarity::UNDEFINED;
	set.padUsage = aSetDoc->FindAttribute("padUsage") ? GetPadUsage(aSetDoc->FindAttribute("padUsase")->Value()) : PadUsage::NONE;  
	set.testPoint = aSetDoc->FindAttribute("testPoint") ? aSetDoc->FindAttribute("testPoint")->BoolValue() : false;
	set.geometry = aSetDoc->FindAttribute("geometry") ? aSetDoc->FindAttribute("geometry")->Value() : "";
	set.plate = aSetDoc->FindAttribute("plate") ? aSetDoc->FindAttribute("plate")->BoolValue() : false;
	set.componentRef = aSetDoc->FindAttribute("componentRef") ? aSetDoc->FindAttribute("componentRef")->Value() : "";
	set.geometryUsage = aSetDoc->FindAttribute("geometryUsage") ? GetGeometryUsage(aSetDoc->FindAttribute("geometryUsage")->Value()) : GeometryUsage::NONE;

	for (auto nonstandardAttributeDoc = aSetDoc->FirstChildElement("NonstandardAttribute"); nonstandardAttributeDoc; nonstandardAttributeDoc = nonstandardAttributeDoc->NextSiblingElement("NonstandardAttribute")) {
		NonstandardAttribute nonstandardAttribute;
		ReadNonstandardAttribute(nonstandardAttributeDoc, nonstandardAttribute);
		set.nonstandardAttributes.emplace_back(nonstandardAttribute);
	}

	for (auto padDoc = aSetDoc->FirstChildElement("Pad"); padDoc; padDoc = padDoc->NextSiblingElement("Pad")) {
		Pad pad;
		ReadPad(padDoc, pad);
		set.pads.emplace_back(pad);
	}

	for (auto fiducialDoc = aSetDoc->FirstChildElement("Fiducial"); fiducialDoc; fiducialDoc = fiducialDoc->NextSiblingElement("Fiducial")) {
		// Todo Read Fiducial
		
	}

	for (auto holeDoc = aSetDoc->FirstChildElement("Hole"); holeDoc; holeDoc = holeDoc->NextSiblingElement("Hole")) {
		Hole hole;
		ReadHole(holeDoc, hole);
		set.holes.emplace_back(hole);
	}

	for (auto slotCavityDoc = aSetDoc->FirstChildElement("SlotCavity"); slotCavityDoc; slotCavityDoc = slotCavityDoc->NextSiblingElement("SlotCavity")) {
		SlotCavity slotCavity;
		ReadSlotCavity(slotCavityDoc, slotCavity);
		set.slotCavities.emplace_back(slotCavity);
	}

	for (auto specRefDoc = aSetDoc->FirstChildElement("SpecRef"); specRefDoc; specRefDoc = specRefDoc->NextSiblingElement("SpecRef")) {
		// Todo Read Spec Ref
	}

	for (auto featuresDoc = aSetDoc->FirstChildElement("Features"); featuresDoc; featuresDoc = featuresDoc->NextSiblingElement("Features")) {
		Features features;
		ReadFeatures(featuresDoc, features);
		set.features.emplace_back(features);
	}

	return true;
}

bool EcadSection::ReadHole(tinyxml2::XMLElement* aHoleDoc, Hole& hole)
{
	hole.name = aHoleDoc->FindAttribute("name")->Value();
	hole.type = aHoleDoc->FindAttribute("type") ? GetHoleShape(aHoleDoc->FindAttribute("type")->Value()) : HoleShape::CIRCLE;
	hole.diameter = aHoleDoc->FindAttribute("diameter")->DoubleValue();
	hole.platingStatus = GetPlatingStatus(aHoleDoc->FindAttribute("platingStatus")->Value());
	hole.plusTol = aHoleDoc->FindAttribute("plusTol")->DoubleValue();
	hole.minusTol = aHoleDoc->FindAttribute("minusTol")->DoubleValue();
	hole.x = aHoleDoc->FindAttribute("x")->DoubleValue();
	hole.y = aHoleDoc->FindAttribute("y")->DoubleValue();

	for (auto specRefDoc = aHoleDoc->FirstChildElement("SpecRef"); specRefDoc; specRefDoc = specRefDoc->NextSiblingElement("SpecRef")) {
		// Todo Read Spec Ref
	}

	auto xformDoc = aHoleDoc->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, hole.xform);

	return false;
}

bool EcadSection::ReadSlotCavity(tinyxml2::XMLElement* aSlotCavityDoc, SlotCavity& slotCavity)
{
	return false;
}

bool EcadSection::ReadFeatures(tinyxml2::XMLElement* aFeaturesDoc, Features& features)
{
	auto xformDoc = aFeaturesDoc->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, features.xform);

	auto locationDoc = aFeaturesDoc->FirstChildElement("Location");
	if (locationDoc != nullptr)
		ReadLocation(locationDoc, features.location);

	for (auto shapeDoc = locationDoc->NextSiblingElement(); shapeDoc; shapeDoc = shapeDoc->NextSiblingElement()) {
		features.featureShapes.emplace_back(m_standardShape->ReadFeature(shapeDoc));
	}

	return false;
}