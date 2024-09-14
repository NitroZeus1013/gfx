#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>

enum class ShaderType
{
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
};

void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << "[OpenGL error] (" << type << ")" << message << std::endl;
}

static std::pair<std::string, std::string> ParseShader(const std::string &file_path)
{
    std::ifstream stream(file_path);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            if (type == ShaderType::NONE)
                continue;
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static int CompileShader(unsigned int type, const std::string &source)
{
    unsigned int shaderId = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shaderId, 1, &src, NULL);
    glCompileShader(shaderId);

    int is_success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &is_success);
    if (!is_success)
    {
        char log[1024];
        glGetShaderInfoLog(shaderId, 1024, NULL, log);
        std::cout << "Error while compiling " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << log << std::endl;
    }

    return shaderId;
}

// creates and return the id of the shader program
// takes 2 shader program and compiles them and then combines them to single shader and return its id
static unsigned int CreateShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    int is_success;
    glGetProgramiv(program, GL_LINK_STATUS, &is_success);
    if (!is_success)
    {
        char log[1024];
        glGetProgramInfoLog(program, 1024, NULL, log);
        std::cout << "Unable to link program  " << log << std::endl;
    }
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{

    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // set glfw to enable debug messages
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    GLenum err = glewInit(); // glew needs valid glfw context that's why called after glfwMakeContextCurrent

    glDebugMessageCallback(debugCallback, nullptr); // glDebugMessageCallback needs valid glfw context that's why called after glfwMakeContextCurrent

    if (err != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }
    else
    {
        std::cout << "GLEW initialized successfully" << std::endl;
    }

    // Your OpenGL code goes here...
    std::cout << glGetString(GL_VERSION) << std::endl;
    // vertex buffer first two co-ordinates are position and
    /*
    each vertex has 2 attributes ( 4 floats )
    each attribute has 2 float
    1st attribute is position
    2nd attribute is texture co-ordinate for telling the fragment shader where to use which color
     */
    // float postions[] = {
    //     -0.5f,
    //     -0.5f,
    //     1.0f,
    //     1.0f,

    //     0.5f,
    //     -0.5f,
    //     1.0f,
    //     0.0f,

    //     0.5f,
    //     0.5f,
    //     0.0f,
    //     0.0f,

    //     -0.5f,
    //     0.5f,
    //     0.0f,
    //     1.0f,

    // };

    float postions[] = {
        -0.5f,
        -0.5f,

        0.5f,
        -0.5f,

        0.5f,
        0.5f,

        -0.5f,
        0.5f,

    };

    // index buffer
    unsigned int indices[] = {
        0,
        1,
        2,
        2,
        3,
        0

    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), postions, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);

    /**
     * in a vertex we can have many attributes
     * stride is the bytes between each vertex or say size of each vertex
     * pointer is the position/offset of attribute inside a vertex.
     * lets say a vertex has position(x,y) and color(rgb)
     * so pointer for position is 0 because we first write postion then color, pointer for color is 8
     * as position takes 8 bytes (0-7)
     */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // position attribute
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float))); // texture co-ordinate

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STREAM_DRAW);

    auto shaders = ParseShader("res/shaders/basic.shader");

    unsigned int shader_program = CreateShaderProgram(shaders.first, shaders.second);
    glUseProgram(shader_program);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 3); // our shader will be executed on this draw call
        // if you don't do this call no shader will be called
        // if we dont write glUseProgram(our_program), it will use default one provided by GPU.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteProgram(shader_program);
    glfwTerminate();
    return 0;
}