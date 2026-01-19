#include <QApplication>
#include "mini_frame.hxx"
#include "gal/include/utils.hxx"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <ipc-2581/include/reader.hxx>
#include "TranslateToData.hxx"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    auto file_logger = spdlog::basic_logger_mt("default", "log.txt", true);
    spdlog::set_default_logger(file_logger);

    spdlog::set_level(spdlog::level::level_enum::trace);

    MiniFrame w;
    w.show();


    //w.GeneratorData();
    

    std::string filePath = "C:\\Users\\Administrator\\Documents\\333\\default1.xml";
    IPC2581Document doc(filePath);
    doc.DocumentReader();
    doc.ContentReader();
    doc.EcadReader();
    
    TranslateToData translate(&doc.m_ecad, w.m_dataManager);
    translate.Translate();

    w.InitialViewData();

    qDebug() << (const char*)glGetString(GL_VENDOR);
    qDebug() << (const char*)glGetString(GL_RENDERER);
    qDebug() << (const char*)glGetString(GL_VERSION);

    
    return app.exec();
}