#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


string vertexShader = R"(
    #version 410
    layout(location=0) in vec3 pos;
    layout(location=1) in vec3 color;
    out vec4 vertexColor;
    void main()
    {
        gl_Position = vec4(pos, 1);
        vertexColor = vec4(color, 1);
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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


    GLfloat vertices[] = {// triangle vertex coordinates
        -0.5, -0.5, 0,
        0.5, -0.5, 0,
        0, 0.5, 0
    };
    GLfloat vertexColor[] = {
        1,0,0,
        0,1,0,
        0,0,1
    };
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);  // Crucial for core profile

    GLuint vertexBuffer;
    GLuint vertexColorBuffer;
    // allocate buffer sapce and pass data to it
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(0); // Enable this attribute array linked to 'pos'
    
    glGenBuffers(1, &vertexColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);
    
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



//    GLuint programID = LoadShaders( "vertexShader.glsl", "fragmentShader.glsl");


    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0,0,0,1);

//        glUseProgram(programId);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
