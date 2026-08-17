#pragma once

#include "Material.h"

class BlinnPhongMaterial : public Material
{
public:
    BlinnPhongMaterial() = default;
    BlinnPhongMaterial(glm::vec3 kd, glm::vec3 ks, int pow)
        : m_Kd(kd), m_Ks(ks), m_KsPow(pow) {}

    void SetKd(const glm::vec3& kd) { m_Kd = kd; }
    void SetKs(const glm::vec3& ks) { m_Ks = ks; }
    void SetKsPow(int pow) { m_KsPow = pow; }

    //将材质属性应用到着色器中
    void Apply(Shader& shader) const override
    {
        shader.SetUniform3f("u_Kd", m_Kd);
        shader.SetUniform3f("u_Ks", m_Ks);
        shader.SetUniform1i("u_KsPow", m_KsPow);
    }

private:
    glm::vec3 m_Kd;
    glm::vec3 m_Ks;
    int m_KsPow;  
};