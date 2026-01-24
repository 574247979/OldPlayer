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

# 在这里添加 Core5Compat 模块
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia Core5Compat)

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
    resources.qrc
    singleapplication.h
    singleapplication.cpp
)

# ↓↓↓ 添加这个代码块来包含 Windows 图标资源 ↓↓↓
if(WIN32)
    target_sources(OldPlayer PRIVATE appicon.rc)
endif()


# 在这里链接 Qt6::Core5Compat 库
target_link_libraries(OldPlayer
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Core5Compat
    Qt6::Network
)

# Windows 平台设置
if(WIN32)
    set_target_properties(OldPlayer PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()