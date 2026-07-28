#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Transform
{
public:
	Transform() : m_Position(0.0f), m_Rotation(0.0f), m_Scale(1.0f) {}

	//设置位置
	void SetPosition(const glm::vec3& pos)
	{
		m_Position = pos;
		m_dirty = true; 
	}
	const glm::vec3& GetPosition() const { return m_Position; }

	//欧拉角旋转
	void SetRotation(const glm::vec3& rot)
	{
		m_Rotation = rot;
		m_dirty = true; 
	}
	const glm::vec3& GetRotation() const { return m_Rotation; }

	//设置Scale
	void SetScale(const glm::vec3& scale) 
	{
		m_Scale = scale;
		m_dirty = true; 
	}
	const glm::vec3& GetScale() const { return m_Scale; }

	 const glm::mat4& getModelMatrix() {
        if (m_dirty) {
            updateModelMatrix();
        }
        return m_modelMatrix;
    }

private:
	 void updateModelMatrix() {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, m_Position);
        mat = glm::rotate(mat, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
        mat = glm::scale(mat, m_Scale);
        m_modelMatrix = mat;
        m_dirty = false;
    }

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;  //欧拉角(度)
	glm::vec3 m_Scale;
	glm::mat4 m_modelMatrix;
	bool m_dirty;
};

