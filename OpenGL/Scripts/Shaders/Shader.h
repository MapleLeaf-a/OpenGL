#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

class Shader
{
private:
	mutable std::unordered_map<std::string, int> m_UniformlocationCache;
	//声明为mutable,声明“这个成员不算对象的逻辑状态，可以修改”,于是这个成员经过mutable标记以后就可以在const函数中更改了
	unsigned int m_RendererID;
	unsigned int m_type;
public:
	Shader(const std::string& vertFilePath, const std::string& fragFilePath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform3f(const std::string& name, const glm::vec3& vec);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	/// <summary>
	/// 解析Shader文件
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	std::string PraseShader(const std::string& filePath);
	/// <summary>
	/// 返回着色器对象id
	/// </summary>
	/// <param name="type"></param>
	/// <param name="source"></param>
	/// <returns></returns>
	unsigned int CompileShader(unsigned int type, const std::string& source);

	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	int GetUniformLocation(const std::string& name) const;
};

