#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace DefaultDockLayout
{
    inline void BuildDefaultDockLayout(ImGuiID dockspaceID)
    {
        const ImVec2 viewportSize = ImGui::GetMainViewport()->WorkSize; //工作区域:视口大小减去任务栏、菜单栏、状态栏后的剩余空间
        
        //清空该id下的整棵旧节点树
        ImGui::DockBuilderRemoveNode(dockspaceID);
    
        //建一个空的DockSpace根节点
        //必须带ImGuiDockNodeFlags_DockSpace，
        //否则只是普通节点，不会被后面的 DockSpace()/DockSpaceOverViewport() 接管
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);

        //设置根节点尺寸(切分比例的基准)
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewportSize);
        
        // ==================== 切分节点树 ====================
        // 签名: DockBuilderSplitNode(要切的节点, 方向, 新节点占比, 出参_该方向节点, 出参_剩余节点)
        //      返回值 == 该方向节点; 剩余部分通过第 5 个参数写回
        //
        // 目标形状(仿 Unity)：
        //   ┌──────────┬─────────────────────────┬───────────┐
        //   │          │                         │ Inspector │
        //   │Hierarchy │         Scene           ├───────────┤
        //   │  (左)    │        (中)             │  Stats    │
        //   └──────────┴─────────────────────────┴───────────┘
        
        ImGuiID dockMain = dockspaceID; //尚未切分的块
        ImGuiID dockLeft = ImGui::DockBuilderSplitNode(
            dockspaceID, //在dockspace上切
            ImGuiDir_Left, //新节点贴左边
            0.16f, //16%给Hierarchy
            nullptr, //不需要"左边节点"的id(不再往里切)，传nullptr
            &dockMain ); //剩余84%写回
        
        //在右侧切24%
        ImGuiID dockRight = ImGui::DockBuilderSplitNode(
            dockMain, ImGuiDir_Right, 0.24f, nullptr, &dockMain
        );

        //把右侧竖栏再横着切成上下两块：上面Inspector(82%)，下面Stats(18%)
        ImGuiID dockRightBottom = dockRight;
        ImGuiID dockRightTop = ImGui::DockBuilderSplitNode(
            dockRight, ImGuiDir_Up, 0.82f, nullptr, &dockRightBottom
        );

        //中间切60%给Scene
        ImGuiID dockCenterUp = ImGui::DockBuilderSplitNode(
            dockMain, ImGuiDir_Up, 0.6f, nullptr, &dockMain 
        );

        //登记窗口进节点
        //这里的字符串必须和 ImGui::Begin("xxx") 里的名字"逐字符一致"
        //不一致不会报错、也不会有任何提示，结果就是那个窗口静默地浮在外面
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
        ImGui::DockBuilderDockWindow("Scene", dockCenterUp);
        ImGui::DockBuilderDockWindow("Inspector", dockRightTop);
        ImGui::DockBuilderDockWindow("Stats", dockRightBottom);

        //收尾，必须调，否则节点树不落地
        ImGui::DockBuilderFinish(dockspaceID);
    }
}