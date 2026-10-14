#pragma once

class Time
{
public:
    static void Update();
    static float DeltaTime() { return m_DeltaTime; }
    static float TimeSinceStart() { return m_Time; }
    static int FrameCount() { return m_FrameCount; }
private:
    static float m_DeltaTime; //上一帧到当前帧的时间间隔
    static float m_Time; //从程序开始运行到当前帧的时间
    static float m_LastFrameTime; //上一帧的时间
    static int m_FrameCount; //帧数计数器
};