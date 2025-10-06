#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QStyle>
#include <QMediaMetaData>
#include <QSplitter>
#include <random>     // <--- 包含用于生成高质量随机数的头文件
#include <algorithm>  // <--- 包含用于 shuffle 算法的头文件
#include <QRandomGenerator> // <--- Qt 6 的随机数生成器
#include <QDirIterator>
#include <QDebug>
#include <QMenu>
#include <QSettings>   //包含 QSettings 头文件
#include <QSplitter>   //包含 QSplitter 的完整头文件
#include <QAction>   // <--- 包含 QAction
#include <QApplication> // <--- 包含 QApplication 以便调用 quit
#include <QToolTip> // <--- 1. 包含 QToolTip 头文件
#include <QFontDialog>
#include <QFont>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_currentPlaylistIndex(0)
    , m_currentSongIndex(-1)
    , m_inListMode(InListMode::Sequential)
    , m_crossListMode(CrossListMode::ListLoop)
    , m_shuffledPlaybackIndex(0)             // 初始化随机播放索引
    , m_isFirstShow(true)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    
    m_playlistManager = new PlaylistManager(this);
    
    setupUI();
    
    // 连接播放器信号
    connect(m_player, &QMediaPlayer::positionChanged, 
            this, &MainWindow::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, 
            this, &MainWindow::onDurationChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::metaDataChanged,
            this, &MainWindow::onMetaDataChanged); // <--- 连接新信号
    
    updatePlaylistView();
    updateSongListView();
    updateInListModeButton();   // 在启动时更新按钮状态
    updateCrossListModeButton(); // 在启动时更新按钮状态

    // 1. 设置窗口图标（这将显示在标题栏和任务栏）
    //    我们使用 Qt 资源系统中的路径
    setWindowIcon(QIcon(":/icons/appicon.ico"));

    // 2. 创建托盘图标的右键菜单
    m_restoreAction = new QAction("显示/隐藏", this);
    connect(m_restoreAction, &QAction::triggered, this, [this]() {
        setVisible(!isVisible()); // 切换窗口的可见性
    });

    QAction* fontAction = new QAction("字体...", this);
    connect(fontAction, &QAction::triggered, this, &MainWindow::onShowFontSettings);

    QMenu* settingsMenu = new QMenu("设置", this);
    settingsMenu->addAction(fontAction);
    
    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit); 

    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(m_restoreAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addMenu(settingsMenu); // <--- 将“设置”子菜单添加到主菜单
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    // 3. 创建系统托盘图标对象
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/appicon.ico"));
    m_trayIcon->setToolTip("MusicPlayer");
    m_trayIcon->setContextMenu(m_trayMenu); // <-- 将我们正确创建的菜单设置给托盘

    // 4. 连接托盘图标的点击信号
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    
    // 5. 显示托盘图标
    m_trayIcon->show();

    // 6. 加载设置
    QSettings settings;
    if (settings.contains("geometry")) {
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    if (settings.contains("splitterState")) {
        m_mainSplitter->restoreState(settings.value("splitterState").toByteArray());
    }

    QFont defaultListFont = m_playlistListWidget->font();

    //从设置中加载字体，如果不存在则使用默认值
    QFont savedFont = settings.value("listFont", defaultListFont).value<QFont>();
    
    //将加载的字体分别设置给两个列表控件
    m_playlistListWidget->setFont(savedFont);
    m_songListWidget->setFont(savedFont);

    m_volumeSlider->setValue(settings.value("volume", 70).toInt());
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("OldPlayer");
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(m_centralWidget);
    
    // 顶部：当前播放信息
    QGroupBox* infoGroup = new QGroupBox("正在播放", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    
    m_songTitleLabel = new QLabel("未选择歌曲", this);
    m_songTitleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    m_songTitleLabel->setAlignment(Qt::AlignCenter);
    
    m_songArtistLabel = new QLabel("", this);
    m_songArtistLabel->setStyleSheet("font-size: 12px; color: gray;");
    m_songArtistLabel->setAlignment(Qt::AlignCenter);
    
    infoLayout->addWidget(m_songTitleLabel);
    infoLayout->addWidget(m_songArtistLabel);
    mainLayout->addWidget(infoGroup);
    
    // 进度条区域
    QHBoxLayout* progressLayout = new QHBoxLayout();
    m_currentTimeLabel = new QLabel("00:00", this);
    m_progressSlider = new ClickableSlider(Qt::Horizontal, this);
    m_totalTimeLabel = new QLabel("00:00", this);
    
    connect(m_progressSlider, &ClickableSlider::sliderMoved,
            this, &MainWindow::onProgressSliderMoved);
    
    progressLayout->addWidget(m_currentTimeLabel);
    progressLayout->addWidget(m_progressSlider);
    progressLayout->addWidget(m_totalTimeLabel);
    mainLayout->addLayout(progressLayout);
    
    // 控制按钮区域
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addStretch();
    
    m_previousBtn = new QPushButton(this);
    m_previousBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    m_previousBtn->setIconSize(QSize(32, 32));
    m_previousBtn->setFixedSize(50, 50);
    connect(m_previousBtn, &QPushButton::clicked, 
            this, &MainWindow::onPreviousClicked);
    
    m_playPauseBtn = new QPushButton(this);
    m_playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playPauseBtn->setIconSize(QSize(32, 32));
    m_playPauseBtn->setFixedSize(50, 50);
    connect(m_playPauseBtn, &QPushButton::clicked, 
            this, &MainWindow::onPlayPauseClicked);
    
    m_nextBtn = new QPushButton(this);
    m_nextBtn->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    m_nextBtn->setIconSize(QSize(32, 32));
    m_nextBtn->setFixedSize(50, 50);
    connect(m_nextBtn, &QPushButton::clicked, 
            this, &MainWindow::onNextClicked);
    
    // 创建【列表内模式】按钮 (顺序/随机)
    m_inListModeBtn = new QPushButton(this);
    m_inListModeBtn->setIconSize(QSize(28, 28));
    m_inListModeBtn->setFixedSize(50, 50);
    connect(m_inListModeBtn, &QPushButton::clicked,
            this, &MainWindow::onInListModeClicked);

    // 创建【列表间模式】按钮 (循环/前进等)
    m_crossListModeBtn = new QPushButton(this);
    m_crossListModeBtn->setIconSize(QSize(28, 28));
    m_crossListModeBtn->setFixedSize(50, 50);
    connect(m_crossListModeBtn, &QPushButton::clicked,
            this, &MainWindow::onCrossListModeClicked);


    controlLayout->addWidget(m_previousBtn);
    controlLayout->addWidget(m_playPauseBtn);
    controlLayout->addWidget(m_nextBtn);
    controlLayout->addWidget(m_inListModeBtn);
    controlLayout->addWidget(m_crossListModeBtn);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);
    
    // 音量控制
    QHBoxLayout* volumeLayout = new QHBoxLayout();
    volumeLayout->addStretch();
    QLabel* volumeLabel = new QLabel("音量:", this);
    m_volumeSlider = new ClickableSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setMaximumWidth(150);
    connect(m_volumeSlider, &ClickableSlider::valueChanged,
            this, &MainWindow::onVolumeChanged);
    
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(m_volumeSlider);
    volumeLayout->addStretch();
    mainLayout->addLayout(volumeLayout);
    

    // 使用 QSplitter 来创建可拖动的左右两个面板
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setChildrenCollapsible(false);

    // --- 左侧面板：播放列表 ---
    QWidget* leftPanel = new QWidget(m_mainSplitter); 
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0); // 无边距

    QGroupBox* playlistGroup = new QGroupBox("播放列表", this);
    QVBoxLayout* playlistGroupLayout = new QVBoxLayout(playlistGroup);
    
    m_playlistListWidget = new PlaylistListWidget(this);
    m_playlistListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_playlistListWidget, &QListWidget::currentRowChanged,
            this, &MainWindow::onPlaylistSelectionChanged);
    // ↓↓↓ 连接新信号到新槽 ↓↓↓
    connect(m_playlistListWidget, &PlaylistListWidget::foldersDropped,
            this, &MainWindow::onFoldersDropped);
    connect(m_playlistListWidget, &QWidget::customContextMenuRequested,
            this, &MainWindow::onPlaylistContextMenuRequested);
            
    playlistGroupLayout->addWidget(m_playlistListWidget);

    leftLayout->addWidget(playlistGroup);


    // --- 右侧面板：歌曲列表 ---
    QWidget* rightPanel = new QWidget(m_mainSplitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* songListGroup = new QGroupBox("歌曲列表", this);
    QVBoxLayout* songListLayout = new QVBoxLayout(songListGroup);

    m_songListWidget = new SongListWidget(this);
    m_songListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_songListWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::onSongDoubleClicked);
    // ↓↓↓ 连接新信号到新槽 ↓↓↓
    connect(m_songListWidget, &SongListWidget::filesDropped,
            this, &MainWindow::onFilesDroppedToSongList);
    connect(m_songListWidget, &QWidget::customContextMenuRequested,
            this, &MainWindow::onSongListContextMenuRequested);
    
    songListLayout->addWidget(m_songListWidget);
    rightLayout->addWidget(songListGroup);

    // 将左右面板添加到 Splitter
    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(rightPanel);
    m_mainSplitter->setStretchFactor(0, 1); // 左侧面板拉伸因子
    m_mainSplitter->setStretchFactor(1, 3); // 右侧面板拉伸因子 (使其更宽)

    // 将 Splitter 添加到主布局
    mainLayout->addWidget(m_mainSplitter);

    mainLayout->setStretch(mainLayout->indexOf(m_mainSplitter), 1);

    // 设置初始音量
    m_audioOutput->setVolume(0.7);
}

