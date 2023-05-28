#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;

out vec3 vs_world_pos;
out vec3 vs_world_normal;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
    gl_Position=Projection*View*Model*vec4(aPos,1.0);
    vs_world_pos=(Model*vec4(aPos,1.0)).xyz;
    vs_world_normal=(Model*vec4(normal,0.0)).xyz;
}