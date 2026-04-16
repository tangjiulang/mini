#include "draw_panel_gal.hxx"
#include "geometry_utils.hxx"
#include "data_painter.hxx"
#include "data_manager.hxx"
#include "gal/include/vertex_thread_pool.hxx"
#include "data_thread_painter.hxx"

// Scale limits for zoom (especially mouse wheel) for Data
#define ZOOM_MAX_LIMIT_DATA 50000
#define ZOOM_MIN_LIMIT_DATA 0.1

const int GAL_LAYER_ORDER[] =
{
    LAYER_UI_START + 9,
    LAYER_UI_START + 8,
    LAYER_UI_START + 7,
    LAYER_UI_START + 6,
    LAYER_UI_START + 5,
    LAYER_UI_START + 4,
    LAYER_UI_START + 3,
    LAYER_UI_START + 2,
    LAYER_UI_START + 1,
    LAYER_UI_START,

    LAYER_GP_OVERLAY,
    LAYER_SELECT_OVERLAY,
    LAYER_CONFLICTS_SHADOW,

    LAYER_DRC_ERROR, LAYER_DRC_WARNING, LAYER_DRC_EXCLUSION, LAYER_MARKER_SHADOWS, LAYER_DRC_SHAPES,
    LAYER_PAD_NETNAMES, LAYER_VIA_NETNAMES,
    Dwgs_User,  ZONE_LAYER_FOR(Dwgs_User), POINT_LAYER_FOR(Dwgs_User),
    Cmts_User,  ZONE_LAYER_FOR(Cmts_User), POINT_LAYER_FOR(Cmts_User),
    Eco1_User,  ZONE_LAYER_FOR(Eco1_User), POINT_LAYER_FOR(Eco1_User),
    Eco2_User,  ZONE_LAYER_FOR(Eco2_User), POINT_LAYER_FOR(Eco2_User),
    Edge_Cuts,  ZONE_LAYER_FOR(Edge_Cuts), POINT_LAYER_FOR(Edge_Cuts),
    Margin,     ZONE_LAYER_FOR(Margin),    POINT_LAYER_FOR(Margin),

    User_1,     ZONE_LAYER_FOR(User_1),    POINT_LAYER_FOR(User_1),
    User_2,     ZONE_LAYER_FOR(User_2),    POINT_LAYER_FOR(User_2),
    User_3,     ZONE_LAYER_FOR(User_3),    POINT_LAYER_FOR(User_3),
    User_4,     ZONE_LAYER_FOR(User_4),    POINT_LAYER_FOR(User_4),
    User_5,     ZONE_LAYER_FOR(User_5),    POINT_LAYER_FOR(User_5),
    User_6,     ZONE_LAYER_FOR(User_6),    POINT_LAYER_FOR(User_6),
    User_7,     ZONE_LAYER_FOR(User_7),    POINT_LAYER_FOR(User_7),
    User_8,     ZONE_LAYER_FOR(User_8),    POINT_LAYER_FOR(User_8),
    User_9,     ZONE_LAYER_FOR(User_9),    POINT_LAYER_FOR(User_9),
    User_10,    ZONE_LAYER_FOR(User_10),   POINT_LAYER_FOR(User_10),
    User_11,    ZONE_LAYER_FOR(User_11),   POINT_LAYER_FOR(User_11),
    User_12,    ZONE_LAYER_FOR(User_12),   POINT_LAYER_FOR(User_12),
    User_13,    ZONE_LAYER_FOR(User_13),   POINT_LAYER_FOR(User_13),
    User_14,    ZONE_LAYER_FOR(User_14),   POINT_LAYER_FOR(User_14),
    User_15,    ZONE_LAYER_FOR(User_15),   POINT_LAYER_FOR(User_15),
    User_16,    ZONE_LAYER_FOR(User_16),   POINT_LAYER_FOR(User_16),
    User_17,    ZONE_LAYER_FOR(User_17),   POINT_LAYER_FOR(User_17),
    User_18,    ZONE_LAYER_FOR(User_18),   POINT_LAYER_FOR(User_18),
    User_19,    ZONE_LAYER_FOR(User_19),   POINT_LAYER_FOR(User_19),
    User_20,    ZONE_LAYER_FOR(User_20),   POINT_LAYER_FOR(User_20),
    User_21,    ZONE_LAYER_FOR(User_21),   POINT_LAYER_FOR(User_21),
    User_22,    ZONE_LAYER_FOR(User_22),   POINT_LAYER_FOR(User_22),
    User_23,    ZONE_LAYER_FOR(User_23),   POINT_LAYER_FOR(User_23),
    User_24,    ZONE_LAYER_FOR(User_24),   POINT_LAYER_FOR(User_24),
    User_25,    ZONE_LAYER_FOR(User_25),   POINT_LAYER_FOR(User_25),
    User_26,    ZONE_LAYER_FOR(User_26),   POINT_LAYER_FOR(User_26),
    User_27,    ZONE_LAYER_FOR(User_27),   POINT_LAYER_FOR(User_27),
    User_28,    ZONE_LAYER_FOR(User_28),   POINT_LAYER_FOR(User_28),
    User_29,    ZONE_LAYER_FOR(User_29),   POINT_LAYER_FOR(User_29),
    User_30,    ZONE_LAYER_FOR(User_30),   POINT_LAYER_FOR(User_30),
    User_31,    ZONE_LAYER_FOR(User_31),   POINT_LAYER_FOR(User_31),
    User_32,    ZONE_LAYER_FOR(User_32),   POINT_LAYER_FOR(User_32),
    User_33,    ZONE_LAYER_FOR(User_33),   POINT_LAYER_FOR(User_33),
    User_34,    ZONE_LAYER_FOR(User_34),   POINT_LAYER_FOR(User_34),
    User_35,    ZONE_LAYER_FOR(User_35),   POINT_LAYER_FOR(User_35),
    User_36,    ZONE_LAYER_FOR(User_36),   POINT_LAYER_FOR(User_36),
    User_37,    ZONE_LAYER_FOR(User_37),   POINT_LAYER_FOR(User_37),
    User_38,    ZONE_LAYER_FOR(User_38),   POINT_LAYER_FOR(User_38),
    User_39,    ZONE_LAYER_FOR(User_39),   POINT_LAYER_FOR(User_39),
    User_40,    ZONE_LAYER_FOR(User_40),   POINT_LAYER_FOR(User_40),
    User_41,    ZONE_LAYER_FOR(User_41),   POINT_LAYER_FOR(User_41),
    User_42,    ZONE_LAYER_FOR(User_42),   POINT_LAYER_FOR(User_42),
    User_43,    ZONE_LAYER_FOR(User_43),   POINT_LAYER_FOR(User_43),
    User_44,    ZONE_LAYER_FOR(User_44),   POINT_LAYER_FOR(User_44),
    User_45,    ZONE_LAYER_FOR(User_45),   POINT_LAYER_FOR(User_45),

    POINT_LAYER_FOR(F_Cu),

    LAYER_FP_TEXT, LAYER_FP_REFERENCES, LAYER_FP_VALUES,

    LAYER_RATSNEST,
    LAYER_ANCHOR,
    LAYER_POINTS,
    LAYER_LOCKED_ITEM_SHADOW,
    LAYER_VIA_HOLES, LAYER_VIA_HOLEWALLS,
    LAYER_PAD_PLATEDHOLES, LAYER_PAD_HOLEWALLS, LAYER_NON_PLATEDHOLES,
    LAYER_VIA_THROUGH, LAYER_VIA_BLIND, LAYER_VIA_BURIED, LAYER_VIA_MICROVIA,

    LAYER_PAD_FR_NETNAMES,
    NETNAMES_LAYER_INDEX(F_Cu),
    PAD_COPPER_LAYER_FOR(F_Cu),
    VIA_COPPER_LAYER_FOR(F_Cu),
    CLEARANCE_LAYER_FOR(F_Cu),
    // POINT_LAYER_FOR( F_Cu ),
    F_Cu, ZONE_LAYER_FOR(F_Cu),
    F_Mask, ZONE_LAYER_FOR(F_Mask),
    F_SilkS, ZONE_LAYER_FOR(F_SilkS),
    F_Paste, ZONE_LAYER_FOR(F_Paste),
    F_Adhes, ZONE_LAYER_FOR(F_Adhes),
    F_CrtYd, ZONE_LAYER_FOR(F_CrtYd),
    F_Fab, ZONE_LAYER_FOR(F_Fab),

    NETNAMES_LAYER_INDEX(In1_Cu), PAD_COPPER_LAYER_FOR(In1_Cu),
    VIA_COPPER_LAYER_FOR(In1_Cu), CLEARANCE_LAYER_FOR(In1_Cu),
    POINT_LAYER_FOR(In1_Cu),
    In1_Cu,   ZONE_LAYER_FOR(In1_Cu),
    NETNAMES_LAYER_INDEX(In2_Cu), PAD_COPPER_LAYER_FOR(In2_Cu),
    VIA_COPPER_LAYER_FOR(In2_Cu), CLEARANCE_LAYER_FOR(In2_Cu),
    POINT_LAYER_FOR(In2_Cu),
    In2_Cu,   ZONE_LAYER_FOR(In2_Cu),
    NETNAMES_LAYER_INDEX(In3_Cu), PAD_COPPER_LAYER_FOR(In3_Cu),
    VIA_COPPER_LAYER_FOR(In3_Cu), CLEARANCE_LAYER_FOR(In3_Cu),
    POINT_LAYER_FOR(In3_Cu),
    In3_Cu,   ZONE_LAYER_FOR(In3_Cu),
    NETNAMES_LAYER_INDEX(In4_Cu), PAD_COPPER_LAYER_FOR(In4_Cu),
    VIA_COPPER_LAYER_FOR(In4_Cu), CLEARANCE_LAYER_FOR(In4_Cu),
    POINT_LAYER_FOR(In4_Cu),
    In4_Cu,   ZONE_LAYER_FOR(In4_Cu),
    NETNAMES_LAYER_INDEX(In5_Cu), PAD_COPPER_LAYER_FOR(In5_Cu),
    VIA_COPPER_LAYER_FOR(In5_Cu), CLEARANCE_LAYER_FOR(In5_Cu),
    POINT_LAYER_FOR(In5_Cu),
    In5_Cu,   ZONE_LAYER_FOR(In5_Cu),
    NETNAMES_LAYER_INDEX(In6_Cu), PAD_COPPER_LAYER_FOR(In6_Cu),
    VIA_COPPER_LAYER_FOR(In6_Cu), CLEARANCE_LAYER_FOR(In6_Cu),
    POINT_LAYER_FOR(In6_Cu),
    In6_Cu,   ZONE_LAYER_FOR(In6_Cu),
    NETNAMES_LAYER_INDEX(In7_Cu), PAD_COPPER_LAYER_FOR(In7_Cu),
    VIA_COPPER_LAYER_FOR(In7_Cu), CLEARANCE_LAYER_FOR(In7_Cu),
    POINT_LAYER_FOR(In7_Cu),
    In7_Cu,   ZONE_LAYER_FOR(In7_Cu),
    NETNAMES_LAYER_INDEX(In8_Cu), PAD_COPPER_LAYER_FOR(In8_Cu),
    VIA_COPPER_LAYER_FOR(In8_Cu), CLEARANCE_LAYER_FOR(In8_Cu),
    POINT_LAYER_FOR(In8_Cu),
    In8_Cu,   ZONE_LAYER_FOR(In8_Cu),
    NETNAMES_LAYER_INDEX(In9_Cu), PAD_COPPER_LAYER_FOR(In9_Cu),
    VIA_COPPER_LAYER_FOR(In9_Cu), CLEARANCE_LAYER_FOR(In9_Cu),
    POINT_LAYER_FOR(In9_Cu),
    In9_Cu,   ZONE_LAYER_FOR(In9_Cu),
    NETNAMES_LAYER_INDEX(In10_Cu), PAD_COPPER_LAYER_FOR(In10_Cu),
    VIA_COPPER_LAYER_FOR(In10_Cu), CLEARANCE_LAYER_FOR(In10_Cu),
    POINT_LAYER_FOR(In10_Cu),
    In10_Cu,  ZONE_LAYER_FOR(In10_Cu),
    NETNAMES_LAYER_INDEX(In11_Cu), PAD_COPPER_LAYER_FOR(In11_Cu),
    VIA_COPPER_LAYER_FOR(In11_Cu), CLEARANCE_LAYER_FOR(In11_Cu),
    POINT_LAYER_FOR(In11_Cu),
    In11_Cu,  ZONE_LAYER_FOR(In11_Cu),
    NETNAMES_LAYER_INDEX(In12_Cu), PAD_COPPER_LAYER_FOR(In12_Cu),
    VIA_COPPER_LAYER_FOR(In12_Cu), CLEARANCE_LAYER_FOR(In12_Cu),
    POINT_LAYER_FOR(In12_Cu),
    In12_Cu,  ZONE_LAYER_FOR(In12_Cu),
    NETNAMES_LAYER_INDEX(In13_Cu), PAD_COPPER_LAYER_FOR(In13_Cu),
    VIA_COPPER_LAYER_FOR(In13_Cu), CLEARANCE_LAYER_FOR(In13_Cu),
    POINT_LAYER_FOR(In13_Cu),
    In13_Cu,  ZONE_LAYER_FOR(In13_Cu),
    NETNAMES_LAYER_INDEX(In14_Cu), PAD_COPPER_LAYER_FOR(In14_Cu),
    VIA_COPPER_LAYER_FOR(In14_Cu), CLEARANCE_LAYER_FOR(In14_Cu),
    POINT_LAYER_FOR(In14_Cu),
    In14_Cu,  ZONE_LAYER_FOR(In14_Cu),
    NETNAMES_LAYER_INDEX(In15_Cu), PAD_COPPER_LAYER_FOR(In15_Cu),
    VIA_COPPER_LAYER_FOR(In15_Cu), CLEARANCE_LAYER_FOR(In15_Cu),
    POINT_LAYER_FOR(In15_Cu),
    In15_Cu,  ZONE_LAYER_FOR(In15_Cu),
    NETNAMES_LAYER_INDEX(In16_Cu), PAD_COPPER_LAYER_FOR(In16_Cu),
    VIA_COPPER_LAYER_FOR(In16_Cu), CLEARANCE_LAYER_FOR(In16_Cu),
    POINT_LAYER_FOR(In16_Cu),
    In16_Cu,  ZONE_LAYER_FOR(In16_Cu),
    NETNAMES_LAYER_INDEX(In17_Cu), PAD_COPPER_LAYER_FOR(In17_Cu),
    VIA_COPPER_LAYER_FOR(In17_Cu), CLEARANCE_LAYER_FOR(In17_Cu),
    POINT_LAYER_FOR(In17_Cu),
    In17_Cu,  ZONE_LAYER_FOR(In17_Cu),
    NETNAMES_LAYER_INDEX(In18_Cu), PAD_COPPER_LAYER_FOR(In18_Cu),
    VIA_COPPER_LAYER_FOR(In18_Cu), CLEARANCE_LAYER_FOR(In18_Cu),
    POINT_LAYER_FOR(In18_Cu),
    In18_Cu,  ZONE_LAYER_FOR(In18_Cu),
    NETNAMES_LAYER_INDEX(In19_Cu), PAD_COPPER_LAYER_FOR(In19_Cu),
    VIA_COPPER_LAYER_FOR(In19_Cu), CLEARANCE_LAYER_FOR(In19_Cu),
    POINT_LAYER_FOR(In19_Cu),
    In19_Cu,  ZONE_LAYER_FOR(In19_Cu),
    NETNAMES_LAYER_INDEX(In20_Cu), PAD_COPPER_LAYER_FOR(In20_Cu),
    VIA_COPPER_LAYER_FOR(In20_Cu), CLEARANCE_LAYER_FOR(In20_Cu),
    POINT_LAYER_FOR(In20_Cu),
    In20_Cu,  ZONE_LAYER_FOR(In20_Cu),
    NETNAMES_LAYER_INDEX(In21_Cu), PAD_COPPER_LAYER_FOR(In21_Cu),
    VIA_COPPER_LAYER_FOR(In21_Cu), CLEARANCE_LAYER_FOR(In21_Cu),
    POINT_LAYER_FOR(In21_Cu),
    In21_Cu,  ZONE_LAYER_FOR(In21_Cu),
    NETNAMES_LAYER_INDEX(In22_Cu), PAD_COPPER_LAYER_FOR(In22_Cu),
    VIA_COPPER_LAYER_FOR(In22_Cu), CLEARANCE_LAYER_FOR(In22_Cu),
    POINT_LAYER_FOR(In22_Cu),
    In22_Cu,  ZONE_LAYER_FOR(In22_Cu),
    NETNAMES_LAYER_INDEX(In23_Cu), PAD_COPPER_LAYER_FOR(In23_Cu),
    VIA_COPPER_LAYER_FOR(In23_Cu), CLEARANCE_LAYER_FOR(In23_Cu),
    POINT_LAYER_FOR(In23_Cu),
    In23_Cu,  ZONE_LAYER_FOR(In23_Cu),
    NETNAMES_LAYER_INDEX(In24_Cu), PAD_COPPER_LAYER_FOR(In24_Cu),
    VIA_COPPER_LAYER_FOR(In24_Cu), CLEARANCE_LAYER_FOR(In24_Cu),
    POINT_LAYER_FOR(In24_Cu),
    In24_Cu,  ZONE_LAYER_FOR(In24_Cu),
    NETNAMES_LAYER_INDEX(In25_Cu), PAD_COPPER_LAYER_FOR(In25_Cu),
    VIA_COPPER_LAYER_FOR(In25_Cu), CLEARANCE_LAYER_FOR(In25_Cu),
    POINT_LAYER_FOR(In25_Cu),
    In25_Cu,  ZONE_LAYER_FOR(In25_Cu),
    NETNAMES_LAYER_INDEX(In26_Cu), PAD_COPPER_LAYER_FOR(In26_Cu),
    VIA_COPPER_LAYER_FOR(In26_Cu), CLEARANCE_LAYER_FOR(In26_Cu),
    POINT_LAYER_FOR(In26_Cu),
    In26_Cu,  ZONE_LAYER_FOR(In26_Cu),
    NETNAMES_LAYER_INDEX(In27_Cu), PAD_COPPER_LAYER_FOR(In27_Cu),
    VIA_COPPER_LAYER_FOR(In27_Cu), CLEARANCE_LAYER_FOR(In27_Cu),
    POINT_LAYER_FOR(In27_Cu),
    In27_Cu,  ZONE_LAYER_FOR(In27_Cu),
    NETNAMES_LAYER_INDEX(In28_Cu), PAD_COPPER_LAYER_FOR(In28_Cu),
    VIA_COPPER_LAYER_FOR(In28_Cu), CLEARANCE_LAYER_FOR(In28_Cu),
    POINT_LAYER_FOR(In28_Cu),
    In28_Cu,  ZONE_LAYER_FOR(In28_Cu),
    NETNAMES_LAYER_INDEX(In29_Cu), PAD_COPPER_LAYER_FOR(In29_Cu),
    VIA_COPPER_LAYER_FOR(In29_Cu), CLEARANCE_LAYER_FOR(In29_Cu),
    POINT_LAYER_FOR(In29_Cu),
    In29_Cu,  ZONE_LAYER_FOR(In29_Cu),
    NETNAMES_LAYER_INDEX(In30_Cu), PAD_COPPER_LAYER_FOR(In30_Cu),
    VIA_COPPER_LAYER_FOR(In30_Cu), CLEARANCE_LAYER_FOR(In30_Cu),
    POINT_LAYER_FOR(In30_Cu),
    In30_Cu,  ZONE_LAYER_FOR(In30_Cu),

    LAYER_PAD_BK_NETNAMES,
    NETNAMES_LAYER_INDEX(B_Cu),
    PAD_COPPER_LAYER_FOR(B_Cu),
    VIA_COPPER_LAYER_FOR(B_Cu),
    CLEARANCE_LAYER_FOR(B_Cu),
    POINT_LAYER_FOR(B_Cu),
    B_Cu, ZONE_LAYER_FOR(B_Cu),
    B_Mask, ZONE_LAYER_FOR(B_Mask),
    B_SilkS, ZONE_LAYER_FOR(B_SilkS),
    B_Paste, ZONE_LAYER_FOR(B_Paste),
    B_Adhes, ZONE_LAYER_FOR(B_Adhes),
    B_CrtYd, ZONE_LAYER_FOR(B_CrtYd),
    B_Fab, ZONE_LAYER_FOR(B_Fab),

    BITMAP_LAYER_FOR(Dwgs_User),
    BITMAP_LAYER_FOR(Cmts_User),
    BITMAP_LAYER_FOR(Eco1_User), BITMAP_LAYER_FOR(Eco2_User),
    BITMAP_LAYER_FOR(Edge_Cuts), BITMAP_LAYER_FOR(Margin),

    BITMAP_LAYER_FOR(User_1),
    BITMAP_LAYER_FOR(User_2),
    BITMAP_LAYER_FOR(User_3),
    BITMAP_LAYER_FOR(User_4),
    BITMAP_LAYER_FOR(User_5),
    BITMAP_LAYER_FOR(User_6),
    BITMAP_LAYER_FOR(User_7),
    BITMAP_LAYER_FOR(User_8),
    BITMAP_LAYER_FOR(User_9),
    BITMAP_LAYER_FOR(User_10),
    BITMAP_LAYER_FOR(User_11),
    BITMAP_LAYER_FOR(User_12),
    BITMAP_LAYER_FOR(User_13),
    BITMAP_LAYER_FOR(User_14),
    BITMAP_LAYER_FOR(User_15),
    BITMAP_LAYER_FOR(User_16),
    BITMAP_LAYER_FOR(User_17),
    BITMAP_LAYER_FOR(User_18),
    BITMAP_LAYER_FOR(User_19),
    BITMAP_LAYER_FOR(User_20),
    BITMAP_LAYER_FOR(User_21),
    BITMAP_LAYER_FOR(User_22),
    BITMAP_LAYER_FOR(User_23),
    BITMAP_LAYER_FOR(User_24),
    BITMAP_LAYER_FOR(User_25),
    BITMAP_LAYER_FOR(User_26),
    BITMAP_LAYER_FOR(User_27),
    BITMAP_LAYER_FOR(User_28),
    BITMAP_LAYER_FOR(User_29),
    BITMAP_LAYER_FOR(User_30),
    BITMAP_LAYER_FOR(User_31),
    BITMAP_LAYER_FOR(User_32),
    BITMAP_LAYER_FOR(User_33),
    BITMAP_LAYER_FOR(User_34),
    BITMAP_LAYER_FOR(User_35),
    BITMAP_LAYER_FOR(User_36),
    BITMAP_LAYER_FOR(User_37),
    BITMAP_LAYER_FOR(User_38),
    BITMAP_LAYER_FOR(User_39),
    BITMAP_LAYER_FOR(User_40),
    BITMAP_LAYER_FOR(User_41),
    BITMAP_LAYER_FOR(User_42),
    BITMAP_LAYER_FOR(User_43),
    BITMAP_LAYER_FOR(User_44),
    BITMAP_LAYER_FOR(User_45),

    BITMAP_LAYER_FOR(F_Cu),
    BITMAP_LAYER_FOR(F_Mask),
    BITMAP_LAYER_FOR(F_SilkS),
    BITMAP_LAYER_FOR(F_Paste),
    BITMAP_LAYER_FOR(F_Adhes),
    BITMAP_LAYER_FOR(F_CrtYd),
    BITMAP_LAYER_FOR(F_Fab),

    BITMAP_LAYER_FOR(In1_Cu),
    BITMAP_LAYER_FOR(In2_Cu),
    BITMAP_LAYER_FOR(In3_Cu),
    BITMAP_LAYER_FOR(In4_Cu),
    BITMAP_LAYER_FOR(In5_Cu),
    BITMAP_LAYER_FOR(In6_Cu),
    BITMAP_LAYER_FOR(In7_Cu),
    BITMAP_LAYER_FOR(In8_Cu),
    BITMAP_LAYER_FOR(In9_Cu),
    BITMAP_LAYER_FOR(In10_Cu),
    BITMAP_LAYER_FOR(In11_Cu),
    BITMAP_LAYER_FOR(In12_Cu),
    BITMAP_LAYER_FOR(In13_Cu),
    BITMAP_LAYER_FOR(In14_Cu),
    BITMAP_LAYER_FOR(In15_Cu),
    BITMAP_LAYER_FOR(In16_Cu),
    BITMAP_LAYER_FOR(In17_Cu),
    BITMAP_LAYER_FOR(In18_Cu),
    BITMAP_LAYER_FOR(In19_Cu),
    BITMAP_LAYER_FOR(In20_Cu),
    BITMAP_LAYER_FOR(In21_Cu),
    BITMAP_LAYER_FOR(In22_Cu),
    BITMAP_LAYER_FOR(In23_Cu),
    BITMAP_LAYER_FOR(In24_Cu),
    BITMAP_LAYER_FOR(In25_Cu),
    BITMAP_LAYER_FOR(In26_Cu),
    BITMAP_LAYER_FOR(In27_Cu),
    BITMAP_LAYER_FOR(In28_Cu),
    BITMAP_LAYER_FOR(In29_Cu),
    BITMAP_LAYER_FOR(In30_Cu),

    BITMAP_LAYER_FOR(B_Cu),
    BITMAP_LAYER_FOR(B_Mask),
    BITMAP_LAYER_FOR(B_SilkS),
    BITMAP_LAYER_FOR(B_Paste),
    BITMAP_LAYER_FOR(B_Adhes),
    BITMAP_LAYER_FOR(B_CrtYd),
    BITMAP_LAYER_FOR(B_Fab),

    LAYER_BOARD_OUTLINE_AREA,

    LAYER_DRAWINGSHEET
};

