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
    GLCall(glClear(GL_COLOR_BUFFER_BIT)); //清空屏幕（颜色缓冲区）的命令，把当前帧缓冲中的所有像素设置为预设的"清除颜色"，相当于"擦黑板"
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    va.Bind();
    shader.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
