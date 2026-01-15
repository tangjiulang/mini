#include "StandardShape.hxx"
#include "Content.hxx"

StandardShape::StandardShape(ContentSection* content)
	: m_content(content) {}

bool StandardShape::ReadButterfly(tinyxml2::XMLElement* aElement, Butterfly& butterfly) {
	butterfly.shape = aElement->FindAttribute("shape")->Value();
	std::string exp = butterfly.shape == "ROUND" ? "diameter" : "side";
	butterfly.expand = aElement->FindAttribute(exp.data())->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &butterfly.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &butterfly.fillDesc);

	return true;
}


bool StandardShape::ReadCircle(tinyxml2::XMLElement* aElement, Circle& circle) {
	circle.diameter = aElement->FindAttribute("diameter")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &circle.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &circle.fillDesc);

	return true;
}

bool StandardShape::ReadContour(tinyxml2::XMLElement* aElement, Contour& contour)
{
	return false;
}


bool StandardShape::ReadRectCenter(tinyxml2::XMLElement* aElement, RectCenter& rectCenter) {
	rectCenter.height = aElement->FindAttribute("height")->DoubleValue();
	rectCenter.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, m_content->m_lineDescPreDefs, &rectCenter.lineDesc);
	ReadFillDescHelper(aElement, m_content->m_fillDescPreDefs, &rectCenter.fillDesc);

	return true;
}