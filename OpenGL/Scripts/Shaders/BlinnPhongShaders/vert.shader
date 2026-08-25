#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Pos; //世界坐标
out vec3 v_Normal;
out vec2 v_TexCoord; //(v代表Varing)通过Varing将顶点着色器传递到片段着色器

void main()
{
    vec4 worldPos = u_Model * vec4(position, 1.0);
    v_Pos = worldPos.xyz;

    v_Normal = mat3(transpose(inverse(u_Model))) * normal; //使用Model的转置逆矩阵
    v_TexCoord = texCoord;

    gl_Position = u_Projection * u_View * worldPos;
}