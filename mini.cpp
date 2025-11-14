#include <QApplication>
#include "mini_frame.hxx"
#include "gal/include/utils.hxx"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    auto file_logger = spdlog::basic_logger_mt("default", "log.txt", true);
    spdlog::set_default_logger(file_logger);

    spdlog::set_level(spdlog::level::level_enum::trace);

    MiniFrame w;
    w.show();


    w.GeneratorData();

    w.InitialViewData();


    return app.exec();
}