#pragma once
#include "data_circle.hxx"
#include "data_line.hxx"
#include "data_rectangle.hxx"
#include "data_triangle.hxx"
#include "data_polyline.hxx"
#include "data_arc.hxx"
#include "data_polygon.hxx"
#include "data_poly_set.hxx"

class KIGFX::VIEW;

class DataManager {
public:
	DataManager() = default;
	void GenerateData(KIGFX::VIEW* view);
	std::vector<KIGFX::DATA_Circle>		m_circles;
	std::vector<KIGFX::DATA_Line>		m_lines;
	std::vector<KIGFX::DATA_Rectangle>	m_rectangles;
	std::vector<KIGFX::DATA_Triangle>	m_triangles;
	std::vector<KIGFX::DATA_Polyline>	m_polylines;
	std::vector<KIGFX::DATA_Polygon>	m_polygons;
	std::vector<KIGFX::DATA_Arc>		m_arcs;
	std::vector<KIGFX::DATA_PolySet>	m_polySet;
};