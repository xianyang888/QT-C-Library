#include "userEditDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFont>
#include <QFrame>

UserEditDialog::UserEditDialog(const QString& account,
                               const QString& name,
                               const QString& major,
                               const QString& userType,
                               QWidget* parent)
    : QDialog(parent), m_account(account)
{
    setWindowTitle(u8"修改用户信息");
    setFixedSize(400, 500);
    setModal(true);
    
    setupUI();
    
    // // 预填充数据  乱码不知道怎么解决
    // nameEdit->setText(name);
    // majorEdit->setText(major);
    
    // 设置用户类型下拉框
    int typeIndex = typeCombo->findText(userType);
    if (typeIndex != -1) {
        typeCombo->setCurrentIndex(typeIndex);
    }
    
    // 连接信号和槽
    connect(confirmButton, &QPushButton::clicked, this, &UserEditDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &UserEditDialog::onCancelClicked);
}

void UserEditDialog::setupUI()
{
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // 标题
    QLabel* titleLabel = new QLabel(u8"修改用户信息", this);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // 分隔线
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: #bdc3c7;");
    mainLayout->addWidget(line);
    
    // 表单布局
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight);
    
    // 账号标签（只读）
    accountLabel = new QLabel(m_account, this);
    formLayout->addRow(u8"账号:", accountLabel);
    
    // 姓名输入框
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText(u8"请输入姓名");
    formLayout->addRow(u8"姓名:", nameEdit);
    
    // 专业输入框
    majorEdit = new QLineEdit(this);
    majorEdit->setPlaceholderText(u8"请输入专业");

    formLayout->addRow(u8"专业:", majorEdit);
    
    // 用户类型下拉框
    typeCombo = new QComboBox(this);
    typeCombo->addItems({u8"读者", u8"管理员"});
    formLayout->addRow(u8"用户类型:", typeCombo);
    
    mainLayout->addLayout(formLayout);
    
    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    
    // 取消按钮
    cancelButton = new QPushButton(u8"取消", this);
    cancelButton->setFixedSize(100, 35);

    
    // 确认按钮
    confirmButton = new QPushButton(u8"确认修改", this);
    confirmButton->setFixedSize(100, 35);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    
    mainLayout->addLayout(buttonLayout);
}

QString UserEditDialog::getName() const
{
    return nameEdit->text().trimmed();
}

QString UserEditDialog::getMajor() const
{
    return majorEdit->text().trimmed();
}

QString UserEditDialog::getUserType() const
{
    return typeCombo->currentText();
}

void UserEditDialog::onConfirmClicked()
{
    // 验证输入
    if (getName().isEmpty()) {
        QMessageBox::warning(this, u8"输入错误", u8"姓名不能为空！");
        nameEdit->setFocus();
        return;
    }
    
    if (getMajor().isEmpty()) {
        QMessageBox::warning(this, u8"输入错误", u8"专业不能为空！");
        majorEdit->setFocus();
        return;
    }
    
    // 确认修改
    int ret = QMessageBox::question(this, u8"确认修改", 
                                   QString(u8"确定要修改用户 %1 的信息吗？").arg(m_account),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        accept(); // 关闭对话框并返回 QDialog::Accepted
    }
}

void UserEditDialog::onCancelClicked()
{
    reject(); // 关闭对话框并返回 QDialog::Rejected
}
