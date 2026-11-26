# ImGui-Docking 可停靠窗口
## 什么是DockSpace
|概念|是什么|谁决定|
|:---:|:---:|:---:|
|DockSpace|	一个空的停靠容器（本质是个可被切分的大矩形区域）|	你每帧提交一次|
|DockNode|	容器被切分出的节点树（左/右/上/下）+ 每个节点里停靠了哪些窗口|	由 imgui.ini 或 DockBuilder 决定|
|「预设」|	用代码把这棵树一次性切成固定形状	ImGui| 本身不自带，要自己写|

DockNode树，底层实际上是一棵二叉树
![](./1.png)

## 为什么需要预设
DockSpace 只是个空容器，窗口摆在哪由 DockNode 树决定；
这棵树默认只能靠用户手动拖 + ImGui 自动存 imgui.ini。
一旦 ini 丢失(新机器/首次运行/删了 ini)，所有面板就会散成浮动窗口。
BuildDefaultDockLayout函数把这棵树显式切成固定形状，等于一份"代码版预设"。