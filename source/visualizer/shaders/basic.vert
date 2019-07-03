#version 330

in vec3 vertex;
in vec3 color;

uniform mat4 MVP;
uniform vec3 eye;

out vec3 vColor;

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);
    gl_PointSize = 100.0f;
    vColor = color;
}
