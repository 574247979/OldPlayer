#include "customtimedialog.h"
#include <QVBoxLayout>
#include <QTimeEdit>
#include <QDialogButtonBox>

CustomTimeDialog::CustomTimeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("选择时间");

    m_timeEdit = new QTimeEdit(this);
    m_timeEdit->setDisplayFormat("HH:mm:ss"); // 设置时间格式
    m_timeEdit->setTime(QTime::currentTime()); // 默认显示当前时间

    // 创建标准的 "OK" 和 "Cancel" 按钮
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // 连接按钮的 accepted/rejected 信号到对话框的 accept/reject 槽
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_timeEdit);
    layout->addWidget(m_buttonBox);
}

QTime CustomTimeDialog::selectedTime() const
{
    return m_timeEdit->time();
}

void CustomTimeDialog::setCurrentTime(const QTime &time)
{
    m_timeEdit->setTime(time);
}