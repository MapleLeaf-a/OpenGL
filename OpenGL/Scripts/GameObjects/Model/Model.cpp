#include "Model.h"
#include "BlinnPhongMaterial.h"

#include <iostream>

class Texture; //前向声明,因为Material中include过Texture

bool Model::Load(const std::string &filePath)
{
    Assimp::Importer importer;

    //设置导入选项
    unsigned int flags = aiProcess_Triangulate | // 将多边形三角化
        aiProcess_GenNormals |           // 生成法线（如果没有）
        aiProcess_CalcTangentSpace |     // 计算切线空间（法线贴图用）
        aiProcess_JoinIdenticalVertices | // 合并重复顶点
        aiProcess_SortByPType |          // 按图元类型排序
        aiProcess_ImproveCacheLocality;  // 改善缓存局部性

    const aiScene* scene = importer.ReadFile(filePath, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE  //检查这个模型文件是否残缺,如果mFlags里包含AI_SCENE_FLAGS_INCOMPLETE这个标记,说明文件损坏或读取出错
        || !scene->mRootNode) //模型里的物体是有层级结构的,mRootNode就是这棵层级树的树根,由它能找到模型里所有的子节点
    {
        std::cerr << "Assimp error : " << importer.GetErrorString() << std::endl;
        return false;
    }

    size_t lastSlash = filePath.find_last_of("/\\"); //从后往前找第一个/或\(两个\第一个是转义)
    size_t lastDot = filePath.find_last_of(".");
    if (lastSlash != std::string::npos)
        m_Name = filePath.substr(lastSlash + 1, lastDot - lastSlash - 1); //截取模型文件的名字
    else 
        m_Name = filePath.substr(0, lastDot);

    m_Materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* aiMat = scene->mMaterials[i];
        m_Materials[i] = ProcessMaterial(aiMat, scene);
    }

    //递归处理节点
    ProcessNode(scene->mRootNode, scene);

    std::cout << "Loaded model: " << m_Name 
              << " (" << m_Meshes.size() << " meshes)" << std::endl;
    
    return true;
}

//处理节点
void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) //node->mNumMeshes:这个节点下直接挂载的网格数量
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        /*scene->mMeshes	aiMesh**	所有网格的数组（零件盒）	用 node->mMeshes[i] 做索引取数据
        node->mMeshes	unsigned int*	网格索引数组（指向 scene->mMeshes 的编号）	scene->mMeshes[node->mMeshes[i]]取出网格数据*/
        ModelMesh modelMesh = ProcessMesh(mesh, scene);

        //从材质池中取出对应的材质
        if (mesh->mMaterialIndex < m_Materials.size())
        {
            modelMesh.material = m_Materials[mesh->mMaterialIndex];
        }

        m_Meshes.push_back(modelMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
        /*node->mChildren	aiNode**	子节点数组	ProcessNode(node->mChildren[i], scene)递归处理子节点*/
    }
}

//处理网格
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

    unsigned int offset = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i]; //mFaces面数组（每个面包含顶点索引）
        for (unsigned int j = 0; j < face.mNumIndices; j++) //mNumIndices这个面包含的顶点数（三角形是 3）
        {
            indices[offset++] = face.mIndices[j];
        }
    }

    //创建Mesh
    result.mesh = std::make_shared<Mesh>(vertices, indices);


    return result;
}

//处理材质
std::shared_ptr<Material> Model::ProcessMaterial(aiMaterial* aiMat, const aiScene* scene)
{
    //Kd,漫反射
    aiColor3D diffuseColor(0.8f, 0.8f, 0.8f);
    aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

    //Ks,镜面反射
    aiColor3D specularColor(0.5f, 0.5f, 0.5f);
    aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

    //ksPow
    float pow = 150.0f;
    aiMat->Get(AI_MATKEY_SHININESS, pow);

    std::shared_ptr<BlinnPhongMaterial> material = std::make_shared<BlinnPhongMaterial>(glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b), glm::vec3(specularColor.r, specularColor.g, specularColor.b), (int)pow);   

    //尝试读取贴图
    aiString texturePath;
    if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) //获取漫反射贴图的第0套贴图(一般0是主贴图)
    {
        std::string path = texturePath.C_Str();

        std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);

        material->SetDiffuseTexture(texture);
    }

    return material;
}