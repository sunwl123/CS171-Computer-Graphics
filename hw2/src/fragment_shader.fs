#version 330 core

in vec3 world_pos;
in vec3 world_normal;
out vec3 color;

uniform vec3 cam_pos;

uniform struct Light {
    float low;
    float ambient;
    float specular;
    vec3 power;
    float angle;
    vec3 direction;
    vec3 position;
} all_Lights[2];


vec3 Phone_lighting(Light light,vec3 world_pos,vec3 cam_pos,vec3 normal)
{   
    float light_ambient = light.ambient;

    vec3 normal_direction=normalize(normal);
    vec3 light_direction=normalize(light.position-world_pos); 
    float diffuse=max(dot(normal_direction,light_direction),0);

    vec3 view_direction=normalize(cam_pos-world_pos); 
    float light_specular=pow(max(dot(normal_direction,normalize(light_direction + view_direction)),0.0),light.specular);

    float light_low=1.0/(1.0+light.low*pow(length(light.position-world_pos),2));
    float light_pos_angle=degrees(acos(dot(-light_direction,normalize(light.direction)))); 
    if (light_pos_angle>light.angle) 
    {
        light_low=0.0;
    }
    vec3 result_color;
    result_color=(light_ambient+light_low*(diffuse+light_specular))*light.power;
    return result_color;
}

void main() {
    color=vec3(0.0f);
    for (int i=0;i<1;i++)
    {
        color+=Phone_lighting(all_Lights[i],world_pos,cam_pos,world_normal)*vec3(1.0f,0.0f,0.0f);
    }
}