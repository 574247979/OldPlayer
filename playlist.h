#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>
#include <QList>
#include <QUrl>
#include <QRegularExpression>

struct Song {
    QString title;
    QString artist;
    QString album;      // <--- 在这里添加 album 字段
    QString filePath;
    qint64 duration;  // 毫秒
    
    Song(const QString& path = "") 
        : filePath(path), duration(0), artist("未知艺术家"), album("未知专辑") { // <--- 初始化新增字段
        // 从文件路径提取歌曲名（使用正确的分隔符）
        QStringList parts = path.split(QRegularExpression("[/\\\\]"));
        if (!parts.isEmpty()) {
            title = parts.last();
            title.replace(QRegularExpression("\\.(mp3|wav|ogg|flac|m4a)$", 
                         QRegularExpression::CaseInsensitiveOption), "");
        }
    }
};

class Playlist {
public:
    explicit Playlist(const QString& name = "新建列表");
    
    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    int songCount() const { return m_songs.size(); }
    const QList<Song>& getSongs() const { return m_songs; }
    Song getSong(int index) const;
    
    void addSong(const Song& song);
    void addSongs(const QList<Song>& songs);
    void removeSong(int index);
    void clear();

    void updateSongMetaData(int index, const QString& title, const QString& artist, const QString& album);
    


private:
    QString m_name;
    QList<Song> m_songs;
};

#endif // PLAYLIST_H
