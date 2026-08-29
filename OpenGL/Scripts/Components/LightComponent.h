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

    LightType GetLightType() const { return (LightType)lightData.type; }

    //获取Model矩阵
    const glm::mat4& GetLightModelMatrix()
    {
        Transform* trans = GetTransform();
        return trans->GetModelMatrix();
    }

    //获取View矩阵
    glm::mat4 GetLightViewMatrix() //不返回引用是因为要
    {
        Transform* trans = GetTransform();
        
        glm::vec3 pos = trans->GetPosition();
        glm::vec3 forward = trans->GetForward();
        glm::vec3 up = trans->GetUp();

        return glm::lookAt(pos, pos + forward, up);
    }

    //获取Proj矩阵
    glm::mat4 GetLightProjMatrix(float nearPlane = 0.1f, float farPlane = 100.0f)
    {
        //方向光用正交投影
        float size = 20.0f;
        return glm::ortho(-size, size, -size, size, nearPlane, farPlane);
    }

    glm::mat4 GetLightVPMatrix()
    {
        return GetLightProjMatrix() * GetLightViewMatrix();
    }

private:
    LightData lightData;
};