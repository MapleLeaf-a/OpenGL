#pragma once

#include "GameObject.h"

namespace EditorPanel
{
    //cpp17 inline变量:header-only被多处include也只有一个实例。在 C++17 之前，如果你在头文件中定义一个全局变量（非 const 或 constexpr），当这个头文件被多个源文件包含时，链接器会报“多重定义”错误
    inline GameObject* selectedGO = nullptr;

    
}