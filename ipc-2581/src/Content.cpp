#include "Content.hxx"

ContentSection::ContentSection(tinyxml2::XMLElement* aContent)
	: m_content(aContent) 
{
	m_standardShape = new StandardShape(this);
}

WrongType ContentSection::Read()
{
	// Read FunctionMode in Content
	auto funcionMode = m_content->FirstChildElement("FunctionMode");
	if (!funcionMode)
		return WrongType::UnSuccessfulReadSection;
	m_mode = funcionMode->FindAttribute("mode")->Value();

	// Read StepRef in Content
	auto stepRef = m_content->FirstChildElement("StepRef");
	if (!stepRef)
		return WrongType::UnSuccessfulReadSection;
	m_stepRef = stepRef->FindAttribute("name")->Value();

	// Read LayerRef in Content
	auto layerRef = m_content->FirstChildElement("LayerRef");
	do {
		m_layerRefs.push_back(layerRef->FindAttribute("name")->Value());
	} while (layerRef = layerRef->NextSiblingElement("LayerRef"));

	// Read DictionaryColor in Content
	auto dictionaryColor = m_content->FirstChildElement("DictionaryColor");
	for (auto color = dictionaryColor->FirstChildElement(); color; color = color->NextSiblingElement()) {
		m_colorRefs.push_back(color->FindAttribute("name")->Value());
	}

	// Read DictionaryLineDesc in Content
	auto dictionaryLineDesc = m_content->FirstChildElement("DictionaryLineDesc");
	for (auto entryLineDesc = dictionaryLineDesc->FirstChildElement(); entryLineDesc; entryLineDesc = entryLineDesc->NextSiblingElement()) {
		LineDescPreDef line;
		ReadLinePreDef(entryLineDesc, line);
		m_lineDescPreDefs[line.id] = line.lineDesc;
	}

	// Read DictionaryFillDesc in Content;
	auto dictionaryFillDesc = m_content->FirstChildElement("DictionaryFillDesc");
	for (auto entryFillDesc = dictionaryFillDesc->FirstChildElement(); entryFillDesc; entryFillDesc = entryFillDesc->NextSiblingElement()) {
		FillDescPreDef fill;
		ReadFillPreDef(entryFillDesc, fill);
		m_fillDescPreDefs[fill.id] = fill.fillDesc;
	}

	// Read DictionaryStandard in Content
	// Todo all standard shape, there just circle and rectCenter
	auto dictionaryStandard = m_content->FirstChildElement("DictionaryStandard");
	for (auto entryStandard = dictionaryStandard->FirstChildElement(); entryStandard; entryStandard = entryStandard->NextSiblingElement()) {
		std::string shape = entryStandard->FirstChildElement()->Name();
		if (shape == "Circle") {
			CircleDef circleDef;
			circleDef.id = entryStandard->FindAttribute("id")->Value();
			m_standardShape->ReadCircle(entryStandard->FirstChildElement(), circleDef.circle);
			m_circlePreDefs.push_back(circleDef);
		}
		else if (shape == "RectCenter") {
			RectDef rectDef;
			rectDef.id = entryStandard->FindAttribute("id")->Value();
			m_standardShape->ReadRectCenter(entryStandard->FirstChildElement(), rectDef.rectCenter);
			m_rectPreDefs.push_back(rectDef);
		}

	}

	// Read DictionaryUser in Content
	// not complete all dictionaryUser, there just circle
	auto dictionaryUser = m_content->FirstChildElement("DictionaryUser");
	for (auto entryUser = dictionaryUser->FirstChildElement(); entryUser; entryUser = entryUser->NextSiblingElement()) {
		std::string id = entryUser->FindAttribute("id")->Value();
		auto userSpecial = entryUser->FirstChildElement();
		auto shape = userSpecial->FirstChildElement();
		if (std::string(shape->Name()) == "Circle") {
			CircleDef circleDef;
			circleDef.id = id;
			m_standardShape->ReadCircle(shape, circleDef.circle);
			m_circlePreDefs.push_back(circleDef);
		}
		else {
			// Todo any other shape
		}

	}

	return WrongType::Success;
}

bool ContentSection::IsValid()
{
	return m_content != nullptr;
}