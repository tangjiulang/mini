#include "StandardShape.hxx"
#include "Content.hxx"

StandardShape::StandardShape(){}

Shape StandardShape::ReadSimple(tinyxml2::XMLElement* aElement)
{
	SimpleType type = GetSimpleType(aElement->Name());
	if (type == SimpleType::Arc) {
		return ReadArc(aElement);
	}
	else if (type == SimpleType::Line) {
		return ReadLine(aElement);
	}
	else if (type == SimpleType::Outline) {
		return ReadOutline(aElement);
	}
	else if (type == SimpleType::Polyline) {
		return ReadPolyline(aElement);
	}
	else 
		return Shape{ ShapeType::Other, -1 };
}

void StandardShape::SetContent(ContentSection* aContent)
{
	m_content = aContent;
}

Shape StandardShape::ReadStandard(tinyxml2::XMLElement* aElement)
{
	StandardType type = GetStandardType(aElement->Name());
	if (type == StandardType::BUTTERFLY)
		return ReadButterfly(aElement);
	else if (type == StandardType::CIRCLE)
		return ReadCircle(aElement);
	else if (type == StandardType::CONTOUR)
		return ReadContour(aElement);
	else if (type == StandardType::DIAMOND)
		return ReadDiamond(aElement);
	else if (type == StandardType::DONUT)
		return ReadDonut(aElement);
	else if (type == StandardType::ELLIPSE)
		return ReadEllipse(aElement);
	else if (type == StandardType::HEXAGON)
		return ReadHexagon(aElement);
	else if (type == StandardType::MOIRE)
		return ReadMoire(aElement);
	else if (type == StandardType::OCTAGON)
		return ReadOctagon(aElement);
	else if (type == StandardType::OVAL)
		return ReadOval(aElement);
	else if (type == StandardType::RECTCENTER)
		return ReadRectCenter(aElement);
	else if (type == StandardType::RECTCHAM)
		return ReadRectCham(aElement);
	else if (type == StandardType::RECTCORNER)
		return ReadRectCorner(aElement);
	else if (type == StandardType::RECTROUND)
		return ReadRectRound(aElement);
	else if (type == StandardType::THERMAL)
		return ReadThermal(aElement);
	else if (type == StandardType::TRIANGLE)
		return ReadTriangle(aElement);
		
	return Shape{ ShapeType::Other, -1, StandardType::OTHER };   // OTHER / 不支持
}

// Simple Begin
Shape StandardShape::ReadLine(tinyxml2::XMLElement* aElement) {
	m_line.push_back(Line{});
	auto& line = m_line.back();
	line.startX = aElement->FindAttribute("startX")->DoubleValue();
	line.startY = aElement->FindAttribute("startY")->DoubleValue();
	line.endX = aElement->FindAttribute("endX")->DoubleValue();
	line.endY = aElement->FindAttribute("endY")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &line.lineDesc);

	return Shape{ ShapeType::Simple, static_cast<int32_t>(m_line.size() - 1), SimpleType::Line };
}

Shape StandardShape::ReadArc(tinyxml2::XMLElement* aElement) {
	m_arc.push_back(Arc{});
	auto& arc = m_arc.back();
	
	arc.startX = aElement->FindAttribute("startX")->DoubleValue();
	arc.startY = aElement->FindAttribute("startY")->DoubleValue();
	arc.endX = aElement->FindAttribute("endX")->DoubleValue();
	arc.endY = aElement->FindAttribute("endY")->DoubleValue();
	arc.centerX = aElement->FindAttribute("centerX")->DoubleValue();
	arc.centerY = aElement->FindAttribute("centerY")->DoubleValue();
	arc.clockwise = aElement->FindAttribute("clockwise")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &arc.lineDesc);
	return Shape{ ShapeType::Simple, static_cast<int32_t>(m_arc.size() - 1), SimpleType::Arc };
}

bool StandardShape::ReadPolyBegin(tinyxml2::XMLElement* aElement, PolyBegin& poly)
{
	poly.x = aElement->FindAttribute("x")->DoubleValue();
	poly.y = aElement->FindAttribute("y")->DoubleValue();
	
	return true;
}

