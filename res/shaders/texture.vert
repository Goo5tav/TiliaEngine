#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texture_coords;

void main()
{
    texture_coords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}