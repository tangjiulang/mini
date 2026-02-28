#include "gal/include/painter.hxx"

using namespace MINI;

PAINTER::PAINTER(MINI::GAL* gal, bool isCached) :
	m_gal(gal) 
{
	OPENGL_GAL* openglGal = dynamic_cast<OPENGL_GAL*>(m_gal);
	m_insertVertex = new INSERT_VERTEX(openglGal->GetCurrentManager(), isCached);
}

PAINTER::~PAINTER() {}