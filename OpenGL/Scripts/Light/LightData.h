#pragma once

#include "glm/glm.hpp"

struct LightData
{
    glm::vec4 position;
    glm::vec4 direction;
    glm::vec4 color;
    float intensity;
    int type; //光源类型,0:方向光,1:点光源
    float padding[2]; //填充8字节至一共64字节,与GPU端对齐
};

static_assert(sizeof(LightData) == 64, "LightData size must be 64 bytes,so as to align with GPU");