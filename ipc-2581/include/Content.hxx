#pragma once
#include <tinyxml2.h>
#include <string>
#include <vector>
#include "BaseElement.hxx"
#include "StandardShape.hxx"
#include "BaseTypeDef.hxx"

struct CircleDef {
	std::string id;
	Circle circle;
};

struct RectDef {
	std::string id;
	RectCenter rectCenter;
};

class ContentSection {
public:
	ContentSection() = default;
	ContentSection(tinyxml2::XMLElement* aContent);
	WrongType Read();
	bool IsValid();
private:
	tinyxml2::XMLElement*		m_content;
public:
	std::string					m_mode;
	std::string					m_stepRef;
	std::vector<std::string>	m_layerRefs;
	std::vector<std::string>	m_colorRefs;
	std::vector<LineDescPreDef> m_lineDescPreDefs;
	std::vector<FillDescPreDef> m_fillDescPreDefs;
	std::vector<CircleDef>		m_circlePreDefs;
	std::vector<RectDef>		m_rectPreDefs;
};


bool ReadEntryUser(tinyxml2::XMLElement* aElement);