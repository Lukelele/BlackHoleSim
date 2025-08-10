#define PI 3.1415926

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <glm/glm.hpp>
#include "VertexArray.h"

using namespace std;
using namespace glm;


string vertexShader = R"(
    #version 410
    layout(location=0) in vec3 pos;
    out vec4 vertexColor;
    void main()
    {
        gl_Position = vec4(pos, 1);
        vertexColor = vec4(1, 1, 1, 1);
    }
)";

string fragmentShader = R"(
    #version 410
    in vec4 vertexColor;
    out vec4 FragColor;
    void main()
    {
        FragColor = vertexColor;
    }
)";



// Compile and create shader object and returns its id
GLuint compileShaders(string shader, GLenum type)
{
    const char *shaderCode = shader.c_str();
    GLuint shaderId = glCreateShader(type);

    if (shaderId == 0)
    { // Error: Cannot create shader object
        cout << "Error creating shaders!";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object

    GLint compileStatus;

    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus)
    { // If compilation was not successfull
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}


GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // crate a program

    if (programId == 0)
    {
        cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    { // If the linking failed
        cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}



int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac

    int width = 640;
    int height = 640;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
       {
           std::cout << "Failed to initialize OpenGL context" << std::endl;
           return -1;
       }

    
    VertexArray circleVA;

    int segments = 36;
    vec3 vertices[segments*3];

    float radius = 0.25;
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

    VertexBuffer circleVertexBuffer(vertices, sizeof(vertices));
    circleVA.AddBuffer(circleVertexBuffer, VertexAttribute(0, 3, GL_FLOAT));

    
    GLuint vShaderId = compileShaders(vertexShader, GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(fragmentShader, GL_FRAGMENT_SHADER);
    
    if (vShaderId == 0 || fShaderId == 0) {
        glfwTerminate();
        return -1;
    }

    GLuint programId = linkProgram(vShaderId, fShaderId);
    if (programId == 0) {
        glfwTerminate();
        return -1;
    }

    // Use this program for rendering.
    glUseProgram(programId);


    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0,0,0,1);


        glDrawArrays(GL_TRIANGLES, 0, 3 * segments);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
