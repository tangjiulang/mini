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

    void update();
protected:
    //void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

    void wheelEvent(QWheelEvent* event)
    {
        m_drawPanelGal->onWheel(event);
    }

    void paintEvent(QPaintEvent* event) {
        m_drawPanelGal->Paint(event);
    }

    DrawPanelGal*   m_drawPanelGal;
    DataManager*        m_dataManager;
};