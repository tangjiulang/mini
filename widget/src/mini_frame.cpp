#include "mini_frame.hxx"
#include "mini_frame.hxx"
#include "mini_frame.hxx"
#include "mini_frame.hxx"

#include <QBoxLayout>

MiniFrame::MiniFrame(QWidget* parent)
	: QMainWindow(parent)
{
	resize(1000, 1000);
	this->setMouseTracking(true);
	
	auto* layout = new QHBoxLayout();

	m_drawPanelGal = new DrawPanelGal(this, this->size(), DrawPanelGal::GAL_TYPE::GAL_TYPE_OPENGL);

	layout->addWidget(m_drawPanelGal->m_gal);

	m_centralWidget = new QWidget(this);
	m_centralWidget->setLayout(layout);
	m_centralWidget->setMouseTracking(true);

	this->setCentralWidget(m_centralWidget);

	m_dataManager = new DataManager();
}

MiniFrame::~MiniFrame()
{
	delete m_drawPanelGal;
	delete m_dataManager;
}

void MiniFrame::GeneratorData()
{
	m_dataManager->GenerateData();
}

void MiniFrame::InitialViewData()
{
	m_drawPanelGal->InitialViewData(m_dataManager);
}

void MiniFrame::resizeEvent(QResizeEvent*)
{ 
	m_drawPanelGal->resize(this->size());
}

void MiniFrame::mousePressEvent(QMouseEvent* event)
{
	m_drawPanelGal->CreateSelectRect();
}

void MiniFrame::mouseMoveEvent(QMouseEvent* event)
{
	m_drawPanelGal->SetCursor();
}

void MiniFrame::mouseReleaseEvent(QMouseEvent* event)
{
	m_drawPanelGal->UpdateSelectRect();
	m_drawPanelGal->DrawSelectRect();
}
