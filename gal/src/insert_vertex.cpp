#include "gal/include/graphics_abstraction_layer.hxx"
#include "gal/include/insert_vertex.hxx"
#include <geometry_utils.hxx>
#include <tesselator.h>

void MINI::INSERT_VERTEX::DrawLine(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, COLOR4D& aStrokeColor, float aLineWidth, double aLayerDepth)
{
    drawLineQuad(aStartPoint, aEndPoint, aStrokeColor, aLineWidth, aLayerDepth);
}

void MINI::INSERT_VERTEX::drawLineQuad(const VECTOR2D& aStartPoint, const  VECTOR2D& aEndPoint, COLOR4D& strokeColor, float aLineWidth, double aLayerDepth)
{
	m_currentVertex =  m_container->Allocate(6);
    auto v1 = m_transform
        * glm::vec4(aStartPoint.x, aStartPoint.y, 0.0, 0.0);
    auto v2 = m_transform
        * glm::vec4(aEndPoint.x, aEndPoint.y, 0.0, 0.0);

    VECTOR2D vs(v2.x - v1.x, v2.y - v1.y);
	InsertVertex(aStartPoint, aLayerDepth, strokeColor, { (GLfloat)SHADER_LINE_A, (GLfloat)aLineWidth, (GLfloat)vs.x, (GLfloat)vs.y });
    InsertVertex(aStartPoint, aLayerDepth, strokeColor, { (GLfloat)SHADER_LINE_B, (GLfloat)aLineWidth, (GLfloat)vs.x, (GLfloat)vs.y });
    InsertVertex(aEndPoint,   aLayerDepth, strokeColor, { (GLfloat)SHADER_LINE_C, (GLfloat)aLineWidth, (GLfloat)vs.x, (GLfloat)vs.y });
    InsertVertex(aEndPoint,   aLayerDepth, strokeColor, { (GLfloat)SHADER_LINE_D, (GLfloat)aLineWidth, (GLfloat)vs.x, (GLfloat)vs.y });
    InsertVertex(aEndPoint,   aLayerDepth, strokeColor, { (GLfloat)SHADER_LINE_E, (GLfloat)aLineWidth, (GLfloat)vs.x, (GLfloat)vs.y });
    InsertVertex(aStartPoint, aLayerDepth, strokeColor, { (GLfloat)SHADER_LINE_F, (GLfloat)aLineWidth, (GLfloat)vs.x, (GLfloat)vs.y });
}

bool MINI::INSERT_VERTEX::InsertVertex(const VECTOR2D& aPoint, double aLayerDepth, COLOR4D& aColor, std::array<GLfloat, 4> aShader)
{
    m_currentVertex->x = aPoint.x;
    m_currentVertex->y = aPoint.y;
    m_currentVertex->z = aLayerDepth;                                                                                                                                                                                                                                                                           
   
    m_currentVertex->r = aColor.r;
    m_currentVertex->g = aColor.g;
    m_currentVertex->b = aColor.b;
	m_currentVertex->a = aColor.a;

    for (int i = 0; i < 4; i++)
		m_currentVertex->shader[i] = aShader[i];

    m_currentVertex++;
    return true;
}

void MINI::INSERT_VERTEX::DrawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, float aLineWidth, double aLayerDepth)
{
    drawSegment(aStartPoint, aEndPoint, isFilledEnabled, isStrokeEnabled, aFillColor, aStrokeColor, aLineWidth, aLayerDepth);
}

