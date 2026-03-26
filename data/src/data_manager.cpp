#include <random>

#include "data_manager.hxx"
#include "view.hxx"

void DataManager::GenerateData(MINI::VIEW* view)
{
    constexpr int N = 20000; // 数量
    constexpr double WIDTH = 10000.0;
    constexpr double HEIGHT = 10000.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distX(0.0, WIDTH);
    std::uniform_real_distribution<double> distY(0.0, HEIGHT);
    std::uniform_real_distribution<double> distR(5.0, 10000.0);

    m_rectangles.reserve(N);
    m_circles.reserve(N);
	m_arcs.reserve(N);

    //for (int i = 0; i < N; ++i) {
    //    double x1 = distX(gen);
    //    double y1 = distY(gen);
    //    double x2 = distX(gen);
    //    double y2 = distY(gen);

    //    // 确保 x1 < x2, y1 < y2
    //    if (x1 > x2) std::swap(x1, x2);
    //    if (y1 > y2) std::swap(y1, y2);

    //    m_rectangles.push_back(SHAPE_RECT{ view->ToWorld(VECTOR2D(x1, y1)), view->ToWorld(VECTOR2D(x2, y2)) });
    //}
    //
    for (int i = 1; i < N; ++i) {
        m_circles.push_back(SHAPE_CIRCLE{ view->ToWorld(VECTOR2D(500, 500)), static_cast<int32_t>(view->ToWorld(i % 500)) });
    }

    //for (int i = 0; i < N; i++) {
    //    VECTOR2I center(0, 0);
    //    VECTOR2I start(1000, 0);

    //    // 90 度
    //    EDA_ANGLE angle(90.0, DEGREES_T);

    //    SHAPE_ARC Arc(center, start, angle, 100);
    //    MINI::DATA_Arc arc(Arc);
    //    m_arcs.push_back(arc);
    //}

    //TEXT_ATTRIBUTES attrs;
    //attrs.m_StrokeWidth = 30000;
    //attrs.m_Size = { 1500000, 1500000 };
    //attrs.m_Mirrored = true;
    //attrs.m_Angle = EDA_ANGLE(180);
    //attrs.m_Font = KIFONT::FONT::GetFont("Arial");

    //m_texts.push_back(MINI::DATA_Text{ "你好 hello, world", attrs, view->ToWorld(VECTOR2I{ 500, 500 }) });
}