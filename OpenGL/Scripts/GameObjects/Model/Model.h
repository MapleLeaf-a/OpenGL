#pragma once

#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Material.h"

struct ModelMesh
{
    std::string name; //每个ModelMesh都有其名字,因为建模师不会只做一个物体,一个模型文件里通常包含多个物体,建模师需要给它们起名字来区分
    std::shared_ptr<Mesh> mesh;    
    std::shared_ptr<Material> material;
};


class Model
{
public:
    Model() = default;

    //加载模型
    bool Load(const std::string& filePath);

    //获取所有子网格
    const std::vector<ModelMesh>& GetMeshes() const { return m_Meshes; }
    
    //获取模型名称
    const std::string& GetName() const { return m_Name; }

private:
    //递归处理节点
    void ProcessNode(aiNode* node, const aiScene* scene);

    //
    ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    //
    std::shared_ptr<Material> ProcessMaterial(aiMaterial* aiMat, const aiScene* scene);

    std::string m_Name;
    //存储模型Mesh的数组
    std::vector<ModelMesh> m_Meshes;

    //存储该模型加载出来的所有材质
    std::vector<std::shared_ptr<Material>> m_Materials;
};