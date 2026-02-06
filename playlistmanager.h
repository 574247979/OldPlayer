#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QList>
#include "playlist.h"

class PlaylistManager : public QObject {
    Q_OBJECT
    
public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager(); // <--- 添加析构函数声明
    
    int playlistCount() const { return m_playlists.size(); }
    Playlist* getPlaylist(int index);
    const QList<Playlist*>& getPlaylists() const { return m_playlists; }
    
    void addPlaylist(const QString& name);
    void removePlaylist(int index);
    // 对播放列表本身进行排序
    void sortPlaylistsByName();
    // 根据指针查找索引（用于排序后恢复状态）
    int getPlaylistIndex(Playlist* playlist);
    void savePlaylists() const;  // 保存播放列表到配置文件
    
signals:
    void playlistAdded(int index);
    void playlistRemoved(int index);
    
private:
    void loadPlaylists();       // <--- 添加加载函数声明

    QList<Playlist*> m_playlists;
    QString m_configFilePath;   // <--- 用于保存配置文件的路径
};

#endif // PLAYLISTMANAGER_H
