#pragma once
#include <string>
#include <vector>
#include <tinyxml2.h>
#include "BaseElement.hxx"
#include "StandardShape.hxx"
#include "BaseTypeDef.hxx"
#include "Content.hxx"
#include "Ecad.hxx"

struct IPC2581Header {
	std::string revision;
	std::string xmlns;
	std::string schemaLocation;
};

class IPC2581Document {
public:
	IPC2581Document(std::string fileName);
	WrongType DocumentReader();

	WrongType ContentReader();
	WrongType EcadReader();
	WrongType LogisticHeaderReader();
	WrongType HistoryRecordReader();
	WrongType BomReader();
	WrongType AvlReader();

public:
	std::string				m_fileName;
	tinyxml2::XMLDocument	m_document;
	IPC2581Header			m_header;
	ContentSection			m_content;
	EcadSection				m_ecad;						// Electronic Computer Aided Design
	tinyxml2::XMLElement*	m_logisticHeader;
	tinyxml2::XMLElement*   m_historyRecord;
	tinyxml2::XMLElement*   m_bom;	
	tinyxml2::XMLElement*   m_avl;						// Approved Vendor List
};
