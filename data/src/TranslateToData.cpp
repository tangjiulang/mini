#include <magic_enum.hpp>
#include "TranslateToData.hxx"
#include "view.hxx"

bool TranslateToData::Translate(KIGFX::VIEW* view)
{
	int count = 0;
	for (auto step : m_ecad->m_steps) {
		TranslateShape(step.profile);
		for (auto component : step.components) {
			VECTOR2D location = { component.location.x, component.location.y };
			Package& package = step.packages[component.packageRef];
			//TranslateShape(package.outline, location);
		}
		for (auto layerFeatures : step.layerFeatures) {
			m_currentLayer = PCB_LAYER_ID(count++);
			for (auto set : layerFeatures.sets) {
				if (set.geometryUsage == GeometryUsage::TEXT)
					continue;
				for (auto pad : set.pads) {
					VECTOR2D location = { pad.location.x, pad.location.y };
					TranslateShape(pad.feature, location);
					auto padStackDef = step.padStackDefs[pad.padstackDefRef];
					for (auto holeStackPadDef : padStackDef.holes) {
						Circle circle;
						circle.diameter = holeStackPadDef.diameter;
						TranslateCircle(&circle, location);
					}
					for (auto padStackPadDef : padStackDef.pads) {
						if (padStackPadDef.layerRef != layerFeatures.layerRef)
							continue;
						VECTOR2D padLocation = { padStackPadDef.location.x, padStackPadDef.location.y };
						TranslateShape(padStackPadDef.feature, location + padLocation);
					}
					
				}

				for (auto feature : set.features) {
					VECTOR2D location = { feature.location.x, feature.location.y };
					for (auto shape : feature.featureShapes) {
						TranslateShape(shape, location);
					}
				}
			}
		}
	}


	return true;
}

bool TranslateToData::TranslateShape(Shape shape, const VECTOR2D& location)
{
	if (shape.type == ShapeType::Simple)
		return TranslateSimple(shape, location);
	else if (shape.type == ShapeType::Standard)
		return TranslateStandard(shape, location);
	else if (shape.type == ShapeType::UserSpecial)
		return TranslateUserSpecial(shape, location);
	else 
		return false;
}

bool TranslateToData::TranslateSimple(Shape shape, const VECTOR2D& location)
{
	if (std::get<SimpleType>(shape.shape_type) == SimpleType::Arc)
		return TranslateArc(&m_ecad->m_standardShape->m_arc[shape.index], location);
	else if (std::get<SimpleType>(shape.shape_type) == SimpleType::Line)
		return TranslateLine(&m_ecad->m_standardShape->m_line[shape.index], location);
	else if (std::get<SimpleType>(shape.shape_type) == SimpleType::Outline)
		return TranslateOutline(&m_ecad->m_standardShape->m_outline[shape.index], location);
	else if (std::get<SimpleType>(shape.shape_type) == SimpleType::Polyline)
		return TranslatePolyline(&m_ecad->m_standardShape->m_polyline[shape.index], location);
	else 
		return false;
}

bool TranslateToData::TranslateArc(Arc* aArc, const VECTOR2D& location)
{
	VECTOR2D startPoint{ aArc->startX, aArc->startY };
	startPoint += location;
	VECTOR2D endPoint{ aArc->endX, aArc->endY };
	endPoint += location;
	VECTOR2D centerPoint{ aArc->centerX, aArc->centerY };
	centerPoint += location;
	double lineWidth = aArc->lineDesc ? aArc->lineDesc->lineWidth : 0;

	SHAPE_ARC shape_arc;
	shape_arc.ConstructFromStartEndCenter(m_view->ToWorld(startPoint), m_view->ToWorld(endPoint), m_view->ToWorld(centerPoint), 1, m_view->ToWorld(lineWidth));

	m_dataManager->m_arcs.push_back(KIGFX::DATA_Arc{ shape_arc, m_currentLayer, m_view->ToWorld(lineWidth) });

	return true;
}

bool TranslateToData::TranslateLine(Line* line, const VECTOR2D& location)
{
	VECTOR2D startPoint{ line->startX, line->startY };
	VECTOR2D endPoint{ line->endX, line->endY };
	startPoint += location;
	endPoint += location;
	double lineWidth = line->lineDesc ? line->lineDesc->lineWidth : 0;

	SHAPE_SEGMENT shape_line{ m_view->ToWorld(startPoint), m_view->ToWorld(endPoint) };
	m_dataManager->m_lines.push_back(KIGFX::DATA_Line{ shape_line, m_currentLayer, m_view->ToWorld(lineWidth) });

	return true;
}

