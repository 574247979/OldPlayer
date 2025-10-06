# OldPlayer - 一个基于 Qt6 的现代桌面音乐播放器

这是一个使用 C++ 和 Qt6 框架构建的功能丰富的桌面音乐播放器。它的灵感来源于经典的桌面播放器，拥有简洁的界面和强大的功能。
<img width="430" height="575" alt="image" src="https://github.com/user-attachments/assets/d9d62d94-7e16-4371-8f18-36bd8f4c5960" />


A feature-rich desktop music player built with C++ and the Qt6 framework. Inspired by classic desktop players, it features a clean interface and powerful functionality.

## ✨ 主要功能 (Features)

*   **核心播放功能**: 支持播放、暂停、上一曲、下一曲，以及精确的进度条和音量控制。
*   **多种播放模式**:
    *   顺序播放 (Sequential)
    *   列表循环 (List Loop)
    *   单曲循环 (Single Loop)
    *   随机播放 (Random)
*   **强大的播放列表管理**:
    *   支持创建和管理多个播放列表。
    *   通过右键菜单轻松创建和删除播放列表。
    *   通过右键菜单或拖放文件来添加和删除歌曲。
*   **便捷的拖放操作**:
    *   直接拖动**音乐文件**到歌曲列表以快速添加。
    *   直接拖动**文件夹**到播放列表区域，自动以文件夹名创建新列表并导入所有音乐。
*   **持久化存储**:
    *   自动保存所有播放列表和歌曲，下次打开时无需重新设置。
    *   自动记忆窗口的大小、位置以及布局分割状态。
*   **系统托盘集成**:
    *   拥有自定义的应用程序图标。
    *   点击关闭按钮时最小化到系统托盘，而不是退出程序。
    *   通过托盘图标可快速显示/隐藏窗口或退出程序。
*   **现代化界面**:
    *   采用美观的深色主题。
    *   自适应布局，窗口大小改变时，列表区域会自动伸缩。

## 🛠️ 技术栈 (Tech Stack)

*   **框架 (Framework)**: Qt 6
*   **语言 (Language)**: C++17
*   **构建系统 (Build System)**: CMake
*   **多媒体后端 (Multimedia Backend)**: FFmpeg (通过 Qt Multimedia 模块)

## 🚀 如何从源码构建 (Building from Source)

### 先决条件 (Prerequisites)

*   [Qt 6](https://www.qt.io/download) (本项目使用 MinGW 编译器套件)
*   支持 C++17 的编译器 (如 GCC/MinGW)
*   CMake

### 构建步骤 (Build Steps)

#### 1. 使用 Qt Creator (推荐)

1.  下载或克隆本仓库: `git clone https://github.com/YourUsername/MusicPlayer.git`
2.  使用 Qt Creator 打开项目根目录下的 `CMakeLists.txt` 文件。
3.  Qt Creator 会自动配置项目，选择你的构建套件 (Kit)。
4.  在左下角将构建模式从 "Debug" 切换到 **"Release"**。
5.  点击“构建”或“运行”按钮。

#### 2. 使用命令行

```bash
# 克隆仓库
git clone https://github.com/YourUsername/MusicPlayer.git
cd MusicPlayer

# 创建构建目录
mkdir build && cd build

# 运行 CMake 配置 (请将 D:/Qt/6.x.x/mingw_64 替换为你自己的 Qt 安装路径)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=D:/Qt/6.x.x/mingw_64

# 编译项目
cmake --build .
```
编译完成后，可执行文件将位于 `build` 目录下。

## 📦 打包与部署 (Deployment)

本项目使用 Qt 官方工具 `windeployqt` 进行打包。

1.  首先，构建 **Release** 版本的程序。
2.  打开 Qt 提供的专用命令行工具 (e.g., "Qt 6.x.x for Desktop (MinGW ...)")。
3.  进入包含 `MusicPlayer.exe` 的 Release 输出目录。
4.  运行命令:
    ```bash
    windeployqt MusicPlayer.exe
    ```
5.  该工具会自动将所有必需的 `.dll` 文件和插件复制到当前目录。
6.  将整个文件夹压缩后即可分发给他人使用。

## 📜 开源许可 (License)

本项目采用 [MIT License](LICENSE.txt) 开源许可。

---
