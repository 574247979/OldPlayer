#ifndef CUSTOMTIMEDIALOG_H
#define CUSTOMTIMEDIALOG_H

#include <QDialog>
#include <QTime>

// 向前声明，避免包含完整头文件
class QTimeEdit;
class QDialogButtonBox;

class CustomTimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomTimeDialog(QWidget *parent = nullptr);

    // 获取用户选择的时间
    QTime selectedTime() const;
    // 设置对话框打开时显示的初始时间
    void setCurrentTime(const QTime &time);

private:
    QTimeEdit *m_timeEdit;
    QDialogButtonBox *m_buttonBox;
};

#endif // CUSTOMTIMEDIALOG_H