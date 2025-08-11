#define PI 3.1415926

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <glm/glm.hpp>
#include "Engine/VertexArray.h"
#include "Engine/Shader.h"
#include "Engine/Renderer.h"

using namespace std;
using namespace glm;


int main()
{
    Renderer renderer(1080, 1080);


    Shader shader("src/Shaders/VertexShader.vert", "src/Shaders/FragmentShader.frag");


    const int segments = 72;
    array<vec3, segments * 3> vertices;

    float radius = 0.15;
    vec3 origin(0,0,0);

    for (int i = 0; i < segments; i++) {
        float angle = 2 * PI / segments * i;
        float next_angle = 2 * PI / segments * (i + 1);
        vec3 p1 = origin + vec3(radius * cos(angle), radius * sin(angle), 0);
        vec3 p2 = origin + vec3(radius * cos(next_angle), radius * sin(next_angle), 0);

        vertices[i*3] = origin;
        vertices[i*3+1] = p1;
        vertices[i*3+2] = p2;
    }

    VertexBuffer circleVertexBuffer(vertices.data(), sizeof(vertices));


    VertexArray va;
    va.AddBuffer(circleVertexBuffer, VertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));


    // Main Loop
    while (renderer.IsWindowOpen())
    {
        renderer.StartFrame(vec4(0.05,0.05,0.05,1));

        
        renderer.Draw(shader, va);


        renderer.EndFrame();
    }

    return 0;
}
