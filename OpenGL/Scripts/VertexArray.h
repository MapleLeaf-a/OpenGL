/*关于VAO：
Vertex Array Object（顶点数组对象，简称 VAO） 是一个 OpenGL 容器对象，用于存储所有与顶点数据配置相关的状态
如果没有VAO，每次绘制都要重新配置绑定vbo、ibo(ebo)
- VAO 存储了什么？
  VAO 会保存以下状态：
	  存储内容				说明
	顶点属性配置	glVertexAttribPointer 的所有参数
	启用的属性数组	glEnableVertexAttribArray 的状态
	顶点属性绑定	每个属性关联的 VBO
	索引缓冲区		绑定的 GL_ELEMENT_ARRAY_BUFFER（重要）
	顶点属性格式	数据类型、归一化、步长、偏移量
*/

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