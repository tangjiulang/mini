#pragma once

#include <tinyxml2.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "BaseTypeDef.hxx"


struct Xform {
	double xOffset = 0;
	double yOffset = 0;
	double rotation = 0;
	double scale = 0;
	bool mirror = 0;
	bool faceUp = 0;
};

struct LineDesc {
	double lineWidth;
	LineEndType lineEndType;
};

struct LineDescPreDef {
	std::string id;
	LineDesc lineDesc;
};

struct Color;

struct FillDesc {
	FillPropertyType fillType;
	double lineWidth = -1;
	double pitch1 = -1;
	double pitch2 = -1;
	double angle1 = -1;
	double angle2 = -1;
	Color* color;
};

struct FillDescPreDef {
	std::string id;
	FillDesc fillDesc;
};

struct Location {
	double x = 0;
	double y = 0;
};

struct PinRef {
	std::string componentRef;
	std::string pin;
	std::string title;
};

struct Property;

bool ReadXform(tinyxml2::XMLElement* aElement, Xform& xForm);
bool ReadLineDesc(tinyxml2::XMLElement* aElement, LineDesc* lineDesc);
bool ReadLinePreDef(tinyxml2::XMLElement* aElement, LineDescPreDef& lineDescPreDef);
bool ReadFillDesc(tinyxml2::XMLElement* aElement, FillDesc* fillDesc);
bool ReadFillPreDef(tinyxml2::XMLElement* aElement, FillDescPreDef& fillDescPreDef);
bool ReadLineDescHelper(tinyxml2::XMLElement* aElement, std::unordered_map<std::string, LineDesc>& lineDescPreDef, LineDesc** lineDesc);
bool ReadFillDescHelper(tinyxml2::XMLElement* aElement, std::unordered_map<std::string, FillDesc>& fillDescPreDef, FillDesc** fillDesc);
bool ReadLocation(tinyxml2::XMLElement* aElement, Location& location);
bool ReadPinRef(tinyxml2::XMLElement* aElement, PinRef& pinRef);
bool ReadProperty(tinyxml2::XMLElement* aElement, Property& property);