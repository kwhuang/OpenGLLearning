//
//  Model.cpp
//  OpenGLAssimp
//
//  Created by mac on 2020/4/20.
//  Copyright © 2020 kven. All rights reserved.
//

#ifndef MODEL_H
#define MODEL_H

#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Importer.hpp"
#include "postprocess.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

// 构造方法
Model:: Model(string const &path, bool gamma = false)
{
    loadModel(path);
}

// 绘制所有网格
void Model:: Draw(Shader shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}
    
// 通过本地路径加载model至assimp的aiScene对象
void Model:: loadModel(string const &path)
{
    // 通过assimp读取文件
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // 通过secene的flag判断是否成功
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // 检索文件路径的目录路径
    directory = path.substr(0, path.find_last_of('/'));

    // 处理scene第一个根节点
    processNode(scene->mRootNode, scene);
}

// 递归处理所有子节点
void Model:: processNode(aiNode *node, const aiScene *scene)
{
    // 处理当前节点的网格
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // 通过子节点网格索引获取scene中的网格
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //
        meshes.push_back(processMesh(mesh, scene));
    }
    // 处理当前节点的子节点直到没有子节点位置
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model:: processMesh(aiMesh *mesh, const aiScene *scene)
{
    // 处理顶点、面、纹理坐标
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // 顶点
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // 因为assimp使用了自己的向量类，该类不会直接转换为glm的vec3类，因此我们首先将数据传输到该占位符glm :: vec3。
        glm::vec3 vector;
        // 位置
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // 法线
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // 纹理坐标
        // 判断网格是否包含纹理坐标
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            // 一个顶点最多可以包含8个不同的纹理坐标。 因此，我们假设我们不会
            // 使用顶点可以具有多个纹理坐标的模型，因此我们始终采用第一组（0）。
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        // 切线
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
        // 双切线
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
        vertices.push_back(vertex);
    }
    // 现在遍历每个网格面（一个面是其三角形的网格）并检索相应的顶点索引。
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // 检索面部的所有索引并将其存储在索引向量中
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    // 材质
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // 我们假定着色器中采样器名称的约定。 每个漫反射纹理都应命名
    // 作为'texture_diffuseN'，其中N是从1到MAX_SAMPLER_NUMBER的序列号。
    // 镜面反射：texture_specularN
    // 正常：texture_normalN

    // 1. 漫反射贴图
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. 镜面贴图
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. 法线贴图
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. 高度贴图
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    
    // 通过数据生成并返回网格对象
    return Mesh(vertices, indices, textures);
}

// 检查给定类型的所有材质纹理，如果尚未加载，则加载纹理。
// 所需的信息作为Texture结构返回。
vector<Texture> Model:: loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // 检查纹理是否以加载
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            // 将其存储为整个模型加载的纹理，以确保我们不会不必要地加载重复的纹理。
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


#endif
