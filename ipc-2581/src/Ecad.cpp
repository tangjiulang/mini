#include "Ecad.hxx"

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
	if (outline != nullptr) {
		spec.outline = new Outline;
		ReadOutline(outline, *spec.outline);
	}

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
			Contour prof;
			m_standardShape->ReadContour(profile, prof);
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
		m_standardShape->ReadContour(profile, step.profile);

	// Read StepRepeat
	for (auto stepRepeatDoc = aStep->FirstChildElement("StepRepeat"); stepRepeatDoc; stepRepeatDoc = stepRepeatDoc->NextSiblingElement("StepRepeat")) {
		StepRepeat stepRepeat;
		// Read StepRepeat

		step.stepRepeats.emplace_back(stepRepeat);
	}

	// Read Package
	for (auto packageDoc = aStep->FirstChildElement("Package"); packageDoc; packageDoc = packageDoc->NextSiblingElement("Package")) {
		Package package;
		// Read Package

		step.packages.emplace_back(package);
	}

	// Read Component
	for (auto componentDoc = aStep->FirstChildElement("Component"); componentDoc; componentDoc = componentDoc->NextSiblingElement("Component")) {
		Component component;
		// Read Component

		step.components.emplace_back(component);
	}

	// Read LogicalNet

	// Read PhyNetGroup

	// Read LayerFeature
	for (auto layerFeatureDoc = aStep->FirstChildElement("LayerFeature"); layerFeatureDoc; layerFeatureDoc = layerFeatureDoc->NextSiblingElement("LayerFeature")) {
		LayerFeature layerFeature;
		// Read LayerFeature

		step.layerFeatures.emplace_back(layerFeature);
	}

	// Read BendArea

	// Read StackupZone

	// Read Port

	// Read Model

	// Read DfxMeasurementList

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

	ReadLocation(aPadStackPadDefDoc->FirstChildElement("Location"), padStackPadDef.location);

	// Read Feature

	return true;
}