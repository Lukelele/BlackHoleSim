#version 410
layout(location=0) in vec3 pos;
out vec4 vertexColor;

void main()
{
    gl_Position = vec4(pos, 1);
    vertexColor = vec4(1, 1, 1, 1);
}