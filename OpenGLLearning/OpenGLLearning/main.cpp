//
//  main.cpp
//  OpenGLLearning
//
//  Created by huangkaiwei on 2020/3/14.
//  Copyright © 2020 hq_kven. All rights reserved.
//

#include <stdio.h>

#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>

// 着色器管理实例
GLShaderManager shaderManager;

// 批次类
GLBatch triangleBatch;

void changeSize(int w,int h)
{
    // 视口
    glViewport(0, 0, w, h);
}

void RenderScene(void)
{
    // 清除一个或一组特定的缓冲区
    // GL_COLOR_BUFFER_BIT：颜色缓冲区
    // GL_DEPTH_BUFFER_BIT：深度缓冲区
    // GL_STENCIL_BUFFER_BIT：模版缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    // 红色
    GLfloat vRed[] = {1.0,0.0,0.0,1.0f};

    
    // 传递到存储着色器（GLT_SHADER_IDENTITY：使用指定颜色以默认笛卡尔坐标系在屏幕上渲染几何图）
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    // 将几何图形提交到着色器
    triangleBatch.Draw();
    
    // 缓冲区交换
    glutSwapBuffers();
}

void setupRC()
{
    // glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
    // 设置用来进行窗口清楚的颜色
    // 每个参数都包含最终颜色所要求的这种颜色分量的权值
    // 不会立即清楚背景，而是设置在以后颜色缓冲区被清除时使用的颜色
    glClearColor(0.98f, 0.40f, 0.7f, 1);
    
    // 编译连接自己的着色器
    shaderManager.InitializeStockShaders();
    
    // 指定顶点
    GLfloat vVerts[] = {
        -0.5f,0.0f,0.0f,
        0.5f,0.0f,0.0f,
        0.0f,0.5f,0.0f
    };
    
    // 批次容齐初始化，设置图元(GL_TRIANGLES三角形图元)、顶点个数以及（可选）一组或两组纹理
    triangleBatch.Begin(GL_TRIANGLES, 3);
    
    // 复制一个由3分量（x，y，z）顶点组成的数组
    triangleBatch.CopyVertexData3f(vVerts);
    
    // 还可以选择复制表面法线、颜色和纹理坐标
    // CopyNormalDataf(GLfloat *vNorms) { CopyNormalDataf((M3DVector3f *)(vNorms)); }
    // CopyColorData4f(GLfloat *vColors) { CopyColorData4f((M3DVector4f *)(vColors)); }
    // CopyTexCoordData2f(GLfloat *vTex, GLuint uiTextureLayer) { CopyTexCoordData2f((M3DVector2f *)(vTex), uiTextureLayer); }
    
    // 声明已经完成了数据复制工作，并且将设置内部标记，以通知这个类包含哪些属性
    triangleBatch.End();
}

int main(int argc,char *argv[])
{
    // GLTolls设置当前工作目录
    gltSetWorkingDirectory(argv[0]);
    
    // 传输命令行参数并初始化GLUT库
    glutInit(&argc, argv);
    
    // 设置使用哪种类型的显示模式
    // GLUT_DOUBLE：双缓冲窗口，绘图是在离屏缓冲区执行，然后迅速转换成窗口试图
    // GLUT_RGBA：RGBA颜色模式
    // GLUT_DEPTH：位标志将一个深度缓冲区分配为显示的一部分
    // GLUT_STENCIL：确保我们也会有一个可用的模版缓冲区
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    // 告诉GLUT窗口大小
    glutInitWindowSize(800, 600);
    
    // 以Triangle为窗口标题
    glutCreateWindow("Triangle");
    
    // 窗口改变大小回调函数
    glutReshapeFunc(changeSize);
    
    // 注册函数以包含OpenGL渲染代码
    glutDisplayFunc(RenderScene);
    
    // 重新调用GLEW库初始化Openglc驱动程序中所有丢失的入口点，以确保OpenGL API对我们来说完全可用
    GLenum status = glewInit();
    
    // 判断驱动程序的初始化过程有无问题
    if (GLEW_OK != status) {
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return 1;

    }
    
    // 设置渲染环境
    setupRC();
    
    // 开始主消息循环：调用后主窗口被关闭前都不会返回，并且一个应用程序中只需调用一次，这个函数负责处理所有操作系统特定的消息、按键操作，直到我们关闭程序为止，t确保我们注册的这些函数回调被正确的调用
    glutMainLoop();
    return  0;
}

