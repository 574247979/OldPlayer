#ifndef PLAYLISTLISTWIDGET_H
#define PLAYLISTLISTWIDGET_H

#include <QListWidget>
#include <QUrl>

class PlaylistListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit PlaylistListWidget(QWidget* parent = nullptr);

signals:
    // 当一个或多个文件夹被成功拖放时，发射此信号
    void foldersDropped(const QList<QUrl>& urls);

protected:
    // 重写 QWidget 的事件处理函数来处理拖放
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

#endif // PLAYLISTLISTWIDGET_H