#include <QApplication>
#include "mainwindow.h"

// 包含 Windows 头文件（仅在 Windows 平台）
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
    // 设置应用程序属性，确保高 DPI 缩放正常
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    
    // 在 Windows 平台下，将控制台输出编码设置为 UTF-8
    #ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    // 这一行在 Qt 6 中已废弃且无效，建议移除
    // QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    
    MainWindow window;
    window.show();
    
    return app.exec();
}