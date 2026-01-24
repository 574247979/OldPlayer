#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QShowEvent>
#include <QDateTime>
#include "playlistmanager.h"
#include "playlistlistwidget.h"
#include "songlistwidget.h"
#include "transcodedialog.h"
#include "songinfodialog.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


// 自定义 Slider 类，支持点击跳转
class ClickableSlider : public QSlider {
    Q_OBJECT
public:
    explicit ClickableSlider(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QSlider(orientation, parent) {}

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            int value;
            if (orientation() == Qt::Horizontal) {
                double pos = event->position().x() / (double)width();
                value = pos * (maximum() - minimum()) + minimum();
            } else {
                double pos = event->position().y() / (double)height();
                value = pos * (maximum() - minimum()) + minimum();
            }
            setValue(value);
            emit sliderMoved(value);
            emit sliderPressed();
        }
        QSlider::mousePressEvent(event);
    }
};

enum class InListMode {
    Sequential,     // 顺序播放
    Random          // 随机播放
};

enum class CrossListMode {
    Stop,           // 播完当前列表后停止
    ListLoop,       // 循环当前列表
    SingleLoop,     // 循环当前单曲
    Advance         // 前进到下一个列表
};

class QSplitter;

class QProcess;
class QTimer;

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void wakeUpWindow(); 

protected:
    //closeEvent 函数声明
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    
private slots:
    void onPlayPauseClicked();
    void onPreviousClicked();
    void onNextClicked();
    void onPlaylistContextMenuRequested(const QPoint& pos);
    void onSongListContextMenuRequested(const QPoint& pos);
    void onAddSongsClicked();
    void onCreatePlaylistClicked();
    void onDeletePlaylistClicked();
    void onDeleteSongClicked();
    void onDeleteSongFromDiskClicked(); // 从磁盘删除歌曲
    void onPlaylistSelectionChanged();
    void onSongDoubleClicked(QListWidgetItem* item);
    void onFoldersDropped(const QList<QUrl>& urls);
    void onFilesDroppedToSongList(const QList<QUrl>& urls);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowFontSettings();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onProgressSliderMoved(int position);
    void onVolumeChanged(int value);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onMetaDataChanged();
    void onInListModeClicked();
    void onCrossListModeClicked();
    //定时关机功能
    void onShutdownTimerTimeout();       // 定时器触发时调用
    void onSetShutdownAfter30Mins();     // "30分钟后" 菜单项
    void onSetShutdownAfter60Mins();     // "60分钟后" 菜单项
    void onSetCustomShutdownTime();      // "自定义时间" 菜单项
    void onCancelShutdown();             // "取消定时" 菜单项

    void onSortPlaylistsAction(); // 左侧：排序播放列表
    void onSortSongsAction();     // 右侧：排序歌曲
    void onTranscodeAudioClicked(); // 音频转码
    void onEditSongInfoClicked();   // 编辑歌曲信息
    
private:
    void setupUI();
    void updatePlaylistView();
    void updateSongListView();
    void playSong(int index);
    void updatePlayPauseButton();
    void resetPlayerState();
    void playNextSong();             //处理“下一曲”逻辑的辅助函数
    void generateShuffledPlaylist();
    void updateInListModeButton();
    void updateCrossListModeButton();
    bool m_isFirstShow;
    void startShutdownTimer(int msecs); 
    QString formatTime(qint64 milliseconds);
    
    // UI 组件
    QWidget* m_centralWidget;
    QPushButton* m_inListModeBtn;   // 用于 顺序/随机
    QPushButton* m_playPauseBtn;
    QPushButton* m_previousBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_crossListModeBtn;  // 用于 列表循环/单曲循环/列表前进
    
    ClickableSlider* m_progressSlider;
    ClickableSlider* m_volumeSlider;
    QLabel* m_currentTimeLabel;
    QLabel* m_totalTimeLabel;
    QLabel* m_songTitleLabel;
    QLabel* m_songArtistLabel;

    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayMenu;
    QAction* m_restoreAction;
    QAction* m_quitAction;
    
    //成员变量存储高亮字体
    QFont m_playingSongFont;
    
    // 播放器
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QSplitter* m_mainSplitter; // <--- 4. 将 Splitter 声明为成员变量
    PlaylistManager* m_playlistManager;


    //定时关机相关的成员变量
    QTimer* m_shutdownTimer;
    QProcess* m_shutdownProcess;
    QDateTime m_shutdownDateTime;      // 用于存储关机时间，方便UI显示
    QAction* m_cancelShutdownAction;   // 用于方便地启用/禁用“取消”菜单项

    

    PlaylistListWidget* m_playlistListWidget;
    SongListWidget* m_songListWidget;
    
    int m_currentPlaylistIndex;
    int m_currentSongIndex;

    int m_playingPlaylistIndex; 

    InListMode m_inListMode;     // <-- 新的模式变量
    CrossListMode m_crossListMode; // <-- 新的模式变量


    QList<int> m_shuffledIndices;      //用于存储打乱后的歌曲索引
    int m_shuffledPlaybackIndex;       //当前在随机列表中的播放位置
};

#endif // MAINWINDOW_H
