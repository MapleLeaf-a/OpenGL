#pragma once

#include <GL/glew.h>
#include "LightData.h"
#include "Renderer.h"

class LightUBO  //UBO:Uniform Buffer Object —— 用于批量数据共享,当你有大量 Uniform 数据需要传递（比如多光源、骨骼动画矩阵），使用单个 glUniform 调用次数太多，效率低下。UBO 允许你像 VBO 一样批量上传。
{
public:
    LightUBO()
    {
        //创建UBO
        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID)); //GL_UNIFORM_BUFFER 表示这是一个 Uniform 缓冲区
        
        //分配GPU显存空间
        GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), nullptr, GL_DYNAMIC_DRAW));
            /*usage 参数的常见值
                    值      	含义	        适用场景
            GL_STATIC_DRAW	数据几乎不变  顶点数据（模型加载后不变）
            GL_DYNAMIC_DRAW	数据经常变化  Uniform 数据（每帧更新）
            GL_STREAM_DRAW	数据每帧都变  每帧完全重写的临时数据*/
        
        //解绑
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

        GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_RendererID));
        /*glBindBufferBase —— 把 UBO 绑定到 Shader 的 binding point
          参数	     类型	                含义
        第 1 个	GLenum target	缓冲区类型（GL_UNIFORM_BUFFER）
        第 2 个	GLuint index	Binding point 索引（Shader 中用 binding = 0 对应）
        第 3 个	GLuint buffer	要绑定的缓冲区 ID
        作用：把 UBO 绑定到一个"插槽"（binding point）上，Shader 通过这个插槽来读取数据*/
    }    

    ~LightUBO()
    {
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }

    void Update(const LightData& lightData)
    {
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
        GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &lightData));
        /*更新 UBO 数据
          参数	      类型	            含义
        第 1 个	GLenum target	    缓冲区类型
        第 2 个	GLintptr offset	    从缓冲区的哪个位置开始写入（字节偏移）
        第 3 个	GLsizeiptr size	    要写入的数据大小（字节）
        第 4 个	const void* data	CPU 端的数据指针
        作用：将 CPU 端的数据（&light）拷贝到 GPU 显存中，覆盖指定位置的数据*/
        /*  方法	            作用	               适用场景
        glBufferData	分配新显存（可能重新分配）	 首次分配，或数据大小变化时
        glBufferSubData	更新已有显存的内容          每帧更新数据 
        glBufferSubData 不会重新分配内存，只是覆盖数据，效率更高*/
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

private:
    unsigned int m_RendererID;
};