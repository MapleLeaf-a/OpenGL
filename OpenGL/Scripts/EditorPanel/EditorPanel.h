#pragma once

#include "imgui/imgui.h"

#include "Scene.h"
#include "GameObject.h"

namespace EditorPanel
{
    //选中的GO
    inline GameObject* selectedGO = nullptr; //cpp17 inline变量:header-only被多处include也只有一个实例。在 C++17 之前，如果你在头文件中定义一个全局变量（非 const 或 constexpr），当这个头文件被多个源文件包含时，链接器会报“多重定义”错误

    inline void DrawNode(Scene& scene, GameObject* go); //前向声明给DrawHierarchy调用
    
    //在面板上绘制所有GO&处理面板点击
    inline void DrawHierarchy(Scene& scene)
    {
        ImGui::Begin("Hierarchy");

        for (const std::unique_ptr<GameObject>& obj : scene.GetGameObjects())
        {
            if (obj->GetTransform().GetParent() == nullptr) //只有根节点才Draw
            {
                DrawNode(scene, obj.get());
            }
        }

        //点空白取消选中
        if (ImGui::IsWindowHovered() //当前鼠标是否悬停在这个 ImGui 窗口（面板）内 
            && !ImGui::IsAnyItemHovered() //鼠标是否悬停在当前窗口里的某个具体控件上（比如按钮、滑块、输入框
            && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) //鼠标左键点击,注意不要用Input,防止“事件穿透”到背后的3D场景中
        {
            selectedGO = nullptr;
        }

        ImGui::End();
    }

    //递归地绘制子物体
    inline void DrawNode(Scene& scene, GameObject* go)
    {
        const auto& children = go->GetTransform().GetChildren();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow //默认行为：如果不加这个标志，你点击树节点的任何地方（文字、空白处），都会触发展开/折叠。加上之后：只有点击文字左侧的 小三角箭头 才会展开/折叠
                                 | ImGuiTreeNodeFlags_SpanAvailWidth; //作用：让这个树节点的选中高亮背景填满整行可用宽度。效果：就像 Unity 里点击某个物体，整个蓝色的选中条是从左拉到右的。不加这个，高亮只包住文字部分
        if (go == selectedGO) flags |= ImGuiTreeNodeFlags_Selected; //选中物体：当前行会显示一个高亮背景（通常是蓝色），表示“这个物体被选中了
        if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf  //作用：把这个节点标记为“叶子节点”。效果：它不会显示展开/折叠的小三角箭头。点击它时，不会触发展开/折叠逻辑 
                                    | ImGuiTreeNodeFlags_NoTreePushOnOpen; //作用：告诉 ImGui，当这个节点被打开时，不要调用 ImGui::TreePush()。为什么需要：因为它是叶子节点（没有子物体），即使“打开”了也没有子节点可以缩进显示。如果不加这个标志，你调用 ImGui::TreeNodeEx 后，必须配对调用 ImGui::TreePop()，否则 ImGui 的 ID 栈会错乱，导致后续 UI 崩溃。加上这个标志，就可以省去 TreePop()

        ImGui::PushID(go); //压入指针防重名。通常，ImGui 会拿你传入的标签（Label）字符串来做哈希，都同名就会冲突。PushID作用是在当前的 ID 栈（ID Stack）中压入一个新的标识符。ImGui 在计算后续控件的 ID 时，会把你压入的这个标识符作为“前缀”一起参与哈希计算
        
        bool open = ImGui::TreeNodeEx("##node", flags, go->GetName().c_str()); //返回当前这个树节点是否处于“展开”状态

        //点文字选中;点箭头只展开,所以排除IsItemToggledOpen
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedGO = go;
        }

        //右键菜单
        if (ImGui::BeginPopupContextItem("##ctx")) //上下文菜单（Context Menu）是自动拦截右键事件的，不需要手动写点击检测
        {
            if (ImGui::MenuItem("Delete"))
            {
                if (go == selectedGO)
                {
                    selectedGO = nullptr;
                }
                scene.DestroyGameObject(go);
            }
            ImGui::EndPopup(); //EndPopup() 只是结束代码书写，不代表关闭动作。在 ImGui 中，所有以 Begin 开头的函数，都必须以对应的 End 函数结束。这是一种“作用域”的界定
        }

        //当前节点展开时递归地画子物体
        if (open && !children.empty())
        {
            for (Transform* child : children)
            {
                DrawNode(scene, child->GetOwner());
            }
            ImGui::TreePop(); //与TreeNodeEx严格配对
        }
        ImGui::PopID();
    }

    //绘制Transform组件
    inline void DrawTransform(Transform& t)
    {
        glm::vec3 pos = t.GetPosition();
        if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) t.SetPosition(pos);
        glm::vec3 euler = t.GetEulerAngles();
        if (ImGui::DragFloat3("Rotation", &euler.x, 0.1f)) t.SetRotation(euler);
        glm::vec3 scale = t.GetScale();
        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) t.SetScale(scale);
    }


    //绘制Inspector
    inline void DrawInspector(const Scene& scene)
    {
        ImGui::Begin("Inspector");
        
        if (!selectedGO) //没有GO被选中 
        {
            ImGui::TextDisabled("Click a GO to show its inspector"); //显示灰暗色文本
            ImGui::End();
            return;
        }

        //Inspector改名功能
        char nameBuf[128]; //c风格字符串
        std::strncpy(nameBuf, selectedGO->GetName().c_str(), sizeof(nameBuf) - 1); //127字符,超过会截断不会缓冲区溢出
        nameBuf[sizeof(nameBuf) - 1] = '\0'; //c风格字符串末尾\0
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) //ImGui::InputText 的底层是 C 风格的，它需要一个预先分配好内存的字符数组（缓冲区）来让你打字。所以必须走一遍 string -> char[] -> 修改后 -> string 的流程
        {
            selectedGO->SetName(nameBuf);
        }

        ImGui::Separator(); //画一条分割线

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) //可折叠的标题栏
        {
            DrawTransform(selectedGO->GetTransform());
        }

        for (const auto& comp : selectedGO->GetComponents())
        {
            ImGui::PushID(comp.get()); //防止多个同类型组件ID重名冲突
            if (ImGui::CollapsingHeader(comp->GetTypeName(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                comp->DrawInspectorGUI();
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    
    //绘制帧率
    inline void DrawStats(const Scene& scene)
    {
        ImGui::Begin("Stats");
        ImGui::Text("%.3f ms / frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("MeshRenderers: %d", (int) scene.GetMeshRenderers().size());
        ImGui::End();
    }

    
}