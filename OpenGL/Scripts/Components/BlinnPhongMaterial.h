#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

class BlinnPhongMaterial{
public:
    BlinnPhongMaterial() = default;
    BlinnPhongMaterial(glm::vec3 kd, glm::vec3 ks, glm::vec3 ka, float pow)
        : m_Kd(kd), m_Ks(ks), m_Ka(ka), m_KsPow(pow) {}

    void SetKd(const glm::vec3& kd) { m_Kd = kd; }
    void SetKs(const glm::vec3& ks) { m_Ks = ks; }
    void SetKa(const glm::vec3& ka) { m_Ka = ka; }
    void SetKsPow(float pow) { m_KsPow = pow; }

    //将材质属性应用到着色器中
    void Apply(Shader& shader) const
    {
        shader.SetUniform3f("u_Kd", m_Kd);
        shader.SetUniform3f("u_Ks", m_Ks);
        shader.SetUniform3f("u_Ka", m_Ka);
        shader.SetUniform1f("u_KsPow", m_KsPow);
    }

private:
    glm::vec3 m_Kd;
    glm::vec3 m_Ks;
    glm::vec3 m_Ka;
    float m_KsPow;  
};