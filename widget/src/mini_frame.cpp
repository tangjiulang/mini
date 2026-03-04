#include "mini_frame.hxx"
#include "reader.hxx"
#include "TranslateToData.hxx"
#include <QBoxLayout>

MiniFrame::MiniFrame(QWidget* parent)
	: QMainWindow(parent)
{
	resize(1000, 1000);
	this->setMouseTracking(true);
	
	auto* layout = new QHBoxLayout();

	m_drawPanelGal = new DrawPanelGal(this, this->size(), DrawPanelGal::GAL_TYPE::GAL_TYPE_OPENGL);

	m_drawPanelGal->SetDefaultLayerDeps();

	layout->addWidget(m_drawPanelGal->m_gal);

	m_centralWidget = new QWidget(this);
	m_centralWidget->setLayout(layout);
	m_centralWidget->setMouseTracking(true);

	this->setCentralWidget(m_centralWidget);

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
	std::string filePath = "C:\\Users\\Administrator\\Documents\\preview.xml";
	IPC2581Document doc(filePath);
	doc.DocumentReader();
	doc.ContentReader();
	doc.EcadReader();

	TranslateToData translate(&doc.m_ecad, m_dataManager, m_drawPanelGal->m_view);
	translate.Translate(m_drawPanelGal->m_view);
	//m_dataManager->GenerateData(m_drawPanelGal->m_view);
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
	m_drawPanelGal->m_selectionTool.ClearSelection();
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
