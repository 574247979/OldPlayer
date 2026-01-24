#include "transcodedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QRegularExpression>

TranscodeDialog::TranscodeDialog(const QStringList& filePaths, QWidget* parent)
    : QDialog(parent)
    , m_inputFiles(filePaths)
    , m_currentFileIndex(-1)
    , m_successCount(0)
    , m_failCount(0)
    , m_isTranscoding(false)
    , m_ffmpegProcess(nullptr)
    , m_currentFileDuration(0)
{
    setWindowTitle("音频转码");
    setMinimumSize(500, 450);
    
    m_ffmpegPath = findFFmpegPath();
    
    setupUI();
}

TranscodeDialog::~TranscodeDialog()
{
    if (m_ffmpegProcess) {
        if (m_ffmpegProcess->state() != QProcess::NotRunning) {
            m_ffmpegProcess->kill();
            m_ffmpegProcess->waitForFinished();
        }
        delete m_ffmpegProcess;
    }
}

void TranscodeDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    
    // 文件列表
    QLabel* fileListLabel = new QLabel(QString("待转码文件 (%1):").arg(m_inputFiles.size()));
    mainLayout->addWidget(fileListLabel);
    
    m_fileListWidget = new QListWidget();
    m_fileListWidget->setMaximumHeight(120);
    for (const QString& filePath : m_inputFiles) {
        QFileInfo fi(filePath);
        QListWidgetItem* item = new QListWidgetItem("○ " + fi.fileName());
        item->setData(Qt::UserRole, filePath);
        m_fileListWidget->addItem(item);
    }
    mainLayout->addWidget(m_fileListWidget);
    
    // 格式选择
    QHBoxLayout* formatLayout = new QHBoxLayout();
    formatLayout->addWidget(new QLabel("目标格式:"));
    m_formatCombo = new QComboBox();
    m_formatCombo->addItem("WAV (无损)", static_cast<int>(AudioFormat::WAV));
    m_formatCombo->addItem("FLAC (无损)", static_cast<int>(AudioFormat::FLAC));
    m_formatCombo->addItem("MP3", static_cast<int>(AudioFormat::MP3));
    m_formatCombo->addItem("AAC (M4A)", static_cast<int>(AudioFormat::AAC));
    m_formatCombo->addItem("OGG", static_cast<int>(AudioFormat::OGG));
    formatLayout->addWidget(m_formatCombo);
    formatLayout->addStretch();
    mainLayout->addLayout(formatLayout);
    
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TranscodeDialog::onFormatChanged);
    
    // 质量设置组
    m_qualityGroup = new QGroupBox("质量设置");
    QHBoxLayout* qualityLayout = new QHBoxLayout(m_qualityGroup);
    qualityLayout->addWidget(new QLabel("比特率:"));
    m_bitrateCombo = new QComboBox();
    m_bitrateCombo->addItem("128 kbps", 128);
    m_bitrateCombo->addItem("192 kbps", 192);
    m_bitrateCombo->addItem("256 kbps", 256);
    m_bitrateCombo->addItem("320 kbps", 320);
    m_bitrateCombo->setCurrentIndex(3); // 默认320kbps
    qualityLayout->addWidget(m_bitrateCombo);
    qualityLayout->addStretch();
    m_qualityGroup->setVisible(false); // 默认选WAV，不显示质量设置
    mainLayout->addWidget(m_qualityGroup);
    
    // 输出目录
    QHBoxLayout* outputLayout = new QHBoxLayout();
    outputLayout->addWidget(new QLabel("输出目录:"));
    m_outputDirEdit = new QLineEdit();
    // 默认输出到源文件所在目录
    if (!m_inputFiles.isEmpty()) {
        QFileInfo fi(m_inputFiles.first());
        m_outputDirEdit->setText(fi.absolutePath());
    }
    outputLayout->addWidget(m_outputDirEdit);
    m_browseBtn = new QPushButton("浏览...");
    connect(m_browseBtn, &QPushButton::clicked, this, &TranscodeDialog::onBrowseOutputDir);
    outputLayout->addWidget(m_browseBtn);
    mainLayout->addLayout(outputLayout);
    
    // 当前文件进度
    mainLayout->addSpacing(10);
    m_currentFileLabel = new QLabel("当前文件: -");
    mainLayout->addWidget(m_currentFileLabel);
    
    m_currentProgressBar = new QProgressBar();
    m_currentProgressBar->setRange(0, 100);
    m_currentProgressBar->setValue(0);
    mainLayout->addWidget(m_currentProgressBar);
    
    // 总体进度
    m_overallProgressLabel = new QLabel("总体进度: 0/0");
    mainLayout->addWidget(m_overallProgressLabel);
    
    m_overallProgressBar = new QProgressBar();
    m_overallProgressBar->setRange(0, m_inputFiles.size());
    m_overallProgressBar->setValue(0);
    mainLayout->addWidget(m_overallProgressBar);
    
    // 按钮
    mainLayout->addStretch();
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_startBtn = new QPushButton("开始转码");
    m_startBtn->setDefault(true);
    connect(m_startBtn, &QPushButton::clicked, this, &TranscodeDialog::onStartTranscode);
    buttonLayout->addWidget(m_startBtn);
    
    m_cancelBtn = new QPushButton("取消");
    connect(m_cancelBtn, &QPushButton::clicked, this, &TranscodeDialog::onCancelTranscode);
    buttonLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(buttonLayout);
}

