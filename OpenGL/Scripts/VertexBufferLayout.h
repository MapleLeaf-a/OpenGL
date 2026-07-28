#pragma once

#include <vector>
#include "Renderer.h"

//对应glVertexAttribPointer中的部分参数
struct VertexBufferElement
{
	unsigned int type;  //每个分量的数据类型
	unsigned int count; //每个顶点的属性分量个数
	unsigned char normalized; //是否将数据归一化

	static unsigned int GetSizeofType(unsigned int type)  //注意不能直接sizeof(GL_FLOAT)是因为GL_FLOAT 等是 OpenGL 枚举常量，不是 C++ 类型
	{
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride; //步长，即相邻两个顶点之间相隔多少字节
public:
	VertexBufferLayout() : m_Stride(0) {}

	template<typename T>
	void Push(unsigned int count)
	{
		//static_assert(false); //编译期间检查条件
	}

	//全特化模板（Explicit / Full Specialization），是指为模板的某一组特定类型，提供一个完全独立的实现版本
	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeofType(GL_FLOAT) * count;
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeofType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += VertexBufferElement::GetSizeofType(GL_UNSIGNED_BYTE) * count;
	}

	/*
		为什么是 += 而不是 =？
		因为一个顶点布局可能包含多个属性，每个属性都要累加到总步长中。
		cpp
		VertexBufferLayout layout;
		layout.Push<float>(2);  //位置：2 个 float  → stride += 8
		layout.Push<float>(3);  //颜色：3 个 float  → stride += 12
		layout.Push<float>(2);  //纹理坐标：2 个 float → stride += 8
		最终 m_Stride = 8 + 12 + 8 = 28 字节
	*/



	//返回的是常量引用（不能修改返回的对象）(const ... &)
	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	
	inline unsigned int GetStride() const { return m_Stride; }
};