DrawPanelGal::DrawPanelGal(QWidget* parent, QSize aSize, GAL_TYPE aGalType)
	: m_parent(parent),
	  m_gal(nullptr),
	  m_view(nullptr),
	  m_painter(nullptr),
	  m_control(nullptr),
	  m_backend(GAL_TYPE_NONE)
{
	SwitchBackend(aGalType);
	m_view = new MINI::VIEW;
	m_view->SetGAL(m_gal);

	m_painter = std::make_unique<MINI::DATA_PAINTER>(m_gal);
	m_view->SetPainter(m_painter.get());

	// This fixes the zoom in and zoom out limits:
	m_view->SetScaleLimits(ZOOM_MAX_LIMIT_DATA, ZOOM_MIN_LIMIT_DATA);

	for (int i = 0; i < MINI::VIEW::VIEW_MAX_LAYERS; i++)
		m_view->SetLayerTarget(i, MINI::TARGET_NONCACHED);

	qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
	m_gal->show();
	m_gal->SetScreenDPI(dpi);
    m_gal->SetGridSize(VECTOR2D{ 1000, 1000 });

	m_control = new ViewControler(m_gal, m_view, m_painter.get());

    m_selectionTool.SetView(m_view);

    m_view->SetThreadAccelerate(true);

    if (m_view->GetThreadAccelerate()) {
        m_view->m_threadPool = std::make_unique<MINI::VertexThreadPool>();

        auto ids = m_view->m_threadPool->m_threadPool->get_thread_ids();

        for (int i = 0; i < m_view->m_threadPool->m_threadCount; i++) {
            m_view->m_threadPool->m_painters.push_back(new MINI::DATA_THREAD_PAINTER(m_gal));
            m_view->m_threadPool->m_threadToPainter[ids[i]] = i;
        }
    }
}

