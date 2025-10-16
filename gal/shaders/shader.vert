#version 330 core

// --- 常量定义 ---
const float SHADER_FILLED_CIRCLE  = 2.0;
const float SHADER_STROKED_CIRCLE = 3.0;
const float SHADER_FONT           = 4.0;
const float SHADER_LINE_A         = 5.0;
const float SHADER_LINE_B         = 6.0;
const float SHADER_LINE_C         = 7.0;
const float SHADER_LINE_D         = 8.0;
const float SHADER_LINE_E         = 9.0;
const float SHADER_LINE_F         = 10.0;
const float SHADER_HOLE_WALL      = 11.0;

const float MIN_WIDTH = 1.0;

// --- 顶点输入 ---
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec4 a_shaderParams;

// --- 输出到片段着色器 ---
out vec4 v_color;
out vec4 v_shaderParams;
out vec2 v_circleCoords;
out vec2 v_texCoord;
// --- uniforms ---
uniform mat4  u_mvp;
uniform float u_worldPixelSize;
uniform vec2  u_screenPixelSize;
uniform float u_pixelSizeMultiplier;
uniform float u_minLinePixelWidth;
uniform vec2  u_antialiasingOffset;

float roundr(float f, float r)
{
    return floor(f / r + 0.5) * r;
}

vec4 roundv(vec4 x, vec2 t)
{
    return vec4(roundr(x.x, t.x), roundr(x.y, t.y), x.z, x.w);
}

// 简化后的只画点的逻辑（方便测试）
void main()
{
    mat4(u_mvp);
    float(u_worldPixelSize);
    vec2(u_screenPixelSize);
    float(u_pixelSizeMultiplier);
    float(u_minLinePixelWidth);
    vec2(u_antialiasingOffset);
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_color = a_color;
    v_shaderParams = a_shaderParams;
    v_circleCoords = a_position.xy;
    v_texCoord = a_position.xy;
}
