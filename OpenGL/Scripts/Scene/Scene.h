#pragma once

#include "GameObject.h"
#include "LightComponent.h"
#include "LightUBO.h"
#include "PBRMaterial.h"
#include "Model.h"

class Scene
{
public:
    Scene()
    {
        m_GizmoQuadMesh = Mesh::CreatePlane_XY(1.0f);
    }
    ~Scene() = default;
    
    //禁止拷贝
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    //创建游戏物体
    GameObject* CreateGameObject(const std::string& name = "GameObject")
    {
        std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>(name);
        GameObject* rawPtr = gameObject.get();
        m_GameObjects.push_back(std::move(gameObject));
        return rawPtr;
    }

    //通过指针销毁游戏物体
    void DestroyGameObject(GameObject* gameObject)
    {
        auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
            [gameObject](const std::unique_ptr<GameObject>& obj) { return obj.get() == gameObject; });

        if (it != m_GameObjects.end())
        {
            m_GameObjects.erase(it);
        }
    }

    //通过名称寻找游戏物体(返回第一个匹配的)
    GameObject* FindGameObjectByName(const std::string& name)
    {
        for (const auto& obj : m_GameObjects)
        {
            if (obj->GetName() == name)
            {
                return obj.get();
            }
        }
        return nullptr;
    }

    //每帧更新所有物体
    void OnUpdate(float deltaTime)
    {
        for (auto& obj : m_GameObjects)
        {
            obj->Update(deltaTime);
        }
    }

    //每帧渲染所有物体
    void OnRender()
    {
        for (auto& obj : m_GameObjects)
        {
            obj->Render();
        }
    }

    //清空场景中的所有游戏物体
    void Clear()
    {
        m_GameObjects.clear();
    }

    //获取所有游戏物体
    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_GameObjects; }    
    //获取所有MeshRenderer
    const std::vector<MeshRenderer*>& GetMeshRenderers() const { return m_MeshRenderers; }

    GameObject* CreateCamera(const std::string& name = "Camera")
    {
        std::unique_ptr<GameObject> camera = std::make_unique<GameObject>(name);
        CameraComponent* cameraComp = camera->AddComponent<CameraComponent>();
        cameraComp->SetPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        GameObject* rawPtr = camera.get(); //必须在move前获取原始指针,否则原地址被释放会变成垃圾地址
        m_GameObjects.push_back(std::move(camera));
        return rawPtr;
    }

    template<typename MaterialType, typename... Args>
    GameObject* CreateCube(const std::string& name = "Cube", Args&&... args)
    {
        std::shared_ptr<Mesh> cubeMesh = Mesh::CreateCube();
        
        return Create3DObject<MaterialType>(name, cubeMesh, std::forward<Args>(args)...);
    }

    template<typename MaterialType, typename... Args>
    GameObject* CreatePlane(const std::string& name = "Plane", float size = 5.0f, Args&&... args)
    {
        std::shared_ptr<Mesh> planeMesh = Mesh::CreatePlane_XZ(size);
       
        return Create3DObject<MaterialType>(name, planeMesh, std::forward<Args>(args)...);
    }

    template<typename MaterialType, typename... Args>
    GameObject* CreateSphere(const std::string& name = "Sphere", int segments = 24, Args&&... args)
    {
        std::shared_ptr<Mesh> sphereMesh = Mesh::CreateSphere(segments);

        return Create3DObject<MaterialType>(name, sphereMesh, std::forward<Args>(args)...);
    }

    template<typename MaterialType, typename... Args> //变参模板以满足不同材质对不同参数的要求
    GameObject* Create3DObject(const std::string& name, std::shared_ptr<Mesh> mesh, Args&&... args)
    {
        auto material = std::make_shared<MaterialType>(std::forward<Args>(args)...);

        std::unique_ptr<GameObject> go = std::make_unique<GameObject>(name);
        MeshRenderer* meshRenderer = go->AddComponent<MeshRenderer>(mesh, material);
        m_MeshRenderers.push_back(meshRenderer);
        GameObject* rawPtr = go.get();
        m_GameObjects.push_back(std::move(go));
        return rawPtr;
    }

    void RenderAllMeshRenderers(Shader& shader, const Renderer& renderer) const
    {
        for (const MeshRenderer* meshRenderer: m_MeshRenderers)
        {
            meshRenderer->Render(shader, renderer);
        }
    }

    GameObject* CreateDirectionalLight(const std::string& name = "DirectionalLight", glm::vec4 color = glm::vec4(1.0f), float intensity = 1.0f)
    {
        return CreateLight(name, LightType::Directional, color, intensity);
    }

    GameObject* CreatePointLight(const std::string& name = "PointLight", glm::vec4 color = glm::vec4(1.0f), float intensity = 1.0f)
    {
        return CreateLight(name, LightType::Point, color, intensity);
    }

    GameObject* CreateLight(const std::string& name = "Light", LightType type = LightType::Directional, glm::vec4 color = glm::vec4(1.0f), float intensity = 1.0f)
    {
        std::unique_ptr<GameObject> light = std::make_unique<GameObject>(name);
        LightComponent* lightComponent = light->AddComponent<LightComponent>(type, color, intensity);
        GameObject* rawPtr = light.get();
        m_GameObjects.push_back(std::move(light));
        m_Lights.push_back(lightComponent);
        return rawPtr;
    }

    void RenderAllLights(LightUBO& lightUBO)
    {
        if (m_Lights.empty()) return;

        std::vector<LightData> lights;

        for (auto& i : m_Lights)
        {
            lights.push_back(i->GetLightData());
        }

        lightUBO.Update(lights);
    }

    const std::vector<LightComponent*>& GetLights() { return m_Lights; }

    //添加一个渲染Pass以给灯光处加一个标记
    void RenderLightGizmos(CameraComponent* camera, Shader& gizmoShader, const Renderer& renderer, const Texture* pointLightIconTexture = nullptr, const Texture* dirLightIconTexture = nullptr)
    {
        if (m_Lights.empty()) return;

        if (pointLightIconTexture == nullptr && dirLightIconTexture == nullptr) return;

        //设置绑定shader
        gizmoShader.Bind();
        gizmoShader.SetUniformMat4f("u_View", camera->GetViewMatrix());
        gizmoShader.SetUniformMat4f("u_Projection", camera->GetProjectionMatrix());

        //设置渲染状态
        glEnable(GL_BLEND); //开启透明度混合
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //设置混合方式
        glDisable(GL_DEPTH_TEST); //不写入深度测试

        for (int i = 0; i < m_Lights.size(); i++)
        {
            const LightComponent* light = m_Lights[i];

            LightType type = light->GetLightType();
            switch (type)
            {
                case LightType::Directional:
                    if (dirLightIconTexture != nullptr)
                    {
                        dirLightIconTexture->Bind(1); //和主渲染的贴图插槽区分开
                        gizmoShader.SetUniform1i("u_IconTexture", 1);
                    }
                    break;
                case LightType::Point:
                    if (pointLightIconTexture != nullptr)
                    {
                        pointLightIconTexture->Bind(1); 
                        gizmoShader.SetUniform1i("u_IconTexture", 1);
                    }
                    break;

                default:
                    break;
            }

            //获取光源位置
            glm::vec3 lightPos = light->GetTransform()->GetWorldPosition();

            //Billboarding(公告牌技术)矩阵计算
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, lightPos);

            //让四边形始终朝向相机的-forward方向
            glm::vec3 lookDir = glm::normalize(-camera->GetTransform()->GetForward());
            glm::vec3 right = glm::normalize(glm::cross(lookDir, glm::vec3(0, 1, 0))); //通过叉乘lookDir和世界坐标的Up(0,1,0)得到right
            glm::vec3 up = glm::normalize(glm::cross(right, lookDir));

            glm::mat4 rotMatrix = glm::mat4(1.0f);
            rotMatrix[0] = glm::vec4(right, 0.0f);
            rotMatrix[1] = glm::vec4(up, 0.0f);
            rotMatrix[2] = glm::vec4(-lookDir, 0.0f);

            model = model * rotMatrix;
            model = glm::scale(model, glm::vec3(0.5f)); // 图标大小  

            gizmoShader.SetUniformMat4f("u_Model", model);

            m_GizmoQuadMesh->Draw(gizmoShader, renderer);
        }

        //恢复状态
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }


    //加载模型到场景
    GameObject* LoadModel(const std::string& filePath, const std::string& name = "")
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();

        if (!model->Load(filePath))
        {
            std::cerr << "Failed to load model : " << filePath << std::endl;
            return nullptr;
        }

        //创建父物体
        std::string modelName = name.empty() ? model->GetName() : name;
        GameObject* parent = CreateGameObject(modelName);

        //为每个子网格创建GO
        for (const ModelMesh& modelMesh : model->GetMeshes())
        {
            GameObject* child = CreateGameObject(modelMesh.name);
            child->GetTransform().SetPosition(glm::vec3(0.0f));
            
            child->SetParent(parent);

            MeshRenderer* renderer = child->AddComponent<MeshRenderer>(modelMesh.mesh, modelMesh.material);
            m_MeshRenderers.push_back(renderer);
        }
        
        return parent;
    }

private:
    //游戏物体集合
    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
    
    //MeshRenderer集合,用于在场景中统一管理所有MeshRenderer组件,方便渲染
    std::vector<MeshRenderer*> m_MeshRenderers;

    //光源组件的集合
    std::vector<LightComponent*> m_Lights;

    //用于承载gizmo图标的四边形Mesh
    std::shared_ptr<Mesh> m_GizmoQuadMesh;
};