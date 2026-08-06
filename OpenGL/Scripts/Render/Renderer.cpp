#include "Renderer.h"
#include <iostream>

/// <summary>
/// 清空错误队列
/// </summary>
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
    /*OpenGL 采用异步错误处理机制：
    错误不会立即抛出，而是存储在错误队列中
    调用 glGetError() 会弹出队列中的一个错误
    如果有多个错误，需要多次调用才能清空队列*/
}

/// <summary>
/// 打印错误
/// </summary>
bool GLPrintError(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error Occurred!] (" << error << ")：" << function << " " << file << " line:" << line << std::endl;
        return false;
    }
    return true;
}


Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Clear() const
{
    //同时清除颜色缓冲和深度缓冲:若不清理深度,上一帧的深度值会残留,导致新帧遮挡关系错乱
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    va.Bind();
    shader.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