void MINI::INSERT_VERTEX::drawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, float aLineWidth, double aLayerDepth)
{
    VECTOR2D startEndVector = aEndPoint - aStartPoint;
    double   lineLength = startEndVector.EuclideanNorm();

    // Be careful about floating point rounding.  As we draw segments in larger and larger
    // coordinates, the shader (which uses floats) will lose precision and stop drawing small
    // segments.  In this case, we need to draw a circle for the minimal segment.
    // Check if the coordinate differences can be accurately represented as floats
    float startX = static_cast<float>(aStartPoint.x);
    float startY = static_cast<float>(aStartPoint.y);
    float endX = static_cast<float>(aEndPoint.x);
    float endY = static_cast<float>(aEndPoint.y);

    if (startX == endX && startY == endY)
    {
        drawCircle(aStartPoint, aLineWidth / 2, isFilledEnabled, isStrokeEnabled, aFillColor, aStrokeColor, aLayerDepth);
        return;
    }

    if (isFilledEnabled || aLineWidth == 1.0)
        drawLineQuad(aStartPoint, aEndPoint, aFillColor, aLineWidth, aLayerDepth);
    else
    {
        EDA_ANGLE lineAngle(startEndVector);

        // Outlined tracks
		pushMatrix();

        Translate(aStartPoint.x, aStartPoint.y, 0.0);
        Rotate(lineAngle.AsRadians(), 0.0f, 0.0f, 1.0f);

        drawLineQuad(VECTOR2D(0.0, aLineWidth / 2.0), VECTOR2D(lineLength, aLineWidth / 2.0), aStrokeColor, 1.0, aLayerDepth);

        drawLineQuad(VECTOR2D(0.0, -aLineWidth / 2.0), VECTOR2D(lineLength, -aLineWidth / 2.0), aStrokeColor, 1.0, aLayerDepth);

        // Draw line caps
        drawStrokedSemiCircle(VECTOR2D(0.0, 0.0), aStrokeColor, aLineWidth / 2, M_PI / 2, aLayerDepth, 1.0);
        drawStrokedSemiCircle(VECTOR2D(lineLength, 0.0), aStrokeColor, aLineWidth / 2, -M_PI / 2, aLayerDepth, 1.0);

        popMatrix();
    }
}

void MINI::INSERT_VERTEX::DrawCircle(const VECTOR2D& aCenterPoint, double aRadius, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aStrokeColor, COLOR4D& aFillColor, double aLayerDepth)
{
    drawCircle(aCenterPoint, aRadius, isFillEnabled, isStrokeEnabled, aStrokeColor, aFillColor, aLayerDepth);
}

void MINI::INSERT_VERTEX::drawCircle(const VECTOR2D& aCenterPoint, double aRadius, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& strokeColor, COLOR4D& fillColor, double aLayerDepth)
{
    if (isFillEnabled)
    {
        m_currentVertex = m_container->Allocate(3);

        InsertVertex(aCenterPoint, aLayerDepth, fillColor, { SHADER_FILLED_CIRCLE, 1.0, static_cast<GLfloat>(aRadius) });
        InsertVertex(aCenterPoint, aLayerDepth, fillColor, { SHADER_FILLED_CIRCLE, 2.0, static_cast<GLfloat>(aRadius) });
        InsertVertex(aCenterPoint, aLayerDepth, fillColor, { SHADER_FILLED_CIRCLE, 3.0, static_cast<GLfloat>(aRadius) });
    }

    if (isStrokeEnabled)
    {
        m_currentVertex = m_container->Allocate(3);

        InsertVertex(aCenterPoint, aLayerDepth, strokeColor, { SHADER_STROKED_CIRCLE, 1.0, static_cast<GLfloat>(aRadius) });
        InsertVertex(aCenterPoint, aLayerDepth, strokeColor, { SHADER_STROKED_CIRCLE, 2.0, static_cast<GLfloat>(aRadius) });
        InsertVertex(aCenterPoint, aLayerDepth, strokeColor, { SHADER_STROKED_CIRCLE, 3.0, static_cast<GLfloat>(aRadius) });
    }
}

void MINI::INSERT_VERTEX::drawFilledSemiCircle(const VECTOR2D& aCenterPoint, COLOR4D& aFillColor, double aRadius, double aAngle, double aLayerDepth)
{
    pushMatrix();

    m_currentVertex = m_container->Allocate(3);
    Translate(aCenterPoint.x, aCenterPoint.y, 0.0f);
    Rotate(aAngle, 0.0f, 0.0f, 1.0f);

	InsertVertex(VECTOR2D{ -aRadius * 3.0f / sqrt(3.0f), 0.0f }, aLayerDepth, aFillColor, { SHADER_FILLED_CIRCLE, 4.0f });
    InsertVertex(VECTOR2D{ aRadius * 3.0f / sqrt(3.0f), 0.0f },  aLayerDepth, aFillColor, { SHADER_FILLED_CIRCLE, 5.0f });
    InsertVertex(VECTOR2D{ 0.0f, aRadius * 2.0f},                aLayerDepth, aFillColor, { SHADER_FILLED_CIRCLE, 6.0f });


    popMatrix();
}

