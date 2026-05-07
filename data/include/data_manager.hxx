#pragma once
#include "data_circle.hxx"
#include "data_line.hxx"
#include "data_line_batch.hxx"
#include "data_rectangle.hxx"
#include "data_triangle.hxx"
#include "data_polyline.hxx"
#include "data_arc.hxx"
#include "data_polygon.hxx"
#include "data_poly_set.hxx"
#include "data_text.hxx"

namespace MINI
{
class VIEW;
}

class DataManager
{
public:
    DataManager() = default;
    void                              GenerateData(MINI::VIEW* view);
    void                              GenerateLineBatchTestData(MINI::VIEW* aView);
    void                              GeneratePolygonTestData(MINI::VIEW* aView);
    std::vector<MINI::DATA_Circle>    m_circles;
    std::vector<MINI::DATA_Line>      m_lines;
    std::vector<MINI::DATA_LineBatch> m_lineBatches;
    std::vector<MINI::DATA_Rectangle> m_rectangles;
    std::vector<MINI::DATA_Triangle>  m_triangles;
    std::vector<MINI::DATA_Polyline>  m_polylines;
    std::vector<MINI::DATA_Polygon>   m_polygons;
    std::vector<MINI::DATA_Arc>       m_arcs;
    std::vector<MINI::DATA_PolySet>   m_polySet;
    std::vector<MINI::DATA_Text>      m_texts;
};
