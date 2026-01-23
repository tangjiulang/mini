#pragma once

#include "data_manager.hxx"
#include "Ecad.hxx"

class TranslateToData {
public:
	TranslateToData(EcadSection* ecad, DataManager* dataManager, KIGFX::VIEW *view)
		: m_ecad(ecad), m_dataManager(dataManager), m_view(view) {
	}

	bool Translate(KIGFX::VIEW* view);
private:
	bool TranslateShape(Shape shape, const VECTOR2D& location = { 0, 0 });
	bool TranslateSimple(Shape simple, const VECTOR2D& location = { 0, 0 });
	bool TranslateLine(Line* line, const VECTOR2D& location = { 0, 0 });
	bool TranslateArc(Arc* arc, const VECTOR2D& location = { 0, 0 });
	bool TranslateOutline(Outline* outline, const VECTOR2D& location = { 0, 0 });
	bool TranslatePolyline(Polyline* polyline, const VECTOR2D& location = { 0, 0 });
	bool TranslateStandard(Shape standard, const VECTOR2D& location = { 0, 0 });
	bool TranslateButterfly(Butterfly* butterfly, const VECTOR2D& location = { 0, 0 });
	bool TranslateCircle(Circle* circle, const VECTOR2D& location = { 0, 0 });
	bool TranslatePolygon(Polygon* polygon, const VECTOR2D& location = { 0, 0 });
	bool TranslateCutout(Cutout* cutout, const VECTOR2D& location = { 0, 0 });
	bool TranslateContour(Contour* contour, const VECTOR2D& location = { 0, 0 });
	bool TranslateDiamond(Diamond* diamond, const VECTOR2D& location = { 0, 0 });
	bool TranslateDonut(Donut* donut, const VECTOR2D& location = { 0, 0 });
	bool TranslateEllipse(Ellipse* ellipse, const VECTOR2D& location = { 0, 0 });
	bool TranslateHexagon(Hexagon* hexagon, const VECTOR2D& location = { 0, 0 });
	bool TranslateMoire(Moire* moire, const VECTOR2D& location = { 0, 0 });
	bool TranslateOctagon(Octagon* octagon, const VECTOR2D& location = { 0, 0 });
	bool TranslateOval(Oval* oval, const VECTOR2D& location = { 0, 0 });
	bool TranslateRectCenter(RectCenter* rectCenter, const VECTOR2D& location = { 0, 0 });
	bool TranslateRectCham(RectCham* rectCham, const VECTOR2D& location = { 0, 0 });
	bool TranslateRectCorner(RectCorner* rectCorner, const VECTOR2D& location = { 0, 0 });
	bool TranslateRectRound(RectRound* rectRound, const VECTOR2D& location = { 0, 0 });
	bool TranslateThermal(Thermal* thermal, const VECTOR2D& location = { 0, 0 });
	bool TranslateTriangle(Triangle* triangle, const VECTOR2D& location = { 0, 0 });
	bool TranslateUserSpecial(Shape userSpecial, const VECTOR2D& location = { 0, 0 });

private:
	EcadSection* m_ecad;
	DataManager* m_dataManager;
	KIGFX::VIEW* m_view;
	PCB_LAYER_ID m_currentLayer;
};