void MainWindow::onInListModeClicked() {
    if (m_inListMode == InListMode::Sequential) {
        m_inListMode = InListMode::Random;
        generateShuffledPlaylist(); // 切换到随机时生成列表
    } else {
        m_inListMode = InListMode::Sequential;
        m_shuffledIndices.clear(); // 离开随机时清空列表
    }
    updateInListModeButton();
}

void MainWindow::onCrossListModeClicked() {
    switch (m_crossListMode) {
        case CrossListMode::ListLoop:
            m_crossListMode = CrossListMode::SingleLoop;
            break;
        case CrossListMode::SingleLoop:
            m_crossListMode = CrossListMode::Advance;
            break;
        case CrossListMode::Advance:
            m_crossListMode = CrossListMode::ListLoop;
            break;
        // 'Stop' 模式不在此循环中，可以通过设置界面添加
        case CrossListMode::Stop:
            m_crossListMode = CrossListMode::ListLoop;
            break;
    }
    updateCrossListModeButton();
}


// ↓↓↓ 实现新的函数，根据当前模式更新按钮的UI ↓↓↓
void MainWindow::updateInListModeButton() {
    if (m_inListMode == InListMode::Sequential) {
    // 自定义图标
        m_inListModeBtn->setIcon(QIcon(":/icons/sequential.png"));
        m_inListModeBtn->setToolTip("顺序播放");
    } else {
    // 自定义图标
        m_inListModeBtn->setIcon(QIcon(":/icons/random.png"));
        m_inListModeBtn->setToolTip("随机播放");
    }
}

