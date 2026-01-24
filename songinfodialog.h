#ifndef SONGINFODIALOG_H
#define SONGINFODIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QLabel;
class QPushButton;
class QSpinBox;
class QGroupBox;

class SongInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SongInfoDialog(const QString& filePath, QWidget* parent = nullptr);
    ~SongInfoDialog();

    // 获取编辑后的元数据
    QString title() const;
    QString artist() const;
    QString album() const;
    QString albumArtist() const;
    int year() const;
    int trackNumber() const;
    int trackTotal() const;
    QString genre() const;
    QString comment() const;

private slots:
    void onSaveClicked();

private:
    void setupUI();
    bool loadMetaData();
    bool saveMetaData();

    QString m_filePath;
    bool m_taglibAvailable;

    // UI 组件
    QLabel* m_filePathLabel;
    
    // 基本信息
    QGroupBox* m_basicGroup;
    QLineEdit* m_titleEdit;
    QLineEdit* m_artistEdit;
    QLineEdit* m_albumEdit;
    QLineEdit* m_albumArtistEdit;
    
    // 附加信息
    QGroupBox* m_extraGroup;
    QSpinBox* m_yearSpin;
    QSpinBox* m_trackNumSpin;
    QSpinBox* m_trackTotalSpin;
    QLineEdit* m_genreEdit;
    QLineEdit* m_commentEdit;
    
    // 按钮
    QPushButton* m_saveBtn;
    QPushButton* m_cancelBtn;
};

#endif // SONGINFODIALOG_H
