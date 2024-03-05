//
// Created by vaige on 24.2.2024.
//

#ifndef FPSFROMSCRATCH_CAMERA_H
#define FPSFROMSCRATCH_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Transform.h"


class Camera
{
public:
    Camera();
    void translate(float forwardAmount, float rightAmount);
    void FPSTranslate(float forwardAmount, float rightAmount);
    void setPosition(const glm::vec3& pos);
    void rotateYaw(float amount);
    void rotatePitch(float amount);
    [[nodiscard]] glm::mat4 getViewMatrix();
    [[nodiscard]] glm::mat4 getModelMatrix() const;

    [[nodiscard]] const Transform& getTransform() const
    {
        return mTransform;
    }

    [[nodiscard]] Transform& getTransform()
    {
        return mTransform;
    }

    [[nodiscard]] const glm::vec3& getPosition() const
    {
        return mTransform.position;
    }
    [[nodiscard]] float getYaw() const
    {
        return mTransform.yaw;
    }
    [[nodiscard]] float getPitch() const
    {
        return mTransform.pitch;
    }
    [[nodiscard]] const glm::vec3& getFront() const
    {
        return mFront;
    }
private:
    void updateVectors();
    Transform mTransform;
    glm::vec3 mUp{0.0f, 1.0f, 0.0f};
    glm::vec3 mFront{0.0f, 0.0f, -1.0f};
    glm::vec3 mRight{1.0f, 0.0f, 0.0f};

};


#endif //FPSFROMSCRATCH_CAMERA_H
