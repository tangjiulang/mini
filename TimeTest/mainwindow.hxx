#pragma once
#include <QMainWindow>
#include <QVector>
#include <QPointF>

#include "FpsCounter.hxx"

struct Rect {
    QPointF a, b;
};

struct Circle {
    QPointF c;
    double r;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    void CreateData();

protected:
    void paintEvent(QPaintEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

private:
    // 数据
    std::vector<Rect> rectangles;
    std::vector<Circle> circles;

    // 视图变换
    double m_scale = 1.0;
    QPointF m_offset = { 0, 0 };        // 平移（视图在画布的位置）
    QPointF m_lastMousePos;           // 上次鼠标位置

    FpsCounter m_fpsCounter;
};
