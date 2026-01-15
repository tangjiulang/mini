#pragma once
#include <tinyxml2.h>
#include "BaseElement.hxx"

class ContentSection;

struct Shape {};

struct Butterfly : public Shape {
	std::string shape;
	double expand;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Circle : public Shape {
	double diameter;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Contour : public Shape {
	Polygon polygon;
	std::vector<Cutout> cutouts;
};

struct Diamond : public Shape {
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Donut : public Shape {
	std::string shape;
	double outerDiameter;
	double innerDiameter;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Ellipse : public Shape {
	double width;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct Hexagon : public Shape {
	double length;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Moire : public Shape {
	double diameter;
	double ringWidth;
	double ringGap;
	double ringNumber;
	int lineNumber;
	double lineLength;
	int lineAngle;
};
struct Octagon : public Shape {
	double length;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Oval : public Shape {
	double width;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct RectCenter : public Shape {
	double width;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

struct RectCham : public Shape {
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
struct RectCorner : public Shape {
	double lowerLeftX;
	double lowerLeftY;
	double upperRightX;
	double upperRightY;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct RectRound : public Shape {
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
struct Thermal : public Shape {
	std::string shape;
	double outerDiameter;
	double innerDiameter;
	int spockCount;
	double spokeWidth;
	double spokeStartAngle;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};
struct Triangle : public Shape {
	double base;
	double height;
	LineDesc* lineDesc;
	FillDesc* fillDesc;
};

class StandardShape {
public:
	StandardShape(ContentSection* content);
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
private:
	ContentSection* m_content;
};

