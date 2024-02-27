#version 330 core

in vec2 texCoord;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular1;
out vec4 fragColor; // Output variable for fragment shader

void main()
{
    fragColor = texture(Diffuse0, texCoord);
}