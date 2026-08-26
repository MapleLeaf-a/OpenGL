#version 420 core

layout(location = 0) in vec3 position;

uniform mat4 u_LightMVP;

void main()
{
    gl_Position = u_LightMVP * vec4(position, 1.0);
}