#include "songinfodialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>

// TagLib 头文件
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>

SongInfoDialog::SongInfoDialog(const QString& filePath, QWidget* parent)
    : QDialog(parent)
    , m_filePath(filePath)
    , m_taglibAvailable(true)
{
    setWindowTitle("编辑歌曲信息");
    setMinimumWidth(450);
    setModal(true);

    setupUI();
    
    if (!loadMetaData()) {
        m_taglibAvailable = false;
    }
}

SongInfoDialog::~SongInfoDialog()
{
}

void SongInfoDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // 文件路径显示
    QFileInfo fileInfo(m_filePath);
    m_filePathLabel = new QLabel(QString("文件：%1").arg(fileInfo.fileName()));
    m_filePathLabel->setWordWrap(true);
    m_filePathLabel->setStyleSheet("color: #888; font-size: 11px;");
    m_filePathLabel->setToolTip(m_filePath);
    mainLayout->addWidget(m_filePathLabel);

    // ========== 基本信息组 ==========
    m_basicGroup = new QGroupBox("基本信息");
    QFormLayout* basicLayout = new QFormLayout(m_basicGroup);
    basicLayout->setSpacing(8);
    basicLayout->setContentsMargins(12, 16, 12, 12);

    m_titleEdit = new QLineEdit();
    m_titleEdit->setPlaceholderText("歌曲标题");
    basicLayout->addRow("标题：", m_titleEdit);

    m_artistEdit = new QLineEdit();
    m_artistEdit->setPlaceholderText("演唱/演奏者");
    basicLayout->addRow("艺术家：", m_artistEdit);

    m_albumEdit = new QLineEdit();
    m_albumEdit->setPlaceholderText("所属专辑");
    basicLayout->addRow("专辑：", m_albumEdit);

    m_albumArtistEdit = new QLineEdit();
    m_albumArtistEdit->setPlaceholderText("专辑艺术家（可选）");
    basicLayout->addRow("专辑艺术家：", m_albumArtistEdit);

    mainLayout->addWidget(m_basicGroup);

    // ========== 附加信息组 ==========
    m_extraGroup = new QGroupBox("附加信息");
    QFormLayout* extraLayout = new QFormLayout(m_extraGroup);
    extraLayout->setSpacing(8);
    extraLayout->setContentsMargins(12, 16, 12, 12);

    // 年份
    m_yearSpin = new QSpinBox();
    m_yearSpin->setRange(0, 2100);
    m_yearSpin->setSpecialValueText("未设置");
    m_yearSpin->setValue(0);
    extraLayout->addRow("年份：", m_yearSpin);

    // 音轨号行 - 使用水平布局
    QHBoxLayout* trackLayout = new QHBoxLayout();
    m_trackNumSpin = new QSpinBox();
    m_trackNumSpin->setRange(0, 999);
    m_trackNumSpin->setSpecialValueText("-");
    m_trackNumSpin->setFixedWidth(70);
    
    QLabel* slashLabel = new QLabel(" / ");
    
    m_trackTotalSpin = new QSpinBox();
    m_trackTotalSpin->setRange(0, 999);
    m_trackTotalSpin->setSpecialValueText("-");
    m_trackTotalSpin->setFixedWidth(70);
    
    trackLayout->addWidget(m_trackNumSpin);
    trackLayout->addWidget(slashLabel);
    trackLayout->addWidget(m_trackTotalSpin);
    trackLayout->addStretch();
    extraLayout->addRow("音轨号：", trackLayout);

    // 流派
    m_genreEdit = new QLineEdit();
    m_genreEdit->setPlaceholderText("如：Pop, Rock, Classical");
    extraLayout->addRow("流派：", m_genreEdit);

    // 备注
    m_commentEdit = new QLineEdit();
    m_commentEdit->setPlaceholderText("自定义备注");
    extraLayout->addRow("备注：", m_commentEdit);

    mainLayout->addWidget(m_extraGroup);

    // ========== 按钮区域 ==========
    mainLayout->addSpacing(8);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton("取消");
    m_cancelBtn->setFixedWidth(80);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(m_cancelBtn);

    m_saveBtn = new QPushButton("保存");
    m_saveBtn->setFixedWidth(80);
    m_saveBtn->setDefault(true);
    connect(m_saveBtn, &QPushButton::clicked, this, &SongInfoDialog::onSaveClicked);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);
}