// ↓↓↓ 修改 updateCrossListModeButton 函数 ↓↓↓
void MainWindow::updateCrossListModeButton() {
    switch (m_crossListMode) {
        case CrossListMode::ListLoop:
            m_crossListModeBtn->setIcon(QIcon(":/icons/listloop.png"));
            m_crossListModeBtn->setToolTip("列表循环");
            break;
        case CrossListMode::SingleLoop:
            m_crossListModeBtn->setIcon(QIcon(":/icons/singleloop.png"));
            m_crossListModeBtn->setToolTip("单曲循环");
            break;
        case CrossListMode::Advance:
            m_crossListModeBtn->setIcon(QIcon(":/icons/advance.png"));
            m_crossListModeBtn->setToolTip("列表顺序");
            break;
        case CrossListMode::Stop:
            // 如果您也为 Stop 模式准备了图标
            // m_crossListModeBtn->setIcon(QIcon(":/icons/stop.png"));
            m_crossListModeBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
            m_crossListModeBtn->setToolTip("播完停止");
            break;
}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (this->isVisible() && m_trayIcon->isVisible()) {
        // 隐藏主窗口
        this->hide();
        // 忽略关闭事件，这样程序就不会退出
        event->ignore();
    }
    // 创建 QSettings 对象，它会自动找到我们在 main.cpp 中设置的路径
    QSettings settings;
    
    // 保存窗口的几何信息（位置和大小）
    settings.setValue("geometry", saveGeometry());
    
    // 保存 Splitter 的状态（两个面板的相对大小）
    settings.setValue("splitterState", m_mainSplitter->saveState());

    // 保存当前音量滑块的值
    settings.setValue("volume", m_volumeSlider->value());

    settings.setValue("lastPlaylistIndex", m_currentPlaylistIndex);
    settings.setValue("lastSongIndex", m_currentSongIndex);

    // 调用基类的 closeEvent，确保窗口能正常关闭
    QMainWindow::closeEvent(event);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 我们只关心单击（在Windows上是左键单击）
    switch (reason) {
        case QSystemTrayIcon::Trigger: // 单击
            // 切换窗口的可见性
            setVisible(!isVisible());
            break;
        default:
            break;
    }
}

