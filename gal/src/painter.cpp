#include "gal/include/painter.hxx"


Painter::Painter(KIGFX::GAL* gal) :
	m_gal(gal) 
{}

Painter::~Painter() {}

void Painter::DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint) {
	m_gal->DrawLine(aStartPoint, aEndPoint);
}