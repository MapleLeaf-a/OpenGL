#include "Input.h"

GLFWwindow* Input::m_window = nullptr; //静态成员必须在类外初始化(类内只是声明)
std::array<bool, GLFW_KEY_LAST + 1> Input::m_KeysCurr = { false };
std::array<bool, GLFW_KEY_LAST + 1> Input::m_KeysPrev = { false };
std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> Input::m_MouseCurr = { false };
std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> Input::m_MousePrev = { false };
glm::vec2 Input::m_MousePos = glm::vec2(0.0f);
glm::vec2 Input::m_MousePrevPos = glm::vec2(0.0f);

void Input::Init(GLFWwindow* window)
{
    m_window = window;
}

void Input::Update()
{
    //当前->上一帧
    m_KeysPrev = m_KeysCurr;
    m_MousePrev = m_MouseCurr;

    //轮询键盘
    for (int i = 0; i < GLFW_KEY_LAST + 1; i++)
    {
        m_KeysCurr[i] = glfwGetKey(m_window, i) == GLFW_PRESS;
    }

    //轮询鼠标
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST + 1; i++)
    {
        m_MouseCurr[i] = glfwGetMouseButton(m_window, i) == GLFW_PRESS;
    }

    //更新鼠标
    m_MousePrevPos = m_MousePos;
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    m_MousePos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));


}

bool Input::GetKey(KeyCode key)
{
    return m_KeysCurr[static_cast<int>(key)];
}

bool Input::GetKeyDown(KeyCode key)
{
    int i = static_cast<int>(key);
    return m_KeysCurr[i] && !m_KeysPrev[i]; //前一帧没有按下，当前帧按下
}

bool Input::GetKeyUp(KeyCode key)
{
    int i = static_cast<int>(key);
    return !m_KeysCurr[i] && m_KeysPrev[i]; //当前帧没有按下，前一帧按下
}

bool Input::GetMouseButton(MouseButton button)
{
    return m_MouseCurr[static_cast<int>(button)];
}

bool Input::GetMouseButtonDown(MouseButton button)
{
    int i = static_cast<int>(button);
    return m_MouseCurr[i] && !m_MousePrev[i];
}

bool Input::GetMouseButtonUp(MouseButton button)
{
    int i = static_cast<int>(button);
    return !m_MouseCurr[i] && m_MousePrev[i];
}

glm::vec2 Input::GetMousePosition()
{
    return m_MousePos;
}

glm::vec2 Input::GetMouseDelta()
{
    return m_MousePos - m_MousePrevPos;
}