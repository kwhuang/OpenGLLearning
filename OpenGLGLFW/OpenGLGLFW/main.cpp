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
// glm
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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

// 摄像机位置
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
// 摄像机位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// 上轴
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
// 记录上一个按钮及按下状态
bool keys[1024];
// 根据按下的按键来更新摄像机的值
void do_movement();


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
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 通过本地文件读取着色器程序
    Shader ourShader("shader.vs", "shader.frag");
    
    // 顶点数组
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    // 正方体世界空间位置
    glm::vec3 cubePositions[] = {
      glm::vec3( 0.0f,  0.0f,  0.0f),
      glm::vec3( 2.0f,  5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3( 2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f,  3.0f, -7.5f),
      glm::vec3( 1.3f, -2.0f, -2.5f),
      glm::vec3( 1.5f,  2.0f, -2.5f),
      glm::vec3( 1.5f,  0.2f, -1.5f),
      glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    // VBO:顶点缓冲对象
    // VAO:顶点数组对象（可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中）
    // EBO:索引缓冲对象
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    /// 链接顶点属性
    
    // 参数说明
    // 参数一：顶点位置（如前面顶点着色器声明的layout(location = 0)）
    // 参数二：指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3
    // 参数三：指定数据的类型，这里是GL_FLOAT
    // 参数四：定义我们是否希望数据被标准化（设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间）
    // 参数五：叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔
    // 参数六：表示位置数据在缓冲中起始位置的偏移量(Offset)，当数组总包含顶点、纹理等多个顶点数据的时候用来区分顶点（纹理）以哪个点开始为x（s）
    // 顶点
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) *5,(GLvoid*)0);
    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    
    // 纹理
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) *5,(GLvoid*)(3* sizeof(GLfloat)));
    // 以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(2);
    
    // 取消当前绑定VAO
    glBindVertexArray(0);
    
    /// 纹理
    
    // 声明纹理
    GLuint texture,texture2;
    
    // 第一个纹理
    
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
    
    // 第二个纹理
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int tex2Width,tex2Height;
    unsigned char* image2 = SOIL_load_image("awesomeface.jpg", &tex2Width, &tex2Height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex2Width, tex2Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image2);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // 开启循环绘制
    while (!glfwWindowShouldClose(window))
    {
        /// 检查响应事件（鼠标、键盘输入）
        glfwPollEvents();
        
        // 根据按下的按键来更新摄像机的值
        do_movement();
        
        /// 渲染事件
        
        // 清空屏幕的颜色缓冲、深度测试缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        // 添加纹理
        
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
        
        
        // glm变换组合矩阵（其实就是矩阵相乘得到的结果，注意矩阵组合先后顺序，最右边的优先与向量相乘）
        
        // 模型矩阵
        glm::mat4 model;
        model = glm::rotate(model, (GLfloat)glfwGetTime() * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f));
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // 观察矩阵
        glm::mat4 view;
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // 透视矩阵
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        GLint projectionLoc = glGetUniformLocation(ourShader.Program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // 激活着色程序
        ourShader.Use();
        
        // 绑定顶点数组对象
        glBindVertexArray(VAO);
        
        // 绘制矩形
        for (GLuint i = 0; i < 10; i++)
        {
            // 重新设置观察矩阵
            // 通过cameraPos + cameraFront计算出观看目标位置
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            
            // 重新设置正方体位置
            glm::mat4 model;
            // 移动
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            // 旋转角度
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // 解除绑定VAO
        glBindVertexArray(0);
        
        /// 交换屏幕缓冲区
        glfwSwapBuffers(window);
    }
    
    // 释放缓冲对象资源以便重新分配
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
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
    GLfloat cameraSpeed = 0.05f;
    if(key == GLFW_KEY_W)
        cameraPos += cameraSpeed * cameraFront;
    if(key == GLFW_KEY_S)
        cameraPos -= cameraSpeed * cameraFront;
    if(key == GLFW_KEY_A)
        // 右向量标准化，如果我们没对这个向量进行标准化，最后的叉乘结果会根据cameraFront变量的大小返回不同的大小
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(key == GLFW_KEY_D)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // 按钮按下、松手记录状态
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void do_movement()
{
  // 摄像机控制
  GLfloat cameraSpeed = 0.01f;
  if(keys[GLFW_KEY_W])
    cameraPos += cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_S])
    cameraPos -= cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_A])
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if(keys[GLFW_KEY_D])
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
