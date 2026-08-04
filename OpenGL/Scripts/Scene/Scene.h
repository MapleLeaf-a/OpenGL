#pragma once

#include "GameObject.h"

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

private:
    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
};