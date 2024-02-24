#version 330 core

in vec2 texCoord;
uniform sampler2D Diffuse0;
out vec4 fragColor; // Output variable for fragment shader

void main()
{
    fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}