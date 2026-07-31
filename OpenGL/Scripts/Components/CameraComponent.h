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


private:
    //是否是透视投影
    bool m_IsPerspective = true;

    //透视投影参数
    float m_Fov = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    
    //正交投影参数
    float m_Left = -10.0f; float m_Right = 10.0f;
    float m_Bottom = -10.0f; float m_Top = 10.0f;
    
    float m_NearPlane = 0.1f;
    float m_FarPlane = 100.0f;

    bool m_ProjDirty = true;    
};