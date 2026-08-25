//用于将Gizmo渲染到屏幕

#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = texCoord;

    //直接用已经包含位置朝向的Model矩阵
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
}