bool TranslateToData::TranslateOutline(Outline* outline, const VECTOR2D& location)
{
	VECTOR2D prePoint = { outline->polygon.polyBegin.x, outline->polygon.polyBegin.y };
	prePoint += location;
	std::vector<KIGFX::Segment> segments;
	
	for (auto polyStep : outline->polygon.polyStep) {
		if (std::holds_alternative<PolyStepSegment>(polyStep)) {
			auto& segment = std::get<PolyStepSegment>(polyStep);
			VECTOR2D point = { segment.x, segment.y };
			point += location;
			SHAPE_SEGMENT shape_line{ m_view->ToWorld(prePoint), m_view->ToWorld(point) };
			segments.push_back(shape_line);
			prePoint = point;
		}
		else {
			auto& curve = std::get<PolyStepCurve>(polyStep);
			// Approximate the curve with a line for simplicity
			SHAPE_ARC shape_arc;
			VECTOR2D endPoint = { curve.x, curve.y };
			endPoint += location;
			VECTOR2D centerPoint = { curve.centerX, curve.centerY };
			centerPoint += location;
			shape_arc.ConstructFromStartEndCenter(prePoint, endPoint, centerPoint);
			segments.push_back(shape_arc);
			prePoint = endPoint;
		}
	}
	double lineWidth = outline->polygon.lineDesc ? outline->polygon.lineDesc->lineWidth : 0;
	KIGFX::DATA_Polygon polygon(segments, m_view->ToWorld(lineWidth));
	m_dataManager->m_polygons.push_back(polygon);

	return true;
}

bool TranslateToData::TranslatePolyline(Polyline* polyline, const VECTOR2D& location)
{
	VECTOR2D prePoint = { polyline->polyBegin.x, polyline->polyBegin.y };
	prePoint += location;
	std::vector<KIGFX::Segment> segments;

	for (auto polyStep : polyline->polyStep) {
		if (std::holds_alternative<PolyStepSegment>(polyStep)) {
			auto& segment = std::get<PolyStepSegment>(polyStep);
			VECTOR2D point = { segment.x, segment.y };
			point += location;
			SHAPE_SEGMENT shape_line{ m_view->ToWorld(prePoint), m_view->ToWorld(point) };
			segments.push_back(shape_line);
			prePoint = point;
		}
		else {
			auto& curve = std::get<PolyStepCurve>(polyStep);
			// Approximate the curve with a line for simplicity
			SHAPE_ARC shape_arc;
			VECTOR2D endPoint = { curve.x, curve.y };
			endPoint += location;
			VECTOR2D centerPoint = { curve.centerX, curve.centerY };
			centerPoint += location;
			shape_arc.ConstructFromStartEndCenter(prePoint, endPoint, centerPoint);
			segments.push_back(shape_arc);
			prePoint = endPoint;
		}
	}
	double lineWidth = polyline->lineDesc ? polyline->lineDesc->lineWidth : 0;
	KIGFX::DATA_Polygon polygon(segments, m_currentLayer, m_view->ToWorld(lineWidth));
	m_dataManager->m_polygons.push_back(polygon);

	return true;
}

