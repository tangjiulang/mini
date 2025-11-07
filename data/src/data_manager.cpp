#include <random>

#include "data_manager.hxx"

void DataManager::GenerateData()
{
    constexpr int N = 1000; // 数量
    constexpr double WIDTH = 2000.0;
    constexpr double HEIGHT = 2000.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distX(0.0, WIDTH);
    std::uniform_real_distribution<double> distY(0.0, HEIGHT);
    std::uniform_real_distribution<double> distR(5.0, 500.0);

    m_rectangles.reserve(N);
    m_circles.reserve(N);

    for (int i = 0; i < N; ++i) {
        double x1 = distX(gen);
        double y1 = distY(gen);
        double x2 = distX(gen);
        double y2 = distY(gen);

        // 确保 x1 < x2, y1 < y2
        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);

        m_rectangles.push_back({ VECTOR2D(x1, y1), VECTOR2D(x2, y2) });
    }

    for (int i = 0; i < N; ++i) {
        double cx = distX(gen);
        double cy = distY(gen);
        double r = distR(gen);
        m_circles.push_back({ VECTOR2D(cx, cy), r });
    }
}