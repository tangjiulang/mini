#include <QApplication>
#include <gal/include/definitions.hxx>
#include "gal/include/opengl_gal.hxx"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	KIGFX::GAL_DISPLAY_OPTIONS option;
	KIGFX::OPENGL_GAL gal(option, nullptr);
	gal.show();
	gal.ResizeScreen(1600, 1000);
	gal.BeginDrawing();
	gal.SetTarget(KIGFX::RENDER_TARGET::TARGET_NONCACHED);
	return app.exec();
}