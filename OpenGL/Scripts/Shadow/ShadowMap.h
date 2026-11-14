#pragma once

#include <GL/glew.h>

#include "FrameBuffer.h"

class ShadowMap
{
public:
    ShadowMap(int width = 4096, int height = 4096)
    : m_FBO({.Width = width, .Height = height, .HasColor = false
        , .ClampToBorder = true, .LinearFilter = false})
    {}

    //渲染深度纹理
    void BindForWriting()
    {
        m_FBO.Bind();    
    }
    //解绑,回到默认帧缓冲(屏幕)
    void UnbindForWriting()
    {
        m_FBO.Unbind();
    }
    //绑定深度纹理供主渲染Pass采样
    void BindForReading(int textureSlot = 2)
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, m_FBO.GetDepthTexture());
    }

private:
    FrameBuffer m_FBO;
};