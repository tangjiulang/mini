#pragma once
#include "data_circle.hxx"
#include "data_line.hxx"
#include "data_rectangle.hxx"
#include "data_triangle.hxx"
#include "data_polygon.hxx"

class DataManager {
public:
	DataManager() = default;
	void GenerateData();
	std::vector<KIGFX::DATA_Circle>		m_circles;
	std::vector<KIGFX::DATA_Line>		m_lines;
	std::vector<KIGFX::DATA_Rectangle>	m_rectangles;
	std::vector<KIGFX::DATA_Triangle>	m_triangles;
	std::vector<KIGFX::DATA_Polygon>	m_polygons;
};