#include "gal/include/painter.hxx"

using namespace KIGFX;

PAINTER::PAINTER(KIGFX::GAL* gal) :
	m_gal(gal) 
{}

PAINTER::~PAINTER() {}

void PAINTER::DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint) {
	m_gal->DrawLine(aStartPoint, aEndPoint);
}