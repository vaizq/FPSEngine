#ifndef SKELETON_HPP
#define SKELETON_HPP

#include <string>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>


struct Joint {
    std::string name;
    unsigned parent;
    glm::mat4 transformation;
    glm::mat4 finalTransform;
};

struct BoneInfo {
    std::string name;
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransform;
};

struct Skeleton {
    std::vector<Joint> joints;
    std::vector<BoneInfo> bones;
    std::map<std::string, unsigned> boneIDs;
};

#endif