void MINI::INSERT_VERTEX::drawStrokedSemiCircle(const VECTOR2D& aCenterPoint, COLOR4D& aStrokeColor, double aRadius, double aAngle, double aLayerDepth, double aLineWidth)
{
    double outerRadius = aRadius + (aLineWidth / 2);

    pushMatrix();

    m_currentVertex = m_container->Allocate(3);

    Translate(aCenterPoint.x, aCenterPoint.y, 0.0f);
    Rotate(aAngle, 0.0f, 0.0f, 1.0f);

    InsertVertex(VECTOR2D{ -outerRadius * 3.0f / sqrt(3.0f), 0.0f }, aLayerDepth, aStrokeColor, { SHADER_STROKED_CIRCLE, 4.0f, static_cast<GLfloat>(aRadius), static_cast<GLfloat>(aLineWidth) });
    InsertVertex(VECTOR2D{ outerRadius * 3.0f / sqrt(3.0f), 0.0f },  aLayerDepth, aStrokeColor, { SHADER_STROKED_CIRCLE, 5.0f, static_cast<GLfloat>(aRadius), static_cast<GLfloat>(aLineWidth) });
    InsertVertex(VECTOR2D{ 0.0f, outerRadius * 2.0f },               aLayerDepth, aStrokeColor, { SHADER_STROKED_CIRCLE, 6.0f, static_cast<GLfloat>(aRadius), static_cast<GLfloat>(aLineWidth) });

    popMatrix();
}

void MINI::INSERT_VERTEX::drawSemiCircle(const VECTOR2D& aCenterPoint, double aRadius, double aAngle, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aStrokeColor, COLOR4D& aFillColor, double aLineWidth, double aLayerDepth)
{
    if (isFillEnabled)
        drawFilledSemiCircle(aCenterPoint, aFillColor, aRadius, aAngle, aLayerDepth);

    if (isStrokeEnabled)
    {
        drawStrokedSemiCircle(aCenterPoint, aStrokeColor, aRadius, aAngle, aLayerDepth, aLineWidth);
    }
}

void MINI::INSERT_VERTEX::drawPolyline(const std::function<VECTOR2D(int)>& aPointGetter, int aPointCount, COLOR4D& aStrokeColor, double aLineWidth, double aLayerDepth)
{
    Q_ASSERT(aPointCount > 0);


    if (aPointCount == 1)
    {
        drawLineQuad(aPointGetter(0), aPointGetter(0), aStrokeColor, aLineWidth, aLayerDepth);
        return;
    }

    for (int i = 1; i < aPointCount; ++i)
    {
        auto start = aPointGetter(i - 1);
        auto end = aPointGetter(i);

        drawLineQuad(start, end, aStrokeColor, aLineWidth, aLayerDepth);
    }
}

