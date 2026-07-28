#pragma once

#include "VertexBuffer.h"
//#include "VertexBufferLayout.h" //VertexBufferLayout.h中尝试include Renderer.h,而后者尝试include VertexArray.h,所以在这里不能再include VertexBufferLayout,否则会无限循环下去

class VertexBufferLayout; //采用前向声明

class VertexArray
{
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	
	void Bind() const;
	void Unbind() const;
};