bool StandardShape::ReadPolyStepCurve(tinyxml2::XMLElement* aElement, PolyStepCurve& polyStepCurve)
{
	polyStepCurve.x = aElement->FindAttribute("x")->DoubleValue();
	polyStepCurve.y = aElement->FindAttribute("y")->DoubleValue();
	polyStepCurve.centerX = aElement->FindAttribute("centerX")->DoubleValue();
	polyStepCurve.centerY = aElement->FindAttribute("centerY")->DoubleValue();
	polyStepCurve.clockwise = aElement->FindAttribute("clockwise")->BoolValue();
	
	return true;
}

bool StandardShape::ReadPolyStepSegment(tinyxml2::XMLElement* aElement, PolyStepSegment& polyStepSegment)
{
	polyStepSegment.x = aElement->FindAttribute("x")->DoubleValue();
	polyStepSegment.y = aElement->FindAttribute("y")->DoubleValue();

	return true;
}

bool StandardShape::ReadPolygon(tinyxml2::XMLElement* aElement, Polygon& polygon)
{
	ReadPolyBegin(aElement->FirstChildElement("PolyBegin"), polygon.polyBegin);

	for (auto stepElemDoc = aElement->FirstChildElement()->NextSiblingElement(); stepElemDoc && (stepElemDoc->Name() == std::string("PolyStepCurve") || stepElemDoc->Name() == std::string("PolyStepSegment")); stepElemDoc = stepElemDoc->NextSiblingElement()) {
		if (stepElemDoc->Name() == std::string("PolyStepCurve")) {
			PolyStepCurve polyStepCurve;
			ReadPolyStepCurve(stepElemDoc, polyStepCurve);
			polygon.polyStep.emplace_back(polyStepCurve);
		}
		else if (stepElemDoc->Name() == std::string("PolyStepSegment")) {
			PolyStepSegment polyStepSegment;
			ReadPolyStepSegment(stepElemDoc, polyStepSegment);
			polygon.polyStep.emplace_back(polyStepSegment);
		}
	}

	auto xformDoc = aElement->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, polygon.xform);

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &polygon.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &polygon.fillDesc);

	return true;
}

Shape StandardShape::ReadPolyline(tinyxml2::XMLElement* aElement)
{
	m_polyline.push_back(Polyline{});
	auto& polyline = m_polyline.back();

	ReadPolyBegin(aElement->FirstChildElement("PolyBegin"), polyline.polyBegin);

	for (auto stepElemDoc = aElement->FirstChildElement()->NextSiblingElement(); stepElemDoc && (stepElemDoc->Name() == std::string("PolyStepCurve") || stepElemDoc->Name() == std::string("PolyStepSegment")); stepElemDoc = stepElemDoc->NextSiblingElement()) {
		if (stepElemDoc->Name() == std::string("PolyStepCurve")) {
			PolyStepCurve polyStepCurve;
			ReadPolyStepCurve(stepElemDoc, polyStepCurve);
			polyline.polyStep.emplace_back(polyStepCurve);
		}
		else if (stepElemDoc->Name() == std::string("PolyStepSegment")) {
			PolyStepSegment polyStepSegment;
			ReadPolyStepSegment(stepElemDoc, polyStepSegment);
			polyline.polyStep.emplace_back(polyStepSegment);
		}
	}

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &polyline.lineDesc);

	return Shape{ ShapeType::Simple, static_cast<int32_t>(m_polyline.size() - 1), SimpleType::Polyline };
}

bool StandardShape::ReadCutout(tinyxml2::XMLElement* aElement, Cutout& cutout)
{
	ReadPolyBegin(aElement->FirstChildElement("PolyBegin"), cutout.polyBegin);

	for (auto stepElemDoc = aElement->FirstChildElement()->NextSiblingElement(); stepElemDoc && (stepElemDoc->Name() == std::string("PolyStepCurve") || stepElemDoc->Name() == std::string("PolyStepSegment")); stepElemDoc = stepElemDoc->NextSiblingElement()) {
		if (stepElemDoc->Name() == std::string("PolyStepCurve")) {
			PolyStepCurve polyStepCurve;
			ReadPolyStepCurve(stepElemDoc, polyStepCurve);
			cutout.polyStep.emplace_back(polyStepCurve);
		}
		else if (stepElemDoc->Name() == std::string("PolyStepSegment")) {
			PolyStepSegment polyStepSegment;
			ReadPolyStepSegment(stepElemDoc, polyStepSegment);
			cutout.polyStep.emplace_back(polyStepSegment);
		}
	}

	auto xformDoc = aElement->FirstChildElement("Xform");
	if (xformDoc != nullptr)
		ReadXform(xformDoc, cutout.xform);

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &cutout.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &cutout.fillDesc);

	return true;
}