DrawPanelGal::~DrawPanelGal()
{
	delete m_control;
	delete m_view;
	delete m_gal;
	m_control = nullptr;
	m_view = nullptr;
	m_gal = nullptr;    // Ensure OnShow is not called
}

void DrawPanelGal::onWheel(QWheelEvent* event)
{
    m_selectionTool.ClearSelection();
    m_control->onWheel(event);
    m_selectionTool.SelectRectArea();
}

void DrawPanelGal::Paint()
{
	if (!m_gal->IsInitialized() || !m_gal->IsVisible() || m_gal->IsContextLocked())
		return;

    m_gal->makeCurrent();

	MINI::GAL_DRAWING_CONTEXT ctx(m_gal);

	m_gal->SetCursorEnabled(true);
	if (m_view->IsDirty()) {
        m_view->ClearTargets();
        m_gal->DrawGrid();
		m_view->Redraw();
	}
    m_gal->SetCursorColor(MINI::COLOR4D::WHITE);
	m_gal->DrawCursor(m_cursor);

    m_gal->PostPaint();
}

void DrawPanelGal::SetCursor()
{
	QPoint widgetPos = m_gal->mapFromGlobal(QCursor::pos());
	m_cursor = { (double)widgetPos.x(), m_gal->GetScreenPixelSize().y - (double)widgetPos.y()};
	m_cursor = GetClampedCoords(m_gal->GetGridPoint(m_view->ToWorld(m_cursor)));
}

