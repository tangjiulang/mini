in vec4 v_offset;
in vec2 v_texcoord;
uniform sampler2D colorTex;
uniform sampler2D blendTex;

void main()
{
    gl_FragColor = SMAANeighborhoodBlendingPS(v_texcoord, v_offset, colorTex, blendTex);
}