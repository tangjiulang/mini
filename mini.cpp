#include <QApplication>
#include "mini_frame.hxx"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MiniFrame w;
    w.resize(2000, 1000);

    w.show();

    return app.exec();
}