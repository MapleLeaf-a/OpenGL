#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

bool Model::Load(const std::string &path)
{
    Assimp::Importer importer;

    //设置导入选项
    unsigned int flags = aiProcess_Triangulate | // 将多边形三角化
        aiProcess_GenNormals |           // 生成法线（如果没有）
        aiProcess_CalcTangentSpace |     // 计算切线空间（法线贴图用）
        aiProcess_JoinIdenticalVertices | // 合并重复顶点
        aiProcess_SortByPType |          // 按图元类型排序
        aiProcess_ImproveCacheLocality;  // 改善缓存局部性

    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE  //检查这个模型文件是否残缺,如果mFlags里包含AI_SCENE_FLAGS_INCOMPLETE这个标记,说明文件损坏或读取出错
        || !scene->mRootNode) //模型里的物体是有层级结构的,mRootNode就是这棵层级树的树根,由它能找到模型里所有的子节点
    {
        std::cerr << "Assimp error : " << importer.GetErrorString() << std::endl;
        return false;
    }

    size_t lastSlash = path.find_last_of("/\\"); //从后往前找第一个/或\(两个\第一个是转义)
    size_t lastDot = path.find_last_of(".");
    if (lastSlash != std::string::npos)
        m_Name = path.substr(lastSlash + 1, lastDot - lastSlash - 1); //截取模型文件的名字
    else 
        m_Name = path.substr(0, lastDot);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) //node->mNumMeshes:这个节点下直接挂载的网格数量
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        /*scene->mMeshes	aiMesh**	所有网格的数组（零件盒）	用 node->mMeshes[i] 做索引取数据
        node->mMeshes	unsigned int*	网格索引数组（指向 scene->mMeshes 的编号）	scene->mMeshes[node->mMeshes[i]]取出网格数据*/
        ModelMesh modelMesh = ProcessMesh(mesh, scene);
        m_Meshes.push_back(modelMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
        /*node->mChildren	aiNode**	子节点数组	ProcessNode(node->mChildren[i], scene)递归处理子节点*/
    }
}

ModelMesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    return ModelMesh();
}
