#pragma once

#include "GameObject.h"
#include "LightComponent.h"
#include "LightUBO.h"
#include "PBRMaterial.h"

class Scene
{
public:
    Scene() = default;
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

    GameObject* CreateCube(const std::string& name = "Cube", const glm::vec3& color = glm::vec3(0.5f, 0.5f, 0.5f))
    {
        std::shared_ptr<Mesh> cubeMesh = Mesh::CreateCube();
        std::shared_ptr<PBRMaterial> cubeMaterial = std::make_shared<PBRMaterial>(color, 0.5f, 0.1f);
        
        return Create3DObject(name, cubeMesh, cubeMaterial);
    }

    GameObject* CreatePlane(const std::string& name = "Plane", float size = 5.0f, const glm::vec3& color = glm::vec3(0.5f, 0.5f, 0.5f))
    {
        std::shared_ptr<Mesh> planeMesh = Mesh::CreatePlane(size);
        std::shared_ptr<PBRMaterial> planeMaterial = std::make_shared<PBRMaterial>(color, 0.5f, 0.1f);
        
        return Create3DObject(name, planeMesh, planeMaterial);
    }

    GameObject* CreateSphere(const std::string& name = "Sphere", int segments = 24, const glm::vec3& color = glm::vec3(0.5f, 0.5f, 0.5f))
    {
        std::shared_ptr<Mesh> sphereMesh = Mesh::CreateSphere(segments);
        std::shared_ptr<PBRMaterial> sphereMaterial = std::make_shared<PBRMaterial>(color, 0.5f, 0.1f);

        return Create3DObject(name, sphereMesh, sphereMaterial);
    }

    GameObject* CreateSphere(const std::string& name = "Sphere", int segments = 24, const glm::vec3& kd = glm::vec3(0.0f, 0.0f, 0.0f),
                const glm::vec3& ks = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& ka = glm::vec3(0.0f, 0.0f, 0.0f), float pow = 100)
    {
        std::shared_ptr<Mesh> sphereMesh = Mesh::CreateSphere(segments);
        std::shared_ptr<BlinnPhongMaterial> sphereMaterial = std::make_shared<BlinnPhongMaterial>(kd, ks, ka, pow);

        return Create3DObject(name, sphereMesh, sphereMaterial);
    }

    GameObject* Create3DObject(const std::string& name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
    {
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
        lightUBO.Update(m_Lights[0]->GetLightData());
    }

private:
    //游戏物体集合
    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
    
    //MeshRenderer集合,用于在场景中统一管理所有MeshRenderer组件,方便渲染
    std::vector<MeshRenderer*> m_MeshRenderers;

    //
    std::vector<LightComponent*> m_Lights;
};