# ============================================================
#  OpenGL 项目 CMake 构建脚本
# ------------------------------------------------------------
#  用法（项目根目录 D:/OpenGL 下）：
#    cmake -S . -B build                       # 配置
#    cmake --build build --config Release      # 编译 Release
#    cmake --build build --config Debug        # 编译 Debug
#
#  运行：
#    .\build\Release\OpenGL.exe
#
#  特性：
#    - 自动递归收集 OpenGL/Scripts 下所有 .cpp / .h / .hpp
#    - 自动把含头文件的子目录加入 include 搜索路径
#    - 自动过滤掉与标准库头冲突的目录名（如 Time → <time.h>）
#    - 自动跳过 vendor/glm 下的 .cpp（glm 是 header-only，自带的 .cpp 可能
#      引入 modules 相关 bug）
#    - CONFIGURE_DEPENDS：改动文件树后下次 build 自动重扫
# ============================================================

cmake_minimum_required(VERSION 3.20)
project(OpenGL LANGUAGES CXX)

# ---------- 全局设置 ----------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

if(MSVC)
    add_compile_options(/utf-8 /MP /W3 /EHsc)
endif()

# ---------- 源文件自动收集 ----------
set(SRC_ROOT ${CMAKE_SOURCE_DIR}/OpenGL/Scripts)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SRC_ROOT}/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${SRC_ROOT}/*.h"
    "${SRC_ROOT}/*.hpp"
)

# glm 自带 glm.cpp / glm.cppm（模块接口单元），header-only 用法下不应参与编译，
# 在新版 MSVC 上还会触发模块与 <ctime> 的 ABI 冲突 → 剔除
list(FILTER SOURCES EXCLUDE REGEX ".*/vendor/glm/.*")

# ---------- 自动收集 include 目录 ----------
set(INCLUDE_DIRS "")
foreach(HDR ${HEADERS})
    get_filename_component(DIR ${HDR} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# 显式加入 SRC_ROOT 和 vendor，让 "glm/..." / "imgui/..." / "stb_image/..." 可解析
list(APPEND INCLUDE_DIRS ${SRC_ROOT} ${SRC_ROOT}/vendor)
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# 过滤与 C/C++ 标准头同名的目录（Windows 文件系统大小写不敏感，
# 这些名字会劫持 <xxx> 系统头的解析，产生灵异错误）
set(STD_HEADER_NAMES
    # C 头
    assert complex ctype errno fenv float inttypes iso646 limits locale math
    setjmp signal stdalign stdarg stdatomic stdbit stdbool stdckdint stddef
    stdint stdio stdlib stdnoreturn string tgmath threads time uchar wchar wctype
    # C++ 常见头
    algorithm any array atomic barrier bit bitset charconv chrono codecvt
    compare concepts condition_variable coroutine deque exception execution
    expected filesystem flat_map flat_set format forward_list fstream functional
    future generator hazard_pointer initializer_list iomanip ios iosfwd iostream
    istream iterator latch list locale map mdspan memory memory_resource mutex
    new numbers numeric optional ostream print queue random ranges ratio rcu
    regex scoped_allocator semaphore set shared_mutex source_location span
    spanstream sstream stack stacktrace stdexcept stdfloat stop_token streambuf
    string string_view strstream syncstream system_error text_encoding thread
    tuple type_traits typeindex typeinfo unordered_map unordered_set utility
    valarray variant vector version
)
set(FILTERED_INCLUDE_DIRS "")
foreach(DIR ${INCLUDE_DIRS})
    get_filename_component(DIR_NAME ${DIR} NAME)
    string(TOLOWER "${DIR_NAME}" DIR_NAME_LC)
    if(DIR_NAME_LC IN_LIST STD_HEADER_NAMES)
        message(STATUS "[include 过滤] 跳过与标准头冲突的目录: ${DIR}")
    else()
        list(APPEND FILTERED_INCLUDE_DIRS ${DIR})
    endif()
endforeach()
set(INCLUDE_DIRS ${FILTERED_INCLUDE_DIRS})

# ---------- 目标 ----------
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE OFF
    VS_STARTUP_PROJECT ${PROJECT_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/OpenGL/Scripts
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)
endif()

source_group(TREE ${SRC_ROOT} FILES ${SOURCES} ${HEADERS})

# ---------- 自动切换 CWD，解耦可执行文件和启动目录 ----------
# 代码里用 "../Resources/..." 这种相对路径，依赖 cwd = OpenGL/Scripts。
# 生成一个小的 bootstrap 源文件，在程序启动前 chdir 到那里。
# 这样从任意目录运行 exe 都能找到资源。
set(APP_WORKING_DIR "${CMAKE_SOURCE_DIR}/OpenGL/Scripts")
set(_BOOTSTRAP_CPP ${CMAKE_BINARY_DIR}/_cwd_bootstrap.cpp)
file(WRITE ${_BOOTSTRAP_CPP}
"// 由 CMake 自动生成，启动时切 CWD，使 \"../Resources/...\" 这类相对路径生效\n"
"#if defined(_WIN32)\n"
"  #include <direct.h>\n"
"  #define APP_CHDIR _chdir\n"
"#else\n"
"  #include <unistd.h>\n"
"  #define APP_CHDIR chdir\n"
"#endif\n"
"#ifndef APP_WORKING_DIR\n"
"  #define APP_WORKING_DIR \".\"\n"
"#endif\n"
"namespace {\n"
"  struct _CwdInit { _CwdInit() { APP_CHDIR(APP_WORKING_DIR); } } _cwdInit;\n"
"}\n"
)
target_sources(${PROJECT_NAME} PRIVATE ${_BOOTSTRAP_CPP})
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_WORKING_DIR="${APP_WORKING_DIR}"
)

