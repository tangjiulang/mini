#include <QApplication>
#include "mainwindow.hxx"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.resize(2000, 1000);

    w.CreateData();

    w.show();

    return app.exec();
}
