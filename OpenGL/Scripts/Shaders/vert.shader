#version 330 core

layout(location = 0) in vec4 position; //顶点着色器输入声明,声明一个名为 position的4分量浮点向量输入变量,并将其绑定到位置索引0,用于接收来自CPU的顶点数据
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord; //(v代表Varing)通过Varing将顶点着色器传递到片段着色器

uniform mat4 u_MVP; //4*4的MVP矩阵

void main()
{
    gl_Position = u_MVP * position;
    v_TexCoord = texCoord;
}