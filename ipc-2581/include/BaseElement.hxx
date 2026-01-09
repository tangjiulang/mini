#pragma once

#include <tinyxml2.h>
#include <string>
#include <vector>
#include "BaseTypeDef.hxx"

struct LineDesc {
	double lineWidth;
	LineEndType lineEndType;
};

struct LineDescPreDef {
	std::string id;
	LineDesc lineDesc;
};

struct Line {
	double startX;
	double startY;
	double endX;
	double endY;
	LineDesc* lineDesc;
};

struct Arc {
	double startX;
	double startY;
	double endX;
	double endY;
	double centerX;
	double centerY;
	double clockwise;
	LineDesc* lineDesc;
};

struct PolyBegin {
	double x, y;
};

class PolyStepBase {};
class PolyStepCurve : public PolyStepBase {
public:
	double x;
	double y;
	double centerX;
	double centerY;
	bool clockwise;
};
class PolyStepSegment : public PolyStepBase {
public:
	double x;
	double y;
};

using PolyStep = std::vector<PolyStepBase*>;

struct Xform;

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

struct Polygon {
	PolyBegin polyBegin;
	PolyStep polyStep;
	Xform* xform;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct PolyLine {
	PolyBegin polyBegin;
	PolyStep polyStep;
	LineDesc* lineDesc;
};

struct Cutout {
	PolyBegin polyBegin;
	PolyStep polyStep;
	Xform* xform;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Outline {
	Polygon polygon;
	LineDesc* lineDesc;
};

bool ReadLineDesc(tinyxml2::XMLElement* aElement, LineDesc* lineDesc);
bool ReadLinePreDef(tinyxml2::XMLElement* aElement, LineDescPreDef& lineDescPreDef);
bool ReadLine(tinyxml2::XMLElement* aElement, Line& line);
bool ReadArc(tinyxml2::XMLElement* aElement, Arc& arc);
bool ReadPolyBegin(tinyxml2::XMLElement* aElement, Polygon& poly);
bool ReadFillDesc(tinyxml2::XMLElement* aElement, FillDesc* fillDesc);
bool ReadFillPreDef(tinyxml2::XMLElement* aElement, FillDescPreDef& fillDescPreDef);
bool ReadLineDescHelper(tinyxml2::XMLElement* aElement, LineDesc** lineDesc);
bool ReadFillDescHelper(tinyxml2::XMLElement* aElement, FillDesc** fillDesc);