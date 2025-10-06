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
#include "playlistmanager.h"
#include "playlistlistwidget.h"
#include "songlistwidget.h"


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

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    //closeEvent 函数声明
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    
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
    
private:
    void setupUI();
    void updatePlaylistView();
    void updateSongListView();
    void playSong(int index);
    void updatePlayPauseButton();
    void resetPlayerState();
    void playNextSong();             // <--- 4. 新增一个处理“下一曲”逻辑的辅助函数
    void generateShuffledPlaylist();
    void updateInListModeButton();
    void updateCrossListModeButton();
    bool m_isFirstShow;
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
    
    
    // 播放器
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QSplitter* m_mainSplitter; // <--- 4. 将 Splitter 声明为成员变量
    PlaylistManager* m_playlistManager;

    

    PlaylistListWidget* m_playlistListWidget;
    SongListWidget* m_songListWidget;
    
    int m_currentPlaylistIndex;
    int m_currentSongIndex;

    InListMode m_inListMode;     // <-- 新的模式变量
    CrossListMode m_crossListMode; // <-- 新的模式变量


    QList<int> m_shuffledIndices;      //用于存储打乱后的歌曲索引
    int m_shuffledPlaybackIndex;       //当前在随机列表中的播放位置
};

#endif // MAINWINDOW_H
