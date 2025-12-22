#include <QTimer>

class FpsCounter {
public:
    void frame() {
        if (!timer.isValid()) {
            timer.start();
            frames = 0;
        }
        frames++;
        qint64 elapsed = timer.elapsed();
        if (elapsed >= 1000) {
            fps = frames * 1000.0 / elapsed;
            frames = 0;
            timer.restart();
        }
    }
    double value() const { return fps; }

private:
    QElapsedTimer timer;
    int frames = 0;
    double fps = 0.0;
};
