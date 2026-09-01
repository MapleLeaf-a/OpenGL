#pragma once 

#include "Component.h"

class CameraComponent : public Component
{
public:
    CameraComponent() = default;

    //透视投影参数设置
    void SetPerspective(float fovDegrees, float aspectRatio, float nearPlane, float farPlane) {
        m_Fov = fovDegrees;
        m_AspectRatio = aspectRatio;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        m_IsPerspective = true;
        m_ProjDirty = true;
    }
    //正交投影参数设置
    void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
        m_Left = left;
        m_Right = right;
        m_Bottom = bottom;
        m_Top = top;
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
        m_IsPerspective = false;
        m_ProjDirty = true;
    }

    glm::mat4 GetViewMatrix() const 
    {
        Transform* transform = GetTransform();
        if (!transform) return glm::mat4(1.0f); //如果没有Transform组件,返回单位矩阵
        
        glm::vec3 pos = transform->GetPosition();
        glm::vec3 forward = transform->GetWorldForward();
        glm::vec3 up = transform->GetWorldUp();

        return glm::lookAt(pos, pos + forward, up);
    }

    glm::mat4 GetProjectionMatrix()
    {
        if (m_ProjDirty)
        {
            UpdateProjectionMatrix();
            m_ProjDirty = false;
        }
        return m_ProjectionMatrix;
    }

    glm::mat4 GetViewProjectionMatrix()
    {
        return GetProjectionMatrix() * GetViewMatrix();
    }

private:
    void UpdateProjectionMatrix()
    {
        if (m_IsPerspective)
        {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearPlane, m_FarPlane);
        }
        else
        {
            m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
        }
    }

    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    //透视投影参数
    float m_Fov = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    
    //正交投影参数
    float m_Left = -10.0f; float m_Right = 10.0f;
    float m_Bottom = -10.0f; float m_Top = 10.0f;
    
    float m_NearPlane = 0.1f;
    float m_FarPlane = 1000.0f;

    //是否是透视投影
    bool m_IsPerspective = true;

    bool m_ProjDirty = true;    
};