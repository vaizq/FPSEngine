#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out float lightIntensity;

void main()
{
    vec3 lightPosition = vec3(0.0f, 10.0f, 0.0f);
    vec3 worldPos = (model * vec4(inPosition, 1.0)).xyz;
    vec3 lightDirection = worldPos - lightPosition;

    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    texCoord = inTexCoord;
    lightIntensity = 1.0f;//0.1 + min(0.9, (1.0 + dot(normalize(inNormal), normalize(lightDirection))) / 2.0);
}