void MainWindow::updatePlaylistView() {
    // 阻止信号触发，防止在更新UI时循环调用槽函数
    m_playlistListWidget->blockSignals(true);
    
    m_playlistListWidget->clear();
    for (const Playlist* pl : m_playlistManager->getPlaylists()) {
        m_playlistListWidget->addItem(
            QString("%1 (%2)").arg(pl->getName()).arg(pl->songCount())
        );
    }

    // 恢复当前选择
    if (m_currentPlaylistIndex >= 0 && m_currentPlaylistIndex < m_playlistListWidget->count()) {
        m_playlistListWidget->setCurrentRow(m_currentPlaylistIndex);
    }
    
    m_playlistListWidget->blockSignals(false);
}

void MainWindow::updateSongListView() {
    m_songListWidget->clear();
    
    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist) return;
    
    int index = 0;
    for (const Song& song : playlist->getSongs()) {
        QString itemText = QString("%1. %2 - %3")
            .arg(index + 1)
            .arg(song.title)
            .arg(song.artist);
        
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, index);
        
        if (index == m_currentSongIndex) {
            item->setBackground(QColor(100, 150, 255, 50));
        }
        
        m_songListWidget->addItem(item);
        index++;
    }
}


void MainWindow::playSong(int index) {
    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist || index < 0 || index >= playlist->songCount()) {
        return;
    }
    
    Song song = playlist->getSong(index);
    m_currentSongIndex = index;
    
    // 先设置标题为文件名，元数据加载后会更新
    m_songTitleLabel->setText(song.title);
    m_songArtistLabel->setText("正在加载..."); // 提示正在加载
    
    m_player->setSource(QUrl::fromLocalFile(song.filePath));
    m_player->play();
    
    updateSongListView();
    updatePlayPauseButton();
}

