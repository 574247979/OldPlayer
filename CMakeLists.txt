cmake_minimum_required(VERSION 3.16)
project(OldPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置 UTF-8 编码
if(MSVC)
    add_compile_options(/utf-8)
elseif(MINGW)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 添加 Qt6 模块
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia Core5Compat Network)

# ==========================================
# TagLib 路径配置
# ==========================================
set(TAGLIB_DIR "${CMAKE_SOURCE_DIR}/libs/taglib")
message(STATUS "TagLib Include Path: ${TAGLIB_DIR}/include")
message(STATUS "TagLib Lib Path: ${TAGLIB_DIR}/lib")

add_executable(OldPlayer
    main.cpp
    mainwindow.cpp
    mainwindow.h
    playlist.cpp
    playlist.h
    playlistmanager.cpp
    playlistmanager.h
    playlistlistwidget.cpp
    playlistlistwidget.h
    songlistwidget.cpp
    songlistwidget.h
    customtimedialog.cpp
    customtimedialog.h
    fontsettingsdialog.cpp
    fontsettingsdialog.h
    transcodedialog.cpp
    transcodedialog.h
    songinfodialog.cpp
    songinfodialog.h
    resources.qrc
    singleapplication.h
    singleapplication.cpp
)

# 包含 Windows 图标资源
if(WIN32)
    target_sources(OldPlayer PRIVATE appicon.rc)
endif()

# ==========================================
# 链接与包含配置 (核心修改部分)
# ==========================================

# 1. 添加头文件搜索路径 (强制添加，不再判断 if)
# 这样编译器就能找到 libs/taglib/include/taglib/fileref.h
target_include_directories(OldPlayer PRIVATE 
    "${TAGLIB_DIR}/include"
)

# 2. 添加库文件搜索路径
target_link_directories(OldPlayer PRIVATE 
    "${TAGLIB_DIR}/lib"
)

# 3. 链接 Qt6 库
target_link_libraries(OldPlayer PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Core5Compat
    Qt6::Network
)

# 4. 链接 TagLib 库
# 因为你已经把文件重命名为 tag.lib，所以这里写 tag 即可
# 无论是否找到，都尝试链接，报错比静默失败更好排查
if(WIN32)
    target_link_libraries(OldPlayer PRIVATE tag)
else()
    # Linux/Mac 备用
    target_link_libraries(OldPlayer PRIVATE "${TAGLIB_DIR}/lib/libtag.so")
endif()

# Windows 平台设置 (去除黑框等)
if(WIN32)
    set_target_properties(OldPlayer PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()