#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

class Material
{
public:
    Material() = default;

    //将材质属性应用到着色器中
    virtual void Apply(Shader& shader) const
    {
        
    }

private:
};