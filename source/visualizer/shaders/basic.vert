#version 330


in vec3 vertex;
in float mass;

uniform mat4 MVP;
out float massF;


void main()
{
    gl_Position = MVP * vec4(vertex, 1.0f);
    gl_PointSize = 1;
    massF=mass;
}
