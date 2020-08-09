/*************************************************************************
    > File Name: Shader.hpp
    > Author: Netcan
    > Descripton: Shader.hpp for encapsulation glshader
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-04-21 23:34
************************************************************************/
#ifndef __SHADER_HPP__
#define __SHADER_HPP__
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "base/ScopeGuard.hpp"

class Shader {
public:
    Shader(const GLchar* vertexPath = nullptr, const GLchar* fragPath = nullptr) {
        auto program = glCreateProgram();
        loadSource(vertexPath, GL_VERTEX_SHADER, program);
        loadSource(fragPath, GL_FRAGMENT_SHADER, program);
        linkSource(program);
        std::swap(program, id_);
    }
    void enable() {
        glUseProgram(id_);
    }

    template <typename T>
    void setUniform(const GLchar* key, T value);

    template <>
    void setUniform(const GLchar* key, bool value)
    { glUniform1i(glGetUniformLocation(id_, key), value); }

    template <>
    void setUniform(const GLchar* key, int value)
    { glUniform1i(glGetUniformLocation(id_, key), value); }

    template <>
    void setUniform(const GLchar* key, double value)
    { glUniform1f(glGetUniformLocation(id_, key), value); }

    ~Shader() {
        glDeleteProgram(id_);
    }

private:
    void loadSource(const GLchar* path, GLenum type, GLuint program) {
        FILE* fp = fopen(path, "rb");
        if (! fp) {
            fprintf(stderr, "open %s: %s\n", path, strerror(errno));
            return;
        }
        fseek(fp, 0, SEEK_END);
        size_t fsize = ftell(fp);
        rewind(fp);
        char* buffer = new char[fsize + 1];
        ON_SCOPE_EXIT {
            fclose(fp);
            delete[] buffer;
        };

        if ( fread(buffer, 1, fsize, fp) != fsize) {
            fprintf(stderr, "read %s: %s\n", path, strerror(errno));
            return;
        }
        buffer[fsize] = '\0';

        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &buffer, nullptr);
        glCompileShader(shader);
        {
            int success;
            char info[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (! success) {
                glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
                fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED:\nfile: %s\n%s\n", path, info);
                return;
            }
        }
        glAttachShader(program, shader);
        glDeleteShader(shader);
    };

    void linkSource(GLuint program) {
        glLinkProgram(program);
        {
            int success;
            char info[512];
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (! success) {
                glGetProgramInfoLog(program, sizeof(info), nullptr, info);
                fprintf(stderr, "ERROR::SHADER::LINK_FAILED: %s\n", info);
            }
        }
    }

private:
    GLuint id_ = 0;
};

#endif
