#include "data_polygon.hxx"
#include "data_vw_lod.hxx"
#include "view.hxx"

namespace
{
SHAPE_LINE_CHAIN buildPolygonLineChain(const std::vector<MINI::Segment>& aSegments, int aTolerance)
{
    SHAPE_LINE_CHAIN chain;
    bool             hasPoint = false;

    for(const MINI::Segment& segment : aSegments)
    {
        if(std::holds_alternative<SHAPE_SEGMENT>(segment))
        {
            const SHAPE_SEGMENT& shapeSegment = std::get<SHAPE_SEGMENT>(segment);

            if(!hasPoint)
            {
                chain.Append(shapeSegment.GetSeg().A);
                hasPoint = true;
            }

            chain.Append(shapeSegment.GetSeg().B);
            continue;
        }

        const SHAPE_ARC&       arc = std::get<SHAPE_ARC>(segment);
        const SHAPE_LINE_CHAIN arcChain = arc.ConvertToPolyline(aTolerance);

        for(int idx = 0; idx < arcChain.PointCount(); ++idx)
        {
            const VECTOR2I& point = arcChain.CPoint(idx);

            if(!hasPoint)
            {
                chain.Append(point);
                hasPoint = true;
            }
            else
            {
                chain.Append(point);
            }
        }
    }

    if(chain.PointCount() >= 3)
        chain.SetClosed(true);

    return chain;
}
} // namespace

MINI::DATA_Polygon::DATA_Polygon(std::vector<Segment> points, double lineWidth) :
        BOARD_ITEM(ITEM_TYPE::POLYGON),
        m_segments(std::move(points)),
        m_lineWidth(lineWidth)
{
}

MINI::DATA_Polygon::DATA_Polygon(std::vector<Segment> points, PCB_LAYER_ID aLayer, double lineWidth) :
        BOARD_ITEM(ITEM_TYPE::POLYGON, aLayer),
        m_segments(std::move(points)),
        m_lineWidth(lineWidth)
{
}

const BOX2I MINI::DATA_Polygon::GetBoundingBox() const
{
    BOX2I box;
    for(auto Segment : m_segments)
    {
        if(std::holds_alternative<SHAPE_SEGMENT>(Segment))
        {
            auto segment = std::get<SHAPE_SEGMENT>(Segment);
            box.Merge(segment.BBox());
        }
        else if(std::holds_alternative<SHAPE_ARC>(Segment))
        {
            auto arc = std::get<SHAPE_ARC>(Segment);
            box.Merge(arc.BBox());
        }
    }

    return box;
}

double MINI::DATA_Polygon::ViewGetLOD(int aLayer, const VIEW* aView) const
{
    (void) aLayer;
    (void) aView;

    return LOD_SHOW;
}

const SHAPE_LINE_CHAIN* MINI::DATA_Polygon::GetLodPolyline(const VIEW* aView) const
{
    const int tolerance = DATA_VW_LOD::GetTolerance(aView);

    if(tolerance <= 0)
        return nullptr;

    if(m_lodTolerance != tolerance)
    {
        SHAPE_LINE_CHAIN polygonLineChain = buildPolygonLineChain(m_segments, tolerance);

        if(polygonLineChain.PointCount() < 3)
        {
            m_lodPolyline = SHAPE_LINE_CHAIN();
        }
        else
        {
            m_lodPolyline = DATA_VW_LOD::SimplifyLineChain(polygonLineChain, tolerance, true);
        }

        m_lodTolerance = tolerance;
    }

    if(m_lodPolyline.PointCount() < 3)
        return nullptr;

    return &m_lodPolyline;
}
