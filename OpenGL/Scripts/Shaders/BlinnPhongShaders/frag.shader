#version 420 core

#define MAXLIGHTS 16

//本身属性
in vec3 v_Pos; //世界坐标
in vec3 v_Normal;
in vec2 v_TexCoord;

//材质相关
uniform vec3 u_Kd;
uniform vec3 u_Ks;
const vec3 u_Ka = vec3(0.05f);
uniform int u_KsPow;

uniform sampler2D u_Texture; //贴图
uniform int u_HasTexture; //1表示有纹理,0表无


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
    Light lights[MAXLIGHTS]; // <-- 这块区域里的具体数据
    int count;
} ub_Light;  //这个才是实例名

layout(location = 0) out vec4 color;

vec4 BlinnPhong(Light light)
{
    vec3 baseColor = u_Kd;
    if (u_HasTexture == 1) baseColor = texture2D(u_Texture, v_TexCoord).xyz;


    vec3 Ld, Ls;

    float attenuation = light.intensity; //衰减

    vec3 l; //光离着色点的方向

    if (light.type == 0) //方向光,方向光不衰减
    {
        l = normalize(-light.direction.xyz); //方向光朝向的反方向就是l
    }
    else if (light.type == 1) //点光源
    {
        vec3 diff = light.position.xyz - v_Pos;
        float r = length(diff);
        l = normalize(diff);
        attenuation /= (r * r);
    }

    vec3 n = normalize(v_Normal);
    
    Ld = baseColor * attenuation * max(0, dot(n, l)); 

    vec3 v = normalize(u_ViewPos - v_Pos);
    vec3 h = normalize(v + l);
    Ls = u_Ks * attenuation * pow(max(0, dot(n, h)), u_KsPow);

    return vec4(Ld + Ls, 1.0f);
}

void main()
{
    for (int i = 0; i < ub_Light.count; ++i)
    {
        color += BlinnPhong(ub_Light.lights[i]);
    }
    color += vec4(u_Ka, 1.0f);
}