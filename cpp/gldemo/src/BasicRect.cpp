/*************************************************************************
    > File Name: main.cpp
    > Descripton: gldemo
    > Author: Netcan
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-04-07 23:53
************************************************************************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

static const char* VertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

static const char* FragShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 inColor;
void main()
{
    FragColor = inColor;
}
)";

int main(int argc, char** argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "GlDemo", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
        return glViewport(0, 0, width, height);
    });

    unsigned int shaderProgram;
    {
        auto errorHandle = [](unsigned int shader) {
            int success;
            char info[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (! success) {
                glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
                std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << info << std::endl;
            }
        };
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &VertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        errorHandle(vertexShader);

        unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &FragShaderSource, nullptr);
        glCompileShader(fragShader);
        errorHandle(fragShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragShader);
        glLinkProgram(shaderProgram);
        {
            int success;
            char info[512];
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (! success) {
                glGetProgramInfoLog(shaderProgram, sizeof(info), nullptr, info);
                std::cout << "ERROR::SHADER::LINK_FAILED\n" << info << std::endl;
            }
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    unsigned int VAO, VBO, EBO;
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        float vertices[] = {
            0.5, 0.5, 0,
            0.5, -0.5, 0,
            -0.5, -0.5, 0,
            -0.5, 0.5, 0,
        };
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
        };
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    while (! glfwWindowShouldClose(window)) {
        // Input event
        processInput(window);
        // Render begin
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);

        float greenColor = sin(glfwGetTime()) / 2 + 0.5;
        int inColorPos = glGetUniformLocation(shaderProgram, "inColor");
        glUniform4f(inColorPos, 0, greenColor, 0, 1);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);


        // Render end
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
