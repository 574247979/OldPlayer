#include "playlist.h"
#include <QFileInfo>
#include <algorithm>

Playlist::Playlist(const QString& name) : m_name(name) {}

Song Playlist::getSong(int index) const {
    if (index >= 0 && index < m_songs.size()) {
        return m_songs[index];
    }
    return Song();
}

void Playlist::addSong(const Song& song) {
    m_songs.append(song);
}

void Playlist::addSongs(const QList<Song>& songs) {
    m_songs.append(songs);
}

void Playlist::removeSong(int index) {
    if (index >= 0 && index < m_songs.size()) {
        m_songs.removeAt(index);
    }
}

void Playlist::clear() {
    m_songs.clear();
}


void Playlist::updateSongMetaData(int index, const QString& title, const QString& artist, const QString& album) {
    if (index >= 0 && index < m_songs.size()) {
        if (!title.isEmpty()) {
            m_songs[index].title = title;
        }
        if (!artist.isEmpty()) {
            m_songs[index].artist = artist;
        }
        if (!album.isEmpty()) {
            m_songs[index].album = album;
        }
    }
}

//按名称排序函数
void Playlist::sortByName() {
    // 使用 std::sort 和 lambda 表达式进行排序
    std::sort(m_songs.begin(), m_songs.end(), [](const Song& a, const Song& b) {
        // localeAwareCompare 会根据系统区域设置进行比较
        // 对中文通常是拼音顺序，对英文是不区分大小写的
        return QString::localeAwareCompare(a.title, b.title) < 0;
    });
}