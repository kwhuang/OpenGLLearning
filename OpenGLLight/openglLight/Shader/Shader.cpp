//
//  shader.cpp
//  OpenGLGLFW
//
//  Created by kven on 2020/4/6.
//  Copyright © 2020 hq_kven. All rights reserved.
//

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glew.h"
#include "type_ptr.hpp"

class Shader
{
public:
    GLuint Program;
    // 通过本地文件读取着色器代码，并关联着色器程序
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        // 着色器代码和文件
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // 确保ifstream对象可以引发异常：
        vShaderFile.exceptions (std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::badbit);
        try
        {
            // 打开着色器文件
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            //读取文件内容缓存至 streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // 关闭文件
            vShaderFile.close();
            fShaderFile.close();
            // 将流转换为字符串
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        // 顶点着色器c字符串
        const GLchar* vShaderCode = vertexCode.c_str();
        
        // 片段着色器c字符串
        const GLchar * fShaderCode = fragmentCode.c_str();
        // 生命顶点、片段着色器
        GLuint vertex, fragment;
        // 着色器编译连接信息
        GLint success;
        GLchar infoLog[512];
        // 顶点着色器
        vertex = glCreateShader(GL_VERTEX_SHADER);
        // 连接着色器代码资源
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        // 编译连接着色器代码
        glCompileShader(vertex);
        // 判断着色器程序是否连接成功
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        
        // 片着色器
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        
        // 着色器程序
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // 删除着色器，因为它们已链接到着色器程序中了
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    }
    // 激活着色器程序
    void Use()
    {
        glUseProgram(Program);
    }
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(Program, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(Program, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(Program, name.c_str()), value);
    }
    void setVec3(const std::string &name, glm::vec3 value) const
    {
        glUniform3f(glGetUniformLocation(Program, name.c_str()),value.x,value.y,value.z);
    }
    void setMat4(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
};

#endif
