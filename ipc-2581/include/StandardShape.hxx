#pragma once
#include <variant>
#include <tinyxml2.h>
#include "BaseElement.hxx"

class ContentSection;

struct Shape {
	ShapeType type;
	int index;
	std::variant<StandardType, SimpleType> shape_type;
};

struct Simple {
	SimpleType simpleType;
};

struct Standard {
	StandardType standardType;
};

struct Line{
	double startX;
	double startY;
	double endX;
	double endY;
	LineDesc* lineDesc = nullptr;
};

struct Arc {
	double startX;
	double startY;
	double endX;
	double endY;
	double centerX;
	double centerY;
	double clockwise;
	LineDesc* lineDesc = nullptr;
};

struct PolyBegin {
	double x, y;
};

struct PolyStepCurve {
	double x;
	double y;
	double centerX;
	double centerY;
	bool clockwise;
};
struct PolyStepSegment {

	double x;
	double y;
};

using PolyStepBase = std::variant<PolyStepCurve, PolyStepSegment>;
using PolyStep = std::vector<PolyStepBase>;

struct Polygon {
	PolyBegin polyBegin;
	PolyStep polyStep;
	Xform xform;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Polyline{
	PolyBegin polyBegin;
	PolyStep polyStep;
	LineDesc* lineDesc = nullptr;
};

struct Cutout {
	PolyBegin polyBegin;
	PolyStep polyStep;
	Xform xform;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Outline{
	Polygon polygon;
	LineDesc* lineDesc = nullptr;
};

struct Butterfly{
	std::string shape;
	double expand;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
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
	DonutShape shape;
	double outerDiameter;
	double innerDiameter;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Ellipse {
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Hexagon {
	double length;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Moire {
	double diameter;
	double ringWidth;
	double ringGap;
	int ringNumber;
	double lineWidth;
	double lineLength;
	int lineAngle;
};
struct Octagon {
	double length;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Oval {
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct RectCenter {
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct RectCham {
	double width;
	double height;
	double chamfer;
	bool upperRight;
	bool upperLeft;
	bool lowerLeft;
	bool lowerRight;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct RectCorner {
	double lowerLeftX;
	double lowerLeftY;
	double upperRightX;
	double upperRightY;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct RectRound {
	double width;
	double height;
	double radius;
	bool upperRight;
	bool upperLeft;
	bool lowerRight;
	bool lowerLeft;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Thermal {
	ThermalShape shape;
	double outerDiameter;
	double innerDiameter;
	int spockCount;
	double spokeWidth;
	double spokeStartAngle;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Triangle {
	double base;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct UserSpecial {
	std::vector<Shape> simpleShape;
};

class StandardShape {
public:
	StandardShape();
	void SetContent(ContentSection* aContent);
	Shape ReadSimple(tinyxml2::XMLElement* aElement);
	Shape ReadStandard(tinyxml2::XMLElement* aElement);
	Shape ReadLine(tinyxml2::XMLElement* aElement);
	Shape ReadArc(tinyxml2::XMLElement* aElement);
	bool ReadPolyBegin(tinyxml2::XMLElement* aElement, PolyBegin& poly);
	Shape ReadOutline(tinyxml2::XMLElement* aElement);
	bool ReadPolygon(tinyxml2::XMLElement* aElement, Polygon& polygon);
	Shape ReadPolyline(tinyxml2::XMLElement* aElement);
	bool ReadCutout(tinyxml2::XMLElement* aElement, Cutout& cutout);
	bool ReadPolyStepCurve(tinyxml2::XMLElement* aElement, PolyStepCurve& polyStepCurve);
	bool ReadPolyStepSegment(tinyxml2::XMLElement* aElement, PolyStepSegment& polyStepSegment);
	Shape ReadButterfly(tinyxml2::XMLElement* aElement);
	Shape ReadCircle(tinyxml2::XMLElement* aElement);
	Shape ReadContour(tinyxml2::XMLElement* aElement);
	Shape ReadDiamond(tinyxml2::XMLElement* aElement);
	Shape ReadDonut(tinyxml2::XMLElement* aElement);
	Shape ReadEllipse(tinyxml2::XMLElement* aElement);
	Shape ReadHexagon(tinyxml2::XMLElement* aElement);
	Shape ReadMoire(tinyxml2::XMLElement* aElement);
	Shape ReadOctagon(tinyxml2::XMLElement* aElement);
	Shape ReadOval(tinyxml2::XMLElement* aElement);
	Shape ReadRectCenter(tinyxml2::XMLElement* aElement);
	Shape ReadRectCham(tinyxml2::XMLElement* aElement);
	Shape ReadRectCorner(tinyxml2::XMLElement* aElement);
	Shape ReadRectRound(tinyxml2::XMLElement* aElement);
	Shape ReadThermal(tinyxml2::XMLElement* aElement);
	Shape ReadTriangle(tinyxml2::XMLElement* aElement);
	Shape ReadFeature(tinyxml2::XMLElement* aElement);
	Shape ReadUserSpecial(tinyxml2::XMLElement* aElement);
private:
	ContentSection* m_content;
public:
	std::vector<Arc>		m_arc;
	std::vector<Line>		m_line;
	std::vector<Polyline>	m_polyline;
	std::vector<Outline>	m_outline;
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
	std::vector<UserSpecial> m_userspecial;
};

