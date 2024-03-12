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
    vec3 lightPosition = vec3(0.0f, 100.0f, 0.0f);
    vec4 worldPos = model * vec4(inPosition, 1.0);
    vec3 lightDirection = worldPos.xyz - lightPosition;

    gl_Position = projection * view * worldPos;
    texCoord = inTexCoord;
    lightIntensity =  (dot(normalize(lightDirection), normalize(worldPos).xyz) + 1.0) / 2.0;
}
