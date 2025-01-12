#ifndef SKINNED_MODEL_HPP
#define SKINNED_MODEL_HPP


#include <glm/glm.hpp>
#include <vector>
#include "engine/Shader.hpp"

struct Joint {
    std::string name;
    unsigned parent;
    glm::mat4 inverseBindPose;
};


struct SkinnedVertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texCoord{};
    glm::ivec4 boneIDs{-1, -1, -1, -1};
    glm::vec4 boneWeights{0, 0, 0, 0};
};

struct SkinnedModel {
    std::vector<unsigned> mMeshes;
    std::vector<Joint> mSkeleton;

    void draw(Shader& shader);

    static SkinnedModel* loadFromFile(const char* filename);
};


#endif
