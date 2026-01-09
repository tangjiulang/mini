#pragma once
#include "BaseTypeDef.hxx"
#include "Content.hxx"

struct Spec;
struct ChangeRec;

class EcadSection {
public:
	EcadSection() = default;
	EcadSection(tinyxml2::XMLElement* aEcad, ContentSection* aContent);
	WrongType Read();
	bool IsValid();
private:
	tinyxml2::XMLElement* m_ecad;
	ContentSection* m_content;
public:
	UnitsType				m_unit;
	std::vector<Spec>		m_specs;
	std::vector<ChangeRec>	m_changeRecs;
};