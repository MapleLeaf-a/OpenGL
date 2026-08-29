#version 420 core

layout(location = 0) in vec3 position;

//模型的Model矩阵
uniform mat4 u_Model;
//光源的VP矩阵 
uniform mat4 u_LightView;
uniform mat4 u_LightProj; 

void main()
{
    //从光源视角渲染一遍场景
    gl_Position = u_LightProj * u_LightView * u_Model * vec4(position, 1.0);
}