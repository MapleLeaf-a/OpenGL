#include "Model.h"
#include "BlinnPhongMaterial.h"

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
{ //mesh 是真正的几何数据，包含顶点、索引、法线、UV 等
    ModelMesh result{};
    result.name = mesh->mName.C_Str(); /*mesh->mName	aiString	网格的名字（如 "Cube.001"）	mesh->mName.C_Str() 转成 C 字符串*/

    //顶点数据
    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices); //resize是扩容且初始化(后续不用再push_back),reserve是只分配内存(后续添加要push_back)

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex {};

        //位置
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        
        //法线
        if (mesh->HasNormals())
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        
        //uv
        if (mesh->HasTextureCoords(0)) //是否有第0套uv
            vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        
        vertices[i] = vertex;
    }

    //索引数据
    std::vector<unsigned int> indices;
    indices.resize(mesh->mNumFaces * 3); //mesh->mNumFaces三角形面的数量

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i]; //mFaces面数组（每个面包含顶点索引）
        for (unsigned int j = 0; j < face.mNumIndices; j++) //mNumIndices这个面包含的顶点数（三角形是 3）
        {
            indices[i] = face.mIndices[j];
        }
    }

    //创建Mesh
    result.mesh = std::make_shared<Mesh>(vertices, indices);

    //提取材质
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        /*scene->mMaterials	aiMaterial**	所有材质的数组（色卡）	用 mesh->mMaterialIndex 做索引取材质*/ 
        
        //Kd,漫反射
        aiColor3D diffuseColor();
        aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

        //Ks,镜面反射
        aiColor3D specularColor();
        aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

        //ksPow
        float pow = 150.0f;
        aiMat->Get(AI_MATKEY_SHININESS, pow);

        auto material = std::make_shared<BlinnPhongMaterial>(diffuseColor, specularColor, pow);
        
        result.material = material;
    }
    else
    {
        //默认材质
        auto material = std::make_shared<BlinnPhongMaterial>(
            glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            150
        );
        result.material = material;
    }

    return result;
}
