#version 420 core

in vec3 v_Pos; //世界坐标
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec4 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;



struct Light
{
    vec4 position;
    vec4 direction;
    vec4 color;
    float intensity;
    int type; //光源类型,0:方向光,1:点光源
};

/*UBO 的本质：它是一块“命名的内存区域”
当你使用 UBO 时，你实际上是在 GPU 里划出了一块固定的显存区域，并给它起了个名字叫 LightBlock（在 Shader 里）。*/
layout(std140, binding = 0) uniform LightBlock // <-- 这里定义了一块内存区域（Block）,块名
{
    Light u_Light; // <-- 这块区域里的具体数据
} ub_Light;

layout(location = 0) out vec4 color;

void main()
{
    color = ub_Light.u_Light.color;
}