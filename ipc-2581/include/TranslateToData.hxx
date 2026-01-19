#pragma once

#include "data_manager.hxx"
#include "Ecad.hxx"

class TranslateToData {
public:
	TranslateToData(EcadSection* ecad, DataManager* dataManager)
		: m_ecad(ecad), m_dataManager(dataManager) {
	}

	bool Translate();

private:
	EcadSection* m_ecad;
	DataManager* m_dataManager;
};
