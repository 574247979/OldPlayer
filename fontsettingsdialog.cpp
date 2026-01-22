#include "fontsettingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QFontDatabase>
#include <QGroupBox>

FontSettingsDialog::FontSettingsDialog(const QFont& currentFont, QWidget* parent)
    : QDialog(parent)
    , m_selectedFont(currentFont)
{
    setWindowTitle("字体设置");
    setFixedSize(400, 320);
    setupUI();
    loadFonts();
    
    // 设置当前字体
    int fontIndex = m_fontCombo->findText(currentFont.family());
    if (fontIndex >= 0) {
        m_fontCombo->setCurrentIndex(fontIndex);
    }
    m_sizeSlider->setValue(currentFont.pointSize());
    
    updatePreview();
}

QFont FontSettingsDialog::selectedFont() const
{
    return m_selectedFont;
}

void FontSettingsDialog::setupUI()
{
    // 深色主题样式
    setStyleSheet(R"(
        QDialog {
            background-color: #2b2b2b;
            color: #e0e0e0;
        }
        QLabel {
            color: #e0e0e0;
            font-size: 13px;
        }
        QLabel#previewLabel {
            background-color: #2b2b2b;
            padding: 12px;
            border-radius: 4px;
        }
        QComboBox {
            background-color: #3c3c3c;
            color: #e0e0e0;
            border: 1px solid #555;
            border-radius: 4px;
            padding: 6px 12px;
            min-height: 28px;
            font-size: 13px;
        }
        QComboBox:hover {
            border-color: #6a9ef5;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #aaa;
            margin-right: 8px;
        }
        QComboBox QAbstractItemView {
            background-color: #3c3c3c;
            color: #e0e0e0;
            selection-background-color: #5a7dc5;
            border: 1px solid #555;
        }
        QSlider::groove:horizontal {
            height: 6px;
            background: #444;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            width: 16px;
            height: 16px;
            background: #6a9ef5;
            border-radius: 8px;
            margin: -5px 0;
        }
        QSlider::handle:horizontal:hover {
            background: #7eb3ff;
        }
        QSlider::sub-page:horizontal {
            background: #5a7dc5;
            border-radius: 3px;
        }
        QGroupBox {
            background-color: #333;
            border: 1px solid #444;
            border-radius: 6px;
            margin-top: 12px;
            padding: 16px;
            font-size: 13px;
            color: #ccc;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }
        QPushButton {
            background-color: #4a4a4a;
            color: #e0e0e0;
            border: 1px solid #555;
            border-radius: 6px;
            padding: 8px 24px;
            font-size: 13px;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #555;
            border-color: #6a9ef5;
        }
        QPushButton:pressed {
            background-color: #3a3a3a;
        }
        QPushButton#okButton {
            background-color: #5a7dc5;
            border-color: #5a7dc5;
        }
        QPushButton#okButton:hover {
            background-color: #6a8dd5;
        }
    )");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);
    
    // 字体选择区域
    QLabel* fontLabel = new QLabel("选择字体：", this);
    m_fontCombo = new QComboBox(this);
    m_fontCombo->setMaxVisibleItems(12);
    
    // 字体大小区域
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    QLabel* sizeTitleLabel = new QLabel("字体大小：", this);
    m_sizeSlider = new QSlider(Qt::Horizontal, this);
    m_sizeSlider->setRange(8, 36);
    m_sizeSlider->setValue(12);
    m_sizeLabel = new QLabel("12", this);
    m_sizeLabel->setFixedWidth(30);
    m_sizeLabel->setAlignment(Qt::AlignCenter);
    
    sizeLayout->addWidget(sizeTitleLabel);
    sizeLayout->addWidget(m_sizeSlider, 1);
    sizeLayout->addWidget(m_sizeLabel);
    
    // 预览区域
    QGroupBox* previewGroup = new QGroupBox("预览效果", this);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);
    m_previewLabel = new QLabel("这是预览文字 - Preview Text 123", previewGroup);
    m_previewLabel->setObjectName("previewLabel");
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setMinimumHeight(50);
    previewLayout->addWidget(m_previewLabel);
    
    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_cancelButton = new QPushButton("取消", this);
    m_okButton = new QPushButton("确定", this);
    m_okButton->setObjectName("okButton");
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);
    
    // 添加到主布局
    mainLayout->addWidget(fontLabel);
    mainLayout->addWidget(m_fontCombo);
    mainLayout->addLayout(sizeLayout);
    mainLayout->addWidget(previewGroup);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(m_fontCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &FontSettingsDialog::onFontChanged);
    connect(m_sizeSlider, &QSlider::valueChanged, 
            this, &FontSettingsDialog::onSizeChanged);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void FontSettingsDialog::loadFonts()
{
    QFontDatabase fontDb;
    QStringList families = fontDb.families();
    
    // 按中文字体优先排序
    QStringList chineseFonts;
    QStringList otherFonts;
    
    for (const QString& family : families) {
        // 判断是否包含中文字符（简单判断方法）
        bool hasChinese = false;
        for (const QChar& ch : family) {
            if (ch.unicode() > 0x4E00 && ch.unicode() < 0x9FFF) {
                hasChinese = true;
                break;
            }
        }
        
        if (hasChinese || family.contains("YaHei") || family.contains("SimSun") ||
            family.contains("SimHei") || family.contains("KaiTi") ||
            family.contains("FangSong") || family.contains("Microsoft")) {
            chineseFonts.append(family);
        } else {
            otherFonts.append(family);
        }
    }
    
    // 先添加中文字体，再添加其他字体
    m_fontCombo->addItems(chineseFonts);
    m_fontCombo->addItems(otherFonts);
}

void FontSettingsDialog::onFontChanged(int index)
{
    Q_UNUSED(index);
    updatePreview();
}

void FontSettingsDialog::onSizeChanged(int value)
{
    m_sizeLabel->setText(QString::number(value));
    updatePreview();
}

void FontSettingsDialog::updatePreview()
{
    QString fontFamily = m_fontCombo->currentText();
    int fontSize = m_sizeSlider->value();
    
    m_selectedFont = QFont(fontFamily, fontSize);
    
    // 使用样式表设置预览字体，绕过全局样式表的覆盖问题
    m_previewLabel->setStyleSheet(QString(
        "background-color: #2b2b2b; "
        "padding: 12px; "
        "border-radius: 4px; "
        "font-family: '%1'; "
        "font-size: %2pt; "
        "color: #e0e0e0;"
    ).arg(fontFamily).arg(fontSize));
}
