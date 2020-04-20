//
//  Mesh.cpp
//  OpenGLAssimp
//
//  Created by mac on 2020/4/20.
//  Copyright © 2020 kven. All rights reserved.
//

#ifndef MESH_H
#define MESH_H

#include "glad.h"
#include "glm.hpp"
#include "matrix_transform.hpp"

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
    // 位置
    glm::vec3 Position;
    // 法向量
    glm::vec3 Normal;
    // 纹理坐标
    glm::vec2 TexCoords;
    // 切线
    glm::vec3 Tangent;
    // 双切线
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // 网格数据
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    // 构造方法
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // 设置顶点缓冲区及其属性指针
        setupMesh();
    }

    // 绘制网格
//    void Draw(Shader shader)
//    {
//        // 绑定纹理
//        unsigned int diffuseNr  = 1;
//        unsigned int specularNr = 1;
//        unsigned int normalNr   = 1;
//        unsigned int heightNr   = 1;
//        for(unsigned int i = 0; i < textures.size(); i++)
//        {
//            // 绑定前激活适当的纹理单元
//            glActiveTexture(GL_TEXTURE0 + i);
//            // 检索纹理编号（0～15）
//            string number;
//            string name = textures[i].type;
//            if(name == "texture_diffuse")
//                number = std::to_string(diffuseNr++);
//            else if(name == "texture_specular")
//                number = std::to_string(specularNr++); // transfer unsigned int to stream
//            else if(name == "texture_normal")
//                number = std::to_string(normalNr++); // transfer unsigned int to stream
//             else if(name == "texture_height")
//                number = std::to_string(heightNr++); // transfer unsigned int to stream
//
//            // 设置纹理
//            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
//            // 绑定纹理id
//            glBindTexture(GL_TEXTURE_2D, textures[i].id);
//        }
//
//        // draw mesh
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//        glBindVertexArray(0);
//
//        // 恢复默认纹理
//        glActiveTexture(GL_TEXTURE0);
//    }

private:
    unsigned int VBO, EBO;

    // 初始化缓冲
    void setupMesh()
    {
        // 创建定点数组对象、顶点缓冲对象、顶点索引对象
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // 绑定当前VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 设置顶点属性
        // 位置
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 法线
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // 纹理
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // 切线
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // 双切线
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};


#endif
