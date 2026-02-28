#pragma once
#include <array>
#include <stack>
#include <glm/gtc/matrix_transform.hpp>
#include "vertex_manager.hxx"
#include "color4d.hxx"
#include "eda_angle.hxx"
#include "vertex_container.hxx"

namespace MINI {
class INSERT_VERTEX {
public:
	INSERT_VERTEX(VERTEX_MANAGER* aVertexManager, bool aCached )
		: m_mergeManager(aVertexManager),
		  m_noTransform(true)
	{
		m_container = VERTEX_CONTAINER::MakeContainer( aCached );
	}
	~INSERT_VERTEX()
	{
		delete m_container;
	}
	VERTEX_CONTAINER* GetContainer() const
	{
		return m_container;
	}

	void pushMatrix() {
		m_transformStack.push(m_transform);
		m_noTransform = false;
	}

	void popMatrix() {
		if (!m_transformStack.empty()) {
			m_transform = m_transformStack.top();
			m_transformStack.pop();
		}

		if (m_transformStack.empty())
			m_noTransform = true;
	}

    void Translate(GLfloat aX, GLfloat aY, GLfloat aZ)
    {
        m_transform = glm::translate(m_transform, glm::vec3(aX, aY, aZ));
    }

    void Rotate(GLfloat aAngle, GLfloat aX, GLfloat aY, GLfloat aZ)
    {
        m_transform = glm::rotate(m_transform, aAngle, glm::vec3(aX, aY, aZ));
    }

	void Scale(GLfloat aX, GLfloat aY, GLfloat aZ)
    {
        m_transform = glm::scale(m_transform, glm::vec3(aX, aY, aZ));
    }

	double calcAngleStep(double aRadius) const
	{
		// Bigger arcs need smaller alpha increment to make them look smooth
		return std::min(1e6 / aRadius, 2.0 * M_PI / 64);
	}

	void SetMergeManager(VERTEX_MANAGER* aMergeManager) {
		m_mergeManager = aMergeManager;
	}

	void SetLayerDepth(double aLayerDepth) {
		m_layerDepth = aLayerDepth;
	}

	void SetTransformation(glm::mat4 aTransform) {
		m_transform = aTransform;
	}

	void DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, COLOR4D& aStrokeColor, float aLineWidth);
	void drawLineQuad(const VECTOR2D& startPoint, const VECTOR2D& endPoint, COLOR4D& strokeColor, float aLineWidth);
	void DrawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, float aLineWidth);
	void drawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, float aLineWidth);
	void DrawCircle(const VECTOR2D& aCenterPoint, double aRadius, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aStrokeColor, COLOR4D& aFillColor);
	void drawCircle(const VECTOR2D& aCenterPoint, double aRadius, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& strokeColor, COLOR4D& fillColor);
	void drawFilledSemiCircle(const VECTOR2D& aCenterPoint, COLOR4D& aColor, double aRadius, double aAngle);
	void drawStrokedSemiCircle(const VECTOR2D& aCenterPoint, COLOR4D& aStrokeColor, double aRadius, double aAngle, double aLineWidth);
	void drawSemiCircle(const VECTOR2D& aCenterPoint, double aRadius, double aAngle, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& strokeColor, COLOR4D& fillColor, double aLineWidth);
	void drawPolyline(const std::function<VECTOR2D(int)>& aPointGetter, int aPointCount, COLOR4D& aStrokeColor, double aLineWidth);
	void drawPolygon(GLdouble* aPoints, int aPointCount, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, double aLineWidth);
	void drawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, double aWidth, bool isFilledEnabled, bool isStokeEnabled, COLOR4D& aFilledColor, COLOR4D& aStrokeColor);
	void DrawArc(const VECTOR2D& aCenterPoint, double aRadius,
				 const EDA_ANGLE& aStartAngle, const EDA_ANGLE& aAngle, bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStokeColor, double aLineWidth);
	void DrawArcSegment(const VECTOR2D& aCenterPoint, double aRadius,
						const EDA_ANGLE& aStartAngle, const EDA_ANGLE& aAngle,
						bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor,
						double aWidth, double aLineWidth, double aMaxError);
	void DrawRectangle(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, double aLineWidth);
	void DrawPolyline(const std::deque<VECTOR2D>& aPointList, COLOR4D& aStrokeColor, double aLineWidth);
	void DrawPolyline(const std::vector<VECTOR2D>& aPointList, COLOR4D& aStrokeColor, double aLineWidth);
	void DrawPolyline(const VECTOR2D aPointList[], int aListSize, COLOR4D& aStrokeColor, double aLineWidth);
	void DrawPolyline(const SHAPE_LINE_CHAIN& aLineChain, COLOR4D& aStrokeColor, double aLineWidth);
	void DrawPolygon(const SHAPE_POLY_SET& aPolySet, bool aStrokeTriangulation, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, double aLineWidth);
	void DrawPolygon(const SHAPE_LINE_CHAIN& aPolygon, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, double aLineWidth);
	void drawTriangulatedPolyset(const SHAPE_POLY_SET& aPolySet,
								 bool aStrokeTriangulation, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, double aLineWidth);
	void MergeToManager();
private:
	bool InsertVertex(const VECTOR2D& aPoint, double aLayerDepth, COLOR4D& aColor, std::array<GLfloat, 4> aShader);
private:
	VERTEX_CONTAINER* m_container;
	VERTEX* m_currentVertex;
	glm::mat4 m_transform;
	std::stack<glm::mat4> m_transformStack;
	VERTEX_MANAGER* m_mergeManager;
	double m_layerDepth;
	bool m_noTransform;
};
}

