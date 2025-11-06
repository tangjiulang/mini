#include <QApplication>
#include "mini_frame.hxx"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MiniFrame w;
    w.show();


    w.GeneratorData();

    w.InitialViewData();


    return app.exec();
}