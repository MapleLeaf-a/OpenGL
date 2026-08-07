#pragma once

#include "Component.h"

enum class LightType
{
    Directional,
    Point,
    Spot
};

class LightComponent : public Component
{
public:
    glm::vec3 GetColor() const { return m_Color; }
    float GetIntensity() const { return m_Intensity; }

    glm::vec3 GetPosition() const
    {
        Transform* trans = GetTransform();
        return trans ? trans->GetPosition() : glm::vec3(0.0f);
    }

    glm::vec3 GetDirection() const
    {
        Transform* trans = GetTransform();
        return trans ? trans->GetForward() : glm::vec3(0.0f); //获取Forward作为方向光的方向
    }

private:
    LightType m_LightType = LightType::Directional;

    glm::vec3 m_Color = glm::vec3(1.0f);

    float m_Intensity = 1.0f;
};