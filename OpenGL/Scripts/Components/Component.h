#pragma once

class GameObject; //前向声明

class Component{
public:
    virtual ~Component() = default; //等价于直接实现空函数体,但是不会影响编译器自动生成移动构造/赋值等其他特殊函数

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRenderer() {}

    GameObject* GetOwner() const { return m_Owner; }
private:
    GameObject* m_Owner = nullptr; //裸指针,只"观察",不拥有
};
