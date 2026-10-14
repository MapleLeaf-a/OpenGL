#include "Time.h"

#include <GLFW/glfw3.h>

float Time::m_DeltaTime = 0.0f;
float Time::m_Time = 0.0f;
float Time::m_LastFrameTime = 0.0f;
int Time::m_FrameCount = 0;

void Time::Update()
{
    float now = glfwGetTime();
    m_DeltaTime = now - m_LastFrameTime;
    m_Time = now;
    m_LastFrameTime = now;
    m_FrameCount++;
}