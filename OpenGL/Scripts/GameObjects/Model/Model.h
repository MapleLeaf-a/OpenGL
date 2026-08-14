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
    std::string name;
    std::shared_ptr<Mesh> mesh;    
    std::shared_ptr<Material> material;
};


class Model
{
public:
    Model() = default;

    //加载模型
    bool Load(const std::string& path);

    //递归处理节点
    void ProcessNode(aiNode* node, const aiScene* scene);

    //
    ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    //获取所有子网格
    const std::vector<ModelMesh>& GetMeshes() const { return m_Meshes; }
    
    //获取模型名称
    const std::string& GetName() const { return m_Name; }

private:
    std::string m_Name;
    std::vector<ModelMesh> m_Meshes;
};