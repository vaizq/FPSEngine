#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec2 texCoord;
out vec3 fragPosition;
out vec3 normal;

void main()
{
    vec4 worldPos = model * vec4(inPosition, 1.0);
    gl_Position = projection * view * worldPos;

    texCoord = inTexCoord;
    fragPosition = worldPos.xyz;
    normal = normalize(normalMatrix * inNormal);
}
