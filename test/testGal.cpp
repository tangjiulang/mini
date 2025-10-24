#include <QApplication>
#include <gal/include/definitions.hxx>
#include "gal/include/opengl_gal.hxx"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	KIGFX::GAL_DISPLAY_OPTIONS option;
	KIGFX::OPENGL_GAL gal(option, nullptr);
	qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
	gal.show();
	gal.SetScreenDPI(dpi);
	gal.ResizeScreen(1600, 1000);
	gal.BeginDrawing();
	gal.SetTarget(KIGFX::RENDER_TARGET::TARGET_NONCACHED);
	gal.SetLineWidth(1 / gal.GetWorldScale());
	gal.DrawRectangle(gal.GetScreenWorldMatrix() * VECTOR2D(300, 300), gal.GetScreenWorldMatrix() * VECTOR2D(600, 600));
	gal.DrawCircle(gal.GetScreenWorldMatrix() * VECTOR2D(500, 500), 100 / gal.GetWorldScale());
	return app.exec();
}