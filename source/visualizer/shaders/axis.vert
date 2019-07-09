#version 330

in vec3 vertex;

uniform mat4 MVP;
out vec3 vColor;

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);
    vColor=vertex;
}
