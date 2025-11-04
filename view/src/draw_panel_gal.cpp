#include "draw_panel_gal.hxx"
#include "geometry_utils.hxx"

DrawPanelGal::DrawPanelGal(QWidget* parent)
	: QAbstractScrollArea(parent),
	  m_gal(nullptr),
	  m_view(nullptr),
	  m_painter(nullptr),
	  m_backend(GAL_TYPE_NONE)
{

}

DrawPanelGal::~DrawPanelGal()
{
	delete m_view;
	delete m_gal;
	m_view = nullptr;
	m_gal = nullptr;    // Ensure OnShow is not called
}

void DrawPanelGal::paintEvent(QPaintEvent* event)
{
	if (!m_gal->IsInitialized() || !m_gal->IsVisible() || m_gal->IsContextLocked())
		return;

	KIGFX::GAL_DRAWING_CONTEXT ctx(m_gal);

	if (m_backend == GAL_TYPE_OPENGL)
		m_gal->ClearScreen();
	if (m_view->IsDirty())
		m_view->Redraw();
	
	
	QPoint qcursor = QCursor::pos();
	VECTOR2D cursor = { (double)qcursor.x(), (double)qcursor.y() };
	cursor = GetClampedCoords(m_gal->GetGridPoint(m_view->ToWorld(cursor)));
	m_gal->DrawCursor(cursor);

}