Shape StandardShape::ReadOutline(tinyxml2::XMLElement* aElement) {
	m_outline.push_back(Outline{});
	auto& outline = m_outline.back();

	ReadPolygon(aElement->FirstChildElement("Polygon"), outline.polygon);
	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &outline.lineDesc);

	return Shape{ ShapeType::Simple, static_cast<int32_t>(m_outline.size() - 1), SimpleType::Outline };
}
// Simple End


Shape StandardShape::ReadButterfly(tinyxml2::XMLElement* aElement) {
	m_butterfly.resize(m_butterfly.size() + 1);
	Butterfly& butterfly = m_butterfly.back();
	butterfly.shape = aElement->FindAttribute("shape")->Value();
	std::string exp = butterfly.shape == "ROUND" ? "diameter" : "side";
	butterfly.expand = aElement->FindAttribute(exp.data())->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &butterfly.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &butterfly.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_butterfly.size() - 1), StandardType::BUTTERFLY };
}


Shape StandardShape::ReadCircle(tinyxml2::XMLElement* aElement) {
	m_circle.resize(m_circle.size() + 1);
	Circle& circle = m_circle.back();
	circle.diameter = aElement->FindAttribute("diameter")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &circle.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &circle.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_circle.size() - 1), StandardType::CIRCLE };
}

Shape StandardShape::ReadContour(tinyxml2::XMLElement* aElement)
{
	m_contour.resize(m_contour.size() + 1);
	auto& contour = m_contour.back();
	ReadPolygon(aElement->FirstChildElement("Polygon"), contour.polygon);

	for (auto cutoutDoc = aElement->FirstChildElement("Cutout"); cutoutDoc != nullptr; cutoutDoc = cutoutDoc->NextSiblingElement("Cutout")) {
		Cutout cutout;
		ReadCutout(cutoutDoc, cutout);
		contour.cutouts.push_back(cutout);
	}

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_contour.size() - 1), StandardType::CONTOUR };
}


Shape StandardShape::ReadRectCenter(tinyxml2::XMLElement* aElement) {
	m_rectCenter.resize(m_rectCenter.size() + 1);
	auto& rectCenter = m_rectCenter.back();
	rectCenter.height = aElement->FindAttribute("height")->DoubleValue();
	rectCenter.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &rectCenter.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &rectCenter.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_rectCenter.size() - 1), StandardType::RECTCENTER };
}

Shape StandardShape::ReadDiamond(tinyxml2::XMLElement* aElement)
{
	m_diamond.push_back(Diamond{});
	auto& diamond = m_diamond.back();

	diamond.height = aElement->FindAttribute("height")->DoubleValue();
	diamond.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &diamond.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &diamond.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_diamond.size() - 1), StandardType::DIAMOND };
}

Shape StandardShape::ReadDonut(tinyxml2::XMLElement* aElement)
{
	m_donut.push_back(Donut{});
	auto& donut = m_donut.back();

	donut.shape = GetDonutShape(aElement->FindAttribute("shape")->Value());
	donut.outerDiameter = aElement->FindAttribute("outerDiameter")->DoubleValue();
	donut.innerDiameter = aElement->FindAttribute("innerDiameter")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &donut.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &donut.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_donut.size() - 1), StandardType::DONUT };
}

Shape StandardShape::ReadEllipse(tinyxml2::XMLElement* aElement)
{
	m_ellipse.push_back(Ellipse{});
	auto& ellipse = m_ellipse.back();

	ellipse.height = aElement->FindAttribute("height")->DoubleValue();
	ellipse.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &ellipse.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &ellipse.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_ellipse.size() - 1), StandardType::ELLIPSE };
}

Shape StandardShape::ReadHexagon(tinyxml2::XMLElement* aElement)
{
	m_hexagon.push_back(Hexagon{});
	auto hexagon = m_hexagon.back();

	hexagon.length = aElement->FindAttribute("length")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &hexagon.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &hexagon.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_hexagon.size() - 1), StandardType::HEXAGON };
}

