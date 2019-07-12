#version 330

attribute vec4 vertex;
attribute vec2 colorD;
uniform mat4 MVP;
out vec2 TexCoord;

void main(void)
{
    gl_Position = vec4(vertex.xy,0.0,1.0);
    TexCoord = colorD;
}
