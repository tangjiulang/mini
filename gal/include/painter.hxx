#ifndef PAINTER_H_
#define PAINTER_H_

#include "gal/include/opengl_gal.hxx"


class Painter {
public:
	Painter(KIGFX::GAL* gal);
	virtual ~Painter();
	void DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint);
protected:
	KIGFX::GAL* m_gal;
};

#endif PAINTER_H_