void MINI::INSERT_VERTEX::drawPolygon(GLdouble* aPoints, int aPointCount, bool isFillEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor, double aLineWidth, double aLayerDepth)
{
    if (isFillEnabled)
    {
        TESStesselator* tesselator = tessNewTess(NULL);;

        std::vector<float> vertices;
        vertices.reserve(aPointCount * 3);

        GLdouble* point = aPoints;
        for (int i = 0; i < aPointCount; ++i)
        {
            vertices.push_back(static_cast<float>(point[0]));
            vertices.push_back(static_cast<float>(point[1]));
            vertices.push_back(static_cast<float>(point[2]));
            point += 3;
        }

        tessAddContour(
            tesselator,
            3,                 // 每个顶点3分量
            vertices.data(),   // float 数组
            sizeof(float) * 3, // 步长改成 float 的大小
            aPointCount
        );

        // Any non convex polygon needs to be tesselated
        // for this purpose the GLU standard functions are used


        if (!tessTesselate(tesselator, TESS_WINDING_ODD, TESS_POLYGONS,
            3,                  //  每个顶点 3 分量
            3,                  //  步长
            nullptr))
        {
            qWarning() << "Tessellation failed!";
            tessDeleteTess(tesselator);
            return;
        }


        const float* verts = tessGetVertices(tesselator);
        const int* elems = tessGetElements(tesselator);
        const int nverts = tessGetVertexCount(tesselator);
        const int nelems = tessGetElementCount(tesselator);

 
        for (int i = 0; i < nelems; i++) {
            const int* poly = &elems[i * 3];
            for (int j = 0; j < 3; ++j)
            {
                int index = poly[j];
                if (index != TESS_UNDEF)
                    InsertVertex({ verts[index * 3 + 0],  verts[index * 3 + 1], }, verts[index * 3 + 2], aFillColor, { SHADER_NONE });
            }
        }

    }

    if (isStrokeEnabled)
    {
        drawPolyline(
            [&](int idx)
            {
                return VECTOR2D(aPoints[idx * 3], aPoints[idx * 3 + 1]);
            },
            aPointCount, aStrokeColor, aLineWidth, aLayerDepth);
    }
}

void MINI::INSERT_VERTEX::drawSegment(const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, double aWidth, bool isFilledEnabled, bool isStokeEnabled, COLOR4D& aFilledColor, COLOR4D& aStrokeColor, double aLayerDepth)
{
    VECTOR2D startEndVector = aEndPoint - aStartPoint;
    double   lineLength = startEndVector.EuclideanNorm();

    // Be careful about floating point rounding.  As we draw segments in larger and larger
    // coordinates, the shader (which uses floats) will lose precision and stop drawing small
    // segments.  In this case, we need to draw a circle for the minimal segment.
    // Check if the coordinate differences can be accurately represented as floats
    float startX = static_cast<float>(aStartPoint.x);
    float startY = static_cast<float>(aStartPoint.y);
    float endX = static_cast<float>(aEndPoint.x);
    float endY = static_cast<float>(aEndPoint.y);

    if (startX == endX && startY == endY)
    {
        drawCircle(aStartPoint, aWidth / 2, isFilledEnabled, isStokeEnabled, aStrokeColor, aFilledColor, aLayerDepth);
        return;
    }

    if (isFilledEnabled || aWidth == 1.0)
        drawLineQuad(aStartPoint, aEndPoint, aFilledColor, aWidth, aLayerDepth);
    else
    {
        EDA_ANGLE lineAngle(startEndVector);

        // Outlined tracks
		pushMatrix();


        Translate(aStartPoint.x, aStartPoint.y, 0.0);
        Rotate(lineAngle.AsRadians(), 0.0f, 0.0f, 1.0f);

        drawLineQuad(VECTOR2D(0.0, aWidth / 2.0), VECTOR2D(lineLength, aWidth / 2.0), aStrokeColor, 1.0, aLayerDepth);

        drawLineQuad(VECTOR2D(0.0, -aWidth / 2.0), VECTOR2D(lineLength, -aWidth / 2.0), aStrokeColor, 1.0, aLayerDepth);

        // Draw line caps
        drawStrokedSemiCircle(VECTOR2D(0.0, 0.0), aStrokeColor, aWidth / 2, M_PI / 2, aLayerDepth, 1.0);
        drawStrokedSemiCircle(VECTOR2D(lineLength, 0.0), aStrokeColor, aWidth / 2, -M_PI / 2, aLayerDepth, 1.0);

        popMatrix();
    }
}