void DrawPanelGal::CreateSelectRect()
{
	QPoint widgetPos = m_gal->mapFromGlobal(QCursor::pos());
    VECTOR2I pos = { widgetPos.x(), m_gal->GetScreenPixelSize().y - widgetPos.y() };
    m_selectionTool.SetOrigin(m_view->ToWorld(pos));
}

void DrawPanelGal::UpdateSelectRect()
{
	QPoint widgetPos = m_gal->mapFromGlobal(QCursor::pos());
    VECTOR2I pos = { widgetPos.x(), m_gal->GetScreenPixelSize().y - widgetPos.y() };
	m_selectionTool.SetEnd(m_view->ToWorld(pos));
}

void DrawPanelGal::DrawSelectRect()
{
    m_selectionTool.SelectRectArea();
}

void DrawPanelGal::SetDefaultLayerDeps()
{
	// caching makes no sense for Cairo and other software renderers
	auto target = MINI::TARGET_NONCACHED;

	for (int i = 0; i < MINI::VIEW::VIEW_MAX_LAYERS; i++)
		m_view->SetLayerTarget(i, target);

	for (int i = 0; (unsigned)i < sizeof(GAL_LAYER_ORDER) / sizeof(int); ++i)
	{
		int layer = GAL_LAYER_ORDER[i];
		Q_ASSERT(layer < MINI::VIEW::VIEW_MAX_LAYERS);

		// Set layer display dependencies & targets
		if (IsCopperLayer(layer))
		{
			m_view->SetRequired(ZONE_LAYER_FOR(layer), layer);
			m_view->SetRequired(PAD_COPPER_LAYER_FOR(layer), layer);
			m_view->SetRequired(VIA_COPPER_LAYER_FOR(layer), layer);
			m_view->SetRequired(CLEARANCE_LAYER_FOR(layer), layer);
			m_view->SetRequired(POINT_LAYER_FOR(layer), layer);

			m_view->SetRequired(BITMAP_LAYER_FOR(layer), layer);
			m_view->SetLayerTarget(BITMAP_LAYER_FOR(layer), MINI::TARGET_NONCACHED);
			m_view->SetRequired(GetNetnameLayer(layer), layer);
		}
		else if (IsNonCopperLayer(layer))
		{
			m_view->SetRequired(POINT_LAYER_FOR(layer), layer);
			m_view->SetRequired(ZONE_LAYER_FOR(layer), layer);
			m_view->SetLayerTarget(BITMAP_LAYER_FOR(layer), MINI::TARGET_NONCACHED);
			m_view->SetRequired(BITMAP_LAYER_FOR(layer), layer);
		}
		else if (IsNetnameLayer(layer))
		{
			m_view->SetLayerDisplayOnly(layer);
		}
	}

	m_view->SetLayerTarget(LAYER_ANCHOR, MINI::TARGET_NONCACHED);
	m_view->SetLayerDisplayOnly(LAYER_ANCHOR);

	// Use TARGET_OVERLAY for LAYER_CONFLICTS_SHADOW, it is for items
	// that may change while the view stays the same.
	m_view->SetLayerTarget(LAYER_CONFLICTS_SHADOW, MINI::TARGET_OVERLAY);

	m_view->SetLayerDisplayOnly(LAYER_LOCKED_ITEM_SHADOW);
	m_view->SetLayerDisplayOnly(LAYER_CONFLICTS_SHADOW);
	m_view->SetLayerDisplayOnly(LAYER_BOARD_OUTLINE_AREA);

	// Some more required layers settings
	m_view->SetRequired(LAYER_PAD_NETNAMES, LAYER_PADS);

	// Holes can be independent of their host objects (cf: printing drill marks)
	m_view->SetRequired(LAYER_VIA_HOLES, LAYER_VIAS);
	m_view->SetRequired(LAYER_VIA_HOLEWALLS, LAYER_VIAS);
	m_view->SetRequired(LAYER_PAD_PLATEDHOLES, LAYER_PADS);
	m_view->SetRequired(LAYER_PAD_HOLEWALLS, LAYER_PADS);
	m_view->SetRequired(LAYER_NON_PLATEDHOLES, LAYER_PADS);

	// Via visibility
	m_view->SetRequired(LAYER_VIA_MICROVIA, LAYER_VIAS);
	m_view->SetRequired(LAYER_VIA_BLIND, LAYER_VIAS);
	m_view->SetRequired(LAYER_VIA_BURIED, LAYER_VIAS);
	m_view->SetRequired(LAYER_VIA_THROUGH, LAYER_VIAS);
	m_view->SetRequired(LAYER_VIA_NETNAMES, LAYER_VIAS);

	m_view->SetLayerTarget(LAYER_SELECT_OVERLAY, MINI::TARGET_OVERLAY);
	m_view->SetLayerDisplayOnly(LAYER_SELECT_OVERLAY);
	m_view->SetLayerTarget(LAYER_GP_OVERLAY, MINI::TARGET_OVERLAY);
	m_view->SetLayerDisplayOnly(LAYER_GP_OVERLAY);
	m_view->SetLayerTarget(LAYER_RATSNEST, MINI::TARGET_OVERLAY);
	m_view->SetLayerDisplayOnly(LAYER_RATSNEST);

	m_view->SetLayerTarget(LAYER_DRC_ERROR, MINI::TARGET_OVERLAY);
	//m_view->SetLayerDisplayOnly( LAYER_DRC_ERROR );
	m_view->SetLayerTarget(LAYER_DRC_WARNING, MINI::TARGET_OVERLAY);
	//m_view->SetLayerDisplayOnly( LAYER_DRC_WARNING );
	m_view->SetLayerTarget(LAYER_DRC_EXCLUSION, MINI::TARGET_OVERLAY);
	//m_view->SetLayerDisplayOnly( LAYER_DRC_EXCLUSION );
	m_view->SetLayerTarget(LAYER_MARKER_SHADOWS, MINI::TARGET_OVERLAY);
	m_view->SetLayerDisplayOnly(LAYER_MARKER_SHADOWS);
	m_view->SetLayerTarget(LAYER_DRC_SHAPES, MINI::TARGET_OVERLAY);
	m_view->SetLayerDisplayOnly(LAYER_DRC_SHAPES);

	m_view->SetLayerTarget(LAYER_DRAWINGSHEET, MINI::TARGET_NONCACHED);
	m_view->SetLayerDisplayOnly(LAYER_DRAWINGSHEET);
	m_view->SetLayerDisplayOnly(LAYER_GRID);

	for (int i = LAYER_UI_START; i < LAYER_UI_END; ++i)
	{
		m_view->SetLayerTarget(i, MINI::TARGET_OVERLAY);
		m_view->SetLayerDisplayOnly(i);
	}
}

