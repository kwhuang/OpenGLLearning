//
//  main.cpp
//  OpenGLLight
//
//  Created by huangkaiwei on 2020/4/10.
//  Copyright © 2020 hq_kven. All rights reserved.
//

#include <stdio.h>

#include <iostream>
// GLEW
#define GLEW_STATIC
#include "glew.h"
#include "glfw3.h"
#include "glfw3native.h"

#include "Shader/Shader.cpp"
#include "Camera/Camera.cpp"

// glm
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const GLuint WIDTH = 800, HEIGHT = 600;

// 申明键盘输入回调
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// 根据按下的按键来更新摄像机的值
void do_movement();
// 鼠标事件监听
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// 鼠标滚动监听
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool firstMouse = true;

// 摄像机
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX  =  WIDTH  / 2.0;
GLfloat lastY  =  HEIGHT / 2.0;
bool    keys[1024];

// 光源位置
glm::vec3 lightPos(1.2f, 1.0f, 1.0f);

// 每一帧绘制事件
GLfloat deltaTime = 0.0f;   
GLfloat lastFrame = 0.0f;

int main(int argc,char *argv[]) {
    glfwInit();
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
    
    /// 事件绑定
    
    // 绑定键盘事件
    glfwSetKeyCallback(window, key_callback);
    // 设置捕获鼠标位置并因此光标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 添加鼠标监听
    glfwSetCursorPosCallback(window, mouse_callback);
    // 监听鼠标滚动
    glfwSetScrollCallback(window, scroll_callback);
    
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
    
    Shader ourShader("shader.vs", "shader.frag");
    Shader lightShader("lightShader.vs","lightShader.frag");
    
    // 正方形顶点
    float vertices[] = {
        // 顶点                // 法向量
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    GLuint VBO,VAO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    
    // 开启循环绘制
    while (!glfwWindowShouldClose(window))
    {
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        do_movement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();
        // 物体颜色
        GLint objectColorLoc = glGetUniformLocation(ourShader.Program, "objectColor");
        // 光源颜色
        GLint lightColorLoc  = glGetUniformLocation(ourShader.Program, "lightColor");
        // 光源位置
        GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");
        // 观察者（摄像机）位置
        GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
        glUniform3f(lightColorLoc,  1.0f, 0.5f, 1.0f);
        
        // 设置材质属性
        GLint matAmbientLoc = glGetUniformLocation(ourShader.Program, "material.ambient");
        GLint matDiffuseLoc = glGetUniformLocation(ourShader.Program, "material.diffuse");
        GLint matSpecularLoc = glGetUniformLocation(ourShader.Program, "material.specular");
        GLint matShineLoc = glGetUniformLocation(ourShader.Program, "material.shininess");

        glUniform3f(matAmbientLoc, 1.0f, 0.5f, 0.31f);
        glUniform3f(matDiffuseLoc, 1.0f, 0.5f, 0.31f);
        glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
        glUniform1f(matShineLoc, 32.0f);
        
        // 设置每个光的亮度
        GLint lightAmbientLoc = glGetUniformLocation(ourShader.Program, "light.ambient");
        GLint lightDiffuseLoc = glGetUniformLocation(ourShader.Program, "light.diffuse");
        GLint lightSpecularLoc = glGetUniformLocation(ourShader.Program, "light.specular");

        glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
        glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);// 让我们把这个光调暗一点，这样会看起来更自然
        glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
        
        // 根据时间不同改变不同光源的颜色
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
        glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);

        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(ourShader.Program,  "view");
        GLint projLoc  = glGetUniformLocation(ourShader.Program,  "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glm::mat4 model;
        model = glm::rotate(model, glm::radians(20.f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        lightShader.Use();
        modelLoc = glGetUniformLocation(lightShader.Program, "model");
        viewLoc  = glGetUniformLocation(lightShader.Program, "view");
        projLoc  = glGetUniformLocation(lightShader.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

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
    // 按钮按下、松手记录状态
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void do_movement()
{
  // 摄像机控制
  if(keys[GLFW_KEY_W])
      camera.ProcessKeyboard(FORWARD, deltaTime);
  if(keys[GLFW_KEY_S])
      camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
  if(keys[GLFW_KEY_D])
      camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // 判断是否为首次出现鼠标位置，避免因初始位置偏移量过大产生跳动
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    // 鼠标偏移量
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    // 记录最后一帧鼠标偏移量
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
