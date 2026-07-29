#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class  Camera
{
public:
    Camera();
    ~Camera();

    
private:
    void UpdateViewMatrix();
    
    glm::vec3 m_Position;
};