QString TranscodeDialog::findFFmpegPath()
{
    // 首先查找应用程序目录下的 tools/ffmpeg.exe
    QString appPath = QCoreApplication::applicationDirPath();
    QString ffmpegPath = appPath + "/tools/ffmpeg.exe";
    if (QFile::exists(ffmpegPath)) {
        return ffmpegPath;
    }
    
    // 尝试系统 PATH 中的 ffmpeg
    return "ffmpeg";
}

void TranscodeDialog::onFormatChanged(int index)
{
    AudioFormat format = static_cast<AudioFormat>(m_formatCombo->itemData(index).toInt());
    // 有损格式显示质量设置
    bool showQuality = (format == AudioFormat::MP3 || 
                        format == AudioFormat::AAC || 
                        format == AudioFormat::OGG);
    m_qualityGroup->setVisible(showQuality);
}

void TranscodeDialog::onBrowseOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择输出目录", 
                                                     m_outputDirEdit->text());
    if (!dir.isEmpty()) {
        m_outputDirEdit->setText(dir);
    }
}

void TranscodeDialog::onStartTranscode()
{
    // 验证FFmpeg是否存在
    if (m_ffmpegPath == "ffmpeg") {
        // 尝试运行ffmpeg -version来检查是否在PATH中
        QProcess testProcess;
        testProcess.start("ffmpeg", QStringList() << "-version");
        if (!testProcess.waitForStarted(3000)) {
            QMessageBox::critical(this, "错误", 
                "找不到 FFmpeg！\n\n"
                "请将 ffmpeg.exe 放入程序目录下的 tools 文件夹中，\n"
                "或将 FFmpeg 添加到系统 PATH 环境变量。");
            return;
        }
        testProcess.waitForFinished();
    }
    
    // 验证输出目录
    QString outputDir = m_outputDirEdit->text();
    if (outputDir.isEmpty() || !QDir(outputDir).exists()) {
        QMessageBox::warning(this, "警告", "请选择有效的输出目录！");
        return;
    }
    
    // 开始转码
    m_isTranscoding = true;
    m_currentFileIndex = -1;
    m_successCount = 0;
    m_failCount = 0;
    
    m_startBtn->setEnabled(false);
    m_formatCombo->setEnabled(false);
    m_bitrateCombo->setEnabled(false);
    m_browseBtn->setEnabled(false);
    m_outputDirEdit->setEnabled(false);
    
    m_ffmpegProcess = new QProcess(this);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TranscodeDialog::onProcessFinished);
    connect(m_ffmpegProcess, &QProcess::errorOccurred,
            this, &TranscodeDialog::onProcessError);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError,
            this, &TranscodeDialog::onProcessReadyRead);
    
    transcodeNextFile();
}

void TranscodeDialog::onCancelTranscode()
{
    if (m_isTranscoding && m_ffmpegProcess) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished();
        m_isTranscoding = false;
    }
    reject();
}

void TranscodeDialog::transcodeNextFile()
{
    m_currentFileIndex++;
    
    if (m_currentFileIndex >= m_inputFiles.size()) {
        // 全部完成
        m_isTranscoding = false;
        m_startBtn->setEnabled(true);
        m_formatCombo->setEnabled(true);
        m_bitrateCombo->setEnabled(true);
        m_browseBtn->setEnabled(true);
        m_outputDirEdit->setEnabled(true);
        
        QString message = QString("转码完成！\n\n成功: %1\n失败: %2")
                          .arg(m_successCount).arg(m_failCount);
        QMessageBox::information(this, "完成", message);
        
        if (m_failCount == 0) {
            accept();
        }
        return;
    }
    
    QString inputPath = m_inputFiles[m_currentFileIndex];
    QFileInfo fi(inputPath);
    
    // 更新UI
    updateFileStatus(m_currentFileIndex, "转码中...", true);
    m_currentFileLabel->setText("当前文件: " + fi.fileName());
    m_currentProgressBar->setValue(0);
    m_overallProgressLabel->setText(QString("总体进度: %1/%2")
                                    .arg(m_currentFileIndex)
                                    .arg(m_inputFiles.size()));
    
    // 构建输出路径
    AudioFormat format = static_cast<AudioFormat>(
        m_formatCombo->itemData(m_formatCombo->currentIndex()).toInt());
    QString outputDir = m_outputDirEdit->text();
    QString outputPath = outputDir + "/" + fi.completeBaseName() + getOutputExtension(format);
    
    // 检查输出文件是否已存在
    if (QFile::exists(outputPath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "文件已存在",
            QString("文件 %1 已存在，是否覆盖？").arg(QFileInfo(outputPath).fileName()),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Cancel) {
            onCancelTranscode();
            return;
        } else if (reply == QMessageBox::No) {
            updateFileStatus(m_currentFileIndex, "已跳过", true);
            transcodeNextFile();
            return;
        }
    }
    
    // 构建FFmpeg命令
    QStringList args;
    args << "-y" << "-i" << inputPath;
    
    // 根据格式添加编码参数
    switch (format) {
        case AudioFormat::WAV:
            args << "-c:a" << "pcm_s16le";
            break;
        case AudioFormat::FLAC:
            args << "-c:a" << "flac";
            break;
        case AudioFormat::MP3:
            args << "-c:a" << "libmp3lame";
            args << "-b:a" << QString("%1k").arg(m_bitrateCombo->currentData().toInt());
            break;
        case AudioFormat::AAC:
            args << "-c:a" << "aac";
            args << "-b:a" << QString("%1k").arg(m_bitrateCombo->currentData().toInt());
            break;
        case AudioFormat::OGG:
            args << "-c:a" << "libvorbis";
            args << "-qscale:a" << "6"; // 质量等级
            break;
    }
    
    args << "-progress" << "pipe:2" << outputPath;
    
    m_currentFileDuration = 0;
    m_ffmpegProcess->start(m_ffmpegPath, args);
}

