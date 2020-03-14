//
//  main.cpp
//  演示Mip贴图以及使用纹理对象
//
//  Created by huangkaiwei on 2020/3/14.
//  Copyright © 2020 hq_kven. All rights reserved.
//

#include "GLTools.h"
// 着色器管理
#include "GLShaderManager.h"
// 视景体
#include "GLFrustum.h"
// 批次
#include "GLBatch.h"
// 投影
#include "GLFrame.h"
// 矩阵堆栈
#include "GLMatrixStack.h"
// 矩阵变换
#include "GLGeometryTransform.h"

#include <GLUT/GLUT.h>

// 纹理宏
#include "glew.h"



/// 着色器管理器
GLShaderManager shaderManager;

/// 模型视图矩阵
GLMatrixStack modelViewMatrix;

/// 投影矩阵
GLMatrixStack projectionMatrix;

/// 视景体
GLFrustum viewFrustum;

/// 几何变换管线
GLGeometryTransform transformPipeline;

GLBatch floorBatch;

GLBatch ceilingBatch;

GLBatch leftWallBatch;

GLBatch rightWallBatch;

GLfloat viewZ = -65.0f;

// 纹理对象
#define TEXTURE_BRICK   0
#define TEXTURE_FLOOR   1
#define TEXTURE_CEILING 2
#define TEXTURE_COUNT   3

GLuint textures[TEXTURE_COUNT];

// 缺少tga纹理文件
const char *szTextureFiles[TEXTURE_COUNT] = {"brick.tga","floor.tga","ceiling.tga"};

// 为每个纹理对象改变纹理过滤器
void ProcessMenu(int value)
{
    GLint iLoop;
    
    for (iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
    {
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        
        // 纹理过滤
        switch (value) {
            case 0:
            {
                // 在Mip基层上执行最邻近过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
                break;
            case 1:
            {
                // 在Miph基层上执行线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
                break;
            case 2:
            {
                // 选择最邻近Mip层，并执行最邻近过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            }
                break;
            case 3:
            {
                // 在Mip层之间执行线性插补，并执行最邻近过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            }
                break;
            case 4:
            {
                // 选择最邻近Mip层，并执行线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            }
                break;
            case 5:
            {
                // 在Mip层之间执行线性插补，并执行线性过滤，又称三线性Mip贴图
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            }
                break;
                
            default:
                break;
        }
    }
    
    // 触发重绘
    glutPostRedisplay();
    
}

void SetupRC()
{
    GLbyte *pBytes;
    GLint iWidth,iHeight,iComponents;
    GLenum eFormat;
    GLint iLoop;
    
    // 黑色背景
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 链接当前着色器
    shaderManager.InitializeStockShaders();
    
    // 加载纹理
    glGenTextures(TEXTURE_COUNT, textures);
    
    for (iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
    {
        // 绑定下一个纹理对象
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        
        // 加载纹理
        pBytes = gltReadTGABits(szTextureFiles[iLoop], &iWidth, &iHeight,&iComponents,&eFormat);
        
        // 设置过滤器和环绕模式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // 不在需要初始纹理数据
        free(pBytes);
    }
    
    // 建立几何图形
    GLfloat z;
    // 底部
    floorBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for (z = 60.0f; z >= 0.0f; z-= 10.0f)
    {
        // 纹理坐标
        floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        // 顶点坐标
        floorBatch.Vertex3f(-10.f, -10.f, z);
        
        floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        floorBatch.Vertex3f(10.f, -10.f, z);
        
        floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        floorBatch.Vertex3f(-10.f, -10.f, z - 10.f);
        
        floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        floorBatch.Vertex3f(10.f, -10.f, z - 10.f);
        
    }
    floorBatch.End();
    
    // 顶部
    ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for (z = 60.0f; z >= 0.0f; z-= 10.0f)
    {
        ceilingBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.f);
        
        ceilingBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
        
        ceilingBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        ceilingBatch.Vertex3f(-10.f, 10.0f, z);
        
        ceilingBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z);
        
    }
    ceilingBatch.End();
    
    // 左边
    leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for (z = 60.0f; z >= 0.0f; z-= 10.0f)
    {
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z );
        
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z );
        
        leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.f, -10.0f, z - 10.0f);
        
        leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
        
    }
    leftWallBatch.End();
    
    // 右边
    rightWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for (z = 60.0f; z >= 0.0f; z-= 10.0f)
    {
        rightWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        rightWallBatch.Vertex3f(10.0f, -10.0f, z );
        
        rightWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        rightWallBatch.Vertex3f(10.0f, 10.0f, z );
        
        rightWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        rightWallBatch.Vertex3f(10.f, -10.0f, z - 10.0f);
        
        rightWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        rightWallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
        
    }
    rightWallBatch.End();
}

// 关闭渲染环境
void ShutdownRC(void)
{
    glDeleteTextures(TEXTURE_COUNT, textures);
}

// 前后移动视口来对方向键作出响应
void SpecialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
    {
        viewZ += 0.5f;
    }
    if (key == GLUT_KEY_DOWN)
    {
        viewZ -= 0.5f;
    }
    
    // 更新窗口
    glutPostRedisplay();
}


// 改变视景体和视口，在改变窗口大小是调用
void ChangeSize(int w,int h)
{
    GLfloat fAspect;
    
    // 防止对0进行除法
    if (h == 0)
    {
        h = 1;
    }
    
    // 将视口设置为窗口大小
    glViewport(0, 0, w, h);
    
    fAspect = (GLfloat)w/(GLfloat)h;
    
    // 生成透视投影
    viewFrustum.SetPerspective(80.f, fAspect, 1.0, 120.0);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
}

// 进行调用以绘制场景
void RenderScene(void)
{
    // 用当前清除色清除窗口
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 获取矩阵堆栈栈顶模型视图矩阵
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Translate(0.0f, 0.0f, viewZ);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,transformPipeline.GetModelViewProjectionMatrix(),0);
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
    floorBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILING]);
    ceilingBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    leftWallBatch.Draw();
    rightWallBatch.Draw();
    
    // 将模型视图矩阵存储至矩阵堆栈
    modelViewMatrix.PopMatrix();
    
    // 缓冲区交换
    glutSwapBuffers();
    
}

int main(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    // 标准初始化
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tunnel");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    // 添加菜单入口改变过滤器
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("GL_NEAREST", 0);
    glutAddMenuEntry("GL_LINEAR", 1);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", 2);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 3);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 4);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "GLEW Error: %s\n",glewGetErrorString(err));
        return 1;
    }
    
    // 启动，循环，关闭
    SetupRC();
    
    glutMainLoop();
    
    ShutdownRC();
    
    return 0;
    
}

