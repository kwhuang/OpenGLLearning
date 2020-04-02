//
//  main.cpp
//  OpenGLGLFW
//
//  Created by huangkaiwei on 2020/4/2.
//  Copyright © 2020 hq_kven. All rights reserved.
//

#include <stdio.h>
#include <iostream>
// GLEW
#define GLEW_STATIC
#include "glew.h"
#include "glfw3.h"
#include "glfw3native.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

int main(int argc,char *argv[])
{
    // 初始化GLFW
    glfwInit();
    // 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // 次版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // mac配置需要这一行
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    
    // 判断窗口是否创建成功
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 定义窗口大小
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // 开启循环绘制
    while (!glfwWindowShouldClose(window))
    {
        // 检查响应事件（鼠标、键盘输入）
        glfwPollEvents();
        
        // S交换屏幕缓冲区
        glfwSwapBuffers(window);
    }

    // 释放资源
    glfwTerminate();
    return 0;
}