Shape StandardShape::ReadMoire(tinyxml2::XMLElement* aElement)
{
	m_moire.push_back(Moire{});
	auto& moire = m_moire.back();
	
	moire.diameter = aElement->FindAttribute("diameter")->DoubleValue();
	moire.ringWidth = aElement->FindAttribute("ringWidth")->DoubleValue();
	moire.ringGap = aElement->FindAttribute("ringGap")->DoubleValue();
	moire.ringNumber = aElement->FindAttribute("ringNumber")->IntValue();

	moire.lineWidth = aElement->FindAttribute("lineWidth") ? aElement->FindAttribute("lineWidth")->DoubleValue() : -1;
	moire.lineLength = aElement->FindAttribute("lineLength") ? aElement->FindAttribute("lineLength")->DoubleValue() : -1;
	moire.lineAngle = aElement->FindAttribute("lineAngle") ? aElement->FindAttribute("lineAngle")->DoubleValue() : -1;

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_moire.size() - 1), StandardType::MOIRE };
}

Shape StandardShape::ReadOctagon(tinyxml2::XMLElement* aElement)
{
	m_octagon.push_back(Octagon{});
	auto octagon = m_octagon.back();

	octagon.length = aElement->FindAttribute("length")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &octagon.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &octagon.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_octagon.size() - 1), StandardType::OCTAGON };
}

Shape StandardShape::ReadOval(tinyxml2::XMLElement* aElement)
{
	m_oval.push_back(Oval{});
	auto& oval = m_oval.back();

	oval.height = aElement->FindAttribute("height")->DoubleValue();
	oval.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &oval.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &oval.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_oval.size() - 1), StandardType::OVAL };
}

Shape StandardShape::ReadRectCham(tinyxml2::XMLElement* aElement)
{
	m_rectCham.push_back(RectCham{});
	auto& rectCham = m_rectCham.back();

	rectCham.width = aElement->FindAttribute("width")->DoubleValue();
	rectCham.height = aElement->FindAttribute("height")->DoubleValue();
	rectCham.chamfer = aElement->FindAttribute("chamfer")->DoubleValue();
	rectCham.upperRight = aElement->FindAttribute("upperRight") ? aElement->FindAttribute("upperRight")->BoolValue() : false;
	rectCham.upperLeft = aElement->FindAttribute("upperLeft") ? aElement->FindAttribute("upperLeft")->BoolValue() : false;
	rectCham.lowerLeft = aElement->FindAttribute("lowerLeft") ? aElement->FindAttribute("lowerLeft")->BoolValue() : false;
	rectCham.lowerRight = aElement->FindAttribute("lowerRight") ? aElement->FindAttribute("lowerRight")->BoolValue() : false;

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &rectCham.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &rectCham.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_rectCham.size() - 1), StandardType::RECTCHAM };
}

Shape StandardShape::ReadRectCorner(tinyxml2::XMLElement* aElement)
{
	m_rectCorner.push_back(RectCorner{});
	auto& rectCorner = m_rectCorner.back();

	rectCorner.lowerLeftX = aElement->FindAttribute("lowerLeftX")->DoubleValue();
	rectCorner.lowerLeftY = aElement->FindAttribute("lowerLeftY")->DoubleValue();
	rectCorner.upperRightX = aElement->FindAttribute("upperRightX")->DoubleValue();
	rectCorner.upperRightY = aElement->FindAttribute("upperRightY")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &rectCorner.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &rectCorner.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_rectCorner.size() - 1), StandardType::RECTCORNER };
}

Shape StandardShape::ReadRectRound(tinyxml2::XMLElement* aElement)
{
	m_rectRound.push_back(RectRound{});
	auto& rectRound = m_rectRound.back();

	rectRound.width = aElement->FindAttribute("width")->DoubleValue();
	rectRound.height = aElement->FindAttribute("height")->DoubleValue();
	rectRound.radius = aElement->FindAttribute("chamfer")->DoubleValue();
	rectRound.upperRight = aElement->FindAttribute("upperRight") ? aElement->FindAttribute("upperRight")->BoolValue() : false;
	rectRound.upperLeft = aElement->FindAttribute("upperLeft") ? aElement->FindAttribute("upperLeft")->BoolValue() : false;
	rectRound.lowerLeft = aElement->FindAttribute("lowerLeft") ? aElement->FindAttribute("lowerLeft")->BoolValue() : false;
	rectRound.lowerRight = aElement->FindAttribute("lowerRight") ? aElement->FindAttribute("lowerRight")->BoolValue() : false;

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &rectRound.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &rectRound.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_rectRound.size() - 1), StandardType::RECTROUND };
}

