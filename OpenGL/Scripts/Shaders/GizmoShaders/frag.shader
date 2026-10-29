//用于将Gizmo渲染到屏幕

#version 420 core

in vec2 v_TexCoord;

uniform sampler2D u_IconTexture;

layout(location = 0) out vec4 color;

void main()
{
    vec4 texColor = texture(u_IconTexture, v_TexCoord); //GLSL 1.30+ 已废弃 texture2D，统一用重载的 texture()

    color = texColor;
}