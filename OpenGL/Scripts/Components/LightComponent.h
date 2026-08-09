#pragma once

#include "Component.h"
#include "LightData.h"

enum class LightType : uint8_t //指定只占1字节
{
    Directional,
    Point,
    Spot
};

class LightComponent : public Component
{
public:
    LightComponent(LightType type, glm::vec4 color, float intensity) 
        : lightData{glm::vec4(GetPosition(), 0), glm::vec4(GetDirection(), 0), color, intensity, (int)type} {}

    glm::vec4 GetColor() const { return lightData.color; }
    float GetIntensity() const { return lightData.intensity; }

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

    const LightData& GetLightData() const { return lightData; }

private:
    LightData lightData;
};