void MainWindow::updatePlayPauseButton() {
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        m_playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

QString MainWindow::formatTime(qint64 milliseconds) {
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void MainWindow::onPlayPauseClicked() {
    if (m_currentSongIndex < 0) {
        playSong(0);
        return;
    }
    
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
    updatePlayPauseButton();
}

void MainWindow::onPreviousClicked() {
    if (m_currentSongIndex > 0) {
        playSong(m_currentSongIndex - 1);
    }
}

void MainWindow::onNextClicked() {
    playNextSong();
}


// ↓↓↓ 新增辅助函数，封装“播放下一曲”的全部逻辑 ↓↓↓
// ↓↓↓ 实现新的核心逻辑函数 playNextSong ↓↓↓
void MainWindow::playNextSong() {
    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist || playlist->songCount() == 0) {
        return;
    }

    // --- 第 1 部分：决定列表内的下一首歌曲索引 ---
    int nextIndexInList = -1;
    bool isListFinished = false;

    if (m_crossListMode == CrossListMode::SingleLoop) {
        // 对于单曲循环，下一首永远是当前这首
        nextIndexInList = m_currentSongIndex;
    } else if (m_inListMode == InListMode::Random) {
        // 随机模式
        m_shuffledPlaybackIndex++;
        if (m_shuffledPlaybackIndex < m_shuffledIndices.size()) {
            nextIndexInList = m_shuffledIndices.at(m_shuffledPlaybackIndex);
        } else {
            isListFinished = true; // 随机列表已播完
        }
    } else { // 顺序模式
        if (m_currentSongIndex + 1 < playlist->songCount()) {
            nextIndexInList = m_currentSongIndex + 1;
        } else {
            isListFinished = true; // 顺序列表已播完
        }
    }

    // --- 第 2 部分：根据列表是否播完，以及列表间过渡模式，决定最终动作 ---
    if (!isListFinished) {
        // 情况 A: 列表还没播完，直接播放下一首
        playSong(nextIndexInList);
    } else {
        // 情况 B: 列表已经播完了
        switch (m_crossListMode) {
            case CrossListMode::ListLoop:
                // 重新开始当前列表的第一首歌
                if (m_inListMode == InListMode::Random) {
                    generateShuffledPlaylist(); // 重新生成随机顺序
                    playSong(m_shuffledIndices.at(0));
                } else {
                    playSong(0); // 播放顺序第一首
                }
                break;
            
            case CrossListMode::Advance:
                // 切换到下一个播放列表
                {
                    int nextPlaylistIndex = (m_currentPlaylistIndex + 1) % m_playlistManager->playlistCount();
                    m_playlistListWidget->setCurrentRow(nextPlaylistIndex);
                    // setCurrentRow 会触发 onPlaylistSelectionChanged, 
                    // 它会更新 m_currentPlaylistIndex 并刷新歌曲列表
                    
                    // 自动播放新列表的第一首歌
                    // 需要稍作延迟，等待 onPlaylistSelectionChanged 完成
                    QTimer::singleShot(50, this, [this]() {
                        Playlist* newPlaylist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
                        if (newPlaylist && newPlaylist->songCount() > 0) {
                             if (m_inListMode == InListMode::Random) {
                                generateShuffledPlaylist();
                                playSong(m_shuffledIndices.at(0));
                             } else {
                                playSong(0);
                             }
                        }
                    });
                }
                break;

            case CrossListMode::SingleLoop:
                // 理论上 isListFinished 为 true 时不会进入这里，
                // 但为保险起见，我们还是让它循环当前这首
                playSong(m_currentSongIndex);
                break;

            case CrossListMode::Stop:
                // 播完停止，什么都不做
                break;
        }
    }
}

// ↓↓↓ 实现新增的辅助函数，用于生成随机播放顺序 ↓↓↓
void MainWindow::generateShuffledPlaylist() {
    m_shuffledIndices.clear();
    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist || playlist->songCount() == 0) {
        return;
    }

    // 创建一个从 0 到 n-1 的顺序列表
    for (int i = 0; i < playlist->songCount(); ++i) {
        m_shuffledIndices.append(i);
    }
    
    // 使用 C++11 的标准方法来打乱列表，这比旧的 rand() 效果更好
    std::shuffle(m_shuffledIndices.begin(), m_shuffledIndices.end(), *QRandomGenerator::global());

    m_shuffledPlaybackIndex = 0; // 重置随机播放的起始位置
    qDebug() << "生成新的随机播放顺序:" << m_shuffledIndices;
}


void MainWindow::onAddSongsClicked() {
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "选择音乐文件",
        QDir::homePath(),
        "音频文件 (*.mp3 *.wav *.ogg *.flac *.m4a);;所有文件 (*.*)"
    );
    
    if (files.isEmpty()) return;
    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist) return;
    for (const QString& file : files) {
        playlist->addSong(Song(QDir::toNativeSeparators(file)));
    }
    
    updatePlaylistView();
    updateSongListView();
    
    // 如果当前是随机模式，则重新生成随机列表
    if (m_inListMode == InListMode::Random) {
        generateShuffledPlaylist();
    }
}

void MainWindow::onCreatePlaylistClicked() {
    bool ok;
    QString name = QInputDialog::getText(
        this,
        "新建播放列表",
        "请输入播放列表名称:",
        QLineEdit::Normal,
        "新建列表",
        &ok
    );
    
    if (ok && !name.isEmpty()) {
        m_playlistManager->addPlaylist(name);
        updatePlaylistView();
        // 自动选中新创建的列表
        m_playlistListWidget->setCurrentRow(m_playlistManager->playlistCount() - 1);
    }
}

