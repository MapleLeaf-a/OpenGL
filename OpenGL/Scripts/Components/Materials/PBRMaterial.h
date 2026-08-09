#pragma once

#include "Material.h"

class PBRMaterial : public Material
{
public:
    PBRMaterial() = default;
    PBRMaterial(glm::vec3 albedo, float roughness, float metallic)
        : m_Albedo(albedo), m_Roughness(roughness), m_Metallic(metallic) {}

    void SetAlbedo(const glm::vec3& color) { m_Albedo = color; }
    void SetRoughness(float roughness) { m_Roughness = roughness; }
    void SetMetallic(float metallic) { m_Metallic = metallic; }

    //将材质属性应用到着色器中
    void Apply(Shader& shader) const override
    {
        shader.SetUniform4f("u_Albedo", m_Albedo.r, m_Albedo.g, m_Albedo.b, 1.0f);
        shader.SetUniform1f("u_Roughness", m_Roughness);
        shader.SetUniform1f("u_Metallic", m_Metallic);
    }

private:
    glm::vec3 m_Albedo = glm::vec3(0.5f, 0.5f, 0.5f); //反照率,物体本身的颜色
    float m_Roughness = 0.5f; //粗糙度,影响高光的散射程度,0.0f为镜面反射,1.0f为漫反射
    float m_Metallic = 0.0f; //金属度,影响物体的反射率和颜色,0.0f为非金属,1.0f为金属
};