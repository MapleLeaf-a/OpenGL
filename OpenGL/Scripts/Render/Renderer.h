#pragma once

#include <GL/glew.h>
#include <memory>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

// 前向声明
class Scene;
class LightUBO;
class Texture;
class CameraComponent;
class Mesh;

//调用GLCall以更好地调试opengl代码
#define ASSERT(x) if (!(x)) __debugbreak() //断言(Assertion),如果x为假（false/0）,  __debugbreak()编译器内置函数(MSVC特有),触发断点
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLPrintError(#x, __FILE__, __LINE__))
//反斜杠（\）是 C/C++ 预处理器的续行符，用于告诉预处理器："下一行也是这个宏定义的一部分"。

void GLClearError();
bool GLPrintError(const char* function, const char* file, int line);

class  Renderer
{
public:
	Renderer();
	~Renderer();

	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

	//渲染所有网格
	void RenderAllMeshRenderers(const Scene& scene, Shader& shader) const;

	//渲染所有光源
	void RenderAllLights(const Scene& scene, LightUBO& lightUBO) const;

	//添加一个渲染Pass以给灯光处加一个标记
	void RenderLightGizmos(const Scene& scene, CameraComponent* camera, Shader& gizmoShader, const Texture* pointLightIconTexture = nullptr, const Texture* dirLightIconTexture = nullptr);
private:

	//用于承载gizmo图标的四边形Mesh
    std::shared_ptr<Mesh> m_GizmoQuadMesh;
};
