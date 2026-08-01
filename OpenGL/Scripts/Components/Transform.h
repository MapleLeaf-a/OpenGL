#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

class Transform
{
public:
	Transform() : m_Position(0.0f),
	 m_Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
	 m_Scale(1.0f) {}

	//设置位置
	void SetPosition(const glm::vec3& pos)
	{
		m_Position = pos;
		m_Dirty = true; 
	}
	const glm::vec3& GetPosition() const { return m_Position; }

	//用欧拉角设置旋转
	void SetRotation(const glm::vec3& eulerDegrees)
	{
		glm::quat qx = glm::angleAxis(glm::radians(eulerDegrees.x), glm::vec3(1, 0, 0)); //用x(归一化的)轴和一个弧度角构建一个四元数
		glm::quat qy = glm::angleAxis(glm::radians(eulerDegrees.y), glm::vec3(0, 1, 0));
		glm::quat qz = glm::angleAxis(glm::radians(eulerDegrees.z), glm::vec3(0, 0, 1));
		m_Rotation = qz * qy * qx; //依次左乘应用变换
		m_Dirty = true; 
	}
	//直接设置旋转
	void SetRotation(const glm::quat& quat)
	{
		m_Rotation = quat;
		m_Dirty = true;
	}
	//获取欧拉角
	const glm::vec3& GetEulerAngles() const { return glm::degrees( glm::eulerAngles(m_Rotation) ); }

	//设置Scale
	void SetScale(const glm::vec3& scale) 
	{
		m_Scale = scale;
		m_Dirty = true; 
	}
	const glm::vec3& GetScale() const { return m_Scale; }

	//获取本地坐标系的前方(看向-z方向)
	glm::vec3 GetForward() const
	{
		return m_Rotation * glm::vec3(0, 0, -1);
		/*这非常优雅！ m_rotation * 向量 就是把一个向量绕四元数旋转
		本地坐标系的三个轴（Forward/Right/Up）就是把世界坐标系的标准轴旋转到当前方向
		不用矩阵运算，直接四元数乘法，效率高且无精度损失*/
	}
	//获取本地坐标系的右方
	glm::vec3 GetRight() const
	{
		return m_Rotation * glm::vec3(1, 0 ,0);
	}
	//获取本地坐标系的上方
	glm::vec3 GetUp() const
	{
		return m_Rotation * glm::vec3(0, 1, 0);
	}

	const glm::mat4& GetModelMatrix() 
	{
        if (m_Dirty) 
		{
            UpdateModelMatrix();
        }
        return m_modelMatrix;
    }

private:
	 void UpdateModelMatrix() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
        glm::mat4 rotation = glm::mat4_cast(m_Rotation); //将四元数转换成4*4矩阵,有具体的数学公式,不需要再调用sin cos计算,效率更高
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);
        m_modelMatrix = translation * rotation * scale;
        m_Dirty = false;
    }

	glm::vec3 m_Position;
	glm::quat m_Rotation;   //四元数存储旋转,计算量更小且避免万向锁
	glm::vec3 m_Scale;
	glm::mat4 m_modelMatrix;
	bool m_Dirty;
};

