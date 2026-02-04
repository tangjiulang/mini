// ===== 顶点输入 =====
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_texcoord;

// ===== 输出到 Fragment =====
out vec2 v_texcoord;
out vec4 v_offset[3];

// ===== Uniform =====
uniform mat4 u_mvp;

// SMAA 顶点函数声明（必须有）
void SMAAEdgeDetectionVS(in vec2 texcoord, out vec4 offset[3]);

void main()
{
    // 1️⃣ 传递 UV
    v_texcoord = a_texcoord;

    // 2️⃣ 计算 SMAA offset
    SMAAEdgeDetectionVS(v_texcoord, v_offset);

    // 3️⃣ 计算裁剪空间位置（替代 ftransform）
    gl_Position = u_mvp * vec4(a_pos, 0.0, 1.0);
}