void TranscodeDialog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_successCount++;
        updateFileStatus(m_currentFileIndex, "完成", true);
        m_currentProgressBar->setValue(100);
    } else {
        m_failCount++;
        updateFileStatus(m_currentFileIndex, "失败", false);
    }
    
    m_overallProgressBar->setValue(m_currentFileIndex + 1);
    
    transcodeNextFile();
}

void TranscodeDialog::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    m_failCount++;
    updateFileStatus(m_currentFileIndex, "错误", false);
    m_overallProgressBar->setValue(m_currentFileIndex + 1);
    transcodeNextFile();
}

void TranscodeDialog::onProcessReadyRead()
{
    QString output = QString::fromLocal8Bit(m_ffmpegProcess->readAllStandardError());
    
    // 解析FFmpeg输出获取进度
    // FFmpeg输出格式: out_time_ms=123456 或 Duration: 00:03:45.67
    
    // 首先尝试获取总时长
    if (m_currentFileDuration == 0) {
        QRegularExpression durationRe("Duration:\\s*(\\d+):(\\d+):(\\d+)\\.(\\d+)");
        QRegularExpressionMatch match = durationRe.match(output);
        if (match.hasMatch()) {
            int hours = match.captured(1).toInt();
            int mins = match.captured(2).toInt();
            int secs = match.captured(3).toInt();
            int centis = match.captured(4).toInt();
            m_currentFileDuration = ((hours * 3600 + mins * 60 + secs) * 1000) + (centis * 10);
        }
    }
    
    // 解析当前进度
    QRegularExpression outTimeRe("out_time_ms=(\\d+)");
    QRegularExpressionMatch match = outTimeRe.match(output);
    if (match.hasMatch() && m_currentFileDuration > 0) {
        qint64 currentTime = match.captured(1).toLongLong() / 1000; // 转为毫秒
        int progress = static_cast<int>((currentTime * 100) / m_currentFileDuration);
        if (progress > 100) progress = 100;
        m_currentProgressBar->setValue(progress);
    }
}

void TranscodeDialog::updateFileStatus(int index, const QString& status, bool success)
{
    if (index < 0 || index >= m_fileListWidget->count()) return;
    
    QListWidgetItem* item = m_fileListWidget->item(index);
    QString filePath = item->data(Qt::UserRole).toString();
    QFileInfo fi(filePath);
    
    QString prefix;
    if (status == "转码中...") {
        prefix = "▶ ";
    } else if (success) {
        prefix = "✓ ";
    } else {
        prefix = "✗ ";
    }
    
    item->setText(prefix + fi.fileName() + (status.isEmpty() ? "" : " [" + status + "]"));
}

QString TranscodeDialog::getOutputExtension(AudioFormat format)
{
    switch (format) {
        case AudioFormat::WAV:  return ".wav";
        case AudioFormat::FLAC: return ".flac";
        case AudioFormat::MP3:  return ".mp3";
        case AudioFormat::AAC:  return ".m4a";
        case AudioFormat::OGG:  return ".ogg";
        default: return ".wav";
    }
}

QString TranscodeDialog::getCodecName(AudioFormat format)
{
    switch (format) {
        case AudioFormat::WAV:  return "pcm_s16le";
        case AudioFormat::FLAC: return "flac";
        case AudioFormat::MP3:  return "libmp3lame";
        case AudioFormat::AAC:  return "aac";
        case AudioFormat::OGG:  return "libvorbis";
        default: return "pcm_s16le";
    }
}
