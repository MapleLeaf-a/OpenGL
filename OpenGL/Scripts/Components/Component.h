#pragma once

#include "Transform.h"

class GameObject; //前向声明

class Component{
public:
    virtual ~Component() = default; //等价于直接实现空函数体,但是不会影响编译器自动生成移动构造/赋值等其他特殊函数
    
    virtual void OnAttach(GameObject* owner)
    {
        m_Owner = owner;
    }
    
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}

    GameObject* GetOwner() const { return m_Owner; }

    Transform* GetTransform() const;

private:
    GameObject* m_Owner = nullptr; //裸指针,只"观察",不拥有
};
