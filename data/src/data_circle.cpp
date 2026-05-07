#include "data_circle.hxx"
#include "view.hxx"

#include <cmath>

using namespace MINI;

DATA_Circle::DATA_Circle(SHAPE_CIRCLE circle, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::CIRCLE),
	  m_circle(circle),
	  m_lineWidth(lineWidth) {}

MINI::DATA_Circle::DATA_Circle(SHAPE_CIRCLE circle, PCB_LAYER_ID aLayer, double lineWidth)
	: BOARD_ITEM(ITEM_TYPE::CIRCLE, aLayer),
	  m_circle(circle),
	  m_lineWidth(lineWidth) {}

const BOX2I DATA_Circle::GetBoundingBox() const
{
	return m_circle.BBox();
}

double DATA_Circle::ViewGetLOD(int aLayer, const VIEW* aView) const
{
	(void) aLayer;

	if (!aView || aView->GetPainter()->GetSettings()->IsPrinting())
		return LOD_SHOW;

	const double outerRadius = std::max(0.0, m_circle.GetRadius() + m_lineWidth * 0.5);
	const double screenRadius = std::abs(aView->ToScreen(outerRadius));

	if (screenRadius < 1.0)
		return LOD_HIDE;

	return LOD_SHOW;
}

