#include "view_control.hxx"

constexpr double defaultZoomScale = 0.005;


ViewControler::ViewControler(KIGFX::OPENGL_GAL* aGal, KIGFX::VIEW* aView, KIGFX::PAINTER* aPainter)
	: m_gal(aGal),
	  m_view(aView),
	  m_painter(aPainter),
	  m_cursorPos(0, 0),
	  m_updateCursor(true),
	  m_scrollScale(1.0, 1.0)
{
}

double ViewControler::GetScaleFroRotation(int aRotation) {
	aRotation = (aRotation > 0) ? std::min(aRotation, 100) : std::max(aRotation, -100);

	double dscale = aRotation * defaultZoomScale;

	double zoom_scale = (aRotation > 0) ? (1 + dscale) : 1 / (1 - dscale);

	return zoom_scale;
}

void ViewControler::onWheel(QWheelEvent* aEvent)
{
	const double wheelPanSpeed = 0.001;
	const bool shift = aEvent->modifiers() & Qt::ShiftModifier;
	const bool ctrl = aEvent->modifiers() & Qt::ControlModifier;
	const bool alt = aEvent->modifiers() & Qt::AltModifier;

	const QPoint numDegrees = aEvent->angleDelta();
	double rotation = numDegrees.y();
	if (alt)
		rotation = numDegrees.x() * -1;

	if (ctrl) {
		const double zoomScale = GetScaleFroRotation(rotation);
		m_view->SetScale(m_view->GetScale() * zoomScale);
	} else {
		// Scrolling
		VECTOR2D scrollVec = m_view->ToWorld(m_view->GetScreenPixelSize(), false)
			* (rotation * wheelPanSpeed);
		double scrollX = 0.0;
		double scrollY = 0.0;
		bool   hReverse = false;
		if (alt)
			scrollX = scrollVec.x;
		else
			scrollY = -scrollVec.y;

		VECTOR2D delta(scrollX, scrollY);

		m_view->SetCenter(m_view->GetCenter() + delta);
	}
}