#pragma once
#include "gal/include/opengl_gal.hxx"
#include "view.hxx"
#include "data_painter.hxx"
#include "draw_panel_gal.hxx"
#include "data_manager.hxx"
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QWidget>

using namespace KIGFX;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    void CreateData();

    void update();
protected:
    void paintEvent(QPaintEvent*) override;

public:
    std::vector<DATA_Line> lines;
    std::vector<DATA_Circle> circles;
    std::vector<DATA_Triangle> triangles;
    std::vector<DATA_Rectangle> rectangles;
    std::vector<DATA_Line> lines1;
    std::vector<DATA_Circle> circles1;
    std::vector<DATA_Triangle> triangles1;
    std::vector<DATA_Rectangle> rectangles1;
    DataManager* m_dataManager;
    DrawPanelGal* m_drawPanelGal;
    QWidget* m_rWidget;
    VIEW* m_view;
    DATA_PAINTER* m_painter;

};
