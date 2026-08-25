//用于将Gizmo渲染到屏幕

#version 420 core

in vec2 v_TexCoord;

uniform sampler2D u_IconTexture;

layout(location = 0) out vec4 color;

void main()
{
    vec4 texColor = texture2D(u_IconTexture, v_TexCoord);

    color = texColor;
}