#include <QApplication>
#include <gal/include/definitions.hxx>
#include "gal/include/opengl_gal.hxx"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	KIGFX::GAL_DISPLAY_OPTIONS option;
	KIGFX::OPENGL_GAL gal(option, nullptr);
	gal.show();
	gal.ResizeScreen(gal.width(), gal.height());
	gal.BeginDrawing();
	gal.SetTarget(KIGFX::RENDER_TARGET::TARGET_NONCACHED);
	gal.DrawLine(VECTOR2D(0, 0), VECTOR2D(100, 100));

	return app.exec();
}