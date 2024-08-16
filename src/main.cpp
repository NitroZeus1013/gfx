#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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
    glAttachShader(program, vs);
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
    GLenum err = glewInit();

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
    // Terminate GLFW
    float postions[6] = {
        -0.5f,
        -0.5f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f};
    unsigned int buffer;
    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), postions, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    /**
     * in a vertex we can have many attributes
     * stride is the bytes between each vertex or say size of each vertex
     * pointer is the position/offset of attribute inside a vertex.
     * lets say a vertex has position(x,y) and color(rgb)
     * so pointer for position is 0 because we first write postion then color, pointer for color is 8
     * as position takes 8 bytes (0-7)
     */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    const char *vertex_shader_source = R"(
        #version 330 core
        layout(location = 0) in vec4 pos;
        
        void main(){
            gl_Position = pos;
        }
    )";

    const char *fragment_shader_source = R"(
        #version 330 core
        out vec4 color;
        void main(){
            color = vec4(1.0,0,0,1.0);
        }
    )";

    unsigned int shader_program = CreateShaderProgram(vertex_shader_source, fragment_shader_source);

    glUseProgram(shader_program);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3); // our shader will be executed on this draw call
        // if you don't do this call no shader will be called
        // if we dont write glUseProgram(our_program), it will use default one provided by GPU.

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}