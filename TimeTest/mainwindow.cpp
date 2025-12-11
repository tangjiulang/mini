#include "mainwindow.hxx"
#include <QPainter>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <random>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1200, 800);
    CreateData();
    setStyleSheet("background-color: black;");
}

void MainWindow::CreateData()
{
    constexpr int N = 100000;
    constexpr double WIDTH = 20000.0;
    constexpr double HEIGHT = 20000.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distX(0.0, WIDTH);
    std::uniform_real_distribution<double> distY(0.0, HEIGHT);
    std::uniform_real_distribution<double> distR(5.0, 50.0);

    rectangles.reserve(N);
    circles.reserve(N);

    for (int i = 0; i < N; ++i) {
        double x1 = distX(gen);
        double y1 = distY(gen);
        double x2 = distX(gen);
        double y2 = distY(gen);

        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);

        rectangles.push_back({ QPointF(x1, y1), QPointF(x2, y2) });
    }

    for (int i = 0; i < N; ++i) {
        double cx = distX(gen);
        double cy = distY(gen);
        double r = distR(gen);
        circles.push_back({ QPointF(cx, cy), r });
    }
}

void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    QElapsedTimer timer;
    timer.start();

    p.translate(m_offset);
    p.scale(m_scale, m_scale);

    p.setPen(Qt::white);

    for (const auto& r : rectangles)
        p.drawRect(QRectF(r.a, r.b));

    for (const auto& c : circles)
        p.drawEllipse(c.c, c.r, c.r);

    qDebug() << "Paint cost:" << timer.elapsed() << "ms";
}

void MainWindow::wheelEvent(QWheelEvent* e)
{
    double delta = e->angleDelta().y();

    // ---- Shift = 平移（保持你的旧逻辑） ----
    if (e->modifiers() & Qt::ShiftModifier) {
        double moveStep = delta * 0.5;
        m_offset.rx() += moveStep;   // 左右
        update();
        return;
    }

    // ---- 普通滚轮 = 缩放 ----
    double zoomFactor = 1.0 + delta / 1200.0;  // 适中缩放速度

    // 限制缩放范围
    double newScale = m_scale * zoomFactor;
    if (newScale < 0.02 || newScale > 50.0)
        return;

    // 以鼠标为中心缩放
    QPointF mousePos = e->position();
    QPointF before = (mousePos - m_offset) / m_scale;  // 缩放前场景坐标

    m_scale = newScale;

    QPointF after = before * m_scale + m_offset;       // 缩放后新位置
    m_offset += (mousePos - after);                   // 调整偏移，使鼠标保持不动

    update();
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    m_lastMousePos = e->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton) {
        QPointF delta = e->pos() - m_lastMousePos;
        m_offset += delta;
        m_lastMousePos = e->pos();
        update();
    }
}
