#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 normal;

out vec3 world_pos;
out vec3 world_normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position=Projection*View*Model*vec4(aPos,1.0);
    world_pos=vec3(Model*vec4(aPos,1.0)).xyz;
    world_normal=vec3(Model*vec4(normal,0.0)).xyz;
}