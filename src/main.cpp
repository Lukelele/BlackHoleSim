#include "Engine/Renderer.h"

#define WIDTH 1080
#define HEIGHT 1080

using namespace std;
using namespace glm;


int main()
{
    Renderer renderer(WIDTH, HEIGHT);


    Shader shader("src/Shaders/VertexShader.vert", "src/Shaders/FragmentShader.frag");


    vector<vec3> triangleStripVertices = {
        vec3(-0.5f, -0.2f, 0.0f),
        vec3(-0.25f, 0.2f, 0.0f),
        vec3(0.0f, -0.2f, 0.0f),
        vec3(0.5f, 0.2f, 0.0f)
    };
    Object triangleStrip(new TriangleStrip(triangleStripVertices));


    // Main Loop
    while (renderer.IsWindowOpen())
    {
        renderer.StartFrame(vec4(0,0,0,1));


        renderer.Draw(shader, triangleStrip);


        renderer.EndFrame();
    }

    return 0;
}
