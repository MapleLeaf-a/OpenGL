#pragma once

#include <string>
#include <vector>
#include <concepts>
#include <memory>

#include "Transform.h"
#include "Component.h"


class GameObject
{
public:
    GameObject(const std::string& name = "GameObject") : m_Name(name), m_Transform(), m_Components() {}

    Transform& GetTransform() { return m_Transform; }
    const Transform& GetTransform() const { return m_Transform; }

    //添加组件
    template<typename T, typename... Args>
    requires std::derived_from<T, Component>  //要求T必须派生自Component
    T* AddComponent(Args&&... args)
    {
        std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = component.get();

        component->OnAttach(this);

        m_Components.push_back(std::move(component));

        return rawPtr;
    }

    //获取第一个匹配类型的组件
    template<typename T>
    requires std::derived_from<T, Component> 
    T* GetComponent()
    {
        for (auto& comp : m_Components)
        {
            T* result = dynamic_cast<T*>(comp.get()); //向下转换Downcasting
            if (result)
            {
                return result;
            } 
        }
        return nullptr;
    }

    void Update(float deltaTime)
    {
        for (auto& comp : m_Components)
        {
            comp->OnUpdate(deltaTime);
        }
    }

    void Render()
    {
        for (auto& comp : m_Components)
        {
            comp->OnRender();
        }
    }

private:
    std::string m_Name;
    Transform m_Transform; //每个GO都应该有一个transform
    std::vector<std::unique_ptr<Component>> m_Components; //组件和GO同生命周期
};
