#ifndef SCENE_H
#define SCENE_H

#include "Camera.hpp"
#include "GameObject.hpp"

class Scene {
public:
    Camera* camera;
    std::unique_ptr<GameObject> content;
};

#endif
