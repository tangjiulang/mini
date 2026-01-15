#include "BaseElement.hxx"

bool ReadXform(tinyxml2::XMLElement* aElement, Xform& xForm)
{
	auto xOffset = aElement->FindAttribute("xOffset");
	if (xOffset != nullptr)
		xForm.xOffset = xOffset->DoubleValue();

	auto yOffset = aElement->FindAttribute("yOffset");
	if (yOffset != nullptr)
		xForm.yOffset = yOffset->DoubleValue();

	auto rotation = aElement->FindAttribute("rotation");
	if (rotation != nullptr)
		xForm.rotation = rotation->DoubleValue();

	auto scale = aElement->FindAttribute("scale");
	if (scale != nullptr)
		xForm.scale = scale->DoubleValue();

	auto mirror = aElement->FindAttribute("mirror");
	if (mirror != nullptr)
		xForm.mirror = aElement->BoolAttribute("mirror");

	auto faceUp = aElement->FindAttribute("faceUp");
	if (faceUp != nullptr)
		xForm.faceUp = aElement->BoolAttribute("faceUp");

	return false;
}

bool ReadLineDesc(tinyxml2::XMLElement* aElement, LineDesc* lineDesc) {
	lineDesc->lineWidth = aElement->FindAttribute("lineWidth")->DoubleValue();

	std::string endTypeStr = aElement->FindAttribute("lineEnd")->Value();
	if (endTypeStr == "ROUND") {
		lineDesc->lineEndType = LineEndType::ROUND;
	}
	else if (endTypeStr == "SQUARE") {
		lineDesc->lineEndType = LineEndType::SQUARE;
	}
	else {
		lineDesc->lineEndType = LineEndType::NONE;
	}
	return true;
}

bool ReadLine(tinyxml2::XMLElement* aElement, Line& line) {
	line.startX = aElement->FindAttribute("startX")->DoubleValue();
	line.startY = aElement->FindAttribute("startY")->DoubleValue();
	line.endX = aElement->FindAttribute("endX")->DoubleValue();
	line.endY = aElement->FindAttribute("endY")->DoubleValue();

	tinyxml2::XMLElement* lineDesc = nullptr;
	if ((lineDesc = aElement->FirstChildElement("LineDesc")) == nullptr) {
		lineDesc = aElement->FirstChildElement("LineDescRef");
		auto id = lineDesc->FindAttribute("id")->Value();

		// todo find lineDesc by id from lineDescPreDefinitions
	}
	else {
		line.lineDesc = new LineDesc();
		ReadLineDesc(lineDesc, line.lineDesc);
	}

	return true;
}

bool ReadLinePreDef(tinyxml2::XMLElement* aElement, LineDescPreDef& lineDescPreDef) {
	lineDescPreDef.id = aElement->FindAttribute("id")->Value();
	ReadLineDesc(aElement->FirstChildElement("LineDesc"), &lineDescPreDef.lineDesc);

	return true;
}

bool ReadArc(tinyxml2::XMLElement* aElement, Arc& arc) {
	arc.startX = aElement->FindAttribute("startX")->DoubleValue();
	arc.startY = aElement->FindAttribute("startY")->DoubleValue();
	arc.endX = aElement->FindAttribute("endX")->DoubleValue();
	arc.endY = aElement->FindAttribute("endY")->DoubleValue();
	arc.centerX = aElement->FindAttribute("centerX")->DoubleValue();
	arc.centerY = aElement->FindAttribute("centerY")->DoubleValue();
	arc.clockwise = aElement->FindAttribute("clockwise")->DoubleValue();
	tinyxml2::XMLElement* lineDesc = nullptr;
	if ((lineDesc = aElement->FirstChildElement("LineDesc")) == nullptr) {
		lineDesc = aElement->FirstChildElement("LineDescRef");
		auto id = lineDesc->FindAttribute("id")->Value();
		// todo find lineDesc by id from lineDescPreDefinitions
	}
	else {
		arc.lineDesc = new LineDesc();
		ReadLineDesc(lineDesc, arc.lineDesc);
	}
	return 1;
}

