#version 420 core

//本身属性
in vec3 v_Pos; //世界坐标
in vec3 v_Normal;
in vec2 v_TexCoord;

//材质相关
uniform vec4 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform vec3 u_Ka;
uniform float u_KsPow;


//相机相关
uniform vec3 u_ViewPos;

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
} ub_Light;  //这个才是实例名

layout(location = 0) out vec4 color;

vec4 BlinnPhong(Light light)
{
    if (light.type == 1) //点光源
    {
        vec3 n = normalize(v_Normal);
        
        vec3 diff = light.position.xyz - v_Pos;
        float r = length(diff);
        float r_square = r * r;
        
        vec3 l = normalize(diff);
        
        float I = light.intensity;
        float divi = I / r_square;
        
        vec3 Ld = u_Kd * u_Albedo * divi * max(0, dot(n, l)); 

        vec3 v = normalize(u_ViewPos - v_Pos);
        vec3 h = normalize(v + l);
        vec3 Ls = u_Ks * divi * pow(max(0, dot(n, h)), u_KsPow);

        vec3 La = u_Ka;
    }

    return ;
}

void main()
{
    color = BlinnPhong(ub_Light.u_Light);
}