void MINI::INSERT_VERTEX::DrawArc(const VECTOR2D& aCenterPoint, double aRadius,
                         const EDA_ANGLE& aStartAngle, const EDA_ANGLE& aAngle, bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStokeColor, double aLineWidth, double aLayerDepth)
{
    if (aRadius <= 0)
        return;

    double startAngle = aStartAngle.AsRadians();
    double endAngle = startAngle + aAngle.AsRadians();

    // Normalize arc angles
    normalize(startAngle, endAngle);

    const double alphaIncrement = calcAngleStep(aRadius);

    pushMatrix();
    Translate(aCenterPoint.x, aCenterPoint.y, 0.0);

    if (isFilledEnabled)
    {
        double alpha;

        // Triangle fan
        for (alpha = startAngle; (alpha + alphaIncrement) < endAngle; )
        {
            m_currentVertex = m_container->Allocate(3);
			InsertVertex({ 0.0, 0.0 }, aLayerDepth, aFillColor, { SHADER_NONE });
            InsertVertex({ cos(alpha) * aRadius, sin(alpha) * aRadius }, aLayerDepth, aFillColor, { SHADER_NONE });
            alpha += alphaIncrement;
            InsertVertex({ cos(alpha) * aRadius, sin(alpha) * aRadius }, aLayerDepth, aFillColor, { SHADER_NONE });
        }

        // The last missing triangle
        const VECTOR2D endPoint(cos(endAngle) * aRadius, sin(endAngle) * aRadius);

        m_currentVertex = m_container->Allocate(3);
        InsertVertex({ 0.0, 0.0 }, aLayerDepth, aFillColor, { SHADER_NONE });
        InsertVertex({ cos(alpha) * aRadius, sin(alpha) * aRadius }, aLayerDepth, aFillColor, { SHADER_NONE });
        alpha += alphaIncrement;
        InsertVertex({ endPoint.x, endPoint.y }, aLayerDepth, aFillColor, { SHADER_NONE });
    }

    if (isStrokeEnabled)
    {

        VECTOR2D p(cos(startAngle) * aRadius, sin(startAngle) * aRadius);
        double   alpha;
        unsigned int lineCount = 0;

        for (alpha = startAngle + alphaIncrement; alpha <= endAngle; alpha += alphaIncrement)
            lineCount++;

        if (alpha != endAngle)
            lineCount++;

        
        for (alpha = startAngle + alphaIncrement; alpha <= endAngle; alpha += alphaIncrement)
        {
            VECTOR2D p_next(cos(alpha) * aRadius, sin(alpha) * aRadius);
            drawLineQuad(p, p_next, aStokeColor, aLineWidth, aLayerDepth);

            p = p_next;
        }

        // Draw the last missing part
        if (alpha != endAngle)
        {
            VECTOR2D p_last(cos(endAngle) * aRadius, sin(endAngle) * aRadius);
            drawLineQuad(p, p_last, aStokeColor, aLineWidth, aLayerDepth);
        }
    }

    popMatrix();
}

