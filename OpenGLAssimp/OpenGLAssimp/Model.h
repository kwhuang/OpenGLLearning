//
//  Model.h
//  OpenGLAssimp
//
//  Created by mac on 2020/4/20.
//  Copyright © 2020 kven. All rights reserved.
//

#include "glad.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Mesh.cpp"
#include "Shader/Shader.cpp"

#include "scene.h"


class Model
{
public:
    // 存储到目前为止已加载的所有纹理，进行优化以确保纹理加载不超过一次。
    vector<Texture> textures_loaded;
    // 网格
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    // 构造方法
    Model(string const &path, bool gamma);

    // 绘制所有网格
    void Draw(Shader shader);
    
private:
    // 通过本地路径加载model至assimp的aiScene对象
    void loadModel(string const &path);

    // 递归处理所有子节点
    void processNode(aiNode *node, const aiScene *scene);
    
    // 解析网格
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    // 检查给定类型的所有材质纹理，如果尚未加载，则加载纹理。
    // 所需的信息作为Texture结构返回。
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

