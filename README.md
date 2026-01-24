# OldPlayer - 一个基于 Qt6 的现代桌面音乐播放器

<img width="430" height="575" alt="OldPlayer Screenshot" src="https://github.com/user-attachments/assets/d9d62d94-7e16-4371-8f18-36bd8f4c5960" />

这是一个使用 C++ 和 Qt6 框架构建的功能丰富的桌面音乐播放器。它的灵感来源于经典的桌面播放器，拥有简洁的界面和强大的功能。

A feature-rich desktop music player built with C++ and the Qt6 framework. Inspired by classic desktop players, it features a clean interface and powerful functionality.

---

## ✨ 主要功能

### 🎵 播放控制
*   **多种播放模式**:
    *   列表内模式: 顺序播放 / 随机播放
    *   跨列表模式: 列表循环 / 单曲循环 / 列表前进 / 播完停止
*   **便捷操作**: 点击进度条可直接跳转到指定位置
*   **音量控制**: 滑动调节，带百分比提示

### 📋 播放列表管理
*   支持创建和管理多个播放列表
*   通过右键菜单轻松创建、删除、排序播放列表
*   通过右键菜单添加、删除、排序歌曲
*   **从磁盘删除**: 支持直接从磁盘删除歌曲文件（带确认提示）
*   播放列表数据自动持久化保存

### 📁 便捷的拖放操作
*   直接拖动**音乐文件**到歌曲列表以快速添加
*   直接拖动**文件夹**到播放列表区域，自动以文件夹名创建新列表并递归导入所有音乐

### 🎨 现代化界面
*   采用美观的深色主题 (Nord 配色风格)
*   支持自定义列表字体设置
*   可调节左右分栏宽度
*   当前播放歌曲高亮显示

### 🔧 系统集成
*   **系统托盘**: 关闭窗口后最小化到系统托盘继续运行
*   **单实例运行**: 防止程序重复启动，重复打开会唤醒已运行的窗口
*   **睡眠恢复**: 系统从睡眠唤醒后自动恢复播放状态
*   **状态记忆**: 记住上次播放的列表和歌曲位置

### ⏰ 定时关机
*   预设 30 分钟 / 60 分钟 后关机
*   支持自定义关机时间
*   可随时取消定时

### 📂 支持的音频格式
*   MP3, WAV, OGG, FLAC, M4A

---

## 🛠️ 技术栈

| 类别 | 技术 |
|------|------|
| **框架** | Qt 6 |
| **语言** | C++17 |
| **构建系统** | CMake |
| **多媒体后端** | FFmpeg (通过 Qt Multimedia 模块) |
| **Qt 模块** | Core, Widgets, Multimedia, Network, Core5Compat |

---

## 🚀 如何从源码构建

### 先决条件

*   [Qt 6](https://www.qt.io/download) (本项目使用 MinGW 编译器套件)
*   支持 C++17 的编译器 (如 GCC/MinGW)
*   CMake

### 构建步骤

#### 1. 使用 Qt Creator (推荐)

1.  下载或克隆本仓库: `git clone https://github.com/574247979/OldPlayer.git`
2.  使用 Qt Creator 打开项目根目录下的 `CMakeLists.txt` 文件。
3.  Qt Creator 会自动配置项目，选择你的构建套件 (Kit)。
4.  在左下角将构建模式从 "Debug" 切换到 **"Release"**。
5.  点击"构建"或"运行"按钮。

#### 2. 使用命令行

```bash
# 克隆仓库
git clone https://github.com/574247979/OldPlayer.git
cd OldPlayer

# 创建构建目录
mkdir build && cd build

# 运行 CMake 配置 (请将 D:/Qt/6.x.x/mingw_64 替换为你自己的 Qt 安装路径)
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=D:/Qt/6.x.x/mingw_64

# 编译项目
cmake --build .
```

编译完成后，可执行文件将位于 `build` 目录下。

---

## 📦 打包与部署

本项目使用 Qt 官方工具 `windeployqt` 进行打包。

1.  首先，构建 **Release** 版本的程序。
2.  打开 Qt 提供的专用命令行工具 (e.g., "Qt 6.x.x for Desktop (MinGW ...)")。
3.  进入包含 `OldPlayer.exe` 的 Release 输出目录。
4.  运行命令:
    ```bash
    windeployqt OldPlayer.exe
    ```
5.  该工具会自动将所有必需的 `.dll` 文件和插件复制到当前目录。

---

## 📜 开源许可

本项目采用 [MIT License](LICENSE.txt) 开源许可。

---

## 🤝 贡献 (Contributing)

欢迎提交 Issue 和 Pull Request！

