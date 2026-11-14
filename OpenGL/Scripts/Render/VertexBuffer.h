#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const; //const成员函数:不会修改成员变量(可以读取)
	void Unbind() const;
};

