#include "songlistwidget.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>

SongListWidget::SongListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
}

void SongListWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void SongListWidget::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void SongListWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> fileUrls;
        const auto& urls = event->mimeData()->urls();
        QStringList supportedExtensions = {"mp3", "flac", "wav", "ogg", "m4a"};

        for (const QUrl& url : urls) {
            if (url.isLocalFile()) {
                QFileInfo fileInfo(url.toLocalFile());
                // 确保它是一个文件，并且后缀名是我们支持的音频格式
                if (fileInfo.isFile() && supportedExtensions.contains(fileInfo.suffix().toLower())) {
                    fileUrls.append(url);
                }
            }
        }

        if (!fileUrls.isEmpty()) {
            emit filesDropped(fileUrls);
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}