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
#include "SOIL.h"
#include "Shader/Shader.cpp"

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
    "layout (location = 1) in vec3 color;\n"
    "layout (location = 2) in vec2 texCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "ourColor = ourColor;\n"
    "TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);\n"
    "}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "out vec4 color;\n"
    "uniform sampler2D ourTexture1;\n"
    "uniform sampler2D ourTexture2;\n"
    "void main()\n"
    "{\n"
    "color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);\n"
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
    
    // 通过本地文件读取着色器程序
    Shader ourShader("shader.vs", "shader.frag");
    
    // 顶点数组
    GLfloat vertices[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    // 索引
    GLuint indices[] = { // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };
    
    // VBO:顶点缓冲对象
    // VAO:顶点数组对象（可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中）
    // EBO:索引缓冲对象
    GLuint VBO,VAO,EBO;
    // 创建顶点数组对象
    glGenVertexArrays(1, &VAO);
    // 创建顶点缓冲对象
    glGenBuffers(1, &VBO);
    // 创建索引缓冲对象
    glGenBuffers(1,&EBO);
    
    // 绑定点点数组对象
    glBindVertexArray(VAO);
    // 绑定顶点缓冲对象到GL_ARRAY_BUFFER上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // GL_STATIC_DRAW ：数据不会或几乎不会改变。
    // GL_DYNAMIC_DRAW：数据会被改变很多。
    // GL_STREAM_DRAW ：数据每次绘制时都会改变。
    // 制定绑定数据data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 绑定索引缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    // 缓冲对象数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    
    /// 链接顶点属性
    
    // 参数说明
    // 参数一：顶点位置（如前面顶点着色器声明的layout(location = 0)）
    // 参数二：指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3
    // 参数三：指定数据的类型，这里是GL_FLOAT
    // 参数四：定义我们是否希望数据被标准化（设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间）
    // 参数五：叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔
    // 参数六：表示位置数据在缓冲中起始位置的偏移量(Offset)，当数组总包含顶点、纹理等多个顶点数据的时候用来区分顶点（纹理）以哪个点开始为x（s）
    // 顶点
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) *8,(GLvoid*)0);
    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    
    // 颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(1);
    
    // 纹理
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) *8,(GLvoid*)(6* sizeof(GLfloat)));
    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(2);
    
    // 取消当前绑定VAO
    glBindVertexArray(0);
    
    /// 纹理
    
    // 声明常见纹理
    GLuint texture,texture2;
    
    /// 第一个纹理
    
    glGenTextures(1, &texture);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    // 配置纹理过滤
    // 设置S轴过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    // 设置T轴过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    // 设置缩小过滤，多级渐远过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // 设置放大过滤时使用渐远纹理过滤方式设置无效，此处设置线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载纹理数据
    // 制定纹理宽高
    int texWidth,texHeight;
    // 加载图片数据
    unsigned char* image = SOIL_load_image("wall.jpg", &texWidth, &texHeight, 0, SOIL_LOAD_RGB);
    // 载入纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    // 设置此选项OpenGL会自动生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放图片资源
    SOIL_free_image_data(image);
    // 解除绑定纹理
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    /// 第二个纹理
    
    glGenTextures(1, &texture2);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 配置纹理过滤
    // 设置S轴过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    // 设置T轴过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    // 设置缩小过滤，多级渐远过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // 设置放大过滤时使用渐远纹理过滤方式设置无效，此处设置线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载纹理数据
    // 制定纹理宽高
    int tex2Width,tex2Height;
    // 加载图片数据
    unsigned char* image2 = SOIL_load_image("awesomeface.jpg", &tex2Width, &tex2Height, 0, SOIL_LOAD_RGB);
    // 载入纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex2Width, tex2Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    // 设置此选项OpenGL会自动生成多级渐远纹理
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放图片资源
    SOIL_free_image_data(image2);
    // 解除绑定纹理
    glBindTexture(GL_TEXTURE_2D, 0);
    
    

    // 开启循环绘制
    while (!glfwWindowShouldClose(window))
    {
        /// 检查响应事件（鼠标、键盘输入）
        glfwPollEvents();
        
        
        /// 渲染事件
        
        // 清空屏幕的颜色缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 激活纹理对象0
        glActiveTexture(GL_TEXTURE0);
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, texture);
        // uniform设置纹理
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        
        // 激活纹理对象1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(ourShader.Program,"ourTexture2"),1);
        
        // 激活着色程序
        ourShader.Use();
        
        // 绑定顶点数组对象
        glBindVertexArray(VAO);
        // 绘制三角形
        // 参数一：制定绘制图元类型
        // 参数二：制定顶点其实索引
        // 参数三：制定绘制顶点个数
//        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // 绘制矩形
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // 解除绑定VAO
        glBindVertexArray(0);
        
        
        /// 交换屏幕缓冲区
        glfwSwapBuffers(window);
    }
    
    // 释放缓冲对象资源以便重新分配
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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
