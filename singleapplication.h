#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int &argc, char *argv[], const QString uniqueKey);

    // 检查是否已经有实例在运行
    bool isRunning();
    
    // 如果已经运行，发送消息给主实例
    bool sendMessage(const QString &message);

public slots:
    // 处理新连接的槽函数
    void receiveMessage();

signals:
    // 当收到唤醒消息时，发射此信号
    void showUp();

private:
    bool _isRunning;
    QString _uniqueKey;
    QLocalServer *_localServer;
};

#endif // SINGLEAPPLICATION_H