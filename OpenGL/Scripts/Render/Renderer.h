#pragma once

#include <GL/glew.h>
#include <memory>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h" //显式包含:让 Texture 在 Renderer.h 中即为完整类型,避免 IntelliSense 误判为不完整类型
#include "ShadowMap.h"

// 前向声明:这些类型在 Renderer 中仅用作引用/指针参数或智能指针成员,无需完整定义
// (完整定义由 .cpp 里 include 对应头文件提供,避免头文件循环依赖)
class Scene;
class LightUBO;
class CameraComponent;
class LightComponent;
class Mesh;
class GameObject;

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

	//渲染所有网格，参数三为是否启用材质
	void RenderAllMeshRenderers(const Scene& scene, Shader& shader, bool enableMaterial) const;

	//渲染所有光源
	void RenderAllLights(const Scene& scene, LightUBO& lightUBO) const;

	//添加一个渲染Pass以给灯光处加一个标记
	void RenderLightGizmos(const Scene& scene, CameraComponent* camera, Shader& gizmoShader, const Texture* pointLightIconTexture = nullptr, const Texture* dirLightIconTexture = nullptr);

	//渲染深度Pass
	void RenderShadowPass(const Scene& scene, GameObject* light, ShadowMap& shadowMap, Shader& shadowShader);
private:

	//用于承载gizmo图标的四边形Mesh
    std::shared_ptr<Mesh> m_GizmoQuadMesh;
};