bool TranslateToData::TranslateStandard(Shape shape, const VECTOR2D& location)
{
	if (std::get<StandardType>(shape.shape_type) == StandardType::BUTTERFLY)
		return TranslateButterfly(&m_ecad->m_standardShape->m_butterfly[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::CIRCLE)
		return TranslateCircle(&m_ecad->m_standardShape->m_circle[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::CONTOUR)
		return TranslateContour(&m_ecad->m_standardShape->m_contour[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::DIAMOND)
		return TranslateDiamond(&m_ecad->m_standardShape->m_diamond[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::DONUT)
		return TranslateDonut(&m_ecad->m_standardShape->m_donut[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::ELLIPSE)
		return TranslateEllipse(&m_ecad->m_standardShape->m_ellipse[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::HEXAGON)
		return TranslateHexagon(&m_ecad->m_standardShape->m_hexagon[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::MOIRE)
		return TranslateMoire(&m_ecad->m_standardShape->m_moire[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::OCTAGON)
		return TranslateOctagon(&m_ecad->m_standardShape->m_octagon[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::OVAL)
		return TranslateOval(&m_ecad->m_standardShape->m_oval[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::RECTCENTER)
		return TranslateRectCenter(&m_ecad->m_standardShape->m_rectCenter[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::RECTCHAM)
		return TranslateRectCham(&m_ecad->m_standardShape->m_rectCham[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::RECTCORNER)
		return TranslateRectCorner(&m_ecad->m_standardShape->m_rectCorner[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::RECTROUND)
		return TranslateRectRound(&m_ecad->m_standardShape->m_rectRound[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::THERMAL)
		return TranslateThermal(&m_ecad->m_standardShape->m_thermal[shape.index], location);
	else if (std::get<StandardType>(shape.shape_type) == StandardType::TRIANGLE)
		return TranslateTriangle(&m_ecad->m_standardShape->m_triangle[shape.index], location);
	else
		return false;

}

bool TranslateToData::TranslateButterfly(Butterfly* butterfly, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateCircle(Circle* circle, const VECTOR2D& location)
{
	SHAPE_CIRCLE shape_circle{ m_view->ToWorld(location), static_cast<int32_t>(m_view->ToWorld(circle->diameter / 2)) };
	m_dataManager->m_circles.push_back(KIGFX::DATA_Circle(shape_circle, m_currentLayer));
	
	return true;
}

bool TranslateToData::TranslateContour(Contour* contour, const VECTOR2D& location)
{
	TranslatePolygon(&contour->polygon, location);
	for (auto cutout : contour->cutouts) {
		TranslateCutout(&cutout, location);
	}
	return true;
}

bool TranslateToData::TranslateDiamond(Diamond* diamond, const VECTOR2D& location)
{
	double width = diamond->width;
	double height = diamond->height;
	VECTOR2D point1 = { - width / 2, 0 }, point2 = { 0, height / 2 },
		point3 = { width / 2, 0 }, point4 = { 0, -height / 2 };
	SHAPE_LINE_CHAIN shape_diamond;
	shape_diamond.Append(m_view->ToWorld(location + point1));
	shape_diamond.Append(m_view->ToWorld(location + point2));
	shape_diamond.Append(m_view->ToWorld(location + point3));
	shape_diamond.Append(m_view->ToWorld(location + point4));
	shape_diamond.Append(m_view->ToWorld(location + point1));
	double lineWidth = diamond->lineDesc ? diamond->lineDesc->lineWidth : 0;
	m_dataManager->m_polylines.push_back(KIGFX::DATA_Polyline{ shape_diamond, m_currentLayer, m_view->ToWorld(lineWidth) });
	
	return true;
}

bool TranslateToData::TranslateDonut(Donut* donut, const VECTOR2D& location)
{
	DonutShape shape = donut->shape;
	double outDiameter = donut->outerDiameter, innerDiameter = donut->innerDiameter;
	if (shape == DonutShape::ROUND) {

	}
	else if (shape == DonutShape::SQUARE) {

	}
	else if (shape == DonutShape::HECAGON) {

	}
	else {

	}
	return false;
}

bool TranslateToData::TranslateEllipse(Ellipse* ellipse, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateHexagon(Hexagon* hexagon, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateMoire(Moire* moire, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateOctagon(Octagon* octagon, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateOval(Oval* oval, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateRectCenter(RectCenter* rectCenter, const VECTOR2D& location)
{
	double lineWidth = rectCenter->lineDesc ? rectCenter->lineDesc->lineWidth : 0;
	VECTOR2D startPoint = { location.x - rectCenter->width / 2.0, location.y - rectCenter->height / 2.0 };
	VECTOR2D endPoint = { location.x + rectCenter->width / 2.0, location.y + rectCenter->height / 2.0 };
	KIGFX::DATA_Rectangle data_rectangle(SHAPE_RECT{ m_view->ToWorld(startPoint), m_view->ToWorld(endPoint)}, m_currentLayer, m_view->ToWorld(lineWidth));
	m_dataManager->m_rectangles.push_back(data_rectangle);
	return true;
}

bool TranslateToData::TranslateRectCham(RectCham* rectCham, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateRectCorner(RectCorner* rectCorner, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateRectRound(RectRound* rectRound, const VECTOR2D& location)
{
	//SHAPE_POLY_SET polySet;
	//double width = rectRound->width;
	//double height = rectRound->height;
	//double radius = rectRound->radius;
	//if (abs(width - radius * 2) < 0.0001) {
	//	if (rectRound->lowerLeft == true) {
	//		polySet.Append(-width / 2 + radius, -height / 2);
	//		SHAPE_ARC arc;
	//		arc.ConstructFromStartEndCenter({ -width / 2 + radius, -height / 2 }, { -width / 2, -height / 2 + radius }, { -width / 2, -height / 2 });
	//		polySet.Append(arc);
	//	}
	//	else
	//		polySet.Append(-width / 2, -height / 2);
	//	if (rectRound->upperLeft) {
	//		polySet.Append(-width / 2, height / 2 - radius);
	//		SHAPE_ARC arc;
	//		arc.ConstructFromStartEndCenter({ -width / 2, height / 2 - radius }, { -width / 2 + radius, height / 2 }, { -width / 2 + radius, -height / 2 + radius });
	//		polySet.Append(arc);
	//	}
	//	else
	//		polySet.Append(-width / 2, height / 2);

	//	if (rectRound->upperRight) {
	//		polySet.Append(width / 2 - radius, height / 2);
	//		SHAPE_ARC arc;
	//		arc.ConstructFromStartEndCenter({ width / 2 - radius, height / 2 }, { width / 2, height / 2 - radius }, { width / 2 - radius, height / 2 - radius });
	//		polySet.Append(arc);
	//	}
	//	else
	//		polySet.Append(width / 2, height / 2);

	//	if (rectRound->lowerRight) {
	//		polySet.Append(width / 2, -height / 2 + radius);
	//		SHAPE_ARC arc;
	//		arc.ConstructFromStartEndCenter({ width / 2, -height / 2 + radius }, { width / 2 - radius, -height / 2 }, { width / 2 - radius, -height / 2 + radius });
	//		polySet.Append(arc);
	//	}
	//	else
	//		polySet.Append(width / 2, -height / 2);

	//	if (rectRound->lowerLeft)
	//		polySet.Append(-width / 2 + radius, -height / 2);
	//	else
	//		polySet.Append(-width / 2, -height / 2);
	//}


	return true;
}

bool TranslateToData::TranslateThermal(Thermal* thermal, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateTriangle(Triangle* triangle, const VECTOR2D& location)
{
	return false;
}

bool TranslateToData::TranslateUserSpecial(Shape shape, const VECTOR2D& location)
{
	UserSpecial* userSpecial = &m_ecad->m_standardShape->m_userspecial[shape.index];
	for (auto shape : userSpecial->simpleShape) {
		TranslateShape(shape, location);
	}

	return true;
}

bool TranslateToData::TranslatePolygon(Polygon* polygon, const VECTOR2D& location)
{
	VECTOR2D prePoint = { polygon->polyBegin.x, polygon->polyBegin.y };
	prePoint += location;
	std::vector<KIGFX::Segment> segments;

	for (auto polyStep : polygon->polyStep) {
		if (std::holds_alternative<PolyStepSegment>(polyStep)) {
			auto& segment = std::get<PolyStepSegment>(polyStep);
			VECTOR2D point = { segment.x, segment.y };
			point += location;
			SHAPE_SEGMENT shape_line{ m_view->ToWorld(prePoint), m_view->ToWorld(point) };
			segments.push_back(shape_line);
			prePoint = point;
		}
		else {
			auto& curve = std::get<PolyStepCurve>(polyStep);
			// Approximate the curve with a line for simplicity
			SHAPE_ARC shape_arc;
			VECTOR2D endPoint = { curve.x, curve.y };
			endPoint += location;
			VECTOR2D centerPoint = { curve.centerX, curve.centerY };
			centerPoint += location;
			shape_arc.ConstructFromStartEndCenter(prePoint, endPoint, centerPoint);
			segments.push_back(shape_arc);
			prePoint = endPoint;
		}
	}
	double lineWidth = polygon->lineDesc ? polygon->lineDesc->lineWidth : 0;
	KIGFX::DATA_Polygon poly(segments, m_currentLayer, m_view->ToWorld(lineWidth));
	m_dataManager->m_polygons.push_back(poly);

	return true;
}

bool TranslateToData::TranslateCutout(Cutout* cutout, const VECTOR2D& location)
{
	VECTOR2D prePoint = { cutout->polyBegin.x, cutout->polyBegin.y };
	prePoint += location;
	std::vector<KIGFX::Segment> segments;

	for (auto polyStep : cutout->polyStep) {
		if (std::holds_alternative<PolyStepSegment>(polyStep)) {
			auto& segment = std::get<PolyStepSegment>(polyStep);
			VECTOR2D point = { segment.x, segment.y };
			point += location;
			SHAPE_SEGMENT shape_line{ m_view->ToWorld(prePoint), m_view->ToWorld(point) };
			segments.push_back(shape_line);
			prePoint = point;
		}
		else {
			auto& curve = std::get<PolyStepCurve>(polyStep);
			// Approximate the curve with a line for simplicity
			SHAPE_ARC shape_arc;
			VECTOR2D endPoint = { curve.x, curve.y };
			endPoint += location;
			VECTOR2D centerPoint = { curve.centerX, curve.centerY };
			centerPoint += location;
			shape_arc.ConstructFromStartEndCenter(prePoint, endPoint, centerPoint);
			segments.push_back(shape_arc);
			prePoint = endPoint;
		}
	}
	double lineWidth = cutout->lineDesc ? cutout->lineDesc->lineWidth : 0;
	KIGFX::DATA_Polygon poly(segments, m_currentLayer, m_view->ToWorld(lineWidth));
	m_dataManager->m_polygons.push_back(poly);

	return true;
}