Shape StandardShape::ReadThermal(tinyxml2::XMLElement* aElement)
{
	m_thermal.push_back(Thermal{});
	auto& thermal = m_thermal.back();

	thermal.shape = GetThermalShape(aElement->FindAttribute("shape")->Value());
	thermal.outerDiameter = aElement->FindAttribute("outerDiameter")->DoubleValue();
	thermal.innerDiameter = aElement->FindAttribute("innerDiameter")->DoubleValue();
	thermal.spockCount = aElement->FindAttribute("spockCount") ? aElement->FindAttribute("spockCount")->IntValue() : -1;
	thermal.spokeWidth = aElement->FindAttribute("spockWidth") ? aElement->FindAttribute("spockWidth")->DoubleValue() : -1;
	thermal.spokeStartAngle = aElement->FindAttribute("spockStartAngle")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &thermal.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &thermal.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_thermal.size() - 1), StandardType::THERMAL };
}

Shape StandardShape::ReadTriangle(tinyxml2::XMLElement * aElement)
{
	m_triangle.push_back(Triangle{});
	auto& triangle = m_triangle.back();

	triangle.base = aElement->FindAttribute("base")->DoubleValue();
	triangle.height = aElement->FindAttribute("height")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &triangle.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &triangle.fillDesc);

	return Shape{ ShapeType::Standard, static_cast<int32_t>(m_triangle.size() - 1), StandardType::TRIANGLE };
}

Shape StandardShape::ReadFeature(tinyxml2::XMLElement* aElement)
{
	if (aElement->Name() == std::string("StandardPrimitiveRef")) {
		std::string ref = aElement->FindAttribute("id")->Value();
		auto it = m_content->m_standaredPrimitive.find(ref);
		if (it != m_content->m_standaredPrimitive.end())
			return it->second;
	} 
	else if (aElement->Name() == std::string("UserPrimitiveRef")) {
		std::string ref = aElement->FindAttribute("id")->Value();
		auto it = m_content->m_userPrimitive.find(ref);
		if (it != m_content->m_userPrimitive.end())
			return it->second;
	}
	else if (GetStandardType(aElement->Name()) != StandardType::OTHER){
		return ReadStandard(aElement);
	} 
	else if (GetSimpleType(aElement->Name()) != SimpleType::OTHER){
		return ReadSimple(aElement);
	}
	else {
		return ReadUserSpecial(aElement);
	}
	return Shape{ ShapeType::Other, -1 };
}

Shape StandardShape::ReadUserSpecial(tinyxml2::XMLElement* aElement)
{
	m_userspecial.push_back(UserSpecial{});
	auto& userSpecial = m_userspecial.back();
	for (auto shapeDoc = aElement->FirstChildElement(); shapeDoc; shapeDoc = shapeDoc->NextSiblingElement()) {
		if (shapeDoc->Name() == std::string("UserPrimitiveRef")) {
			std::string ref = shapeDoc->FindAttribute("id")->Value();
			auto it = m_content->m_userPrimitive.find(ref);
			if (it != m_content->m_userPrimitive.end())
				userSpecial.simpleShape.push_back(it->second);
		}
		else if (GetSimpleType(shapeDoc->Name()) != SimpleType::OTHER) {
			userSpecial.simpleShape.push_back(ReadSimple(shapeDoc));
		}
		else if (GetStandardType(shapeDoc->Name()) != StandardType::OTHER)
			userSpecial.simpleShape.push_back(ReadStandard(shapeDoc));
		else
			userSpecial.simpleShape.push_back(ReadUserSpecial(shapeDoc));
	}
	return Shape{ ShapeType::UserSpecial, static_cast<int32_t>(m_userspecial.size() - 1) };
}