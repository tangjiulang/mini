#include "mini_frame.hxx"

#include <QBoxLayout>

MiniFrame::MiniFrame(QWidget* parent)
	: QMainWindow(parent)
{
	resize(1000, 1000);
	auto* layout = new QHBoxLayout();

	m_drawPanelGal = new DrawPanelGal(this, this->size(), DrawPanelGal::GAL_TYPE::GAL_TYPE_OPENGL);

	layout->addWidget(m_drawPanelGal->m_gal);

	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);

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
