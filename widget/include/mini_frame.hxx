#pragma once

#include <QMainWindow>
#include "draw_panel_gal.hxx"
#include "data_manager.hxx"

class MiniFrame : public QMainWindow {
    Q_OBJECT
public:
    MiniFrame(QWidget* parent = nullptr);
    virtual ~MiniFrame();

    void GeneratorData();

    void InitialViewData();

protected:
    //void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

    void wheelEvent(QWheelEvent* event)
    {
        m_drawPanelGal->onWheel(event);
        m_drawPanelGal->Paint();
        m_drawPanelGal->m_gal->update();
    }

    void paintEvent(QPaintEvent* event) {
    }

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event);

    QWidget*            m_centralWidget;
    DrawPanelGal*       m_drawPanelGal;
public:
    DataManager*        m_dataManager;
};