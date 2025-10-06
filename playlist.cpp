#include "playlist.h"
#include <QFileInfo>

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