#version 330 core
layout (points) in;
    layout (triangle_strip, max_vertices = 5) out;
    //layout (line_strip, max_vertices = 5) out;
    //layout (points, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

in vec3 pos[];

out vec3 fColor;

uniform mat4 projection;
uniform mat4 view;

void build_house(vec3 position) {
    vec4 transformedPosition = projection * view * vec4(position, 1.0);
    
    gl_Position = transformedPosition + vec4(-0.2, -0.2, 0.0, 0.0); // Left vertex
    fColor = vec3(1, 0.0, 0.0);
    EmitVertex();

    gl_Position = transformedPosition + vec4(0.2, -0.2, 0.0, 0.0); // Right vertex
    fColor = vec3(0.0, 1.0, 0.0);
    EmitVertex();

    gl_Position = transformedPosition + vec4(-0.2, 0.2, 0.0, 0.0); // Top vertex
    fColor = vec3(0.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = transformedPosition + vec4(0.2, 0.2, 0.0, 0.0); // Bottom-left vertex
    fColor = vec3(1.0, 1.0, 0.0);
    EmitVertex();

    gl_Position = transformedPosition + vec4(0.0, 0.4, 0.0, 0.0); // Bottom-right vertex
    fColor = vec3(0.0, 1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    build_house(pos[0]);
}