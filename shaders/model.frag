#version 330 core

in vec2 texCoord;
in vec3 fragPosition;
in vec3 normal;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform vec3 lightPosition;
out vec4 fragColor; // Output variable for fragment shader

void main()
{
    vec3 lightDirection = fragPosition - lightPosition;
    float lightIntensity = max(0.1, dot(normalize(normal), normalize(-lightDirection)));
    fragColor = lightIntensity * texture(texture_diffuse0, texCoord);
}