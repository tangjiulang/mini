#version 330 core

#define USE_MSDF

const float SHADER_FILLED_CIRCLE  = 2.0;
const float SHADER_STROKED_CIRCLE = 3.0;
const float SHADER_FONT           = 4.0;
const float SHADER_LINE_A         = 5.0;

in vec4 v_shaderParams;
in vec2 v_circleCoords;
in vec4 v_color;
in vec2 v_texCoord;

out vec4 fragColor;

uniform sampler2D u_fontTexture;
uniform float u_worldPixelSize;
uniform int u_fontTextureWidth;

float pixelSegDistance(vec2 aCoord)
{
    float aspect = v_shaderParams[1];
    vec2 v = vec2(1.0 - (aspect - abs(aCoord.s)), aCoord.t);
    float dist;
    if (v.x <= 0.0)
        dist = abs(aCoord.t);
    else
        dist = length(v);
    return dist;
}

int isPixelInSegment(vec2 aCoord)
{
    return pixelSegDistance(aCoord) <= 1.0 ? 1 : 0;
}

void filledCircle(vec2 aCoord)
{
    if (dot(aCoord, aCoord) < 1.0)
        fragColor = v_color;
    else
        discard;
}

void strokedCircle(vec2 aCoord, float aRadius, float aWidth)
{
    float outerRadius = max(aRadius, 0.0);
    float innerRadius = max(aRadius - aWidth, 0.0);

    float len2 = dot(aCoord, aCoord);
    if ((len2 < 1.0) && (len2 * (outerRadius * outerRadius) > innerRadius * innerRadius))
        fragColor = v_color;
    else
        discard;
}

void drawLine(vec2 aCoord)
{
    if (isPixelInSegment(aCoord) != 0)
        fragColor = v_color;
    else
        discard;
}

#ifdef USE_MSDF
float median(vec3 v)
{
    return max(min(v.r, v.g), min(max(v.r, v.g), v.b));
}
#endif

void main()
{
    float mode = floor(v_shaderParams[0] + 0.5);

    if (mode == SHADER_LINE_A)
        drawLine(v_texCoord);
    else if (mode == SHADER_FILLED_CIRCLE)
        filledCircle(v_circleCoords);
    else if (mode == SHADER_STROKED_CIRCLE)
        strokedCircle(v_circleCoords, v_shaderParams[2], v_shaderParams[3]);
    else
        fragColor = v_color;
}
