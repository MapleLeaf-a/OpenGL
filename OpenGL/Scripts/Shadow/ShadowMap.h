#pragma once

#include <GL/glew.h>

#include <iostream>

class ShadowMap
{
public:
    ShadowMap(int width = 2048, int height = 2048)
    : m_Width(width), m_Height(height)
    {
        //创建FBO(注意:必须用glGenFramebuffers生成帧缓冲对象,不能用glGenBuffers!
        //否则生成的名字可能与纹理对象冲突,导致深度渲染到错误的对象,阴影贴图采样全为0,场景全黑)
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); //绑定FBO,参一为绑定目标，常用 GL_FRAMEBUFFER（读写）、GL_DRAW_FRAMEBUFFER（只写）、GL_READ_FRAMEBUFFER（只读）

        //创建&绑定深度纹理
        glGenTextures(1, &m_DepthTexture);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);

        //分配深度纹理（只存深度，不存颜色）
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        /*和Texture.cpp中的参数对比：
        参数	          普通纹理	            深度纹理            	说明
        internalformat	GL_RGBA8	        GL_DEPTH_COMPONENT	GPU 内部存储格式
        format	        GL_RGBA	            GL_DEPTH_COMPONENT	CPU 数据格式
        type	        GL_UNSIGNED_BYTE	GL_FLOAT	        数据类型（深度需要浮点精度）深度值通常是 0~1 之间的浮点(NDC)，需要高精度来区分远近物体。如果用 GL_UNSIGNED_BYTE，只有 256 个级别，远远不够
        data	        imageData（有数据）  nullptr（无数据）	  深度纹理由 GPU 渲染填充*/

        //设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        /*参数对比：
             参数	                    普通纹理	        深度纹理	            原因
        GL_TEXTURE_WRAP_S/T	        GL_CLAMP_TO_EDGE	GL_CLAMP_TO_BORDER	超出范围时，深度纹理需要用远平面深度值填充
        GL_TEXTURE_MIN/MAG_FILTER	GL_LINEAR	        GL_NEAREST	        深度纹理用最近邻采样（线性采样会导致深度值被平均，产生错误的阴影）*/
        
        //边框颜色(超出范围时使用)
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        //超出纹理范围时，返回 1.0（远平面深度），这样超出阴影范围的区域就不会产生阴影

        //把深度纹理附加到FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
        
        //
        glDrawBuffer(GL_NONE); //不写入颜色
        glReadBuffer(GL_NONE); //不读取颜色
        /*参数	                        含义
        GL_NONE	                禁用颜色写入/读取（只写深度）
        GL_COLOR_ATTACHMENT0	写入第一个颜色附件
        作用：当你只需要渲染深度（如 Shadow Map）时，可以禁用颜色写入，提升性能。*/

        //检查FBO是否完整
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "ShadowMap FBO incomplete!" << std::endl;
        }

        //解绑,并恢复默认帧缓冲的颜色读写状态(否则会污染后续主渲染,导致屏幕全黑)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
    }

    ~ShadowMap()
    {
        glDeleteFramebuffers(1, &m_FBO);
        glDeleteTextures(1, &m_DepthTexture);
    }

    //渲染深度纹理
    void BindForWriting()
    {
        glGetIntegerv(GL_VIEWPORT, m_SavedViewport); //保存当前视口(主窗口),渲染完阴影后要恢复
        glViewport(0, 0, m_Width, m_Height); //设置画布尺寸为阴影贴图大小
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); //把渲染目标从“屏幕”切换到“深度纹理”
        glClear(GL_DEPTH_BUFFER_BIT); //清空旧深度缓冲，防止旧数据残留
    }

    //解绑,回到默认帧缓冲(屏幕)
    void UnbindForWriting()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //把渲染目标从“深度纹理”切换回“屏幕”,如果不恢复，后续所有渲染操作（比如画模型）都会被写入深度纹理，而不是显示在屏幕上
        //恢复主窗口视口!否则后续主渲染仍按阴影贴图尺寸(如2048x2048)进行,超出窗口的部分被裁剪,导致画面错乱甚至全黑
        glViewport(m_SavedViewport[0], m_SavedViewport[1], m_SavedViewport[2], m_SavedViewport[3]);
        //恢复默认帧缓冲的颜色读写状态(否则主渲染不写颜色,屏幕全黑)
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
    }

    //绑定深度纹理供主渲染Pass采样
    void BindForReading(int textureSlot = 2)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    }

private:
    //保存进入阴影Pass前的视口,渲染完阴影后恢复主窗口视口
    GLint m_SavedViewport[4] = { 0, 0, 0, 0 };

    unsigned int m_FBO; //FBOFramebuffer Object，帧缓冲对象
    /* FBO（Framebuffer Object，帧缓冲对象） 是 OpenGL 提供的一个“画板”。
    默认情况下，OpenGL 的渲染结果会直接显示在屏幕上，这个默认的“画板”叫默认帧缓冲（Default Framebuffer）。
    而 FBO 是一个离屏（Off-screen）画板，它的渲染结果不会显示在屏幕上，而是存入纹理（Texture）或渲染缓冲（Renderbuffer）中，供后续使用。
    */

    unsigned int m_DepthTexture;

    int m_Width, m_Height;
};