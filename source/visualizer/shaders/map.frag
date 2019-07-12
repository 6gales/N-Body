#version 330

uniform sampler2D text;
in vec2 TexCoord;

void main(void)
{
    gl_FragColor = texture2D(text, TexCoord).rgba;
}
