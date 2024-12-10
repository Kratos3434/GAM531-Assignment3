#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;
out VS_OUT {
    vec3 color;
} vs_out;

void main()
{
    vs_out.color = aColor;
    vec4 position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    gl_Position = projection * position * view; 
}
