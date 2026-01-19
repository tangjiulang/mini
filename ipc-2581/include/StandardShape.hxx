#pragma once
#include <variant>
#include <tinyxml2.h>
#include "BaseElement.hxx"
#include "data_manager.hxx"

class ContentSection;

struct Shape {
	ShapeType type;
};

struct Simple : public Shape {
	Simple() { Shape::type = ShapeType::Simple; }
	SimpleType simpleType;
};

struct Standard : public Shape {
	Standard() { Shape::type = ShapeType::Standard; }
	StandardType standardType;
};

struct Line : public Simple {
	Line() { Simple::simpleType = SimpleType::Line; }
	double startX;
	double startY;
	double endX;
	double endY;
	LineDesc* lineDesc = nullptr;
};

struct Arc : public Simple {
	Arc() { Simple::simpleType = SimpleType::Arc; }
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

struct Polyline : public Simple {
	Polyline() { Simple::simpleType = SimpleType::Polyline; }
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

struct Outline : public Simple {
	Outline() { Simple::simpleType = SimpleType::Outline; }
	Polygon polygon;
	LineDesc* lineDesc = nullptr;
};

struct Butterfly : public Standard {
	Butterfly() { Standard::standardType = StandardType::BUTTERFLY; }
	std::string shape;
	double expand;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Circle : public Standard {
	Circle() { Standard::standardType = StandardType::CIRCLE; }
	double diameter;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Contour : public Standard {
	Contour() { Standard::standardType = StandardType::CONTOUR; }
	Polygon polygon;
	std::vector<Cutout> cutouts;
};

struct Diamond : public Standard {
	Diamond() { Standard::standardType = StandardType::DIAMOND; }
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Donut : public Standard {
	Donut() { Standard::standardType = StandardType::DONUT; }
	DonutShape shape;
	double outerDiameter;
	double innerDiameter;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Ellipse : public Standard {
	Ellipse() { Standard::standardType = StandardType::ELLIPSE; }
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct Hexagon : public Standard {
	Hexagon() { Standard::standardType = StandardType::HEXAGON; }
	double length;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Moire : public Standard {
	Moire() { Standard::standardType = StandardType::MOIRE; }
	double diameter;
	double ringWidth;
	double ringGap;
	int ringNumber;
	double lineWidth;
	double lineLength;
	int lineAngle;
};
struct Octagon : public Standard {
	Octagon() { Standard::standardType = StandardType::OCTAGON; }
	double length;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Oval : public Standard {
	Oval() { Standard::standardType = StandardType::OVAL; }
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct RectCenter : public Standard {
	RectCenter() { Standard::standardType = StandardType::RECTCENTER; }
	double width;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct RectCham : public Standard {
	RectCham() { Standard::standardType = StandardType::RECTCHAM; }
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
struct RectCorner : public Standard {
	RectCorner() { Standard::standardType = StandardType::RECTCORNER; }
	double lowerLeftX;
	double lowerLeftY;
	double upperRightX;
	double upperRightY;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct RectRound : public Standard {
	RectRound() { Standard::standardType = StandardType::RECTROUND; }
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
struct Thermal : public Standard {
	Thermal() { Standard::standardType = StandardType::THERMAL; }
	ThermalShape shape;
	double outerDiameter;
	double innerDiameter;
	int spockCount;
	double spokeWidth;
	double spokeStartAngle;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};
struct Triangle : public Standard {
	Triangle() { Standard::standardType = StandardType::TRIANGLE; }
	double base;
	double height;
	LineDesc* lineDesc = nullptr;
	FillDesc* fillDesc = nullptr;
};

struct UserSpecial : public Shape {
	UserSpecial() { Shape::type = ShapeType::UserSpecial; }
	std::vector<Shape*> simpleShape;
};

class StandardShape {
public:
	StandardShape(ContentSection* content);
	Shape* ReadSimple(tinyxml2::XMLElement* aElement);
	Shape* ReadStandard(tinyxml2::XMLElement* aElement);
	Simple* ReadLine(tinyxml2::XMLElement* aElement);
	Simple* ReadArc(tinyxml2::XMLElement* aElement);
	bool ReadPolyBegin(tinyxml2::XMLElement* aElement, PolyBegin& poly);
	Simple* ReadOutline(tinyxml2::XMLElement* aElement);
	bool ReadPolygon(tinyxml2::XMLElement* aElement, Polygon& polygon);
	Simple* ReadPolyline(tinyxml2::XMLElement* aElement);
	bool ReadCutout(tinyxml2::XMLElement* aElement, Cutout& cutout);
	bool ReadPolyStepCurve(tinyxml2::XMLElement* aElement, PolyStepCurve& polyStepCurve);
	bool ReadPolyStepSegment(tinyxml2::XMLElement* aElement, PolyStepSegment& polyStepSegment);
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
	Shape* ReadFeature(tinyxml2::XMLElement* aElement);
	Shape* ReadUserSpecial(tinyxml2::XMLElement* aElement);
	KIGFX::DATA_Circle TransToDataCircle(Circle* circle);
	KIGFX::DATA_Line   TransToDataLine(Line* line);
	KIGFX::DATA_Rectangle TransToDataRectangle(RectCenter* rectCenter);
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

