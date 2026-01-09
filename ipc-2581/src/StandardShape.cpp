#include "StandardShape.hxx"

bool ReadButterfly(tinyxml2::XMLElement* aElement, Butterfly& butterfly) {
	butterfly.shape = aElement->FindAttribute("shape")->Value();
	std::string exp = butterfly.shape == "ROUND" ? "diameter" : "side";
	butterfly.expand = aElement->FindAttribute(exp.data())->DoubleValue();

	ReadLineDescHelper(aElement, &butterfly.lineDesc);
	ReadFillDescHelper(aElement, &butterfly.fillDesc);

	return true;
}


bool ReadCircle(tinyxml2::XMLElement* aElement, Circle& circle) {
	circle.diameter = aElement->FindAttribute("diameter")->DoubleValue();

	ReadLineDescHelper(aElement, &circle.lineDesc);
	ReadFillDescHelper(aElement, &circle.fillDesc);

	return true;
}


bool ReadRectCenter(tinyxml2::XMLElement* aElement, RectCenter& rectCenter) {
	rectCenter.height = aElement->FindAttribute("height")->DoubleValue();
	rectCenter.width = aElement->FindAttribute("width")->DoubleValue();

	ReadLineDescHelper(aElement, &rectCenter.lineDesc);
	ReadFillDescHelper(aElement, &rectCenter.fillDesc);

	return true;
}