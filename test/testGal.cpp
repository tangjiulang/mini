#include <QApplication>
#include "gal/include/opengl_gal.hxx"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	KIGFX::GAL_DISPLAY_OPTIONS option;
	KIGFX::OPENGL_GAL gal(option, nullptr);
	gal.BeginDrawing();
	gal.DrawLine(VECTOR2D(0, 0), VECTOR2D(100, 100));
	gal.EndDrawing();

	return app.exec();
}