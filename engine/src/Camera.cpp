//
// Created by vaige on 24.2.2024.
//

#include "Camera.hpp"
#include <iostream>


Camera::Camera()
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    updateVectors();
    return glm::lookAt(mTransform.position, mTransform.position + mTransform.front(), mTransform.up());
}

void Camera::updateVectors()
{
    mFront = mTransform.front();
    mRight = glm::normalize(glm::cross(mFront, worldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}

void Camera::setPosition(const glm::vec3 &pos)
{
    mTransform.position = pos;
}

void Camera::rotatePitch(float amount)
{
    constexpr float maxAngle = glm::radians(89.f);

    mTransform.rotation.x += amount;
    if (mTransform.rotation.x> maxAngle) {
        mTransform.rotation.x = maxAngle;
    }
    else if (mTransform.rotation.x < -maxAngle) {
        mTransform.rotation.x = -maxAngle;
    }
    updateVectors();
}

void Camera::rotateYaw(float amount)
{
    mTransform.rotation.y += amount;
    updateVectors();
}

void Camera::translate(float forwardAmount, float rightAmount)
{
    mTransform.position += mFront * forwardAmount;
    mTransform.position += mRight * rightAmount;
}

void Camera::FPSTranslate(float forwardAmount, float rightAmount)
{
    glm::vec3 forward(mFront.x, 0.0f, mFront.z);
    forward = glm::normalize(forward);

    mTransform.position += forward * forwardAmount;
    mTransform.position += mRight * rightAmount;
}

glm::mat4 Camera::getModelMatrix() const
{
    return mTransform.modelMatrix();
}
