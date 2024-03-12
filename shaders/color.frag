#version 330 core

uniform vec3 color;
in vec3 fragPosition;
in vec3 normal;
out vec4 fragColor; // Output variable for fragment shader
vec3 lightPosition = vec3(0.0, 100.0, 0.0);

void main()
{
    vec3 lightDirection = fragPosition - lightPosition;
    float lightIntensity = max(0.5, dot(normalize(normal), normalize(-lightDirection)));
    fragColor = vec4(color, 1.0f);
}

