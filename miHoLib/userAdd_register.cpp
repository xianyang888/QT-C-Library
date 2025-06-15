#include "userAdd_register.h"

UserAddDialog::UserAddDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

//基础UI
void UserAddDialog::setupUI() {
    nameEdit = new QLineEdit;
    accountEdit = new QLineEdit;
    passwordEdit = new QLineEdit;
    majorEdit = new QLineEdit;
    typeCombo = new QComboBox;
    typeCombo->addItems({ "READER", "ADMIN" });

    formLayout = new QFormLayout;
    formLayout->addRow(u8"姓名", nameEdit);
    formLayout->addRow(u8"学号", accountEdit);
    formLayout->addRow(u8"密码", passwordEdit);
    formLayout->addRow(u8"专业", majorEdit);
    formLayout->addRow(u8"类型", typeCombo);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

//=========Register============================
RegisterDialog::RegisterDialog(QWidget* parent)
    : UserAddDialog(parent){
    setWindowTitle(u8"注册新读者");
    applyCustomUI();
}

void RegisterDialog::applyCustomUI() {
    // 注册用户时不能选择用户类型
    typeCombo->setCurrentIndex(0);
    typeCombo->setEnabled(false);             // 禁止用户交互
}

//========adminAdd=============================
AdminAddUserDialog::AdminAddUserDialog(QWidget* parent)
    : UserAddDialog(parent) {
    setWindowTitle(u8"添加新用户");
    applyCustomUI(); // 留给子类拓展
}

void AdminAddUserDialog::applyCustomUI() {
    typeCombo->setVisible(true);
}


