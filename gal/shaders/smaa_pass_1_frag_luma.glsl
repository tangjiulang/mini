in vec2 v_texcoord;
in vec4 v_offset[3];
uniform sampler2D colorTex;

void main()
{
    gl_FragColor.xy = SMAALumaEdgeDetectionPS(v_texcoord, v_offset, colorTex).xy;
}