bool ReadFillDesc(tinyxml2::XMLElement* aElement, FillDesc* fillDesc) {
	std::string fillPropertyType = aElement->FindAttribute("fillProperty")->Value();
	if (fillPropertyType == "HOLLOW")
		fillDesc->fillType = FillPropertyType::HOLLOW;
	else if (fillPropertyType == "HATCH")
		fillDesc->fillType = FillPropertyType::HATCH;
	else if (fillPropertyType == "MESH")
		fillDesc->fillType = FillPropertyType::MESH;
	else if (fillPropertyType == "FILL")
		fillDesc->fillType = FillPropertyType::FILL;
	else
		fillDesc->fillType = FillPropertyType::VOID;

	auto lineWidth = aElement->FindAttribute("lineWidth");
	if (lineWidth != nullptr)
		fillDesc->lineWidth = lineWidth->DoubleValue();

	auto pitch1 = aElement->FindAttribute("pitch1");
	if (pitch1 != nullptr)
		fillDesc->pitch1 = pitch1->DoubleValue();

	auto pitch2 = aElement->FindAttribute("pitch2");
	if (pitch2 != nullptr)
		fillDesc->pitch2 = pitch2->DoubleValue();

	auto angle1 = aElement->FindAttribute("angle1");
	if (angle1 != nullptr)
		fillDesc->angle1 = angle1->DoubleValue();

	auto angle2 = aElement->FindAttribute("angle2");
	if (angle2 != nullptr)
		fillDesc->angle2 = angle2->DoubleValue();

	// Todo ReadColorGroup

	return true;
}

bool ReadFillPreDef(tinyxml2::XMLElement* aElement, FillDescPreDef& fillDescPreDef) {
	fillDescPreDef.id = aElement->FindAttribute("id")->Value();

	ReadFillDesc(aElement->FirstChildElement(), &fillDescPreDef.fillDesc);
	return true;
}

bool ReadLineDescHelper(tinyxml2::XMLElement* aElement, std::unordered_map<std::string, LineDesc>& lineDescPreDef, LineDesc** lineDesc) {
	if (aElement->FirstChildElement("LineDesc") == nullptr && aElement->FirstChildElement("LineDescRef") == nullptr)
		return false;

	auto lineDescElement = aElement->FirstChildElement("LineDesc");
	if (lineDescElement == nullptr) {
		lineDescElement = aElement->FirstChildElement("LineDescRef");
		auto id = lineDescElement->FindAttribute("id")->Value();

		if (lineDescPreDef.count(id))
			*lineDesc = &lineDescPreDef[id];
	}
	else {
		*lineDesc = new LineDesc;
		ReadLineDesc(lineDescElement, *lineDesc);
	}

	return true;
}

bool ReadFillDescHelper(tinyxml2::XMLElement* aElement, std::unordered_map<std::string, FillDesc>& fillDescPreDef, FillDesc** fillDesc) {
	if (aElement->FirstChildElement("FillDesc") == nullptr && aElement->FirstChildElement("FillDescRef") == nullptr)
		return false;

	auto fillDescElement = aElement->FirstChildElement("FillDesc");
	if (fillDescElement == nullptr) {
		fillDescElement = aElement->FirstChildElement("FillDescRef");
		auto id = fillDescElement->FindAttribute("id")->Value();

		if (fillDescPreDef.count(id))
			*fillDesc = &fillDescPreDef[id];
	}
	else {
		*fillDesc = new FillDesc;
		ReadFillDesc(fillDescElement, *fillDesc);
	}

	return true;
}

bool ReadOutline(tinyxml2::XMLElement* aElement, Outline& outline) {
	return true;
}

bool ReadLocation(tinyxml2::XMLElement* aElement, Location& location)
{
	location.x = aElement->FindAttribute("x")->DoubleValue();
	location.y = aElement->FindAttribute("y")->DoubleValue();
	return true;
}

bool ReadPinRef(tinyxml2::XMLElement* aElement, PinRef& pinRef)
{
	pinRef.componentRef = aElement->FindAttribute("componentRef")->Value();
	pinRef.pin = aElement->FindAttribute("pin")->Value();
	pinRef.title = aElement->FindAttribute("title")->Value();
	return true;
}