void MainWindow::onDeletePlaylistClicked() {
    if (m_playlistManager->playlistCount() <= 1) {
        QMessageBox::warning(this, "警告", "至少需要保留一个播放列表！");
        return;
    }
    
    // 从 ListWidget 获取当前选中的行
    int indexToDelete = m_playlistListWidget->currentRow();
    if (indexToDelete < 0) return; // 如果没有选中项，则不执行任何操作

    m_playlistManager->removePlaylist(indexToDelete);
    
    // 更新当前播放索引
    m_currentPlaylistIndex = 0;
    m_currentSongIndex = -1;

    updatePlaylistView();
    updateSongListView();

    // 如果是随机模式，重新生成
    if (m_inListMode == InListMode::Random) {
        generateShuffledPlaylist();
    }

}

void MainWindow::onPlaylistSelectionChanged() {
    int index = m_playlistListWidget->currentRow();
    if (index < 0 || index == m_currentPlaylistIndex) {
        return; // 如果选择无效或未改变，则不执行操作
    }
    
    m_currentPlaylistIndex = index;
    m_currentSongIndex = -1; // 切换列表后重置歌曲索引
    updateSongListView();

    // 切换了播放列表，如果当前是随机模式，需要为新列表生成随机顺序
    if (m_inListMode == InListMode::Random) {
        generateShuffledPlaylist();
    }
}

void MainWindow::onSongDoubleClicked(QListWidgetItem* item) {
    int index = item->data(Qt::UserRole).toInt();
    playSong(index);
}

void MainWindow::onPositionChanged(qint64 position) {
    m_progressSlider->setValue(position);
    m_currentTimeLabel->setText(formatTime(position));
}

void MainWindow::onDurationChanged(qint64 duration) {
    m_progressSlider->setRange(0, duration);
    m_totalTimeLabel->setText(formatTime(duration));
}

void MainWindow::onProgressSliderMoved(int position) {
    m_player->setPosition(position);
}

void MainWindow::onVolumeChanged(int value) {
    // a. 设置播放器的实际音量
    m_audioOutput->setVolume(value / 100.0);

    // b. 在当前鼠标光标位置显示一个临时的百分比提示
    QString tooltipText = QString("音量: %1%").arg(value);
    QToolTip::showText(QCursor::pos(), tooltipText, m_volumeSlider);
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        // 当一首歌自然播放结束时，调用播放下一首的逻辑
        playNextSong();
    }
    updatePlayPauseButton();
}

void MainWindow::onMetaDataChanged() {
    if (m_currentSongIndex < 0) return;

    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist) return;

    // 获取元数据
    QMediaMetaData metaData = m_player->metaData();

    QString title = metaData.value(QMediaMetaData::Title).toString();
    QString artist = metaData.value(QMediaMetaData::AlbumArtist).toString();
    if (artist.isEmpty()) {
        artist = metaData.value(QMediaMetaData::ContributingArtist).toString();
    }
    QString album = metaData.value(QMediaMetaData::AlbumTitle).toString();

    // 使用新函数来更新播放列表中的歌曲数据
    playlist->updateSongMetaData(m_currentSongIndex, title, artist, album);

    // 从播放列表重新获取更新后的歌曲信息
    Song updatedSong = playlist->getSong(m_currentSongIndex);
    
    // 更新 UI 显示
    m_songTitleLabel->setText(updatedSong.title);
    m_songArtistLabel->setText(updatedSong.artist);

    // 刷新歌曲列表视图，以显示更新后的信息
    updateSongListView();
}

void MainWindow::onFilesDroppedToSongList(const QList<QUrl>& urls) {
    Playlist* currentPlaylist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!currentPlaylist) {
        return;
    }

    for (const QUrl& url : urls) {
        currentPlaylist->addSong(Song(url.toLocalFile()));
    }

    updatePlaylistView(); // 更新播放列表的歌曲计数
    updateSongListView();

    // 如果是随机模式，重新生成随机列表
    if (m_inListMode == InListMode::Random) {
        generateShuffledPlaylist();
    }
}

