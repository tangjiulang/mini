#pragma once
#include <tinyxml2.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "BaseElement.hxx"
#include "StandardShape.hxx"
#include "BaseTypeDef.hxx"

class ContentSection {
public:
	ContentSection() = default;
	ContentSection(tinyxml2::XMLElement* aContent);
	WrongType Read();
	bool IsValid();
private:
	bool ReadDictionaryStandard(tinyxml2::XMLElement* aDictionaryStandard);
	bool ReadDictionaryUser(tinyxml2::XMLElement* aDictionanryUser);
private:
	tinyxml2::XMLElement*							m_content;
public:
	StandardShape*									m_standardShape;
	std::string										m_mode;
	std::string										m_stepRef;
	std::vector<std::string>						m_layerRefs;
	std::vector<std::string>						m_colorRefs;
	std::unordered_map<std::string, LineDesc>		m_lineDescPreDefs;
	std::unordered_map<std::string, FillDesc>		m_fillDescPreDefs;
	std::unordered_map<std::string, Shape>			m_standaredPrimitive;
	std::unordered_map<std::string, Shape>			m_userPrimitive;
};