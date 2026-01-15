#pragma once
#include <tinyxml2.h>
#include "BaseElement.hxx"

class ContentSection;

struct Shape {
};

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
	Shape* ReadStandard(tinyxml2::XMLElement* aElement);
	Butterfly* ReadButterfly(tinyxml2::XMLElement* aElement);
	Circle* ReadCircle(tinyxml2::XMLElement* aElement);
	Contour* ReadContour(tinyxml2::XMLElement* aElement);
	Diamond* ReadDiamond(tinyxml2::XMLElement* aElement);
	Donut* ReadDonut(tinyxml2::XMLElement* aElement);
	Ellipse* ReadEllipse(tinyxml2::XMLElement* aElement);
	Hexagon* ReadHexagon(tinyxml2::XMLElement* aElement);
	Moire* ReadMoire(tinyxml2::XMLElement* aElement);
	Octagon* ReadOctagon(tinyxml2::XMLElement* aElement);
	Oval* ReadOval(tinyxml2::XMLElement* aElement);
	RectCenter* ReadRectCenter(tinyxml2::XMLElement* aElement);
	RectCham* ReadRectCham(tinyxml2::XMLElement* aElement);
	RectCorner* ReadRectCorner(tinyxml2::XMLElement* aElement);
	RectRound* ReadRectRound(tinyxml2::XMLElement* aElement);
	Thermal* ReadThermal(tinyxml2::XMLElement* aElement);
	Triangle* ReadTriangle(tinyxml2::XMLElement* aElement);
private:
	ContentSection* m_content;
public:
	std::vector<Butterfly>	m_butterfly;
	std::vector<Circle>		m_circle;
	std::vector<Contour>	m_contour;
	std::vector<Diamond>	m_diamond;
	std::vector<Donut>		m_donut;
	std::vector<Ellipse>	m_ellipse;
	std::vector<Hexagon>	m_hexagon;
	std::vector<Moire>		m_moire;
	std::vector<Octagon>	m_octagon;
	std::vector<Oval>		m_oval;
	std::vector<RectCenter> m_rectCenter;
	std::vector<RectCham>	m_rectCham;
	std::vector<RectCorner> m_rectCorner;
	std::vector<RectRound>	m_rectRound;
	std::vector<Thermal>	m_thermal;
	std::vector<Triangle>	m_triangle;
};

