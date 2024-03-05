//
// Created by vaige on 5.3.2024.
//

#ifndef FPSFROMSCRATCH_INPUTMANAGER_H
#define FPSFROMSCRATCH_INPUTMANAGER_H

#include <map>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <memory>

class InputManager
{
public:
    using KeyCode = int;
    using Button = int;
    enum class KeyState
    {
        Pressed,
        Released
    };

    static InputManager& instance()
    {
        static std::unique_ptr<InputManager> self{nullptr};
        if (self == nullptr) {
            self = std::unique_ptr<InputManager>{new InputManager};
        }
        return *self;
    }

    static void registerCallbacks(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, InputManager::keyboardKeyCallback);
        glfwSetCursorPosCallback(window, InputManager::mouseMoveCallback);
        glfwSetMouseButtonCallback(window, InputManager::mouseButtonCallback);
    }

    [[nodiscard]] KeyState keyboardKey(KeyCode key) const
    {
        if (mKeyboard.contains(key)) {
            return mKeyboard.at(key);
        }
        else {
            return KeyState::Released;
        }
    }

    [[nodiscard]] bool keyPressed(KeyCode key) const
    {
        return keyboardKey(key) == KeyState::Pressed;
    }

    [[nodiscard]] glm::vec2 mousePos() const
    {
        return mMousePos;
    }

    [[nodiscard]] KeyState mouseButton(Button button) const
    {
        if (mMouseButtons.contains(button)) {
            return mMouseButtons.at(button);
        }
        else {
            return KeyState::Released;
        }
    }

    [[nodiscard]] bool buttonPressed(Button button) const
    {
        return mouseButton(button) == KeyState::Pressed;
    }

    std::map<KeyCode, std::function<void()>> pressHandlers;
    std::map<KeyCode, std::function<void()>> releaseHandlers;

private:
    InputManager() = default;

    static void keyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto& self = InputManager::instance();
        switch (action)
        {
            case GLFW_PRESS:
                self.mKeyboard[key] = KeyState::Pressed;
                if (self.pressHandlers.contains(key)) {
                    self.pressHandlers[key]();
                }
                break;
            case GLFW_RELEASE:
                self.mKeyboard[key] = KeyState::Released;
                if (self.releaseHandlers.contains(key)) {
                    self.releaseHandlers[key]();
                }
                break;
        }

    }

    static void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos)
    {
        auto& self = InputManager::instance();
        self.mMousePos.x = static_cast<float>(xPos);
        self.mMousePos.y = static_cast<float>(yPos);
    }

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        auto& self = InputManager::instance();
        switch (action)
        {
            case GLFW_PRESS:
                self.mMouseButtons[button] = KeyState::Pressed;
                break;
            case GLFW_RELEASE:
                self.mMouseButtons[button] = KeyState::Released;
                break;
        }
    }

    std::map<KeyCode, KeyState> mKeyboard;
    std::map<KeyCode, KeyState> mMouseButtons;
    glm::vec2 mMousePos{};
};



#endif //FPSFROMSCRATCH_INPUTMANAGER_H
