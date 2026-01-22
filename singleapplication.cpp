#include "singleapplication.h"
#include <QDebug>

SingleApplication::SingleApplication(int &argc, char *argv[], const QString uniqueKey)
    : QApplication(argc, argv), _uniqueKey(uniqueKey), _isRunning(false)
{
    _localServer = new QLocalServer(this);

    // 1. 尝试连接已存在的服务器
    QLocalSocket socket;
    socket.connectToServer(_uniqueKey);

    if (socket.waitForConnected(500)) {
        // 连接成功，说明已经有一个实例在运行
        _isRunning = true;
        // 关闭连接，我们稍后通过 sendMessage 发送指令
        socket.disconnectFromServer(); 
    } else {
        // 连接失败，说明我们是第一个实例
        _isRunning = false;
        
        // 2. 清理可能遗留的旧连接（防止上次崩溃导致的问题）
        QLocalServer::removeServer(_uniqueKey);

        // 3. 启动监听
        connect(_localServer, &QLocalServer::newConnection, this, &SingleApplication::receiveMessage);
        if (!_localServer->listen(_uniqueKey)) {
            // 如果监听失败（极少情况，例如权限问题），至少让程序能跑
            qWarning() << "Local server listen failed:" << _localServer->errorString();
        }
    }
}

bool SingleApplication::isRunning()
{
    return _isRunning;
}

bool SingleApplication::sendMessage(const QString &message)
{
    if (!_isRunning) return false;

    QLocalSocket socket;
    socket.connectToServer(_uniqueKey);
    if (socket.waitForConnected(1000)) {
        // 发送消息（例如 "WAKE_UP"）
        socket.write(message.toUtf8());
        socket.flush();
        socket.waitForBytesWritten(1000);
        return true;
    }
    return false;
}

void SingleApplication::receiveMessage()
{
    // 获取连接进来的 Socket
    QLocalSocket *socket = _localServer->nextPendingConnection();
    if (!socket) return;

    // 等待读取数据
    if (socket->waitForReadyRead(1000)) {
        QByteArray data = socket->readAll();
        QString message = QString::fromUtf8(data);

        // 如果收到唤醒指令
        if (message == "WAKE_UP") {
            qDebug() << "Received WAKE_UP signal.";
            emit showUp(); // 发射信号通知主窗口
        }
    }
    socket->deleteLater();
}