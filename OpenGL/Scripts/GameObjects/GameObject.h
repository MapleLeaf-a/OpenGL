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

    void SetParent(GameObject* parent) 
    {
        if (m_Parent == parent) return;

        if (m_Parent != nullptr)
        { //如果已经有父物体，从旧父物体中移除自己
            m_Parent->RemoveChild(this);
        }

        m_Parent = parent; //设置新父物体

        if (parent)
        {    
            parent->AddChild(this); //把自己加入新父物体的子列表
        }
    }
    GameObject* GetParent() const { return m_Parent; }
    void AddChild(GameObject* child)
    {
        if (!child || child == this) return;

        //如果child已经有父对象,先从原父对象中移除
        if (child->m_Parent) 
        {
            child->m_Parent->RemoveChild(child);
        }

        m_Children.push_back(child);
        child->m_Parent = this; //访问控制是基于类的,不是基于对象的,所以这样是合法的
    }
    void RemoveChild(GameObject* child) 
    {
        auto it = std::find(m_Children.begin(), m_Children.end(), child);
        if (it != m_Children.end())
        {
            m_Children.erase(it);
            child->m_Parent = nullptr;
        }
    }
    const std::vector<GameObject*>& GetChildren() const { return m_Children; }


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

    const std::string& GetName() const { return m_Name; }

private:
    GameObject* m_Parent = nullptr; //父物体指针
    Transform m_Transform; //每个GO都应该有一个transform
    std::string m_Name;
    std::vector<GameObject*> m_Children; //子物体指针
    std::vector<std::unique_ptr<Component>> m_Components; //组件和GO同生命周期
};
