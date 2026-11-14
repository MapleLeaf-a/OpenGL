#pragma once

#include <string>

class Texture
{
private:
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	unsigned int m_RendererID;
	int m_Width, m_Height, m_BPP; //BPP（Bits Per Pixel，每像素位数） 是指存储一个像素颜色信息所需的比特数（bit）
public:
	Texture(const std::string& path);
	~Texture();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="slot">想绑定纹理的GPU插槽</param>
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