void MainWindow::onFoldersDropped(const QList<QUrl>& urls) {
    qDebug() << "--- Folders Dropped Event ---";
    qDebug() << "接收到" << urls.size() << "个拖放项目。";
    
    // 定义我们支持的音频文件后缀名过滤器
    QStringList supportedExtensions = {"*.mp3", "*.flac", "*.wav", "*.ogg", "*.m4a"};

    for (const QUrl& url : urls) {
        QString folderPath = url.toLocalFile();
        QFileInfo folderInfo(folderPath);

        // 安全检查：确保拖进来的是一个真实存在的目录
        if (!folderInfo.isDir()) {
            qDebug() << "跳过非目录项目:" << folderPath;
            continue; // 继续处理下一个拖放项目
        }

        // 1. 使用文件夹名创建新的播放列表
        QString playlistName = folderInfo.fileName();
        qDebug() << "正在创建播放列表:" << playlistName;
        m_playlistManager->addPlaylist(playlistName);
        
        // 2. 获取刚刚创建的播放列表的指针
        int newPlaylistIndex = m_playlistManager->playlistCount() - 1;
        Playlist* newPlaylist = m_playlistManager->getPlaylist(newPlaylistIndex);
        if (!newPlaylist) {
            qWarning() << "错误：无法获取新创建的播放列表！";
            continue;
        }

        // 3. 核心步骤：使用 QDirIterator 递归扫描文件夹
        qDebug() << "开始扫描文件夹:" << folderPath;
        int songsFound = 0;
        // 创建一个迭代器，它会查找指定目录（包括所有子目录）中所有符合后缀名过滤器的文件
        QDirIterator it(folderPath, supportedExtensions, QDir::Files, QDirIterator::Subdirectories);
        
        // 循环直到迭代器找不到更多文件
        while (it.hasNext()) {
            // 获取下一个文件的完整路径
            QString filePath = it.next();
            // 添加到播放列表中
            newPlaylist->addSong(Song(filePath));
            songsFound++;
            qDebug() << "  (+) 已添加:" << filePath; // 打印出每个找到的文件
        }
        qDebug() << "扫描完成。在" << playlistName << "中总共找到并添加了" << songsFound << "首歌曲。";
    }
    
    // 4. 所有文件夹都处理完毕后，一次性更新UI
    updatePlaylistView();
    // 自动选中我们最后创建的那个播放列表
    if (m_playlistManager->playlistCount() > 0) {
        m_playlistListWidget->setCurrentRow(m_playlistManager->playlistCount() - 1);
    }
    qDebug() << "UI更新完成。";
    qDebug() << "--------------------------";
}

void MainWindow::onPlaylistContextMenuRequested(const QPoint& pos) {
    QMenu contextMenu(this);

    // 创建动作
    QAction* createAction = contextMenu.addAction("新建播放列表");
    QAction* deleteAction = contextMenu.addAction("删除当前列表");

    // 逻辑判断：只有当选中了一个列表，并且总列表数大于1时，才允许删除
    if (m_playlistListWidget->currentRow() < 0 || m_playlistManager->playlistCount() <= 1) {
        deleteAction->setEnabled(false);
    }

    // 将动作的 triggered 信号连接到已有的槽函数
    connect(createAction, &QAction::triggered, this, &MainWindow::onCreatePlaylistClicked);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeletePlaylistClicked);

    // 在鼠标光标位置显示菜单
    // mapToGlobal 将控件的局部坐标转换为屏幕的全局坐标
    contextMenu.exec(m_playlistListWidget->mapToGlobal(pos));
}

// 当在歌曲列表上右键点击时调用
void MainWindow::onSongListContextMenuRequested(const QPoint& pos) {
    QMenu contextMenu(this);

    QAction* addAction = contextMenu.addAction("添加歌曲...");
    
    // 只有当用户确实选中了一首歌曲时，才显示删除相关的选项
    if (m_songListWidget->currentItem() != nullptr) {
        contextMenu.addSeparator(); // 添加一条分割线，让UI更清晰
        QAction* deleteAction = contextMenu.addAction("删除选中的歌曲");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteSongClicked);
    }
    
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddSongsClicked);
    
    contextMenu.exec(m_songListWidget->mapToGlobal(pos));
}

