#pragma once

#include "KeyCode.h"

#include <array>
#include <glm/glm.hpp>

class Input
{
public:
    static void Init(GLFWwindow* window);

    //更新Input状态，必须在每帧开始时调用
    static void Update();

    static bool GetKey(KeyCode key);
    static bool GetKeyDown(KeyCode key);
    static bool GetKeyUp(KeyCode key);

    static bool GetMouseButton(MouseButton button);
    static bool GetMouseButtonDown(MouseButton button);
    static bool GetMouseButtonUp(MouseButton button);

    //获取鼠标位置
    static glm::vec2 GetMousePosition();
    //获取鼠标移动的偏移量
    static glm::vec2 GetMouseDelta();
private:
    static GLFWwindow* m_window;
    static std::array<bool, GLFW_KEY_LAST + 1> m_KeysCurr, m_KeysPrev;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_MouseCurr, m_MousePrev;

    static glm::vec2 m_MousePos;
    static glm::vec2 m_MousePrevPos;

};