void MINI::INSERT_VERTEX::DrawArcSegment(const VECTOR2D& aCenterPoint, double aRadius,
                                const EDA_ANGLE& aStartAngle, const EDA_ANGLE& aAngle,
								bool isFilledEnabled, bool isStrokeEnabled, COLOR4D& aFillColor, COLOR4D& aStrokeColor,
                                double aWidth, double aLineWidth, double aLayerDepth, double aMaxError)
{
    if (aRadius <= 0)
    {
        // Arcs of zero radius are a circle of aWidth diameter
        if (aWidth > 0)
            DrawCircle(aCenterPoint, aWidth / 2.0, isFilledEnabled, isStrokeEnabled, aFillColor, aStrokeColor, aLayerDepth);

        return;
    }

    double startAngle = aStartAngle.AsRadians();
    double endAngle = startAngle + aAngle.AsRadians();

    // Swap the angles, if start angle is greater than end angle
    normalize(startAngle, endAngle);

    // Calculate the seg count to approximate the arc with aMaxError or less
    int segCount360 = GetArcToSegmentCount(aRadius, aMaxError, FULL_CIRCLE);
    segCount360 = std::max<int>(64, segCount360);
    double alphaIncrement = 2.0 * M_PI / segCount360;

    // Refinement: Use a segment count multiple of 2, because we have a control point
    // on the middle of the arc, and the look is better if it is on a segment junction
    // because there is no approx error
    int seg_count = KiROUND((endAngle - startAngle) / alphaIncrement);

    if (seg_count % 2 != 0)
        seg_count += 1;

    // Our shaders have trouble rendering null line quads, so delegate this task to DrawSegment.
    if (seg_count == 0)
    {
        VECTOR2D p_start(aCenterPoint.x + cos(startAngle) * aRadius,
                         aCenterPoint.y + sin(startAngle) * aRadius);

        VECTOR2D p_end(aCenterPoint.x + cos(endAngle) * aRadius,
                       aCenterPoint.y + sin(endAngle) * aRadius);

        DrawSegment(p_start, p_end, isFilledEnabled, isStrokeEnabled, aFillColor, aStrokeColor, aWidth, aLayerDepth);
        return;
    }

    // Recalculate alphaIncrement with a even integer number of segment
    alphaIncrement = (endAngle - startAngle) / seg_count;

	pushMatrix();
    Translate(aCenterPoint.x, aCenterPoint.y, 0.0);

    if (isStrokeEnabled)
    {
        double   width = aWidth / 2.0;
        VECTOR2D startPoint(cos(startAngle) * aRadius, sin(startAngle) * aRadius);
        VECTOR2D endPoint(cos(endAngle) * aRadius, sin(endAngle) * aRadius);

        drawStrokedSemiCircle(startPoint, aStrokeColor, width, startAngle + M_PI, aLayerDepth, aLineWidth);
        drawStrokedSemiCircle(endPoint, aStrokeColor, width, endAngle, aLayerDepth, aLineWidth);

        VECTOR2D pOuter(cos(startAngle) * (aRadius + width),
                        sin(startAngle) * (aRadius + width));

        VECTOR2D pInner(cos(startAngle) * (aRadius - width),
                        sin(startAngle) * (aRadius - width));

        double alpha;

        for (alpha = startAngle + alphaIncrement; alpha <= endAngle; alpha += alphaIncrement)
        {
            VECTOR2D pNextOuter(cos(alpha) * (aRadius + width),
                                sin(alpha) * (aRadius + width));
            VECTOR2D pNextInner(cos(alpha) * (aRadius - width),
                                sin(alpha) * (aRadius - width));

            DrawLine(pOuter, pNextOuter, aStrokeColor, aLineWidth, aLayerDepth);
            DrawLine(pInner, pNextInner, aStrokeColor, aLineWidth, aLayerDepth);

            pOuter = pNextOuter;
            pInner = pNextInner;
        }

        // Draw the last missing part
        if (alpha != endAngle)
        {
            VECTOR2D pLastOuter(cos(endAngle) * (aRadius + width),
                                sin(endAngle) * (aRadius + width));
            VECTOR2D pLastInner(cos(endAngle) * (aRadius - width),
                                sin(endAngle) * (aRadius - width));

            DrawLine(pOuter, pLastOuter, aStrokeColor, aLineWidth, aLayerDepth);
            DrawLine(pInner, pLastInner, aStrokeColor, aLineWidth, aLayerDepth);
        }
    }

    if (isFilledEnabled)
    {
        VECTOR2D p(cos(startAngle) * aRadius, sin(startAngle) * aRadius);
        double   alpha;

        int lineCount = 0;

        for (alpha = startAngle + alphaIncrement; alpha <= endAngle; alpha += alphaIncrement)
        {
            lineCount++;
        }

        // The last missing part
        if (alpha != endAngle)
        {
            lineCount++;
        }


        for (alpha = startAngle + alphaIncrement; alpha <= endAngle; alpha += alphaIncrement)
        {
            VECTOR2D p_next(cos(alpha) * aRadius, sin(alpha) * aRadius);
            drawLineQuad(p, p_next, aFillColor, aLineWidth, aLayerDepth);

            p = p_next;
        }

        // Draw the last missing part
        if (alpha != endAngle)
        {
            VECTOR2D p_last(cos(endAngle) * aRadius, sin(endAngle) * aRadius);
            drawLineQuad(p, p_last, aFillColor, aLineWidth, aLayerDepth);
        }
    }

    popMatrix();
}
