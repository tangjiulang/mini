// ===== Vertex attributes =====
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texcoord;

// ===== Uniforms =====
uniform mat4 u_mvp;
uniform vec2 u_renderTargetSize;

// ===== 输出到 Fragment Shader =====
out vec2 v_texcoord;
out vec2 v_pixcoord;
out vec4 v_offset[3];

// SMAA 函数声明（必须存在）
void SMAABlendingWeightCalculationVS(
    in vec2 texcoord,
    out vec2 pixcoord,
    out vec4 offset[3]
);

void main()
{
    v_texcoord = a_texcoord;

    // SMAA 需要像素坐标（0..width, 0..height）
    v_pixcoord = a_texcoord * u_renderTargetSize;

    SMAABlendingWeightCalculationVS(
        v_texcoord,
        v_pixcoord,
        v_offset
    );

    gl_Position = u_mvp * vec4(a_pos, 1.0);
}
