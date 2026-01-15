#include "StandardShape.hxx"
#include "Content.hxx"

StandardShape::StandardShape(ContentSection* content)
	: m_content(content) {}

Shape* StandardShape::ReadStandard(tinyxml2::XMLElement* aElement)
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
		
	return nullptr;   // OTHER / 不支持
}

Butterfly* StandardShape::ReadButterfly(tinyxml2::XMLElement* aElement) {
	m_butterfly.resize(m_butterfly.size() + 1);
	Butterfly& butterfly = m_butterfly.back();
	butterfly.shape = aElement->FindAttribute("shape")->Value();
	std::string exp = butterfly.shape == "ROUND" ? "diameter" : "side";
	butterfly.expand = aElement->FindAttribute(exp.data())->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &butterfly.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &butterfly.fillDesc);

	return &m_butterfly.back();
}


Circle* StandardShape::ReadCircle(tinyxml2::XMLElement* aElement) {
	m_circle.resize(m_butterfly.size() + 1);
	Circle& circle = m_circle.back();
	circle.diameter = aElement->FindAttribute("diameter")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &circle.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &circle.fillDesc);

	return &m_circle.back();
}

Contour* StandardShape::ReadContour(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}


RectCenter* StandardShape::ReadRectCenter(tinyxml2::XMLElement* aElement) {
	m_rectCenter.resize(m_rectCenter.size() + 1);
	auto& rectCenter = m_rectCenter.back();
	rectCenter.height = aElement->FindAttribute("height")->DoubleValue();
	rectCenter.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &rectCenter.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &rectCenter.fillDesc);

	return &m_rectCenter.back();
}

Diamond* StandardShape::ReadDiamond(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Donut* StandardShape::ReadDonut(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Ellipse* StandardShape::ReadEllipse(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Hexagon* StandardShape::ReadHexagon(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Moire* StandardShape::ReadMoire(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Octagon* StandardShape::ReadOctagon(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Oval* StandardShape::ReadOval(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

RectCham* StandardShape::ReadRectCham(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

RectCorner* StandardShape::ReadRectCorner(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

RectRound* StandardShape::ReadRectRound(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Thermal* StandardShape::ReadThermal(tinyxml2::XMLElement* aElement)
{
	return nullptr;
}

Triangle * StandardShape::ReadTriangle(tinyxml2::XMLElement * aElement)
{
return nullptr;
}