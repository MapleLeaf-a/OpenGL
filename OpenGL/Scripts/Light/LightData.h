#pragma once

#include "glm/glm.hpp"

struct LightData
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    int type;
};

