#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

#include "Texture.h"

class Material
{
public:
    Material() = default;

    //将材质属性应用到着色器中
    virtual void Apply(Shader& shader) const = 0;

    //纹理
    void SetDiffuseTexture(std::shared_ptr<Texture> texture) {
        m_Texture = texture;
    }
    std::shared_ptr<Texture> GetDiffuseTexture() const { return m_Texture; }

protected:
    std::shared_ptr<Texture> m_Texture;
};