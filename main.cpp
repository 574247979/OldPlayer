#include <QApplication>
#include <QFile>
#include "mainwindow.h"

// 包含 Windows 头文件（仅在 Windows 平台）
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setOrganizationName("OldCheung"); // 你可以改成自己的名字
    QCoreApplication::setApplicationName("OldPlayer"); // 你的应用名

    QApplication app(argc, argv);
    
    #ifdef Q_OS_WIN
    SetConsoleOutputCP(CP_UTF8);
    #endif


    //添加深色主题样式表
    QString qss = R"(
        QWidget {
            background-color: #2E3440;
            color: #D8DEE9;
            font-family: "Microsoft YaHei";
        }
        QMainWindow {
            background-color: #3B4252;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #4C566A;
            border-radius: 5px;
            margin-top: 1ex;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 3px;
        }
        QListWidget {
            background-color: #2E3440;
            border: 1px solid #4C566A;
            padding: 5px;
        }
        QListWidget::item {
            padding: 5px;
        }
        QListWidget::item:selected {
            background-color: #88C0D0;
            color: #2E3440;
        }
        QPushButton {
            background-color: #4C566A;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #5E81AC;
        }
        QPushButton:pressed {
            background-color: #81A1C1;
        }
        QSlider::groove:horizontal {
            border: 1px solid #4C566A;
            height: 2px;
            background: #434C5E;
            margin: 2px 0;
            border-radius: 1px;
        }
        QSlider::handle:horizontal {
            background: #88C0D0;
            border: 1px solid #88C0D0;
            width: 10px;
            margin: -5px 0;
            border-radius: 5px;
        }
        QLabel {
            color: #ECEFF4;
        }
    )";
    app.setStyleSheet(qss);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}