bool SongInfoDialog::loadMetaData()
{
    // 使用 TagLib 读取文件元数据
    TagLib::FileRef file(m_filePath.toStdWString().c_str());
    
    if (file.isNull() || !file.tag()) {
        QMessageBox::warning(this, "警告", 
            "无法读取此文件的元数据。\n可能是文件格式不支持或文件已损坏。");
        return false;
    }

    TagLib::Tag* tag = file.tag();

    // 读取基本信息
    m_titleEdit->setText(QString::fromStdWString(tag->title().toWString()));
    m_artistEdit->setText(QString::fromStdWString(tag->artist().toWString()));
    m_albumEdit->setText(QString::fromStdWString(tag->album().toWString()));
    
    // 读取附加信息
    m_yearSpin->setValue(tag->year());
    m_trackNumSpin->setValue(tag->track());
    m_genreEdit->setText(QString::fromStdWString(tag->genre().toWString()));
    m_commentEdit->setText(QString::fromStdWString(tag->comment().toWString()));

    // 尝试读取专辑艺术家和音轨总数（通过属性映射）
    TagLib::PropertyMap properties = file.file()->properties();
    
    if (properties.contains("ALBUMARTIST")) {
        TagLib::StringList albumArtists = properties["ALBUMARTIST"];
        if (!albumArtists.isEmpty()) {
            m_albumArtistEdit->setText(QString::fromStdWString(albumArtists.front().toWString()));
        }
    }
    
    if (properties.contains("TRACKTOTAL")) {
        TagLib::StringList trackTotals = properties["TRACKTOTAL"];
        if (!trackTotals.isEmpty()) {
            m_trackTotalSpin->setValue(trackTotals.front().toInt());
        }
    }

    return true;
}

bool SongInfoDialog::saveMetaData()
{
    TagLib::FileRef file(m_filePath.toStdWString().c_str());
    
    if (file.isNull() || !file.tag()) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入。");
        return false;
    }

    TagLib::Tag* tag = file.tag();

    // 写入基本信息
    tag->setTitle(TagLib::String(m_titleEdit->text().toStdWString()));
    tag->setArtist(TagLib::String(m_artistEdit->text().toStdWString()));
    tag->setAlbum(TagLib::String(m_albumEdit->text().toStdWString()));
    
    // 写入附加信息
    tag->setYear(m_yearSpin->value());
    tag->setTrack(m_trackNumSpin->value());
    tag->setGenre(TagLib::String(m_genreEdit->text().toStdWString()));
    tag->setComment(TagLib::String(m_commentEdit->text().toStdWString()));

    // 写入专辑艺术家和音轨总数（通过属性映射）
    TagLib::PropertyMap properties = file.file()->properties();
    
    QString albumArtist = m_albumArtistEdit->text();
    if (!albumArtist.isEmpty()) {
        properties.replace("ALBUMARTIST", TagLib::StringList(TagLib::String(albumArtist.toStdWString())));
    } else {
        properties.erase("ALBUMARTIST");
    }
    
    int trackTotal = m_trackTotalSpin->value();
    if (trackTotal > 0) {
        properties.replace("TRACKTOTAL", TagLib::StringList(TagLib::String::number(trackTotal)));
    } else {
        properties.erase("TRACKTOTAL");
    }
    
    file.file()->setProperties(properties);

    // 保存文件
    if (!file.save()) {
        QMessageBox::critical(this, "错误", 
            "保存失败！\n可能是文件正在被占用或没有写入权限。");
        return false;
    }

    return true;
}

void SongInfoDialog::onSaveClicked()
{
    if (!m_taglibAvailable) {
        QMessageBox::warning(this, "警告", "TagLib 不可用，无法保存元数据。");
        reject();
        return;
    }

    if (saveMetaData()) {
        accept();
    }
}

// Getter 方法实现
QString SongInfoDialog::title() const { return m_titleEdit->text(); }
QString SongInfoDialog::artist() const { return m_artistEdit->text(); }
QString SongInfoDialog::album() const { return m_albumEdit->text(); }
QString SongInfoDialog::albumArtist() const { return m_albumArtistEdit->text(); }
int SongInfoDialog::year() const { return m_yearSpin->value(); }
int SongInfoDialog::trackNumber() const { return m_trackNumSpin->value(); }
int SongInfoDialog::trackTotal() const { return m_trackTotalSpin->value(); }
QString SongInfoDialog::genre() const { return m_genreEdit->text(); }
QString SongInfoDialog::comment() const { return m_commentEdit->text(); }
