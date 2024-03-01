#version 330 core

uniform vec3 color;
out vec4 fragColor; // Output variable for fragment shader
in float lightIntensity;

void main()
{
    fragColor = lightIntensity * vec4(color, 1.0f);
}