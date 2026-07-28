#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color; //统一变量,声明一个名为 u_Color 的 4 分量浮点向量统一变量，用于从 CPU 端向 GPU 着色器传递数据
uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = texColor;
}