#pragma once

#include <QMainWindow>
#include "draw_panel_gal.hxx"
#include "data_manager.hxx"

class MiniFrame : public QMainWindow {
    Q_OBJECT
public:
    MiniFrame(QWidget* parent = nullptr);


    void update();
protected:
    //void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

    DrawPanelGal*   m_drawPanelGal;
    DataManager*        m_dataManager;
};