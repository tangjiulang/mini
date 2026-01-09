#include "Ecad.hxx"

EcadSection::EcadSection(tinyxml2::XMLElement* aEcad, ContentSection* aContent)
	: m_ecad(aEcad),
	  m_content(aContent)
{
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

	}

	return WrongType();
}

bool EcadSection::IsValid()
{
	return m_ecad != nullptr;
}
