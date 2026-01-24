#ifndef TRANSCODEDIALOG_H
#define TRANSCODEDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QProcess>

class QComboBox;
class QProgressBar;
class QLabel;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QGroupBox;

// 支持的输出格式
enum class AudioFormat {
    WAV,
    FLAC,
    MP3,
    AAC,
    OGG
};

class TranscodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TranscodeDialog(const QStringList& filePaths, QWidget* parent = nullptr);
    ~TranscodeDialog();

private slots:
    void onFormatChanged(int index);
    void onBrowseOutputDir();
    void onStartTranscode();
    void onCancelTranscode();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessReadyRead();

private:
    void setupUI();
    QString findFFmpegPath();
    void transcodeNextFile();
    void updateFileStatus(int index, const QString& status, bool success);
    QString getFFmpegArgs(const QString& inputPath, const QString& outputPath);
    QString getOutputExtension(AudioFormat format);
    QString getCodecName(AudioFormat format);

    // 输入文件列表
    QStringList m_inputFiles;
    
    // 转码状态
    int m_currentFileIndex;
    int m_successCount;
    int m_failCount;
    bool m_isTranscoding;
    
    // FFmpeg 进程
    QProcess* m_ffmpegProcess;
    QString m_ffmpegPath;
    qint64 m_currentFileDuration; // 当前文件时长（毫秒）
    
    // UI 组件
    QListWidget* m_fileListWidget;
    QComboBox* m_formatCombo;
    QGroupBox* m_qualityGroup;
    QComboBox* m_bitrateCombo;
    QLineEdit* m_outputDirEdit;
    QPushButton* m_browseBtn;
    QLabel* m_currentFileLabel;
    QProgressBar* m_currentProgressBar;
    QLabel* m_overallProgressLabel;
    QProgressBar* m_overallProgressBar;
    QPushButton* m_startBtn;
    QPushButton* m_cancelBtn;
};

#endif // TRANSCODEDIALOG_H
