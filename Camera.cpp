//
// Created by vaige on 24.2.2024.
//

#include "Camera.h"
#include <iostream>


constexpr glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

Camera::Camera()
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(mPos, mPos + mFront, mUp);
}

void Camera::updateVectors()
{
    mFront = glm::normalize(glm::vec3(std::cos(mYaw) * std::cos(mPitch), std::sin(mPitch), std::sin(mYaw) * std::cos(mPitch)));
    mRight = glm::normalize(glm::cross(mFront, worldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}

void Camera::setPosition(const glm::vec3 &pos)
{
    mPos = pos;
}

void Camera::rotatePitch(float amount)
{
    constexpr float maxAngle = glm::radians(89.f);

    mPitch += amount;
    if (mPitch > maxAngle) {
        mPitch = maxAngle;
    }
    else if (mPitch < -maxAngle) {
        mPitch = -maxAngle;
    }
    updateVectors();
}

void Camera::rotateYaw(float amount)
{
    std::cout << "DYAW: " << amount << '\n';
    mYaw += amount;
    updateVectors();
}

void Camera::translate(float forwardAmount, float rightAmount)
{
    mPos += mFront * forwardAmount;
    mPos += mRight * rightAmount;
}

void Camera::FPSTranslate(float forwardAmount, float rightAmount)
{
    glm::vec3 forward(mFront.x, 0.0f, mFront.z);
    forward = glm::normalize(forward);

    mPos += forward * forwardAmount;
    mPos += mRight * rightAmount;
}
