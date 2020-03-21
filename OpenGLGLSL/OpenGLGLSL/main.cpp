//
//  main.cpp
//  OpenGLGLSL
//
//  Created by huangkaiwei on 2020/3/14.
//  Copyright © 2020 hq_kven. All rights reserved.
//

#include <stdio.h>


int main(int argc,char *argv[])
{
    
}

// GLSL

/// 变量和数据类型 bool int uint float

// 向量类型
// vec2,vec3.vec4：2分量、3分量和4分量浮点向量
// ivec2,ivec3,ivec4：2分量、3分量和4分量整数向量
// uvec2,uvec3,uvec4：2分量、3分量和4分量无符号整数向量
// bvec2,bvec3,bvec4：2分量、3分量和4分量布尔向量

// 矩阵类型(只支持浮点数，不支持整数g或布尔值矩阵)
// mat2,mat2x2：两行两列
// mat3,mat3x3：三行三列
// mat2x3：三行两列
// mat3x4：四行三列
// mat4x3：三行四列


/// 存储限定符
// <none> 只是普通的本地变量，外部不可见，外部不可访问
// const 一个编译时的常量，或者说是一个对函数来说为只读参数
// in 一个从以前的阶段d传递过来的变量
// in centroid 一个从以前的阶段d传递过来的变量，使用质心插值
// out 传递到下一个处理阶段或者在一个函数中指定一个返回值
// out centroid 传递到下一个处理阶段，使用质心插值
// inout 一个读/写变量，只能用于局部函数参数
// uniform 一个从客户端代码传递过来的变量，在顶点之间不做高变


/// 着色器
/*
 结构
 
 首行指定版本，最低版3.3
 #version 330
 
 // 精度
 precision highp float;
 
 属性声明
 in vec4 vVertex; // 4分量顶点位置，标记为in的变量是只读的
 in vec4 vColor; // 4分量顶点颜色值
 
 声明输出
 out vec4 vVaryingColor; // 这个变量将h成为要传递到片段着色器的顶点的颜色值，
 这个变量x必须为片段着色器声明为一个in变量，否则在我们视图将着色器编译和连接到一起时，
 就会得到一个连接错误
 
 顶点动作
 // 在批次中将为每个顶点执行一次
 void main(void)
 {
    vVaryingColor = vColor;
    // gl_Position是一个预定义的内建4分向量，它包含顶点着色器要求的一个输出，输入gl_Position的值
    是几何图形阶段用来装配图元的
    gl_Position = vVertex;
 }
 
 */


// 编译、绑定和连接

