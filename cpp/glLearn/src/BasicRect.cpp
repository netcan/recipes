/*************************************************************************
    > File Name: BasicRect.cpp
    > Descripton: BasicRect
    > Author: Netcan
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-04-07 23:53
************************************************************************/
#include "Shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

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

    unsigned int VAO, VBO, EBO;
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        float vertices[] = { // pos, color
            0.5, 0.5, 0, 1, 0, 0,
            0.5, -0.5, 0, 0, 1, 0,
            -0.5, -0.5, 0, 0, 0, 1,
            -0.5, 0.5, 0, 1, 1, 0,
        };
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

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

    Shader shader {"shaders/basic_rect.vs", "shaders/basic_rect.fs"};
    while (! glfwWindowShouldClose(window)) {
        // Input event
        processInput(window);
        // Render begin
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        shader.enable();
        shader.setUniform("alpha", sin(glfwGetTime() / 2.0) + 0.5);

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

    glfwTerminate();
    return 0;
}
