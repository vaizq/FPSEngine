#version 330 core

in vec2 texCoord;
uniform sampler2D Diffuse0;
out vec4 fragColor; // Output variable for fragment shader

void main()
{
    fragColor = texture(Diffuse0, texCoord);
}