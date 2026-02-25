#pragma once
#include <QEvent>


#include "gal/include/opengl_gal.hxx"
#include "view.hxx"
#include "gal/include/painter.hxx"


class ViewControler {
public:
	ViewControler(MINI::OPENGL_GAL* aGal, MINI::VIEW* aView, MINI::PAINTER* aPainter);

	/// Handler functions
	void onWheel(QWheelEvent* aEvent);
	void onMotion(QMouseEvent* aEvent);
	void onMagnify(QMouseEvent* aEvent);
	void onButton(QMouseEvent* aEvent);

private:

	double GetScaleFroRotation(int aRotation);

	MINI::OPENGL_GAL*	m_gal;
	MINI::VIEW*		m_view;
	MINI::PAINTER*		m_painter;

	/// Ratio used for scaling world coordinates to scrollbar position.
	VECTOR2D m_scrollScale;

	/// Current scrollbar position.
	VECTOR2I m_scrollPos;

	/// Current cursor position (world coordinates).
	VECTOR2D m_cursorPos;

	/// Flag deciding whether the cursor position should be calculated using the mouse position.
	bool m_updateCursor;
};