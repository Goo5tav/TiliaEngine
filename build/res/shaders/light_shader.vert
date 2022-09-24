#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 tex_coords;

out vec3 FragPos;
out vec3 Normal;
out vec3 Tex_Coords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    Tex_Coords = tex_coords;
    FragPos = aPos;
    Normal = aNormal;  
    gl_Position = projection * view * vec4(aPos, 1.0);
}