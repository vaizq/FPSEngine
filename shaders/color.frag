#version 330 core

in vec2 texCoord;
in vec3 vertexColor;
uniform sampler2D Diffuse0;
out vec4 fragColor; // Output variable for fragment shader
in float lightIntensity;

void main()
{
    fragColor = lightIntensity * vec4(0.0f, 1.0f, 0.0f, 1.0f);
}