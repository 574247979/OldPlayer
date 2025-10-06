#include "playlistmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug> // 用于调试输出

PlaylistManager::PlaylistManager(QObject* parent) : QObject(parent) {
    // 1. 确定配置文件的存储路径
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_configFilePath = dataPath + "/playlists.json";
    qDebug() << "配置文件路径：" << m_configFilePath;

    // 2. 加载播放列表
    loadPlaylists();

    // 3. 如果加载后没有任何播放列表，则创建一个默认的
    if (m_playlists.isEmpty()) {
        m_playlists.append(new Playlist("我喜欢"));
    }
}

// 实现析构函数，在对象销毁时自动保存
PlaylistManager::~PlaylistManager() {
    savePlaylists();

    // 释放内存
    qDeleteAll(m_playlists);
    m_playlists.clear();
}

void PlaylistManager::loadPlaylists() {
    QFile configFile(m_configFilePath);
    if (!configFile.exists() || !configFile.open(QIODevice::ReadOnly)) {
        qWarning("无法打开播放列表文件，将创建新的列表。");
        return;
    }

    QByteArray saveData = configFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    
    // 确保顶层是一个 JSON 数组
    if (!loadDoc.isArray()) {
        qWarning("播放列表文件格式错误。");
        return;
    }
    
    QJsonArray playlistsArray = loadDoc.array();

    for (const QJsonValue& playlistValue : playlistsArray) {
        QJsonObject playlistObject = playlistValue.toObject();
        
        QString playlistName = playlistObject["name"].toString();
        Playlist* newPlaylist = new Playlist(playlistName);
        
        QJsonArray songsArray = playlistObject["songs"].toArray();
        for (const QJsonValue& songValue : songsArray) {
            QJsonObject songObject = songValue.toObject();
            
            // 创建 Song 对象并填充数据
            Song newSong(songObject["filePath"].toString());
            newSong.title = songObject["title"].toString();
            newSong.artist = songObject["artist"].toString();
            
            newPlaylist->addSong(newSong);
        }
        m_playlists.append(newPlaylist);
    }
    
    qDebug() << "成功加载" << m_playlists.size() << "个播放列表。";
}

void PlaylistManager::savePlaylists() const {
    QJsonArray playlistsArray;
    
    for (const Playlist* playlist : m_playlists) {
        QJsonObject playlistObject;
        playlistObject["name"] = playlist->getName();
        
        QJsonArray songsArray;
        for (const Song& song : playlist->getSongs()) {
            QJsonObject songObject;
            songObject["filePath"] = song.filePath;
            songObject["title"] = song.title; // 保存标题
            songObject["artist"] = song.artist; // 保存艺术家
            songsArray.append(songObject);
        }
        
        playlistObject["songs"] = songsArray;
        playlistsArray.append(playlistObject);
    }
    
    QJsonDocument saveDoc(playlistsArray);
    
    QFile configFile(m_configFilePath);
    if (!configFile.open(QIODevice::WriteOnly)) {
        qWarning("无法写入播放列表文件！");
        return;
    }
    
    configFile.write(saveDoc.toJson());
    qDebug() << "播放列表已成功保存到" << m_configFilePath;
}

Playlist* PlaylistManager::getPlaylist(int index) {
    if (index >= 0 && index < m_playlists.size()) {
        return m_playlists[index];
    }
    return nullptr;
}

void PlaylistManager::addPlaylist(const QString& name) {
    m_playlists.append(new Playlist(name));
    emit playlistAdded(m_playlists.size() - 1);
}

void PlaylistManager::removePlaylist(int index) {
    if (index >= 0 && index < m_playlists.size() && m_playlists.size() > 1) {
        delete m_playlists[index];
        m_playlists.removeAt(index);
        emit playlistRemoved(index);
    }
}