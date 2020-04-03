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

/*
 图形渲染管线：
 1、顶点数据
 2、图元（点:GL_POINTS、线、线带:GL_LINE_STRIP、三角形:GL_TRIANGLES）装配
 3、几何着色器：通过产生新顶点构造出新的（或是其它的）图元来生成其他形状；顶点着色器：把3D坐标转为另一种3D坐标，同时顶点着色器允许我们对顶点属性进行一些基本处理
 4、测试与混合：检测片段的对应的深度（和模板(Stencil)）值（后面会讲），用它们来判断这个像素是其它物体的前面还是后面，决定是否应该丢弃
 5、片段着色器：计算一个像素的最终颜色
 6、光栅化：图元映射为最终屏幕上相应的像素，生成供片段着色器(Fragment Shader)使用的片段(Fragment)
 */

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// 申明键盘输入回调
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

int main(int argc,char *argv[])
{
    // 初始化GLFW
    glfwInit();
    // 版本好3.3
    // 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // 次版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 窗口尺寸不可变
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
    // 绑定键盘事件
    glfwSetKeyCallback(window, key_callback);
    // 使用现代方法来检索函数指针和扩展
    glewExperimental = GL_TRUE;
    // 初始化GLEW以设置OpenGL函数指针
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 定义窗口大小
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // 设置视口大小，与窗口一样大
    glViewport(0, 0, width, height);
    
    // 着色器程序编译连接判断
    GLint success;
    // 编译错误信息
    GLchar infoLog[512];
    
    /// 顶点着色器
    
    GLuint vertexShader;
    // 创建顶点着色器
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 附加着色器源码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // 编译着色器
    glCompileShader(vertexShader);
    // 判断着色器是否变异成功
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    /// 片段着色器

    GLuint fragmentShader;
    // 创建片段着色器
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 附加着色器源码
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    // 编译着色器
    glCompileShader(fragmentShader);
    // 判断着色器是否变异成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    /// 着色器程序
    
    // 着色器程序对象
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    // 附加顶点着色器
    glAttachShader(shaderProgram,vertexShader);
    // 附加片段着色器
    glAttachShader(shaderProgram,fragmentShader);
    // 链接着色器程序
    glLinkProgram(shaderProgram);
    // 判断着色器对象是否链接成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADERPROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // 激活着色器对象后需要把着色器释放
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // 顶点数组
    GLfloat vertex[] = {
        -0.5f,-0.5f,0.0f,
        0.5f,-0.5f,0.0f,
        0.0f,0.5f,0.0f
    };
    
    // 顶点缓冲对象、顶点数组对象（可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中）
    GLuint VBO,VAO;
    // 创建顶点数组对象
    glGenVertexArrays(1, &VAO);
    // 创建顶点缓冲对象
    glGenBuffers(1, &VBO);
    // 绑定点点数组对象
    glBindVertexArray(VAO);
    // 绑定顶点缓冲对象到GL_ARRAY_BUFFER上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // GL_STATIC_DRAW ：数据不会或几乎不会改变。
    // GL_DYNAMIC_DRAW：数据会被改变很多。
    // GL_STREAM_DRAW ：数据每次绘制时都会改变。
    // 制定绑定数据data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    
    /// 链接顶点属性
    
    // 参数说明
    // 参数一：顶点位置（如前面顶点着色器声明的layout(location = 0)）
    // 参数二：指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3
    // 参数三：指定数据的类型，这里是GL_FLOAT
    // 参数四：定义我们是否希望数据被标准化（设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间）
    // 参数五：叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔
    // 参数六：表示位置数据在缓冲中起始位置的偏移量(Offset)，当数组总包含顶点、纹理等多个顶点数据的时候用来区分顶点（纹理）以哪个点开始为x（s）
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) *3,(GLvoid*)0);
    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    // 通常情况下配置好OpenGL对象以后要解绑它们，这样才不会在其它地方错误地配置它们。
    // 取消绑定VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 取消当前绑定VAO
    glBindVertexArray(0);
    

    // 开启循环绘制
    while (!glfwWindowShouldClose(window))
    {
        /// 检查响应事件（鼠标、键盘输入）
        glfwPollEvents();
        
        
        /// 渲染事件
        
        // 清空屏幕的颜色缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 激活着色程序
        glUseProgram(shaderProgram);
        // 绑定顶点数组对象
        glBindVertexArray(VAO);
        // 绘制三角形
        // 参数一：制定绘制图元类型
        // 参数二：制定顶点其实索引
        // 参数三：制定绘制顶点个数
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // 解除绑定VAO
        glBindVertexArray(0);
        
        
        /// 交换屏幕缓冲区
        glfwSwapBuffers(window);
    }

    // 释放资源
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
    // 关闭应用程序
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
