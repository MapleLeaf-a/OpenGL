#version 330 core

in vec3 v_Pos; //世界坐标
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec4 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;



struct Light
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    int type; //光源类型,0:方向光,1:点光源
};

uniform Light u_Light;

layout(location = 0) out vec4 color;

void main()
{
    color = u_Albedo;
}