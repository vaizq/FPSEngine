#version 330 core

layout(location = 0) in vec3 inPosition; // Vertex position attribute

uniform vec3 dPos;

void main()
{
    gl_Position = vec4(inPosition + dPos, 1.0); // Set vertex position
}