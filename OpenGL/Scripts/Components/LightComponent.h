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
        : lightData{glm::vec4(GetPosition(), 0.0f), glm::vec4(GetDirection(), 0.0f), color, intensity, (int)type} {}

    glm::vec4 GetColor() const { return lightData.color; }
    float GetIntensity() const { return lightData.intensity; }

    glm::vec3 GetPosition() const
    {
        Transform* trans = GetTransform();
        return trans ? trans->GetWorldPosition() : glm::vec3(0.0f);  //修改这里为获取worldPosition,以让点光源的Position能继承父物体的变换
    }

    glm::vec3 GetDirection() const
    {
        Transform* trans = GetTransform();
        return trans ? trans->GetWorldForward() : glm::vec3(0.0f); //获取Forward作为方向光的方向
    }

    const LightData& GetLightData()
    {
        Transform* trans = GetTransform();
        if (trans->GetDirty())
        {
            lightData.direction = glm::vec4(GetDirection(), 0.0f);
            lightData.position = glm::vec4(GetPosition(), 0.0f);
        }
        return lightData;
    }

private:
    LightData lightData;
};