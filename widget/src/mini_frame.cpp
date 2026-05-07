#include "mini_frame.hxx"
#include "reader.hxx"
#include "TranslateToData.hxx"
#include <QBoxLayout>
#include <spdlog/spdlog.h>

MiniFrame::MiniFrame(QWidget* parent)
	: QMainWindow(parent)
{
	resize(1000, 1000);
	this->setMouseTracking(true);
	
	auto* layout = new QHBoxLayout();

	m_drawPanelGal = new DrawPanelGal(this, this->size(), DrawPanelGal::GAL_TYPE::GAL_TYPE_OPENGL);

	m_drawPanelGal->SetDefaultLayerDeps();

	this->setCentralWidget(m_drawPanelGal->m_gal);

	m_dataManager = new DataManager();

	setCursor(Qt::BlankCursor);
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover);
}

MiniFrame::~MiniFrame()
{
	delete m_drawPanelGal;
	delete m_dataManager;
}

void MiniFrame::GeneratorData()
{
    spdlog::info("GeneratorData start");
	//std::string filePath = "C:\\Users\\Administrator\\Documents\\preview.xml";
	//IPC2581Document doc(filePath);
	//doc.DocumentReader();
	//doc.ContentReader();
	//doc.EcadReader();

	//TranslateToData translate(&doc.m_ecad, m_dataManager, m_drawPanelGal->m_view);
	//translate.Translate(m_drawPanelGal->m_view);
	m_dataManager->GenerateData(m_drawPanelGal->m_view);
    spdlog::info("GeneratorData done");
}

void MiniFrame::InitialViewData()
{
    spdlog::info("InitialViewData start");
	m_drawPanelGal->InitialViewData(m_dataManager);
    m_drawPanelGal->Paint();
    spdlog::info("InitialViewData done");
}

void MiniFrame::resizeEvent(QResizeEvent* event)
{
	m_drawPanelGal->ResizeEvent(event);
    m_drawPanelGal->Paint();
}

void MiniFrame::mousePressEvent(QMouseEvent* event)
{
	m_drawPanelGal->m_selectionTool.ClearSelection();
	m_drawPanelGal->CreateSelectRect();
}

void MiniFrame::mouseMoveEvent(QMouseEvent* event)
{
	m_drawPanelGal->SetCursor();
    m_drawPanelGal->Paint();
}

void MiniFrame::mouseReleaseEvent(QMouseEvent* event)
{
	m_drawPanelGal->UpdateSelectRect();
	m_drawPanelGal->DrawSelectRect();
}
