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

        rectangles1.push_back({ VECTOR2D(x1, y1), VECTOR2D(x2, y2) });
    }

    // 生成随机圆
    for (int i = 0; i < N; ++i) {
        double cx = distX(gen);
        double cy = distY(gen);
        double r = distR(gen);
        circles1.push_back({ VECTOR2D(cx, cy), r });
    }
}


void MainWindow::paintEvent(QPaintEvent*) {
    rectangles.clear();
    circles.clear();
    m_view->Clear();

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


    m_gal->BeginDrawing();
    m_gal->SetTarget(KIGFX::RENDER_TARGET::TARGET_NONCACHED);
    m_gal->SetLineWidth(1 / m_gal->GetWorldScale());

    for (int i = 0; i < rectangles1.size(); i++) {
        rectangles.push_back({ m_gal->GetScreenWorldMatrix() * rectangles1[i].m_startPoint , m_gal->GetScreenWorldMatrix() * rectangles1[i].m_endPoint });
    }

    for (int i = 0; i < circles1.size(); i++) {
        circles.push_back({ m_gal->GetScreenWorldMatrix() * circles1[i].m_centerPoint, circles1[i].m_radius / m_gal->GetWorldScale() });
    }

    timer.start();
    //for (DATA_Rectangle& rec : rectangles)
    //    m_view->Add(&rec);
    //for (DATA_Circle& cir : circles)
    //    m_view->Add(&cir, 1);

    //m_view->Redraw();
    for (DATA_Rectangle& rec : rectangles)
        m_gal->DrawRectangle(rec.m_startPoint, rec.m_endPoint);
    for (DATA_Circle& cir : circles)
        m_gal->DrawCircle(cir.m_centerPoint, cir.m_radius);

    ms = timer.elapsed();
    qDebug() << "QOpenGL 耗时:" << ms << "ms";
}