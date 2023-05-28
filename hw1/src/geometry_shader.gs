#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=200) out;

in vec3 vs_world_pos[];
in vec3 vs_world_normal[];

out vec3 world_pos;
out vec3 world_normal;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void set_new_vertex(vec3 x,vec3 y,float factor) {
    world_pos=x+y*factor;
    world_normal=y;
    gl_Position=Projection*View*vec4(world_pos, 1.0);
    EmitVertex();
}


void main() {
    vec3 a = (vs_world_normal[0]+vs_world_normal[1]+vs_world_normal[2]) / 3;
    vec3 b = (vs_world_pos[0]+vs_world_pos[1]+vs_world_pos[2]) / 3;

    set_new_vertex(vs_world_pos[0],vs_world_normal[0], 0);
    set_new_vertex(vs_world_pos[1],vs_world_normal[1], 0);
    set_new_vertex(vs_world_pos[2],vs_world_normal[2], 0);
    set_new_vertex(b,a,0.005);
    EndPrimitive();
}