# ---------- 第三方依赖 ----------
# Windows：使用仓库自带的 Dependencies/*.lib（MSVC 预编译）
# Linux / macOS：使用系统 find_package（通过包管理器安装 glfw / glew / assimp）
set(DEPS_DIR ${CMAKE_SOURCE_DIR}/Dependencies)

if(WIN32)
    add_library(dep_glfw STATIC IMPORTED)
    set_target_properties(dep_glfw PROPERTIES
        IMPORTED_LOCATION ${DEPS_DIR}/GLFW/lib-vc2022/glfw3.lib
        INTERFACE_INCLUDE_DIRECTORIES ${DEPS_DIR}/GLFW/include
    )

    add_library(dep_glew STATIC IMPORTED)
    set_target_properties(dep_glew PROPERTIES
        IMPORTED_LOCATION ${DEPS_DIR}/GLEW/lib/Release/x64/glew32s.lib
        INTERFACE_INCLUDE_DIRECTORIES ${DEPS_DIR}/GLEW/include
    )

    add_library(dep_assimp STATIC IMPORTED)
    set_target_properties(dep_assimp PROPERTIES
        IMPORTED_LOCATION ${DEPS_DIR}/assimp/lib-vc2022/assimp.lib
        INTERFACE_INCLUDE_DIRECTORIES ${DEPS_DIR}/assimp/include
    )

    add_library(dep_zlib STATIC IMPORTED)
    set_target_properties(dep_zlib PROPERTIES
        IMPORTED_LOCATION ${DEPS_DIR}/assimp/lib-vc2022/zlibstatic.lib
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        dep_glfw dep_glew dep_assimp dep_zlib
        opengl32 user32 gdi32 shell32
    )
else()
    # Linux: sudo apt install libglfw3-dev libglew-dev libassimp-dev
    # macOS: brew install glfw glew assimp
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(assimp REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        glfw
        GLEW::GLEW
        assimp::assimp
    )

    if(UNIX AND NOT APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS} pthread)
    endif()
endif()

# ---------- 配置摘要 ----------
list(LENGTH SOURCES _n_src)
list(LENGTH HEADERS _n_hdr)
list(LENGTH INCLUDE_DIRS _n_inc)
message(STATUS "----- OpenGL build summary -----")
message(STATUS "  .cpp 数        : ${_n_src}")
message(STATUS "  头文件数       : ${_n_hdr}")
message(STATUS "  include 目录数 : ${_n_inc}")
message(STATUS "--------------------------------")