void DrawPanelGal::ResizeEvent(QResizeEvent* event)
{
	QSize viewSize = event->size();

	VECTOR2D bottom(0, 0);
	
	if (m_view)
		bottom = m_view->ToWorld(m_gal->GetScreenPixelSize(), true);
	m_gal->ResizeScreen(viewSize.width(), viewSize.height());
	m_gal->ComputeWorldScreenMatrix();

	if (m_view) {
		m_view->MarkTargetDirty(MINI::TARGET_CACHED);
		m_view->MarkTargetDirty(MINI::TARGET_NONCACHED);
        m_view->MarkTargetDirty(MINI::TARGET_OVERLAY);
	}

}


bool DrawPanelGal::SwitchBackend(GAL_TYPE aGalType)
{
	if (aGalType == m_backend && m_gal != nullptr)
		return true;

	VECTOR2D grid_size = m_gal ? m_gal->GetGridSize() : VECTOR2D();
	bool     grid_visibility = m_gal ? m_gal->GetGridVisibility() : true;
	bool     result = true; // assume everything will be fine


	MINI::OPENGL_GAL* new_gal = nullptr;
	if (aGalType == GAL_TYPE::GAL_TYPE_OPENGL) {
		MINI::OPENGL_GAL::CheckFeatures(m_options);
		new_gal = new MINI::OPENGL_GAL(m_options, m_parent);
	}

	if (m_gal)
		delete m_gal;
	m_gal = new_gal;

	m_gal->ResizeScreen(m_parent->size().height(), m_parent->size().width());

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
	MINI::GAL_UPDATE_CONTEXT ctx(m_gal);

	m_gal->SetLineWidth(m_view->ToWorld(1));
	//m_gal->SetIsFill(true);
	//m_gal->SetFillColor(MINI::COLOR4D(1, 1, 1, 1));
	for (auto &circle : data->m_circles) {
		m_view->Add(&circle);
	}

	for (auto& rectangle : data->m_rectangles) {
		m_view->Add(&rectangle);
	}
	
    for (auto& triangle : data->m_triangles) {
		triangle.m_point1 = m_view->ToWorld(triangle.m_point1);
        triangle.m_point2 = m_view->ToWorld(triangle.m_point2);
        triangle.m_point3 = m_view->ToWorld(triangle.m_point3);
		triangle.m_lineWidth = m_view->ToWorld(triangle.m_lineWidth);
        m_view->Add(&triangle);
    }

    for (auto& polygon : data->m_polygons) {
        m_view->Add(&polygon);
    }

    for (auto& polyline : data->m_polylines) {
        m_view->Add(&polyline);
    }

    for (auto& arc : data->m_arcs) {
        BOX2I box = arc.GetBoundingBox();
        m_view->Add(&arc);
    }
        

    for (auto& line : data->m_lines)
        m_view->Add(&line);

    for(auto& text : data->m_texts)
        m_view->Add(&text);

	m_view->MarkDirty();
}
