//
// Created by vaige on 24.2.2024.
//

#ifndef FPSFROMSCRATCH_CAMERA_H
#define FPSFROMSCRATCH_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
    Camera();
    void translate(float forwardAmount, float rightAmount);
    void setPosition(const glm::vec3& pos);
    void rotateYaw(float amount);
    void rotatePitch(float amount);
    [[nodiscard]] glm::mat4 getViewMatrix();
    [[nodiscard]] const glm::vec3& getPosition() const
    {
        return mPos;
    }
private:
    void updateVectors();
    glm::vec3 mPos{};
    glm::vec3 mUp{0.0f, 1.0f, 0.0f};
    glm::vec3 mFront{0.0f, 0.0f, -1.0f};
    glm::vec3 mRight{1.0f, 0.0f, 0.0f};
    float mYaw{glm::radians(-90.f)};
    float mPitch{};
};


#endif //FPSFROMSCRATCH_CAMERA_H
