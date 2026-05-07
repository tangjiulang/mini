#pragma once
#include <array>
#include <stack>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "vertex_manager.hxx"
#include "color4d.hxx"
#include "eda_angle.hxx"
#include "line_segment_data.hxx"
#include "vertex_container.hxx"
#include "box2.hxx"

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

	void SetFillColor(const COLOR4D& aFillColor) {
		m_fillColor = aFillColor;
	}

	void SetStrokeColor(const COLOR4D& aStrokeColor) {
		m_strokeColor = aStrokeColor; 
	}

	void SetFillEnabled(bool isFillEnabled) {
		m_isFillEnabled = isFillEnabled;
	}

	void SetStrokeEnabled(bool isStrokeEnabled) {
		m_isStrokeEnabled = isStrokeEnabled;
	}

	void SetLineWidth(double aLineWidth) {
		m_lineWidth = aLineWidth;
	}

    void SetVisibleWorldExtents( const BOX2D& aExtents )
    {
        m_visibleWorldExtents = aExtents;
        m_visibleWorldExtents.Normalize();
        m_hasVisibleWorldExtents = true;
    }

	void DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint);
    void DrawLineSegments(const std::vector<LineSegmentData>& aLines);
	void drawLineQuad(const VECTOR2D& startPoint, const VECTOR2D& endPoint);
	void DrawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint);
	void drawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint);
	void DrawCircle(const VECTOR2D& aCenterPoint, double aRadius);
	void drawCircle(const VECTOR2D& aCenterPoint, double aRadius);
	void drawFilledSemiCircle(const VECTOR2D& aCenterPoint, double aRadius, double aAngle);
	void drawStrokedSemiCircle(const VECTOR2D& aCenterPoint, double aRadius, double aAngle);
	void drawSemiCircle(const VECTOR2D& aCenterPoint, double aRadius, double aAngle);
	void drawPolyline(const std::function<VECTOR2D(int)>& aPointGetter, int aPointCount);
	void drawPolygon(GLdouble* aPoints, int aPointCount);
	void drawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, double aWidth);
	void DrawArc(const VECTOR2D& aCenterPoint, double aRadius,
				 const EDA_ANGLE& aStartAngle, const EDA_ANGLE& aAngle);
    void DrawArcSegment(const VECTOR2D& aCenterPoint, double aRadius, const EDA_ANGLE& aStartAngle,
                        const EDA_ANGLE& aAngle, double aWidth, double aMaxError);
	void DrawRectangle(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint);
	void DrawPolyline(const std::deque<VECTOR2D>& aPointList);
	void DrawPolyline(const std::vector<VECTOR2D>& aPointList);
	void DrawPolyline(const VECTOR2D aPointList[], int aListSize);
	void DrawPolyline(const SHAPE_LINE_CHAIN& aLineChain);
	void DrawPolygon(const SHAPE_POLY_SET& aPolySet, bool aStrokeTriangulation);
	void DrawPolygon(const SHAPE_LINE_CHAIN& aPolygon);
    void drawTriangulatedPolyset(const SHAPE_POLY_SET& aPolySet, bool aStrokeTriangulation);
    void DrawGlyph(const KIFONT::GLYPH& aGlyph, int aNth, int aTotal);
    void DrawGlyphs(const std::vector<std::unique_ptr<KIFONT::GLYPH>>& aGlyphs);
    void DrawPolylines(const std::vector<std::vector<VECTOR2D>>& aPointList);

	void MergeToManager();
private:
	bool InsertVertex(const VECTOR2D& aPoint, double aLayerDepth, COLOR4D& aColor, std::array<GLfloat, 4> aShader);
private:
    VERTEX_CONTAINER*     m_container;
    VERTEX*               m_currentVertex;
    glm::mat4             m_transform;
	std::stack<glm::mat4> m_transformStack;
    VERTEX_MANAGER*       m_mergeManager;
    double                m_layerDepth;
    bool                  m_noTransform;
    bool                  m_isFillEnabled;
    bool                  m_isStrokeEnabled;
    COLOR4D               m_fillColor;
    COLOR4D               m_strokeColor;
    double                m_lineWidth;
    bool                  m_hasVisibleWorldExtents = false;
    BOX2D                 m_visibleWorldExtents;
};
}

