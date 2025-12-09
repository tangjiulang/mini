#include "mainwindow.hxx"
#include "gal/include/opengl_gal.hxx"
#include "view.hxx"
#include "data_painter.hxx"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QColor>
#include <QDebug>
#include <QPainter>
#include <random>

using namespace KIGFX;

// ---------------------- MainWindow ----------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);


    GAL_DISPLAY_OPTIONS option;
    m_drawPanelGal = new DrawPanelGal(this, this->size(),
                                      DrawPanelGal::GAL_TYPE::GAL_TYPE_OPENGL);
    m_view = m_drawPanelGal->m_view;

    // 右侧：普通 QWidget
    m_rWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rWidget);

    // 加入主布局
    layout->addWidget(m_drawPanelGal, 1);
    layout->addWidget(m_rWidget, 1);

    m_dataManager = new DataManager();
}

void MainWindow::CreateData()
{
    constexpr int N = 1000; // 数量
    constexpr double WIDTH = 1000.0;
    constexpr double HEIGHT = 1000.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distX(0.0, WIDTH);
    std::uniform_real_distribution<double> distY(0.0, HEIGHT);
    std::uniform_real_distribution<double> distR(5.0, 50.0);

    rectangles.reserve(N);
    circles.reserve(N);
    rectangles1.reserve(N);
    circles1.reserve(N);

    // 生成随机矩形
    for (int i = 0; i < N; ++i) {
        double x1 = distX(gen);
        double y1 = distY(gen);
        double x2 = distX(gen);
        double y2 = distY(gen);

        // 确保 x1 < x2, y1 < y2
        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);

        rectangles1.push_back({ VECTOR2D(x1, y1), VECTOR2D(x2, y2) });
        m_dataManager->m_rectangles.push_back(rectangles1.back());
    }

    // 生成随机圆
    for (int i = 0; i < N; ++i) {
        double cx = distX(gen);
        double cy = distY(gen);
        double r = distR(gen);
        circles1.push_back({ VECTOR2D(cx, cy), r });
        m_dataManager->m_circles.push_back(circles1.back());
    }

    m_drawPanelGal->InitialViewData(m_dataManager);
}


void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    painter.setClipRect(m_rWidget->geometry());
    painter.translate(m_rWidget->pos());
    painter.fillRect(m_rWidget->rect(), Qt::black);

    QElapsedTimer timer;
    timer.start();
    painter.setPen(Qt::white);
    for (const auto& r : rectangles1)
        painter.drawRect(QRect(r.m_startPoint.x, r.m_startPoint.y, r.m_endPoint.x - r.m_startPoint.x, r.m_endPoint.y - r.m_startPoint.y));

    for (const auto& c : circles1)
        painter.drawEllipse(QPointF(c.m_centerPoint.x, c.m_centerPoint.y), c.m_radius, c.m_radius);
    painter.restore();
    qint64 ms = timer.elapsed();
    qDebug() << "QPainter 耗时:" << ms << "ms";

    timer.start();

    m_view->MarkDirty();
    m_view->Redraw();

    ms = timer.elapsed();
    qDebug() << "QOpenGL 耗时:" << ms << "ms";
}