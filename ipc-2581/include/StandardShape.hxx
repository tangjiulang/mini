#pragma once
#include <tinyxml2.h>
#include "BaseElement.hxx"

struct Butterfly {
	std::string shape;
	double expand;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Circle {
	double diameter;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Contour {
	Polygon polygon;
	std::vector<Cutout> cutouts;
};

struct Diamond {
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Donut {
	std::string shape;
	double outerDiameter;
	double innerDiameter;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Ellipse {
	double width;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Hexagon {
	double length;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Moire {
	double diameter;
	double ringWidth;
	double ringGap;
	double ringNumber;
	int lineNumber;
	double lineLength;
	int lineAngle;
};
struct Octagon {
	double length;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Oval {
	double width;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct RectCenter {
	double width;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct RectCham {
	double width;
	double height;
	double chamfer;
	bool upperRight;
	bool upperLeft;
	bool lowerLeft;
	bool lowerRight;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct RectCorner {
	double lowerLeftX;
	double lowerLeftY;
	double upperRightX;
	double upperRightY;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct RectRound {
	double width;
	double height;
	double radius;
	bool upperRight;
	bool upperLeft;
	bool lowerRight;
	bool lowerLeft;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Thermal {
	std::string shape;
	double outerDiameter;
	double innerDiameter;
	int spockCount;
	double spokeWidth;
	double spokeStartAngle;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Triangle {
	double base;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};


bool ReadButterfly(tinyxml2::XMLElement* aElement, Butterfly& butterfly);
bool ReadCircle(tinyxml2::XMLElement* aElement, Circle& circle);
bool ReadContour(tinyxml2::XMLElement* aElement, Contour& contour);
bool ReadDiamond(tinyxml2::XMLElement* aElement, Diamond& diamond);
bool ReadDonut(tinyxml2::XMLElement* aElement, Donut& donut);
bool ReadEllipse(tinyxml2::XMLElement* aElement, Ellipse& ellipse);
bool ReadHexagon(tinyxml2::XMLElement* aElement, Hexagon& hexagon);
bool ReadMoire(tinyxml2::XMLElement* aElement, Moire& moire);
bool ReadOctagon(tinyxml2::XMLElement* aElement, Octagon& octagon);
bool ReadOval(tinyxml2::XMLElement* aElement, Oval& oval);
bool ReadRectCenter(tinyxml2::XMLElement* aElement, RectCenter& rectCenter);
bool ReadRectCham(tinyxml2::XMLElement* aElement, RectCham& rectChame);
bool ReadRectCorner(tinyxml2::XMLElement* aElement, RectCorner& rectCorner);
bool ReadRectRound(tinyxml2::XMLElement* aElement, RectRound& rectRound);
bool ReadThermal(tinyxml2::XMLElement* aElement, Thermal& thermal);
bool ReadTriangle(tinyxml2::XMLElement* aElement, Triangle& triangle);
