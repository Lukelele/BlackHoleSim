#include "Engine/Renderer.h"


using namespace std;
using namespace glm;


int main()
{
    Renderer renderer(1080, 1080);


    Shader shader("src/Shaders/VertexShader.vert", "src/Shaders/FragmentShader.frag");


    Circle circle(0.15f, glm::vec3(0.0f, 0.0f, 0.0f));


    VertexBuffer circleVertexBuffer(circle.Vertices.data(), circle.Vertices.size() * sizeof(glm::vec3));

    VertexArray va;
    va.AddBuffer(circleVertexBuffer, VertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0));


    // Main Loop
    while (renderer.IsWindowOpen())
    {
        renderer.StartFrame(vec4(0.05,0.05,0.05,1));

        
        renderer.Draw(shader, va);


        renderer.EndFrame();
    }

    return 0;
}
