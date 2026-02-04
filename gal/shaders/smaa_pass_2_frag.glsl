in vec2 v_texcoord;
in vec2 v_pixcoord;
in vec4 v_offset[3];
uniform sampler2D edgesTex;
uniform sampler2D areaTex;
uniform sampler2D searchTex;

void main()
{
    gl_FragColor = SMAABlendingWeightCalculationPS(v_texcoord, v_pixcoord, v_offset, edgesTex, areaTex, searchTex, vec4(0.,0.,0.,0.));
}