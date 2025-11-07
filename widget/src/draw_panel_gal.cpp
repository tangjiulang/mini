#include "draw_panel_gal.hxx"
#include "geometry_utils.hxx"
#include "data_painter.hxx"
#include "data_manager.hxx"

// Scale limits for zoom (especially mouse wheel) for Data
#define ZOOM_MAX_LIMIT_DATA 50000
#define ZOOM_MIN_LIMIT_DATA 0.1

DrawPanelGal::DrawPanelGal(QWidget* parent, QSize aSize, GAL_TYPE aGalType)
	: QAbstractScrollArea(parent),
	  m_gal(nullptr),
	  m_view(nullptr),
	  m_painter(nullptr),
	  m_backend(GAL_TYPE_NONE)
{
	SwitchBackend(aGalType);
	m_view = new KIGFX::VIEW;
	m_view->SetGAL(m_gal);

	m_painter = std::make_unique<KIGFX::DATA_PAINTER>(m_gal);
	m_view->SetPainter(m_painter.get());

	// This fixes the zoom in and zoom out limits:
	m_view->SetScaleLimits(ZOOM_MAX_LIMIT_DATA, ZOOM_MIN_LIMIT_DATA);

	for (int i = 0; i < KIGFX::VIEW::VIEW_MAX_LAYERS; i++)
		m_view->SetLayerTarget(i, KIGFX::TARGET_NONCACHED);

	qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
	m_gal->show();
	m_gal->SetScreenDPI(dpi);

	m_control = new ViewControler(m_gal, m_view, m_painter.get());
}

DrawPanelGal::~DrawPanelGal()
{
	delete m_view;
	delete m_gal;
	m_view = nullptr;
	m_gal = nullptr;    // Ensure OnShow is not called
}

void DrawPanelGal::Paint(QPaintEvent* event)
{
	if (!m_gal->IsInitialized() || !m_gal->IsVisible() || m_gal->IsContextLocked())
		return;

	KIGFX::GAL_DRAWING_CONTEXT ctx(m_gal);

	if (m_view->IsDirty()) {
		m_view->Redraw();
	}
	
	
	QPoint qcursor = QCursor::pos();
	VECTOR2D cursor = { (double)qcursor.x(), (double)qcursor.y() };
	cursor = GetClampedCoords(m_gal->GetGridPoint(m_view->ToWorld(cursor)));
	m_gal->DrawCursor(cursor);

	m_gal->update();

}

void DrawPanelGal::resizeEvent(QResizeEvent* event)
{
	QSize viewSize = event->size();

	VECTOR2D bottom(0, 0);

	if (m_view)
		bottom = m_view->ToWorld(m_gal->GetScreenPixelSize(), true);
	m_gal->ResizeScreen(viewSize.width(), viewSize.height());
	m_gal->ComputeWorldScreenMatrix();

	if (m_view) {
		m_view->MarkTargetDirty(KIGFX::TARGET_CACHED);
		m_view->MarkTargetDirty(KIGFX::TARGET_NONCACHED);
	}

}


bool DrawPanelGal::SwitchBackend(GAL_TYPE aGalType)
{
	if (aGalType == m_backend && m_gal != nullptr)
		return true;

	VECTOR2D grid_size = m_gal ? m_gal->GetGridSize() : VECTOR2D();
	bool     grid_visibility = m_gal ? m_gal->GetGridVisibility() : true;
	bool     result = true; // assume everything will be fine


	KIGFX::OPENGL_GAL* new_gal = nullptr;
	if (aGalType == GAL_TYPE::GAL_TYPE_OPENGL) {
		KIGFX::OPENGL_GAL::CheckFeatures(m_options);
		new_gal = new KIGFX::OPENGL_GAL(m_options, this);
	}

	if (m_gal)
		delete m_gal;
	m_gal = new_gal;

	m_gal->ResizeScreen(this->size().height(), this->size().width());

	if (m_painter)
		m_painter->SetGAL(m_gal);

	if (m_view)
	{
		m_view->SetGAL(m_gal);
		// Note: OpenGL requires reverse draw order when draw priority is enabled
		m_view->ReverseDrawOrder(aGalType == GAL_TYPE_OPENGL);
	}

	m_backend = aGalType;

}

void DrawPanelGal::InitialViewData(DataManager* data)
{
	KIGFX::GAL_UPDATE_CONTEXT ctx(m_gal);

	m_gal->SetLineWidth(m_view->ToWorld(1));

	for (auto &circle : data->m_circles) {
		circle.m_centerPoint = m_view->ToWorld(circle.m_centerPoint);
		circle.m_radius = m_view->ToWorld(circle.m_radius);
		m_view->Add(&circle);
	}

	for (auto& rectangle : data->m_rectangles) {
		rectangle.m_startPoint = m_view->ToWorld(rectangle.m_startPoint);
		rectangle.m_endPoint = m_view->ToWorld(rectangle.m_endPoint);
		m_view->Add(&rectangle);
	}

	m_view->MarkDirty();
}