#pragma once

#include "KeyCode.h"

#include <array>

class Input
{
public:
    static void Init(GLFWwindow* window);

    static void Update();

    static bool GetKey(KeyCode key);
    static bool GetKeyDown(KeyCode key);
    static bool GetKeyUp(KeyCode key);

    static bool GetMouseButton(MouseButton button);
    static bool GetMouseButtonDown(MouseButton button);
    static bool GetMouseButtonUp(MouseButton button);

private:
    static GLFWwindow* m_window;
    static std::array<bool, GLFW_KEY_LAST + 1> m_KeysCurr, m_KeysPrev;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_MouseCurr, m_MousePrev;
};