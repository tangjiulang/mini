#include "mini_frame.hxx"

MiniFrame::MiniFrame(QWidget* parent)
	: QMainWindow(parent)
{
	m_drawPanelGal = new DrawPanelGal(this, this->size(), DrawPanelGal::GAL_TYPE::GAL_TYPE_OPENGL);
	m_dataManager = new DataManager();
}

void MiniFrame::resizeEvent(QResizeEvent*)
{ 
	m_drawPanelGal->resize(this->size());
}
