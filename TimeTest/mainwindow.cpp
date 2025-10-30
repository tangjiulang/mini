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

    // 左侧：OpenGL 区域
    m_view = new VIEW();

    for (int i = 0; i < KIGFX::VIEW::VIEW_MAX_LAYERS; i++)
        m_view->SetLayerTarget(i, KIGFX::TARGET_NONCACHED);


    GAL_DISPLAY_OPTIONS option;
    m_gal = new OPENGL_GAL(option, nullptr);
    m_view->SetGAL(m_gal);

    m_painter = new DATA_PAINTER(m_gal);
    m_view->SetPainter(m_painter);

    qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    m_gal->show();
    m_gal->SetScreenDPI(dpi);

    // 右侧：普通 QWidget
    m_rWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(m_rWidget);

    // 加入主布局
    layout->addWidget(m_gal, 1);
    layout->addWidget(m_rWidget, 1);
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

        rectangles.push_back({ m_gal->GetScreenWorldMatrix() * VECTOR2D(x1, y1), m_gal->GetScreenWorldMatrix() * VECTOR2D(x2, y2) });
        rectangles1.push_back({ VECTOR2D(x1, y1), VECTOR2D(x2, y2) });
    }

    // 生成随机圆
    for (int i = 0; i < N; ++i) {
        double cx = distX(gen);
        double cy = distY(gen);
        double r = distR(gen);
        circles.push_back({ m_gal->GetScreenWorldMatrix() * VECTOR2D(cx, cy), r / m_gal->GetWorldScale() });
        circles1.push_back({ VECTOR2D(cx, cy), r });
    }
}


void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    painter.setClipRect(m_rWidget->geometry());
    painter.translate(m_rWidget->pos());
    painter.fillRect(m_rWidget->rect(), Qt::black);

    painter.setPen(Qt::blue);
    for (const auto& r : rectangles1)
        painter.drawRect(QRect(r.m_startPoint.x, r.m_startPoint.y, r.m_endPoint.x, r.m_endPoint.y));

    painter.setPen(Qt::red);
    for (const auto& c : circles1)
        painter.drawEllipse(QPointF(c.m_centerPoint.x, c.m_centerPoint.y), c.m_radius, c.m_radius);
    painter.restore();

    m_gal->BeginDrawing();
    m_gal->SetTarget(KIGFX::RENDER_TARGET::TARGET_NONCACHED);

    for (int i = 0; i < rectangles.size(); i++) {
        rectangles[i].m_startPoint = m_gal->GetScreenWorldMatrix() * rectangles[i].m_startPoint;
        rectangles[i].m_endPoint = m_gal->GetScreenWorldMatrix() * rectangles[i].m_endPoint;

    }

    for (int i = 0; i < circles.size(); i++) {
        
        circles[i].m_centerPoint = m_gal->GetScreenWorldMatrix() * circles[i].m_centerPoint;
        circles[i].m_radius = circles[i].m_radius / m_gal->GetWorldScale();
    }

    for (DATA_Rectangle& rec : rectangles)
        m_view->Add(&rec);
    for (DATA_Circle& cir : circles)
        m_view->Add(&cir);

    m_view->Redraw();
        
}