// ↓↓↓ 在文件末尾添加两个新函数的实现 ↓↓↓

// 实现重置播放器状态的辅助函数
void MainWindow::resetPlayerState() {
    m_player->stop();
    m_songTitleLabel->setText("未选择歌曲");
    m_songArtistLabel->setText("");
    m_currentTimeLabel->setText("00:00");
    m_totalTimeLabel->setText("00:00");
    m_progressSlider->setValue(0);
    m_currentSongIndex = -1;
    updatePlayPauseButton();
}


// 实现删除歌曲的槽函数
void MainWindow::onDeleteSongClicked() {
    // 1. 获取当前选中的行号
    int indexToDelete = m_songListWidget->currentRow();
    if (indexToDelete < 0) {
        return; // 如果没有选中任何项，则不执行任何操作
    }

    // 2. 获取当前的播放列表
    Playlist* playlist = m_playlistManager->getPlaylist(m_currentPlaylistIndex);
    if (!playlist) {
        return;
    }

    // 3. 处理核心逻辑：如果删除的歌曲影响了当前播放，需要特殊处理
    if (indexToDelete == m_currentSongIndex) {
        // --- 情况A: 删除的正是当前正在播放的歌曲 ---
        resetPlayerState(); // 停止播放并重置UI
    } else if (indexToDelete < m_currentSongIndex) {
        // --- 情况B: 删除的歌曲在当前播放歌曲的前面 ---
        // 列表项重新排序后，当前播放歌曲的索引需要减1
        m_currentSongIndex--;
    }
    // --- 情况C: 删除的歌曲在当前播放歌曲的后面 ---
    // 这种情况对当前播放索引没有影响，无需处理

    // 4. 从数据模型中删除歌曲
    playlist->removeSong(indexToDelete);

    // 5. 更新UI显示
    updateSongListView();   // 刷新歌曲列表
    updatePlaylistView();   // 刷新播放列表的歌曲计数

    // 6. 如果是随机播放模式，删除歌曲后需要重新生成随机列表
    if (m_inListMode == InListMode::Random) {
        generateShuffledPlaylist();
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    // 首先，必须调用基类的实现
    QMainWindow::showEvent(event);

    // 使用标志位确保这个加载逻辑只在程序启动时执行一次
    // 而不是每次从托盘恢复窗口时都执行
    if (m_isFirstShow) {
        m_isFirstShow = false; // 立刻将标志位设为 false

        QSettings settings;
        int lastPlaylistIndex = settings.value("lastPlaylistIndex", 0).toInt();
        int lastSongIndex = settings.value("lastSongIndex", -1).toInt();

        // 验证加载的播放列表索引是否有效
        if (lastPlaylistIndex >= 0 && lastPlaylistIndex < m_playlistManager->playlistCount()) {
            
            // 切换到上次的播放列表
            m_playlistListWidget->setCurrentRow(lastPlaylistIndex);
            // onPlaylistSelectionChanged 会被自动触发，更新 m_currentPlaylistIndex 和歌曲列表视图

            Playlist* playlist = m_playlistManager->getPlaylist(lastPlaylistIndex);
            // 验证加载的歌曲索引是否有效
            if (playlist && lastSongIndex >= 0 && lastSongIndex < playlist->songCount()) {
                
                // 播放上次的歌曲
                playSong(lastSongIndex);
            }
        }
    }
}


void MainWindow::onShowFontSettings()
{
    // a. 获取当前列表控件的字体作为对话框的默认选项
    QFont currentFont = m_playlistListWidget->font();

    bool ok;
    QFont selectedFont = QFontDialog::getFont(
        &ok,
        currentFont,
        this,
        "选择列表字体"
    );

    // c. 如果用户点击了“确定”
    if (ok) {
        // i. 将新字体分别应用到两个列表控件
        m_playlistListWidget->setFont(selectedFont);
        m_songListWidget->setFont(selectedFont);

        // ii. 使用 QSettings 立即保存用户的选择
        //     使用一个新的键名 "listFont" 以区别于之前的全局设置
        QSettings settings;
        settings.setValue("listFont", selectedFont);
    }
}