// ===== 顶点输入 =====
layout(location = 0) in vec3 a_pos;      // 全屏三角形 / quad 的位置
layout(location = 1) in vec2 a_texcoord; // 纹理坐标

// ===== 传给 Fragment Shader =====
out vec2 v_texcoord;
out vec4 v_offset;

void main()
{
    v_texcoord = a_texcoord;

    SMAANeighborhoodBlendingVS(v_texcoord, v_offset);

    gl_Position = vec4(a_pos, 1.0);
}
