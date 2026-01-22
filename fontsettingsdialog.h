#ifndef FONTSETTINGSDIALOG_H
#define FONTSETTINGSDIALOG_H

#include <QDialog>
#include <QFont>

class QComboBox;
class QSlider;
class QLabel;
class QPushButton;

class FontSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontSettingsDialog(const QFont& currentFont, QWidget* parent = nullptr);
    
    // 获取用户选择的字体
    QFont selectedFont() const;

private slots:
    void onFontChanged(int index);
    void onSizeChanged(int value);
    void updatePreview();

private:
    void setupUI();
    void loadFonts();
    
    QComboBox* m_fontCombo;
    QSlider* m_sizeSlider;
    QLabel* m_sizeLabel;
    QLabel* m_previewLabel;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    QFont m_selectedFont;
};

#endif // FONTSETTINGSDIALOG_H
