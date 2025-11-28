#include <random>

#include "data_manager.hxx"
#include "polyCorrect_SL.hxx"

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
    std::vector<VECTOR2I> points;
    points.push_back(VECTOR2I(0, 0)     + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(0, 100)   + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(75, 100)  + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(50, 50)   + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(50, 150)  + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(150, 150) + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(150, 0)   + VECTOR2I(100, 100));
    points.push_back(VECTOR2I(0, 0)     + VECTOR2I(100, 100));


    m_polygons.push_back({ points });

    points.clear();
    points.push_back(VECTOR2I(0, 0) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(0, 100) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(75, 100) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(50, 50) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(50, 150) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(150, 150) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(150, 0) + VECTOR2I(500, 100));
    points.push_back(VECTOR2I(0, 0) + VECTOR2I(500, 100));

    m_polygons.push_back({ points });

    //points.clear();
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(-59, -81) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(95, 31) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(-95, 31) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(59, -81) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(100, 500));

    //m_polygons.push_back({ points });

    //points.clear();
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(-59, -81) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(95, 31) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(-95, 31) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(59, -81) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(500, 500));

    //m_polygons.push_back({ points });

    //points.clear();
    //points.push_back(VECTOR2I(0, 0) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(100, 0) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(100, 100) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(0, 0) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(100, 500));
    //points.push_back(VECTOR2I(0, 0) + VECTOR2I(100, 500));
    //m_polygons.push_back({ points });

    //points.clear();
    //points.push_back(VECTOR2I(0, 0) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(100, 0) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(100, 100) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(0, 0) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(0, 100) + VECTOR2I(500, 500));
    //points.push_back(VECTOR2I(0, 0) + VECTOR2I(500, 500));
    //m_polygons.push_back({ points });

    std::vector<VECTOR2D> poly;
    poly.push_back(VECTOR2I(0, 100) + VECTOR2I(100, 500));
    poly.push_back(VECTOR2I(-59, -81) + VECTOR2I(100, 500));
    poly.push_back(VECTOR2I(95, 31) + VECTOR2I(100, 500));
    poly.push_back(VECTOR2I(-95, 31) + VECTOR2I(100, 500));
    poly.push_back(VECTOR2I(59, -81) + VECTOR2I(100, 500));
    poly.push_back(VECTOR2I(0, 100) + VECTOR2I(100, 500));


    // use dy small enough for accuracy (e.g., 0.5 or 0.2)
    double dy = 0.5;
    double simplify_eps = 0.2;

    auto result = scanline_to_polygons(poly, dy, simplify_eps);

    for (auto poly : result.outer) {
        points.clear();
        for (auto p : poly)
            points.push_back(p);
        m_polygons.push_back({ points });
    }

    for (auto poly : result.holes) {
        points.clear();
        for (auto p : poly)
            points.push_back(p);
        m_polygons.push_back({ points });
    }

    std::cout << "Outer contours: " << result.outer.size() << "\n";
    for (auto& c : result.outer) {
        std::cout << "Contour: ";
        for (auto& p : c) std::cout << "(" << p.x << "," << p.y << ") ";
        std::cout << "\n";
    }

    std::cout << "Holes: " << result.holes.size() << "\n";

}