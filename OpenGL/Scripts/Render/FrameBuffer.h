#pragma once

#include <GL/glew.h>

#include <iostream>

struct FrameBufferParams
{
    int Width        = 1920;
    int Height       = 1080;
    GLenum ColorFormat = GL_RGBA8;
    GLenum DepthFormat = GL_DEPTH_COMPONENT24;
    bool HasColor    = true; //false表示只写深度
    bool ClampToBorder = false; //纹理参数,阴影图必须true,边框填1.0
    bool LinearFilter = true; //纹理参数,阴影图必须false(NEAREST)
};


class FrameBuffer
{
public:
    FrameBuffer(const FrameBufferParams& params) : m_Params(params)
    {
        Generate();
    }

    ~FrameBuffer()
    {
        Destroy();
    }

    //切换到离屏渲染
    void Bind()
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_SavedFBO); //保存进来前绑定的FBO
        glGetIntegerv(GL_VIEWPORT, m_SavedViewport); //保存进来前视口,渲染完后要恢复
        glViewport(0, 0, m_Params.Width, m_Params.Height); //设置画布尺寸为贴图大小
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); //切换渲染目标
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空旧颜色&深度缓冲，防止旧数据残留
    }

    //解绑,回到默认帧缓冲(屏幕)
    void Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_SavedFBO); //把渲染目标切换回保存的进入前的FBO
        //恢复主窗口视口
        glViewport(m_SavedViewport[0], m_SavedViewport[1], m_SavedViewport[2], m_SavedViewport[3]);
    }

    void Resize(int width, int height)
    {
        if (width <= 0 || height <= 0) return;
        if (width == m_Params.Width && height == m_Params.Height) return;
        m_Params.Width = width;
        m_Params.Height = height;
        Destroy();
        Generate();          
    }

    GLint GetColorTexture() { return m_ColorTexture; }
    GLint GetDepthTexture() { return m_DepthTexture; }

private:
    FrameBufferParams m_Params;

    //保存进入前的视口,渲染完阴影后恢复主窗口视口
    GLint m_SavedViewport[4] = { 0, 0, 0, 0 };
    //保存进入前的FBO,防止给之前的Pass送到0(屏幕)上
    GLint m_SavedFBO = 0; // Bind 时记录,Unbind 时恢复

    unsigned int m_FBO = 0; //FBOFramebuffer Object，帧缓冲对象
    /* FBO（Framebuffer Object，帧缓冲对象） 是 OpenGL 提供的一个“画板”。
    默认情况下，OpenGL 的渲染结果会直接显示在屏幕上，这个默认的“画板”叫默认帧缓冲（Default Framebuffer）。
    而 FBO 是一个离屏（Off-screen）画板，它的渲染结果不会显示在屏幕上，而是存入纹理（Texture）或渲染缓冲（Renderbuffer）中，供后续使用。
    可以由以下附件组成：
    颜色附件：用于存储像素颜色数据（可多个，支持多渲染目标）。
    深度附件：存储深度信息，用于深度测试。
    模板附件：存储模板信息，用于模板测试。
    */

    unsigned int m_ColorTexture = 0; //颜色附件
    unsigned int m_DepthTexture = 0; //深度附件

    //设置纹理参数
    void SetTextureParams()
    {
        GLint filter = m_Params.LinearFilter ? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        if (m_Params.ClampToBorder) //阴影纹理
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //S(U方向)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); //T(V方向)
            //边框颜色(超出范围时使用)
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            //超出纹理范围时，返回 1.0（远平面深度），这样超出阴影范围的区域就不会产生阴影
        }
        else //颜色纹理
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
        }
        /*参数对比：
             参数	                    普通纹理	        深度纹理	            原因
        GL_TEXTURE_WRAP_S/T	        GL_CLAMP_TO_EDGE	GL_CLAMP_TO_BORDER	超出范围时，深度纹理需要用远平面深度值填充
        GL_TEXTURE_MIN/MAG_FILTER	GL_LINEAR	        GL_NEAREST	        深度纹理用最近邻采样（线性采样会导致深度值被平均，产生错误的阴影）*/
    }

    //创建所有资源
    void Generate()
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_SavedFBO);

        //创建FBO(注意:必须用glGenFramebuffers生成帧缓冲对象,不能用glGenBuffers!
        //否则生成的名字可能与纹理对象冲突,导致深度渲染到错误的对象,阴影贴图采样全为0,场景全黑)
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); //绑定FBO,参一为绑定目标，常用 GL_FRAMEBUFFER（读写）、GL_DRAW_FRAMEBUFFER（只写）、GL_READ_FRAMEBUFFER（只读）
         
        if (m_Params.HasColor)
        {
            //创建&绑定颜色纹理
            glGenTextures(1, &m_ColorTexture);
            glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
            //分配颜色纹理
            glTexImage2D(GL_TEXTURE_2D, 0, m_Params.ColorFormat, 
                m_Params.Width, m_Params.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            //设置参数
            SetTextureParams();
            //把第0颜色附件附加到FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);
        }
        else //当只写深度时
        {
            glDrawBuffer(GL_NONE); //不写入颜色
            glReadBuffer(GL_NONE); //不读取颜色
            /*参数	                        含义
            GL_NONE	                禁用颜色写入/读取（只写深度）
            GL_COLOR_ATTACHMENT0	写入第一个颜色附件
            作用：当你只需要渲染深度（如 Shadow Map）时，可以禁用颜色写入，提升性能。*/
        }

        //创建&绑定深度纹理
        glGenTextures(1, &m_DepthTexture);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
        //分配深度纹理（只存深度，不存颜色）
        glTexImage2D(GL_TEXTURE_2D, 0, m_Params.DepthFormat, 
            m_Params.Width, m_Params.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        /*和Texture.cpp中的参数对比：
        参数	          普通纹理	            深度纹理            	说明
        internalformat	GL_RGBA8	        GL_DEPTH_COMPONENT	GPU 内部存储格式
        format	        GL_RGBA	            GL_DEPTH_COMPONENT	CPU 数据格式
        type	        GL_UNSIGNED_BYTE	GL_FLOAT	        数据类型（深度需要浮点精度）深度值通常是 0~1 之间的浮点(NDC)，需要高精度来区分远近物体。如果用 GL_UNSIGNED_BYTE，只有 256 个级别，远远不够
        data	        imageData（有数据）  nullptr（无数据）	  深度纹理由 GPU 渲染填充*/
        //设置参数
        SetTextureParams();
        //把深度纹理附加到FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
    
        //检查FBO是否完整
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "[FrameBuffer] FBO incomplete!" << std::endl;
        }

        //解绑
        glBindFramebuffer(GL_FRAMEBUFFER, m_SavedFBO); 
    }

    //销毁所有资源
    void Destroy()
    {
        if (m_ColorTexture) { glDeleteTextures(1, &m_ColorTexture); m_ColorTexture = 0; }
        if (m_DepthTexture) { glDeleteTextures(1, &m_DepthTexture); m_DepthTexture = 0; }
        if (m_FBO)          { glDeleteFramebuffers(1, &m_FBO);      m_FBO = 0; }
    }
};