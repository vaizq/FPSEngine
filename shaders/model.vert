#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inBoneWeights;

const int MAX_BONES=200;
const int MAX_BONE_INFLUENCES=4;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 finalBoneMatrices[MAX_BONES];

out vec2 texCoord;
out vec3 fragPosition;
out vec3 normal;

void main()
{
    vec4 totalPos = vec4(0);
    vec3 totalNorm = vec3(0);

    for(int i = 0; i < MAX_BONE_INFLUENCES; i++) {
        int boneID = inBoneIDs[i];
        float weight = inBoneWeights[i];

        if (weight > 0.0) {
            mat4 boneMatrix = finalBoneMatrices[boneID];

            totalPos += weight * (boneMatrix * vec4(inPosition, 1.0));
            totalNorm += weight * (mat3(boneMatrix) * inNormal);
        }
    }

    vec4 worldPos = model * totalPos;
    gl_Position = projection * view * worldPos;

    texCoord = inTexCoord;
    fragPosition = worldPos.xyz;
    normal = normalize(normalMatrix * totalNorm);
}
