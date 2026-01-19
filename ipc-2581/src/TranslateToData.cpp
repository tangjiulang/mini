#include "TranslateToData.hxx"

bool TranslateToData::Translate()
{
	
	for (auto component : m_ecad->m_steps[0].components) {
		VECTOR2I location = { static_cast<int32_t>(component.location.x), static_cast<int32_t>(component.location.y) };
		Package& package = m_ecad->m_steps[0].packages[component.packageRef];
		std::vector<VECTOR2I> points;
		double lineWidth = package.outline->lineDesc->lineWidth;
		VECTOR2I point = { static_cast<int32_t>(package.outline->polygon.polyBegin.x), static_cast<int32_t>(package.outline->polygon.polyBegin.y) };
		point += location;
		points.push_back(point);
		for (auto polyStep : package.outline->polygon.polyStep) {
			if (std::holds_alternative<PolyStepSegment>(polyStep)) {
				auto segment = std::get<PolyStepSegment>(polyStep);
				point = { static_cast<int32_t>(segment.x), static_cast<int32_t>(segment.y) };
				point += location;
				points.push_back(point);
			}
			else if (std::holds_alternative<PolyStepCurve>(polyStep)) {
				auto curve = std::get<PolyStepCurve>(polyStep);
				// Approximate the curve with a line for simplicity
				point = { static_cast<int32_t>(curve.x), static_cast<int32_t>(curve.y) };
				point += location;
				points.push_back(point);
			}
		}
		KIGFX::DATA_Polygon polygon(points, lineWidth);
		m_dataManager->m_polygons.push_back(polygon);

	}
	for (auto layerFeatures : m_ecad->m_steps[0].layerFeatures) {
		for (auto set : layerFeatures.sets) {
			for (auto pad : set.pads) {
				VECTOR2D location = { pad.location.x, pad.location.y };
				Shape* shape = pad.feature;
				if (shape->type == ShapeType::Standard) {
					Standard* standardShape = static_cast<Standard*>(shape);
					if (standardShape->standardType == StandardType::CIRCLE) {
						Circle* circle = static_cast<Circle*>(standardShape);
						double radius = circle->diameter / 2.0;
						double lineWidth = circle->lineDesc ? circle->lineDesc->lineWidth : 0;
						KIGFX::DATA_Circle data_circle(location, radius, lineWidth);

						m_dataManager->m_circles.push_back(data_circle);
					}
					else if (standardShape->standardType == StandardType::RECTCENTER) {
						RectCenter* rectCenter = static_cast<RectCenter*>(standardShape);

						double lineWidth = rectCenter->lineDesc ? rectCenter->lineDesc->lineWidth : 0;
						VECTOR2D startPoint = { location.x - rectCenter->width / 2.0, location.y - rectCenter->height / 2.0 };
						VECTOR2D endPoint = { location.x + rectCenter->width / 2.0, location.y + rectCenter->height / 2.0 };
						KIGFX::DATA_Rectangle data_rectangle{ startPoint, endPoint, lineWidth };
						m_dataManager->m_rectangles.push_back(data_rectangle);
					}
				}
			}
		}
	}

	return true;
}