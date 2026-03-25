#include <QApplication>
#include "mini_frame.hxx"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <filesystem>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    spdlog::init_thread_pool(8192, 1);

    std::filesystem::path logPath;
#ifdef MINI_BUILD_DIR
    logPath = std::filesystem::path(MINI_BUILD_DIR) / "log.txt";
#else
    logPath = std::filesystem::current_path() / "log.txt";
#endif

    auto file_logger = spdlog::basic_logger_mt<spdlog::async_factory>(
            "default", logPath.string(), true );
    spdlog::set_default_logger(file_logger);

    spdlog::set_level(spdlog::level::level_enum::trace);

    MiniFrame w;
    w.show();


    w.GeneratorData();

    w.InitialViewData();

    qDebug() << (const char*)glGetString(GL_VENDOR);
    qDebug() << (const char*)glGetString(GL_RENDERER);
    qDebug() << (const char*)glGetString(GL_VERSION);

    
    return app.exec();
}
