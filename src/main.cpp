#include "Engine/Renderer.h"

#define WIDTH 1080
#define HEIGHT 1080

using namespace std;
using namespace glm;


int main()
{
    Renderer renderer(WIDTH, HEIGHT);


    Shader shader("src/Shaders/VertexShader.vert", "src/Shaders/FragmentShader.frag");


    CircleFan circle(0.15f, glm::vec3(0.0f, 0.0f, 0.0f));

    VertexBuffer circleVertexBuffer(circle.Vertices.data(), circle.Vertices.size() * sizeof(glm::vec3));

    VertexArray circleVA;
    circleVA.AddBuffer(circleVertexBuffer, VertexAttribute(0, 3, GL_FLOAT));


    // Main Loop
    while (renderer.IsWindowOpen())
    {
        renderer.StartFrame(vec4(0,0,0,1));


        renderer.Draw(shader, circleVA, circle.VertexCount, circle.DrawMode);


        renderer.EndFrame();
    }

    return 0;
}
