#include "playlistlistwidget.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>

PlaylistListWidget::PlaylistListWidget(QWidget* parent)
    : QListWidget(parent)
{
    // 1. 允许控件接收拖放事件
    setAcceptDrops(true);
    // 2. 设置只接受放下，不支持从这里拖出
    setDragDropMode(QAbstractItemView::DropOnly);
}

void PlaylistListWidget::dragEnterEvent(QDragEnterEvent* event)
{
    // 当拖动进入时，检查 MimeData 中是否包含 URL (文件或文件夹路径)
    if (event->mimeData()->hasUrls()) {
        // 如果有，则接受该动作，光标会变为可拖放的样式
        event->acceptProposedAction();
    } else {
        // 否则，忽略该事件
        event->ignore();
    }
}

void PlaylistListWidget::dragMoveEvent(QDragMoveEvent* event)
{
    // 在控件上移动时，持续接受该动作
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void PlaylistListWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> folderUrls;
        const auto& urls = event->mimeData()->urls();

        // 遍历所有被拖放的条目
        for (const QUrl& url : urls) {
            // 确保是本地文件路径，并且是一个目录
            if (url.isLocalFile()) {
                QFileInfo fileInfo(url.toLocalFile());
                if (fileInfo.isDir()) {
                    folderUrls.append(url);
                }
            }
        }

        // 如果找到了至少一个文件夹，就发射信号
        if (!folderUrls.isEmpty()) {
            emit foldersDropped(folderUrls);
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}