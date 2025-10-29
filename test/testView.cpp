#include <QApplication>
#include <vector>
#include "data_line.hxx"
#include "data_circle.hxx"
#include "data_rectangle.hxx"
#include "data_triangle.hxx"
#include "view.hxx"
#include "gal/include/opengl_gal.hxx"

using namespace KIGFX;

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	std::vector<DATA_Line> lines;
	std::vector<DATA_Circle> circles;
	std::vector<DATA_Triangle> triangles;
	std::vector<DATA_Rectangle> rectangles;

	VIEW* view = new VIEW();
	GAL_DISPLAY_OPTIONS option;
	OPENGL_GAL gal(option, nullptr);
	view->SetGAL(&gal);

	qreal dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
	gal.show();
	gal.SetScreenDPI(dpi);
	gal.ResizeScreen(1600, 1000);
	gal.BeginDrawing();
	gal.SetTarget(KIGFX::RENDER_TARGET::TARGET_NONCACHED);
	gal.SetLineWidth(1 / gal.GetWorldScale());
	
	DATA_Rectangle rec = { gal.GetScreenWorldMatrix() * VECTOR2D(300, 300), gal.GetScreenWorldMatrix() * VECTOR2D(600, 600) };
	DATA_Circle cir = { gal.GetScreenWorldMatrix() * VECTOR2D(500, 500), 100 / gal.GetWorldScale() };

	view->Add(&rec);
	view->Add(&cir);

	view->Redraw();

	return app.exec();
}