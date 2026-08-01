#version 330 core

in vec3 v_Pos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec4 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;

layout(location = 0) out vec4 color;

void main()
{
    color = u_Albedo;
}