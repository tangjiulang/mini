#include <reader.hxx>

IPC2581Document::IPC2581Document(std::string fileName)
	: m_fileName(fileName)
{
	m_document.LoadFile(fileName.data());
}

WrongType IPC2581Document::DocumentReader() {
	tinyxml2::XMLElement* root = m_document.RootElement();
	if (!root)
		return WrongType::WrongRead;
	
	// IPC Header Read
	m_header.revision = root->Attribute("revision");
	m_header.xmlns = root->Attribute("xmlns");
	m_header.schemaLocation = root->Attribute("xsi:schemaLocation");

	// IPC Main Sections Read
	m_content = ContentSection(root->FirstChildElement("Content"));
	m_logisticHeader = root->FirstChildElement("LogisticHeader");
	m_historyRecord = root->FirstChildElement("HistoryRecord");
	m_bom = root->FirstChildElement("Bom");
	m_ecad = root->FirstChildElement("Ecad");
	m_avl = root->FirstChildElement("Avl");


	if (std::string(root->Name()) != "IPC-2581")
		return WrongType::NotIPC2581File;
	if (!m_header.revision.empty() && m_header.revision != "C" && m_header.revision != "c")
		return WrongType::UnsupportedRevision;

	if (!m_content.IsValid() || !m_ecad)
		return WrongType::UnSuccessfulReadSection;

	return WrongType::Success;
}


WrongType IPC2581Document::ContentReader()
{
	return m_content.Read();
}

WrongType IPC2581Document::EcadReader()
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