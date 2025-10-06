#ifndef SONGLISTWIDGET_H
#define SONGLISTWIDGET_H

#include <QListWidget>
#include <QUrl>

class SongListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit SongListWidget(QWidget* parent = nullptr);

signals:
    // 当一个或多个文件被成功拖放时，发射此信号
    void filesDropped(const QList<QUrl>& urls);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

#endif // SONGLISTWIDGET_H